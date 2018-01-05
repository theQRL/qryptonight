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

#ifndef QRYPTONIGHT_POW_Impl_H
#define QRYPTONIGHT_POW_Impl_H

#include <vector>
#include <cstdint>

class PoWHelper {
public:
    PoWHelper( int64_t coeff_a=10,
               int64_t coeff_b=60,
               int64_t coeff_c=-99,
               int64_t coeff_d=16);
    virtual ~PoWHelper()=default;

    std::vector<uint8_t> getDifficulty(uint64_t timestamp,
                                       uint64_t parent_timestamp,
                                       const std::vector<uint8_t> &parent_difficulty);

    std::vector<uint8_t> getBoundary(const std::vector<uint8_t> &difficulty);
private:
    int64_t _coeff_a;
    int64_t _coeff_b;
    int64_t _coeff_c;
    int64_t _coeff_d;
};

#endif //QRYPTONIGHT_POW_Impl_H
