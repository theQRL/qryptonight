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
#include <deque>

struct MinerSolutionEvent
{
    uint32_t nonce;
    uint64_t event_seq;
};

class Qryptominer {
public:
    Qryptominer();
    virtual ~Qryptominer();

    void start(const std::vector<uint8_t> &input,
               size_t nonceOffset,
               const std::vector<uint8_t> &target,
               uint8_t thread_count = 1);

    void cancel();
    bool isRunning();

    virtual void solutionEvent(uint32_t nonce);

    bool solutionAvailable();
    std::vector<uint8_t> solutionInput();
    std::vector<uint8_t> solutionHash();
    uint32_t solutionNonce();
    uint32_t hashRate();

protected:
    void _solutionEvent(uint32_t value, uint64_t event_seq);
    void _eventThreadWorker();

    std::vector<uint8_t> _input;
    std::vector<uint8_t> _target;
    size_t _nonceOffset { 0 };

    std::atomic<std::uint64_t> _work_sequence_id { 0 };

    std::vector<uint8_t> _solution_input;
    std::vector<uint8_t> _solution_hash;

    std::atomic_bool _solution_found { false };
    std::atomic_bool _stop_eventThread { false };
    std::atomic_bool _stop_request { false };

    std::atomic<std::uint32_t> _hash_count { 0 };
    std::atomic<std::uint32_t> _hash_per_sec { 0 };

    std::vector<std::unique_ptr<std::thread>> _runningThreads;
    std::atomic<std::uint32_t> _runningThreads_count { 0 };

    std::recursive_timed_mutex _solution_mutex;
    std::recursive_timed_mutex _solution_event_mutex;
    std::recursive_timed_mutex _runningThreads_mutex;

    std::future<void> _solution_event;
    std::unique_ptr<std::thread> _eventThread;

    std::deque<MinerSolutionEvent> _eventQueue;
    std::mutex _eventQueue_mutex;
    std::condition_variable _eventReleased;
};

#endif //QRYPTONIGHT_QRYPTOMINER_H
