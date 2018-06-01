// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#ifndef QRYPTONIGHT_STRBIGNUM_H
#define QRYPTONIGHT_STRBIGNUM_H

#include <vector>
#include <cstdint>
#include <exception>

std::string UInt256ToString(std::vector<uint8_t> vec);
std::vector<uint8_t> StringToUInt256(const std::string &s);

#endif //QRYPTONIGHT_STRBIGNUM_H
