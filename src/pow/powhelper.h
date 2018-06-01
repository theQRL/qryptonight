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
#include <deque>
#include <memory>

class QryptonightPool; // forward-declare this class to keep swig from including

class PoWHelper {
public:
    explicit PoWHelper( int64_t kp=100,
                        uint64_t set_point=60,
                        int64_t adjfact_lower=-1000,
                        int64_t adjfact_upper=+1000,
                        int64_t adj_quantization=1024);

    virtual ~PoWHelper()=default;

    int64_t getKp() { return _Kp; }

    std::vector<uint8_t> getDifficulty(uint64_t measurement,
                                       const std::vector<uint8_t> &parent_difficulty);

    std::vector<uint8_t> getTarget(const std::vector<uint8_t> &difficulty);

    static bool passesTarget(const std::vector<uint8_t> &hash, const std::vector<uint8_t> &target);
    bool verifyInput(const std::vector<uint8_t> &input, const std::vector<uint8_t> &target);

private:
    int64_t _Kp;
    uint64_t _set_point;
    int64_t _adjfact_lower;
    int64_t _adjfact_upper;
    int64_t _adj_quantization;

    static std::shared_ptr<QryptonightPool> _qnpool;
};

#endif //QRYPTONIGHT_POW_Impl_H
