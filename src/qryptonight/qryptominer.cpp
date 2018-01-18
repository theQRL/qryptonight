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

Qryptominer::Qryptominer()
{
    _nonceOffset = 0;
}

Qryptominer::~Qryptominer()
{
    cancel();
    // Stop threads
}

void Qryptominer::setNonce(std::vector<uint8_t> &input, uint32_t value)
{
    auto p = input.data();
    auto nonce = reinterpret_cast<uint32_t *>(p + _nonceOffset);
    *nonce =  htonl(value);
}

uint32_t Qryptominer::solutionNonce()
{
    auto p = _solution_input.data();
    auto nonce = reinterpret_cast<uint32_t *>(p + _nonceOffset);
    return ntohl(*nonce);
}

void Qryptominer::setInput(const std::vector<uint8_t> &input, size_t nonceOffset, const std::vector<uint8_t> &target)
{
    // Setting a new input immediately cancels any previous work
    cancel();

    _input = input;
    _nonceOffset = nonceOffset;
    _target = target;
}

bool Qryptominer::start(uint8_t thread_count=1)
{
    cancel();
    _stop_request = false;
    _solution_found = false;
    _hash_count = 0;
    _hash_per_sec = 0;

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
                            std::lock_guard<std::mutex> lock(_solution_mutex);
                            if (!_solution_found){
                                _solution_found = true;
                                _solution_input = tmp_input;
                                _solution_hash = hash;
                                _solution_event = std::async(std::launch::async, [this]()
                                {
                                    cancel();
                                    solutionEvent( solutionNonce() );
                                });
                            }
                        }
                    }

                    myNonce += thread_count;
                }
            }, thread_idx, thread_count));
    }
    return true;
}

void Qryptominer::cancel()
{
    _stop_request = true;
    for (auto& t : _runningThreads)
    {
        t.join();
    }
    _runningThreads.clear();
}

void Qryptominer::solutionEvent(uint32_t nonce)
{
    // Derived classes can override to receive a callback
    std::cout << "Nonce: " << nonce << std::endl;
}
