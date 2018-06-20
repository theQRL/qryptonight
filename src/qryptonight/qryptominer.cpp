/*
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  * Additional permission under GNU GPL version 3 section 7
  *
  * If you modify this Program, or any covered work, by linking or combining
  * it with OpenSSL (or a modified version of that library), containing parts
  * covered by the terms of OpenSSL License and SSLeay License, the licensors
  * of this Program grant you additional permission to convey the resulting work.
  *
  */

#include "qryptominer.h"
#include "qryptonight.h"
#include "qryptonightpool.h"
#include "pow/powhelper.h"
#include <iostream>
#include <chrono>

#ifndef _WIN32
#include <netinet/in.h>
#else
#include <winsock.h>
#endif

#define HASHRATE_MEASUREMENT_CYCLE 100
#define HASHRATE_MEASUREMENT_FACTOR 10

class ScopedCounter {
public:
    ScopedCounter(std::atomic<std::uint32_t>& counter)
            :_counter(counter)
    {
        _counter++;
    }
    ~ScopedCounter()
    {
        _counter--;
    }

    std::atomic<std::uint32_t>& _counter;
};

std::shared_ptr<QryptonightPool> Qryptominer::_qnpool = std::make_shared<QryptonightPool>();

Qryptominer::Qryptominer()
{
    _eventThread = std::make_unique<std::thread>([&]() { _eventThreadWorker(); });
    _referenceTime = std::chrono::high_resolution_clock::now();
    _deadline_enabled = false;
    _pause_milliseconds = 0;
}

Qryptominer::~Qryptominer()
{
    cancel();
    {
        std::lock_guard<std::mutex> queue_lock(_eventQueue_mutex);
        _stop_eventThread = true;
        _eventReleased.notify_one();
    }
    _eventThread->join();
}

bool Qryptominer::solutionAvailable()
{
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    return _solution_found;
}

bool Qryptominer::waitForAnswer(uint32_t timeoutSeconds)
{
    for (int i=0; i<timeoutSeconds; i++)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (solutionAvailable())
        {
            return true;
        }
    }
    return false;
}


std::vector<uint8_t> Qryptominer::solutionInput()
{
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    return _solution_input;
}

uint32_t Qryptominer::solutionNonce()
{
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    auto p = _solution_input.data();
    auto nonce = reinterpret_cast<uint32_t*>(p+_nonceOffset);
    return ntohl(*nonce);
}

std::vector<uint8_t> Qryptominer::solutionHash()
{
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    return _solution_hash;
}

uint32_t Qryptominer::hashRate()
{
    return static_cast<uint32_t>(_hash_per_sec);
};

void Qryptominer::disableTimer()
{
    _deadline_enabled = false;
}

void Qryptominer::setTimer(uint32_t stopInMilliseconds)
{
    auto now = std::chrono::high_resolution_clock::now();
    auto current_offset = std::chrono::duration_cast<std::chrono::milliseconds>(now-_referenceTime);

    _deadline_milliseconds = current_offset.count()+stopInMilliseconds;
    _deadline_enabled = true;
}

uint32_t Qryptominer::getSecondsRemaining()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto current_offset = std::chrono::duration_cast<std::chrono::milliseconds>(now-_referenceTime);
    auto remaining = _deadline_milliseconds.load()-current_offset.count();
    if (remaining<=0)
        return 0;
    return (uint32_t) remaining;
}

void Qryptominer::setForcedSleep(uint32_t pauseInMilliseconds)
{
    _pause_milliseconds = pauseInMilliseconds;
}

