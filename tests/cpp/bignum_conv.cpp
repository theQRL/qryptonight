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
#include <qryptonight/qryptominer.h>
#include <pow/powhelper.h>
#include <misc/bignum.h>
#include <misc/strbignum.h>
#include "gtest/gtest.h"

namespace {
    TEST(Bignum, bignum_vector) {
        auto tmp = toByteVector(1000000);
        std::vector<uint8_t> expected {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x42, 0x40
        };

        EXPECT_EQ(expected, tmp);

        auto tmp2 = fromByteVector(expected);

        EXPECT_EQ(1000000, tmp2);
    }

    TEST(Bignum, bignum_vector2) {
        auto tmp = toByteVector(1000488);
        std::vector<uint8_t> expected {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x44, 0x28
        };

        EXPECT_EQ(expected, tmp);

        auto tmp2 = fromByteVector(expected);

        EXPECT_EQ(1000488, tmp2);
    }

    TEST(Bignum, bignum_empty) {
        EXPECT_THROW(auto int256 = fromByteVector({0}), std::invalid_argument);
    }

    TEST(Bignum, bignum_empty_str) {
        auto int256 = StringToUInt256("");

        std::vector<uint8_t> expected {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        EXPECT_EQ(expected, int256);
    }

    TEST(Bignum, bignum_str) {
        auto int256 = StringToUInt256("2");

        std::vector<uint8_t> expected {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02
        };

        EXPECT_EQ(expected, int256);
    }

    TEST(Bignum, bignum_str_invalid) {
        EXPECT_THROW(auto int256 = StringToUInt256("invalid"), std::invalid_argument);
    }
}
