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
#include "misc/bignum.h"

PoWHelper::PoWHelper(int64_t kp,
                     int64_t set_point,
                     int64_t adjfact_lower_percent,
                     int64_t adjfact_upper_percent)
{
    _Kp=kp;
    _set_point=set_point;
    _adjfact_lower_percent = adjfact_lower_percent;
    _adjfact_upper_percent = adjfact_upper_percent;
}

std::vector<uint8_t> PoWHelper::getDifficulty(uint64_t timestamp,
                                              uint64_t parent_timestamp,
                                              const std::vector<uint8_t> &parent_difficulty_vec)
{
    const uint256_t _difficulty_lower_bound = 2;        // To avoid issues with the target
    const uint256_t _difficulty_upper_bound = std::numeric_limits<uint256_t>::max();

    auto parent_difficulty = fromByteVector(parent_difficulty_vec);

    const bigint delta = bigint(timestamp) - parent_timestamp;
    const bigint error = delta - bigint(_set_point);

    // calculate adjustment factor and apply boundaries
    bigint adjustment = (bigint(parent_difficulty) * error) / bigint(_Kp);
    adjustment = std::max<bigint>(adjustment, adjustment - (adjustment * bigint(_adjfact_lower_percent)) / bigint(100));
    adjustment = std::min<bigint>(adjustment, adjustment + (adjustment * bigint(_adjfact_upper_percent)) / bigint(100));

//#ifndef NDEBUG
//    std::cout << std::endl << "--------------- " << std::endl;
//    std::cout << "parent diff    " << parent_difficulty << std::endl;
//    std::cout << "delta          " << delta << std::endl;
//    std::cout << "error          " << error << std::endl;
//    std::cout << "adjFactor      " << adjustment << std::endl;
//#endif

    // calculate difficulty and apply boundaries
    bigint difficulty = bigint(parent_difficulty) - adjustment;
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
