// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include "bignum.h"

uint256_t fromByteVector(const std::vector<uint8_t> &vec)
{
    if (vec.size()!=32)
    {
        throw std::invalid_argument("vector size should be 32");
    }

    uint256_t tmp(0);
    for(uint8_t i=0; i<32; i++)
    {
        tmp<<=8;
        tmp+=vec.at(i);
    }
    return tmp;
}

std::vector<uint8_t> toByteVector(uint256_t val)
{
    std::vector<uint8_t> tmp(32, 0);

    for(uint8_t i=0; i<32; i++)
    {
        tmp[31-i] = uint8_t(val & 0xFF);
        val>>=8;
    }

    return tmp;
}

std::string printByteVector(const std::vector<uint8_t> &vec)
{
    std::stringstream ss;

    for(int i=0; i<vec.size(); i++)
    {
        if (i>0 && i%8==0)
            ss << std::endl;
        ss << "0x" << std::setfill('0') << std::setw(2) << std::hex << (int)(vec[i]);
        if (i<vec.size()-1)
            ss << ", ";
    }

    return ss.str();
}

std::string printByteVector2(const std::vector<uint8_t> &vec)
{
    std::stringstream ss;

    for (unsigned char i : vec) {
        ss << std::setfill('0') << std::setw(2) << std::hex << (int) i;
    }

    return ss.str();
}
