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
#include "gtest/gtest.h"

namespace {
    TEST(PoWHelper, BoundaryCalculationDifficultyZero) {
        PoWHelper ph;

        std::vector<uint8_t> zeros(32, 0);

        auto boundary1 = ph.getBoundary(zeros);

        EXPECT_EQ(zeros, boundary1);
    }

    TEST(PoWHelper, BoundaryCalculationDifficultyOne) {
        PoWHelper ph;

        auto difficulty = toByteVector(1000000);

        std::vector<uint8_t> expected_difficulty{
                0x40, 0x42, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        EXPECT_EQ(expected_difficulty, difficulty);

        auto boundary1 = ph.getBoundary(difficulty);

        std::vector<uint8_t> expected_boundary{
                0xA9, 0x9E, 0xCC, 0x3F, 0xFA, 0x26, 0x4D, 0x83,
                0xA2, 0x79, 0x00, 0x8B, 0xFC, 0xFA, 0x21, 0x36,
                0x58, 0x38, 0x49, 0xF3, 0xC7, 0xB4, 0x36, 0x8D,
                0xED, 0xB5, 0xA0, 0xF7, 0xC6, 0x10, 0x00, 0x00
        };

        EXPECT_EQ(expected_boundary, boundary1);
    }

    TEST(PoWHelper, BoundaryCalculationDifficulty2) {
        PoWHelper ph;

        auto difficulty = toByteVector(1000488);
        auto boundary1 = ph.getBoundary(difficulty);

        std::vector<uint8_t> expected_boundary{
                0xF5, 0xF2, 0xF9, 0x1B, 0x39, 0x81, 0x48, 0x54,
                0xA2, 0xBF, 0xC3, 0x31, 0xB5, 0xBF, 0xEF, 0xE6,
                0xC3, 0xA2, 0xAE, 0x73, 0xF3, 0xC9, 0xD3, 0x45,
                0xC0, 0xEB, 0x53, 0xDF, 0xC4, 0x10, 0x00, 0x00
        };

        EXPECT_EQ(expected_boundary, boundary1);
    }

    TEST(PoWHelper, bignum_vector) {
        auto tmp = toByteVector(1000000);
        std::vector<uint8_t> expected {
                0x40, 0x42, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        EXPECT_EQ(expected, tmp);

        auto tmp2 = fromByteVector(expected);

        EXPECT_EQ(1000000, tmp2);
    }

    TEST(PoWHelper, bignum_vector2) {
        auto tmp = toByteVector(1000488);
        std::vector<uint8_t> expected {
                0x28, 0x44, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        EXPECT_EQ(expected, tmp);

        auto tmp2 = fromByteVector(expected);

        EXPECT_EQ(1000488, tmp2);
    }

    TEST(PoWHelper, DifficultyOne) {
        PoWHelper ph;

        std::vector<uint8_t> answer;

        answer = ph.getDifficulty(105, 100, toByteVector(1000000) );
        EXPECT_EQ(toByteVector(1000488), answer);

        answer = ph.getDifficulty(110, 100, toByteVector(1000000) );
        EXPECT_EQ(toByteVector(1000000), answer);

        answer = ph.getDifficulty(120, 100, toByteVector(1000000) );
        EXPECT_EQ(toByteVector(999512), answer);

        answer = ph.getDifficulty(130, 100, toByteVector(1000000) );
        EXPECT_EQ(toByteVector(999024), answer);

        answer = ph.getDifficulty(140, 100, toByteVector(1000000) );
        EXPECT_EQ(toByteVector(998536), answer);

//        answer = ph.getDifficulty(155, 100, 1000000 );
//        EXPECT_EQ(1000488, answer);
//
//        answer = ph.getDifficulty(160, 100, 1000000 );
//        EXPECT_EQ(1000000, answer);
//
//        answer = ph.getDifficulty(170, 100, 1000000 );
//        EXPECT_EQ(999512, answer);
//
//        answer = ph.getDifficulty(180, 100, 1000000 );
//        EXPECT_EQ(999024, answer);

    }

}
