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

#ifndef QRYPTONIGHT_QRYPTOMINER_H
#define QRYPTONIGHT_QRYPTOMINER_H

#include "qryptonight.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <future>

class Qryptominer {
public:
    Qryptominer();
    virtual ~Qryptominer();

    void setInput(const std::vector<uint8_t> &input, size_t nonceOffset, const std::vector<uint8_t> &target);

    bool start(uint8_t thread_count);
    void cancel();

    virtual void solutionEvent(uint32_t nonce);

    bool solutionFound()
    {
        std::lock_guard<std::mutex> lock(_solution_mutex);
        return _solution_found;
    }
    uint32_t solutionNonce();

    std::vector<uint8_t> solutionInput()
    {
        std::lock_guard<std::mutex> lock(_solution_mutex);
        return _solution_input;
    }

    std::vector<uint8_t> solutionHash()
    {
        std::lock_guard<std::mutex> lock(_solution_mutex);
        return _solution_hash;
    }

    uint32_t hashRate()
    {
        std::lock_guard<std::mutex> lock(_solution_mutex);
        return static_cast<uint32_t>(_hash_per_sec);
    };

protected:
    void setNonce(std::vector<uint8_t> &input, uint32_t value);

    std::vector<uint8_t> _input;
    std::vector<uint8_t> _target;
    size_t _nonceOffset;

    std::vector<uint8_t> _solution_input;
    std::vector<uint8_t> _solution_hash;

    std::atomic_bool _solution_found;
    std::atomic_bool _stop_request;
    std::atomic<std::uint32_t> _hash_count;
    std::atomic<std::uint32_t> _hash_per_sec;

    std::vector<std::thread> _runningThreads;
    std::mutex _solution_mutex;
    std::mutex _runningThreads_mutex;

    std::future<void> _solution_event;
};

#endif //QRYPTONIGHT_QRYPTOMINER_H
