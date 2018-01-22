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
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x42, 0x40
        };

        EXPECT_EQ(expected_difficulty, difficulty);

        auto boundary1 = ph.getBoundary(difficulty);

        std::vector<uint8_t> expected_boundary{
                0x00, 0x00, 0x10, 0xc6, 0xf7, 0xa0, 0xb5, 0xed,
                0x8d, 0x36, 0xb4, 0xc7, 0xf3, 0x49, 0x38, 0x58,
                0x36, 0x21, 0xfa, 0xfc, 0x8b, 0x00, 0x79, 0xa2,
                0x83, 0x4d, 0x26, 0xfa, 0x3f, 0xcc, 0x9e, 0xa9
        };

        EXPECT_EQ(expected_boundary, boundary1);
    }

    TEST(PoWHelper, BoundaryCalculationDifficulty2) {
        PoWHelper ph;

        auto difficulty = toByteVector(1000488);
        auto boundary1 = ph.getBoundary(difficulty);

        std::vector<uint8_t> expected_boundary{
                0x00, 0x00, 0x10, 0xc4, 0xdf, 0x53, 0xeb, 0xc0,
                0x45, 0xd3, 0xc9, 0xf3, 0x73, 0xae, 0xa2, 0xc3,
                0xe6, 0xef, 0xbf, 0xb5, 0x31, 0xc3, 0xbf, 0xa2,
                0x54, 0x48, 0x81, 0x39, 0x1b, 0xf9, 0xf2, 0xf5
        };

//        std::cout << std::endl << printByteVector(boundary1) << std::endl;

        EXPECT_EQ(expected_boundary, boundary1);
    }

    TEST(PoWHelper, DifficultyOne) {
        PoWHelper ph;

        std::vector<uint8_t> answer;

        ph.clearTimestamps();
        ph.addTimestamp(100);
        answer = ph.getDifficulty(130, toByteVector(1000000) );
        EXPECT_EQ(1048800, fromByteVector(answer));

        ph.clearTimestamps();
        ph.addTimestamp(100);
        answer = ph.getDifficulty(140, toByteVector(1000000) );
        EXPECT_EQ(1032208, fromByteVector(answer));

        ph.clearTimestamps();
        ph.addTimestamp(100);
        answer = ph.getDifficulty(155, toByteVector(1000000) );
        EXPECT_EQ(1007808, fromByteVector(answer));

        ph.clearTimestamps();
        ph.addTimestamp(100);
        answer = ph.getDifficulty(160, toByteVector(1000000) );
        EXPECT_EQ(1000000, fromByteVector(answer));

        ph.clearTimestamps();
        ph.addTimestamp(100);
        answer = ph.getDifficulty(170, toByteVector(1000000) );
        EXPECT_EQ(984384, fromByteVector(answer));

        ph.clearTimestamps();
        ph.addTimestamp(100);
        answer = ph.getDifficulty(180, toByteVector(1000000) );
        EXPECT_EQ(967792, fromByteVector(answer));

        ph.clearTimestamps();
        ph.addTimestamp(100);
        answer = ph.getDifficulty(190, toByteVector(1000000) );
        EXPECT_EQ(951200, fromByteVector(answer));
    }

    TEST(PoWHelper, DifficultyExtreme) {
        PoWHelper ph;

        std::vector<uint8_t> answer;

        ph.clearTimestamps();
        ph.addTimestamp(0);
        answer = ph.getDifficulty(247, toByteVector(10727) );

        EXPECT_EQ(7617, fromByteVector(answer));
    }

}