uint64_t Qryptominer::start(const std::vector<uint8_t>& input,
        size_t nonceOffset,
        const std::vector<uint8_t>& target,
        uint32_t thread_count)
{
    cancel();

    _input = input;
    _nonceOffset = nonceOffset;
    _target = target;

    _stop_request = false;
    _solution_found = false;
    _hash_count = 0;
    _hash_per_sec = 0;

    uint64_t current_work_sequence_id = _work_sequence_id.load();

    std::lock_guard<std::recursive_timed_mutex> lock_runningThreads(_runningThreads_mutex);

    if (thread_count==0) {
        thread_count = std::thread::hardware_concurrency();
    }

    for (uint32_t thread_idx = 0; thread_idx<thread_count; thread_idx++) {
        _runningThreads.emplace_back(
                std::make_unique<std::thread>([&]
                        (uint32_t thread_idx, uint8_t thread_count, uint64_t current_work_sequence_id)
                {
                  ScopedCounter thread_counter(_runningThreads_count);

                  auto qn = _qnpool->acquire();
                  auto tmp_input(_input);
                  auto p = tmp_input.data();
                  auto nonce = reinterpret_cast<uint32_t*>(p+_nonceOffset);

                  uint32_t current_nonce = thread_idx;

                  auto hashrateReferenceTime = std::chrono::high_resolution_clock::now();
                  std::chrono::high_resolution_clock::time_point threadTime;
                  double drift = 0;

                  while (!_stop_request && !_solution_found) {
                      *nonce = htonl(current_nonce);
                      auto current_hash = qn->hash(tmp_input);
                      _hash_count++;

                      if (thread_idx==0) {
                          threadTime = std::chrono::high_resolution_clock::now();
                          std::chrono::duration<double, std::milli> delta = threadTime-hashrateReferenceTime;
                          if (delta.count()+drift>HASHRATE_MEASUREMENT_CYCLE) {
                              drift = delta.count()+drift-HASHRATE_MEASUREMENT_CYCLE;
                              hashrateReferenceTime = std::chrono::high_resolution_clock::now();
                              _hash_per_sec = _hash_count*HASHRATE_MEASUREMENT_FACTOR;
                              _hash_count = 0;
                          }

                          if (_deadline_enabled && getSecondsRemaining()==0) {
                              _queueEvent({TIMEOUT, current_work_sequence_id});
                              _stop_request = true;
                              break;
                          }
                      }

                      if (_pause_milliseconds>0)
                      {
                          std::this_thread::sleep_for(std::chrono::milliseconds(_pause_milliseconds));
                      }

                      if (PoWHelper::passesTarget(current_hash, _target)) {
                          std::lock_guard<std::recursive_timed_mutex> lock_solution(_solution_mutex);
                          if (!_solution_found) {
                              _solution_found = true;
                              _solution_input = tmp_input;
                              _solution_hash = current_hash;
                              _queueEvent({SOLUTION, current_work_sequence_id, current_nonce});
                          }
                      }

                      current_nonce += thread_count;
                  }
                }, thread_idx, thread_count, current_work_sequence_id));
    }

    return _work_sequence_id;
}

void Qryptominer::_queueEvent(MinerEvent event)
{
    std::lock_guard<std::mutex> lock_queue(_eventQueue_mutex);
    _eventQueue.push_back(event);
    _eventReleased.notify_one();
}

void Qryptominer::cancel()
{
    std::lock_guard<std::recursive_timed_mutex> lock1(_event_mutex);
    std::lock_guard<std::recursive_timed_mutex> lock2(_runningThreads_mutex);
    _stop_request = true;

    for (auto& t : _runningThreads) {
        t->join();
    }
    _runningThreads.clear();
    _work_sequence_id++;
}

bool Qryptominer::isRunning()
{
    return _runningThreads_count>0;
}

std::uint32_t Qryptominer::runningThreadCount()
{
    return _runningThreads_count;
}

uint8_t Qryptominer::_sendEvent(MinerEvent event)
{
    std::lock_guard<std::recursive_timed_mutex> lock(_event_mutex);

    if (event.seq!=_work_sequence_id)
        return 1;    // handled

    try {
        return handleEvent(event);
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return 1;
}

void Qryptominer::_eventThreadWorker()
{
    while (!_stop_eventThread) {
        std::unique_lock<std::mutex> queue_lock(_eventQueue_mutex);
        _eventReleased.wait(queue_lock,
                [=] { return !_eventQueue.empty() || _stop_eventThread; });

        if (!_eventQueue.empty()) {
            auto event = _eventQueue.front();
            _eventQueue.pop_front();
            queue_lock.unlock();
            if (event.seq==_work_sequence_id) {
                if (!_sendEvent(event))
                {
                    // if event was not processed, put back in the queue and idle for 100ms
                    _queueEvent(event);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        }
        else {
            queue_lock.unlock();
        }
    }
}
