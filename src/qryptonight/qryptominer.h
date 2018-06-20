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

#include <atomic>
#include <thread>
#include <mutex>
#include <future>
#include <deque>
#include <vector>

class QryptonightPool; // forward-declare this class to keep swig from including

enum MinerEventType {
  SOLUTION = 0,
  TIMEOUT = 1
};

struct MinerEvent {
  MinerEventType type;
  uint64_t seq;
  uint32_t nonce;
};

class Qryptominer {
public:
    Qryptominer();
    virtual ~Qryptominer();

    uint64_t start(const std::vector<uint8_t>& input,
            size_t nonceOffset,
            const std::vector<uint8_t>& target,
            uint32_t thread_count = 1);

    uint64_t currentSequenceId() { return _work_sequence_id.load(); }

    void setTimer(uint32_t stopInMilliseconds);
    void disableTimer();
    uint32_t getSecondsRemaining();

    void setForcedSleep(uint32_t pauseInMilliseconds);

    bool waitForAnswer(uint32_t timeoutSeconds);

    void cancel();
    bool isRunning();
    std::uint32_t runningThreadCount();

    virtual uint8_t handleEvent(MinerEvent event) { return 1; };

    bool solutionAvailable();
    std::vector<uint8_t> solutionInput();
    std::vector<uint8_t> solutionHash();
    uint32_t solutionNonce();
    uint32_t hashRate();

protected:
    uint8_t _sendEvent(MinerEvent event);
    void _queueEvent(MinerEvent event);

    void _eventThreadWorker();

    std::vector<uint8_t> _input;
    std::vector<uint8_t> _target;
    size_t _nonceOffset{0};

    std::atomic<std::uint64_t> _work_sequence_id{0};

    std::vector<uint8_t> _solution_input;
    std::vector<uint8_t> _solution_hash;

    std::atomic_bool _solution_found{false};
    std::atomic_bool _stop_eventThread{false};
    std::atomic_bool _stop_request{false};

    std::atomic<std::uint32_t> _hash_count{0};
    std::atomic<std::uint32_t> _hash_per_sec{0};

    std::atomic<std::int32_t> _deadline_milliseconds;
    std::atomic<bool> _deadline_enabled;

    std::atomic<std::int32_t> _pause_milliseconds;

    std::vector<std::unique_ptr<std::thread>> _runningThreads;
    std::atomic<std::uint32_t> _runningThreads_count{0};

    std::recursive_timed_mutex _solution_mutex;
    std::recursive_timed_mutex _event_mutex;
    std::recursive_timed_mutex _runningThreads_mutex;

    std::future<void> _solution_event;
    std::unique_ptr<std::thread> _eventThread;

    std::deque<MinerEvent> _eventQueue;
    std::mutex _eventQueue_mutex;
    std::condition_variable _eventReleased;

    std::chrono::high_resolution_clock::time_point _referenceTime;

    static std::shared_ptr<QryptonightPool> _qnpool;
};

#endif //QRYPTONIGHT_QRYPTOMINER_H
