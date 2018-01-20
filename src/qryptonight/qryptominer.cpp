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

Qryptominer::~Qryptominer()
{
    cancel();
}

void Qryptominer::setNonce(std::vector<uint8_t> &input, uint32_t value)
{
    auto p = input.data();
    auto nonce = reinterpret_cast<uint32_t *>(p + _nonceOffset);
    *nonce =  htonl(value);
}

uint32_t Qryptominer::solutionNonce()
{
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    auto p = _solution_input.data();
    auto nonce = reinterpret_cast<uint32_t *>(p + _nonceOffset);
    return ntohl(*nonce);
}

bool Qryptominer::solutionFound()
{
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    return _solution_found;
}

std::vector<uint8_t> Qryptominer::solutionInput()
{
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    return _solution_input;
}

std::vector<uint8_t> Qryptominer::solutionHash()
{
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    return _solution_hash;
}

uint32_t Qryptominer::hashRate()
{
    std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
    return static_cast<uint32_t>(_hash_per_sec);
};

void Qryptominer::start(const std::vector<uint8_t> &input,
                        size_t nonceOffset,
                        const std::vector<uint8_t> &target,
                        uint8_t thread_count)
{
    cancel();

    _input = input;
    _nonceOffset = nonceOffset;
    _target = target;

    _stop_request = false;
    _solution_found = false;
    _hash_count = 0;
    _hash_per_sec = 0;

    std::lock_guard<std::recursive_timed_mutex> lock(_runningThreads_mutex);
    for(uint32_t thread_idx=0; thread_idx < thread_count; thread_idx++ )
    {
        _runningThreads.emplace_back(
            std::thread([&](uint32_t thread_idx, uint8_t thread_count)
            {
                Qryptonight qn;
                auto tmp_input(_input);
                uint32_t myNonce = thread_idx;

                auto referenceTime = std::chrono::high_resolution_clock::now();
                std::chrono::high_resolution_clock::time_point threadTime;
                double drift = 0;

                while(!_stop_request && !_solution_found)
                {
                    setNonce(tmp_input, myNonce);
                    auto hash = qn.hash(tmp_input);
                    _hash_count++;

                    if (thread_idx==0)
                    {
                        threadTime = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<double, std::milli> delta = threadTime - referenceTime;
                        if (delta.count() + drift > 1000)
                        {
                            drift = delta.count() + drift - 1000;
                            referenceTime = std::chrono::high_resolution_clock::now();
                            _hash_per_sec = 0+_hash_count;
                            _hash_count=0;
                        }
                    }

                    if (PoWHelper::passesTarget(hash, _target))
                    {
                        {
                            std::lock_guard<std::recursive_timed_mutex> lock(_solution_mutex);
                            if (!_solution_found){
                                _solution_found = true;
                                _solution_input = tmp_input;
                                _solution_hash = hash;
                                auto solution_nonce = solutionNonce();
                                _solution_event = std::async(std::launch::async, [this](uint32_t solution_nonce)
                                {
                                    _solutionEvent(solution_nonce);
                                }, solution_nonce);
                            }
                        }
                    }

                    myNonce += thread_count;
                }
            }, thread_idx, thread_count));
    }
}

void Qryptominer::cancel()
{
    std::lock_guard<std::recursive_timed_mutex> lock(_runningThreads_mutex);

    _stop_request = true;
    for (auto& t : _runningThreads)
    {
        t.join();
    }

    _runningThreads.clear();
}

void Qryptominer::_solutionEvent(uint32_t nonce)
{
    std::lock_guard<std::recursive_timed_mutex> lock(_runningThreads_mutex);
    solutionEvent(nonce);
}

void Qryptominer::solutionEvent(uint32_t nonce)
{
    // Derived classes can override to receive a callback
    std::cout << "Nonce: " << nonce << std::endl;
}
