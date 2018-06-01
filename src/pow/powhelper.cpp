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
#include "qryptonightpool.h"
#include "misc/bignum.h"

std::shared_ptr<QryptonightPool> PoWHelper::_qnpool = std::make_shared<QryptonightPool>();

PoWHelper::PoWHelper(int64_t kp,
                     uint64_t set_point,
                     int64_t adjfact_lower,
                     int64_t adjfact_upper,
                     int64_t adj_quantization)
: _Kp(kp),
  _set_point(set_point),
  _adjfact_lower(adjfact_lower),
  _adjfact_upper(adjfact_upper),
  _adj_quantization(adj_quantization)
{
}

std::vector<uint8_t> PoWHelper::getDifficulty(uint64_t measurement,
                                              const std::vector<uint8_t> &parent_difficulty_vec)
{
    const uint256_t _difficulty_lower_bound = 2;                                // To avoid issues with the target
    const uint256_t _difficulty_upper_bound = std::numeric_limits<uint256_t>::max();

    // calculate adjustment factor and apply boundaries

    const auto tmp_val = static_cast<const long>(((double) _Kp) - (double)_Kp*(double)measurement/(double)_set_point);

    bigint adjustment = bigint(tmp_val);

    if (adjustment > _adjfact_upper)
    {
        adjustment = bigint(_adjfact_upper);
    }
    if (adjustment < _adjfact_lower)
    {
        adjustment = bigint(_adjfact_lower);
    }

    uint256_t parent_difficulty = fromByteVector(parent_difficulty_vec);
    bigint difficulty_delta = (parent_difficulty * adjustment) / _adj_quantization;

    if (difficulty_delta == 0 &&  adjustment != 0){
        difficulty_delta = adjustment < 0 ? -1 : 1;
    }

    // calculate difficulty and apply boundaries
    bigint difficulty = parent_difficulty + difficulty_delta;

    difficulty = std::max<bigint>(difficulty, _difficulty_lower_bound);
    difficulty = std::min<bigint>(difficulty, _difficulty_upper_bound);

    return toByteVector(uint256_t(difficulty));
}

std::vector<uint8_t> PoWHelper::getTarget(const std::vector<uint8_t> &difficulty_vec)
{
    std::vector<uint8_t> boundary(32, 0);

    auto difficulty = fromByteVector(difficulty_vec);

    if (difficulty == 0)
        return boundary;

    bigint max_boundary = (bigint(1) << 256)-bigint(1);
    uint256_t tmp2 = uint256_t(max_boundary/bigint(difficulty));

    auto byteVector = toByteVector(tmp2);
    std::reverse(byteVector.begin(), byteVector.end());
    return byteVector;
}

bool PoWHelper::passesTarget(const std::vector<uint8_t> &hash, const std::vector<uint8_t> &target)
{
    // The hash needs to be below or equals to the target (both 32 bytes)
    // Monero uses little endian.. we need to check in reverse order
    if (hash.size()!=32 || target.size()!=32)
    {
        return false;
    }

    for(size_t i = 0; i < 32; i++)
    {
        const uint8_t h = hash[31-i];
        const uint8_t t = target[31-i];
        if (h > t)
            return false;

        if (h < t)
            return true;
    }

    return true;  // they are equal
}

bool PoWHelper::verifyInput(const std::vector<uint8_t> &input, const std::vector<uint8_t> &target)
{
    auto qn = _qnpool->acquire();
    auto hash = qn->hash(input);
    return passesTarget(hash, target);
}
