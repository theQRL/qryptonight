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
#include <iostream>
#include <Python.h>

Qryptominer::Qryptominer()
{
    _nonceOffset = 0;
}

Qryptominer::~Qryptominer()
{
    cancel();
    // Stop threads
}

bool Qryptominer::passesTarget(const std::vector<uint8_t> &hash)
{
    // The hash needs to be below the target (both 32 bytes)
    for(size_t i = 0; i < 32; i++)
    {
        if (hash[i] > _target[i])
            return false;

        if (hash[i] < _target[i])
            return true;
    }

    return false;  // they are equal
}

void Qryptominer::setNonce(std::vector<uint8_t> &input, uint32_t value)
{
    auto p = input.data();
    auto nonce = reinterpret_cast<uint32_t *>(p + _nonceOffset);
    *nonce = value;
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

    for(uint32_t i=0; i < thread_count; i++ )
    {
        _runningThreads.emplace_back(
            std::thread([&](uint32_t i, uint8_t thread_count)
            {
                Qryptonight qn;
                auto tmp_input(_input);
                uint32_t myNonce = i;

                while(!_stop_request && !_solution_found)
                {
                    setNonce(tmp_input, myNonce);

                    if (passesTarget(qn.hash(tmp_input)))
                    {
                        {
                            std::lock_guard<std::mutex> lock(_solution_mutex);
                            if (_solution_found)
                                continue;
                            _solution_found = true;
                        }

                        _solution_nonce = myNonce;
                        _solution_input = tmp_input;
                        solutionEvent(_solution_nonce);
                    }

                    myNonce += thread_count;
                }
            }, i, thread_count));
    }
}

bool Qryptominer::isRunning()
{
    for (auto& t : _runningThreads)
    {
        if (t.joinable())
            return true;
    }

    return false;
}

void Qryptominer::cancel()
{
    _stop_request = true;
    for (auto& t : _runningThreads)
    {
        t.join();
    }
}

void Qryptominer::solutionEvent(uint32_t nonce)
{
    // Derive classes can override to receive a callback
    std::cout << "Nonce: " << nonce << std::endl;
}
