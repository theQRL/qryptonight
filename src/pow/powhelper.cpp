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

PoWHelper::PoWHelper(int64_t coeff_a,
                     int64_t coeff_b,
                     int64_t coeff_c,
                     int64_t coeff_d)
{
    _coeff_a=coeff_a;
    _coeff_b=coeff_b;
    _coeff_c=coeff_c;
    _coeff_d=coeff_d;
}

std::vector<uint8_t> PoWHelper::getDifficulty(uint64_t timestamp,
                                              uint64_t parent_timestamp,
                                              const std::vector<uint8_t> &parent_difficulty_vec)
{
    const uint256_t _difficulty_lower_bound = 0;
    const uint256_t _difficulty_upper_bound = std::numeric_limits<uint256_t>::max();
    auto parent_difficulty = fromByteVector(parent_difficulty_vec);

    auto tmp_a = bigint(_coeff_a);
    auto tmp_b = bigint(_coeff_b);
    auto tmp_c = bigint(_coeff_c);
    auto tmp_d = bigint(_coeff_d);

    std::cout << std::endl;
    const bigint delta = bigint(timestamp) - parent_timestamp;
    const bigint adjFactor = std::max<bigint>(tmp_a - tmp_a*delta / tmp_b, tmp_c);
    bigint difficulty = parent_difficulty + (parent_difficulty * adjFactor)/tmp_d;

//    std::cout << "parent diff    " << parent_difficulty << std::endl;
//    std::cout << "delta          " << delta << std::endl;
//    std::cout << "adjFactor      " << adjFactor << std::endl;

    // Apply boundaries
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
    uint256_t tmp2 = uint256_t(max_boundary/difficulty);

    return toByteVector(tmp2);
}
