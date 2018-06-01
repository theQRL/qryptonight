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
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <cmath>
#include "gtest/gtest.h"

namespace {
    using uint256_t =  boost::multiprecision::uint256_t;

    TEST(Boost_bignum, Sum) {
        uint256_t a { 14 };
        uint256_t b { 17 };

        uint256_t c = a + b;

        EXPECT_EQ(31, c);
    }

    TEST(Boost_bignum, IntDiv) {
        uint256_t a { 100000000000 };
        uint256_t b { 3 };

        uint256_t c = a / b;

        EXPECT_EQ(33333333333, c);
    }

    TEST(Boost_bignum, InitString) {
        uint256_t a { "100000000000" };
        uint256_t b { 3 };

        uint256_t c = a / b;

        EXPECT_EQ(33333333333, c);
    }

    TEST(Boost_bignum, InitString2) {
        uint256_t a { "100000000000000000000000000000000000000000000000000" };
        uint256_t b { 3 };

        uint256_t c = a / b;

        EXPECT_EQ( uint256_t("33333333333333333333333333333333333333333333333333"), c);
    }

    TEST(Boost_bignum, NaNAssert1) {
        double nan_value = std::nan("0");
        EXPECT_DEATH( {uint256_t a { nan_value };} , "boost::math::isnan");
    }

    TEST(Boost_bignum, NaNAssert2) {
        EXPECT_DEATH( {uint256_t a { 0./0.  };} , "boost::math::isnan");
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiv-by-zero"
    TEST(Boost_bignum, NaNAssert3) {
        EXPECT_DEATH( {uint256_t a { 1./0 };} , "boost::math::isinf");
    }
#pragma GCC diagnostic pop

    TEST(Boost_bignum, InvalidNumber) {
        EXPECT_THROW( {uint256_t a { "hello" };} , std::exception);
    }

    // FIXME: This is not supported below Boost 1.60 (xenial will not work)
//    template<typename T>
//    std::vector<uint8_t> toByteVector(T &number)
//    {
//        auto max_precision = boost::multiprecision::backends::max_precision<typename T::backend_type>::value;
//        auto num_bytes = max_precision / 8;
//
//        std::vector<uint8_t> data;
//        data.reserve(num_bytes);
//
//        export_bits(number, std::back_inserter(data), 8, false);
//
//        data.resize(num_bytes);
//
//        return data;
//    }
//
//    template<typename T>
//    T fromByteVector(std::vector<uint8_t> data)
//    {
//        T val;
//        import_bits(val, data.begin(), data.end(), 8, false);
//        return val;
//    }
//
//    TEST(Boost_bignum, SerializeBigEndian) {
//        uint256_t a { 0xfe0923 };
//        auto data = toByteVector(a);
//
//        std::vector<uint8_t> expected {
//            0x23, 0x09, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
//            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
//        };
//
//        EXPECT_EQ( expected, data);
//    }
//
//    TEST(Boost_bignum, DeserializeBigEndian) {
//        std::vector<uint8_t> input {
//            0x23, 0x09, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
//            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
//        };
//
//        uint256_t expected { 0xfe0923 };
//
//        auto val = fromByteVector<uint256_t>(input);
//
//        std::cout << std::hex << val << std::endl;
//
//        EXPECT_EQ( expected, val);
//    }

}
