// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#ifndef QRYPTONIGHT_BIGNUM_H
#define QRYPTONIGHT_BIGNUM_H

#include <vector>
#include <cstdint>
#include <boost/multiprecision/cpp_int.hpp>

using uint256_t =  boost::multiprecision::uint256_t;
using bigint = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<>>;

uint256_t fromByteVector(const std::vector<uint8_t> &vec);
std::vector<uint8_t> toByteVector(uint256_t);
std::string printByteVector(const std::vector<uint8_t> &vec);
std::string printByteVector2(const std::vector<uint8_t> &vec);

#endif //QRYPTONIGHT_BIGNUM_H
