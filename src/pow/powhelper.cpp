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

#include "powhelper.h"
#include "qryptonight.h"
#include "misc/bignum.h"
#include <boost/multiprecision/cpp_dec_float.hpp>

PoWHelper::PoWHelper(int64_t kp,
                     uint64_t set_point,
                     int64_t adjfact_lower,
                     int64_t adjfact_upper,
                     uint16_t history_size)
: _Kp(kp),
  _set_point(set_point),
  _adjfact_lower(adjfact_lower),
  _adjfact_upper(adjfact_upper),
  _history_size(history_size)
{
}

void PoWHelper::addTimestamp(uint64_t timestamp)
{
    prev_timestamps.push_back(timestamp);

    while(prev_timestamps.size() > _history_size)
    {
        prev_timestamps.pop_front();
    }
}

void PoWHelper::clearTimestamps()
{
    prev_timestamps.clear();
}

uint64_t PoWHelper::get_average_delta(uint64_t timestamp)
{
    uint64_t sum_delta {0};
    size_t prev_size = prev_timestamps.size();

    if (prev_size == 0)
    {
        return _set_point;
    }

    for(size_t i = 1; i < prev_size-1; i++)
    {
        sum_delta += prev_timestamps[i-1] - prev_timestamps[i];
    }

    sum_delta += timestamp - prev_timestamps[prev_size-1];

    return sum_delta / prev_size;
}

std::vector<uint8_t> PoWHelper::getDifficulty(uint64_t timestamp,
                                              const std::vector<uint8_t> &parent_difficulty_vec)
{
    const uint256_t _difficulty_lower_bound = 2;                                // To avoid issues with the target
    const uint256_t _difficulty_upper_bound = std::numeric_limits<uint256_t>::max();

    auto parent_difficulty = fromByteVector(parent_difficulty_vec);
    const uint64_t delta = get_average_delta(timestamp);

    // calculate adjustment factor and apply boundaries
    bigint adjustment = bigint(_Kp - _Kp*delta/_set_point);

    if (adjustment > _adjfact_upper)
    {
        adjustment = bigint(_adjfact_upper);
    }
    if (adjustment < _adjfact_lower)
    {
        adjustment = bigint(_adjfact_lower);
    }

    // calculate difficulty and apply boundaries
    bigint difficulty = parent_difficulty + parent_difficulty / 1024 * adjustment;

    difficulty = std::max<bigint>(difficulty, _difficulty_lower_bound);
    difficulty = std::min<bigint>(difficulty, _difficulty_upper_bound);

    return toByteVector(uint256_t(difficulty));
}

std::vector<uint8_t> PoWHelper::getBoundary(const std::vector<uint8_t> &difficulty_vec)
{
    std::vector<uint8_t> boundary(32, 0);

    auto difficulty = fromByteVector(difficulty_vec);

    if (difficulty == 0)
        return boundary;

    bigint max_boundary = bigint(1) << 256;
    uint256_t tmp2 = uint256_t(max_boundary/bigint(difficulty));

    return toByteVector(tmp2);
}

bool PoWHelper::passesTarget(const std::vector<uint8_t> &hash, const std::vector<uint8_t> &target)
{
    // The hash needs to be below the target (both 32 bytes)
    for(size_t i = 0; i < 32; i++)
    {
        if (hash[i] > target[i])
            return false;

        if (hash[i] < target[i])
            return true;
    }

    return false;  // they are equal
}

bool PoWHelper::verifyInput(const std::vector<uint8_t> &input, const std::vector<uint8_t> &target)
{
    Qryptonight qn;
    auto hash = qn.hash(input);
    return passesTarget(hash, target);
}
