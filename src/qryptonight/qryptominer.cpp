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
#include "pow/powhelper.h"
#include <iostream>
#include <chrono>
#include <netinet/in.h>

#define HASHRATE_MEASUREMENT_CYCLE 100
#define HASHRATE_MEASUREMENT_FACTOR 10

class ScopedCounter
{
public:
    ScopedCounter(std::atomic<std::uint32_t> &counter): _counter(counter){
        _counter++;
    }
    ~ScopedCounter() {
        _counter--;
    }

    std::atomic<std::uint32_t> &_counter;
};

Qryptominer::Qryptominer() {
    _eventThread = std::make_unique<std::thread>([&]() { _eventThreadWorker(); });
}

Qryptominer::~Qryptominer() {
    cancel();
    _stop_eventThread = true;
    _eventReleased.notify_one();
    _eventThread->join();
}

bool Qryptominer::solutionAvailable() {
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    return _solution_found;
}

std::vector<uint8_t> Qryptominer::solutionInput() {
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    return _solution_input;
}

uint32_t Qryptominer::solutionNonce() {
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    auto p = _solution_input.data();
    auto nonce = reinterpret_cast<uint32_t *>(p + _nonceOffset);
    return ntohl(*nonce);
}

std::vector<uint8_t> Qryptominer::solutionHash() {
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    return _solution_hash;
}

uint32_t Qryptominer::hashRate() {
    return static_cast<uint32_t>(_hash_per_sec);
};

void Qryptominer::start(const std::vector<uint8_t> &input,
                        size_t nonceOffset,
                        const std::vector<uint8_t> &target,
                        uint32_t thread_count) {
    cancel();
    _work_sequence_id++;

    _input = input;
    _nonceOffset = nonceOffset;
    _target = target;

    _stop_request = false;
    _solution_found = false;
    _hash_count = 0;
    _hash_per_sec = 0;

    std::lock_guard<std::recursive_timed_mutex> lock_runningThreads(_runningThreads_mutex);

    if (thread_count==0)
    {
        thread_count = std::thread::hardware_concurrency();
    }

    for (uint32_t thread_idx = 0; thread_idx < thread_count; thread_idx++) {
        _runningThreads.emplace_back(
                std::make_unique<std::thread>([&](uint32_t thread_idx, uint8_t thread_count) {
                    ScopedCounter thread_counter(_runningThreads_count);

                    Qryptonight qn;
                    auto tmp_input(_input);
                    auto p = tmp_input.data();
                    auto nonce = reinterpret_cast<uint32_t *>(p + _nonceOffset);

                    uint32_t current_nonce = thread_idx;

                    auto referenceTime = std::chrono::high_resolution_clock::now();
                    std::chrono::high_resolution_clock::time_point threadTime;
                    double drift = 0;

                    while (!_stop_request && !_solution_found) {
                        *nonce = htonl(current_nonce);
                        auto current_hash = qn.hash(tmp_input);
                        _hash_count++;

                        if (thread_idx == 0) {
                            threadTime = std::chrono::high_resolution_clock::now();
                            std::chrono::duration<double, std::milli> delta = threadTime - referenceTime;
                            if (delta.count() + drift > HASHRATE_MEASUREMENT_CYCLE) {
                                drift = delta.count() + drift - HASHRATE_MEASUREMENT_CYCLE;
                                referenceTime = std::chrono::high_resolution_clock::now();
                                _hash_per_sec = _hash_count * HASHRATE_MEASUREMENT_FACTOR;
                                _hash_count = 0;
                            }
                        }

                        if (PoWHelper::passesTarget(current_hash, _target)) {
                            std::lock_guard<std::recursive_timed_mutex> lock_solution(_solution_mutex);
                            if (!_solution_found) {
                                _solution_found = true;
                                _solution_input = tmp_input;
                                _solution_hash = current_hash;

                                {
                                    std::lock_guard<std::mutex> lock_queue(_eventQueue_mutex);

                                    MinerSolutionEvent event = {
                                            .nonce = current_nonce,
                                            .event_seq = _work_sequence_id.load()
                                    };

                                    _eventQueue.push_back(event);
                                }
                                _eventReleased.notify_one();
                            }
                        }

                        current_nonce += thread_count;
                    }
                }, thread_idx, thread_count));
    }
}

void Qryptominer::cancel() {
    std::lock_guard<std::recursive_timed_mutex> lock(_runningThreads_mutex);
    _stop_request = true;

    for (auto &t : _runningThreads) {
        t->join();
    }

    _runningThreads.clear();
}

bool Qryptominer::isRunning() {
    return _runningThreads_count > 0;
}

std::uint32_t Qryptominer::runningThreadCount()
{
    return _runningThreads_count;
}

void Qryptominer::_solutionEvent(uint32_t nonce, uint64_t event_seq) {
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_event_mutex);
    if (event_seq == _work_sequence_id)
    {
        try
        {
            solutionEvent(nonce);
        }
        catch(std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
}

void Qryptominer::solutionEvent(uint32_t nonce) {
    // Derived classes can override to receive a callback
    std::cout << "Nonce: " << nonce << std::endl;
}

void Qryptominer::_eventThreadWorker() {
    while (!_stop_eventThread) {
        std::unique_lock<std::mutex> queue_lock(_eventQueue_mutex);
        _eventReleased.wait(queue_lock,
                            [=] { return !_eventQueue.empty() || _stop_eventThread; });

        if (!_eventQueue.empty()) {
            auto &event = _eventQueue.front();
            _eventQueue.pop_front();
            queue_lock.unlock();
            _solutionEvent(event.nonce, event.event_seq);
        } else {
            queue_lock.unlock();
        }
    }
}