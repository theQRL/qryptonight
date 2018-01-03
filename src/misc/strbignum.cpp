// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include "misc/bignum.h"

std::string UInt256ToString(std::vector<uint8_t> vec)
{
    uint256_t val = fromByteVector(vec);
    return val.str();
}

std::vector<uint8_t> StringToUInt256(const std::string &s)
{
    uint256_t val(s);
    return toByteVector(val);
}
