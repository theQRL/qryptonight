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
    TEST(PoWHelper, TargetCalculationDifficultyZero) {
        PoWHelper ph;

        std::vector<uint8_t> zeros(32, 0);

        auto target = ph.getTarget(zeros);

        EXPECT_EQ(zeros, target);
    }

    TEST(PoWHelper, TargetCalculationDifficultyOne) {
        PoWHelper ph;

        auto difficulty = toByteVector(1);

        std::vector<uint8_t> expected_difficulty{
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
        };

        EXPECT_EQ(expected_difficulty, difficulty);

        auto target = ph.getTarget(difficulty);

        std::cout << printByteVector(target) << std::endl;

        std::vector<uint8_t> expected_target{
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
        };

        EXPECT_EQ(expected_target, target);
    }

    TEST(PoWHelper, TargetCalculationDifficultyTwo) {
        PoWHelper ph;

        auto difficulty = toByteVector(2);

        std::vector<uint8_t> expected_difficulty{
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02
        };

        EXPECT_EQ(expected_difficulty, difficulty);

        auto target = ph.getTarget(difficulty);

        std::cout << printByteVector(target) << std::endl;

        std::vector<uint8_t> expected_target{
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f
        };

        EXPECT_EQ(expected_target, target);
    }

    TEST(PoWHelper, TargetCalculationDifficultyLow) {
        PoWHelper ph;

        auto difficulty = toByteVector(1000000);

        std::vector<uint8_t> expected_difficulty{
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x42, 0x40
        };

        EXPECT_EQ(expected_difficulty, difficulty);

        auto target = ph.getTarget(difficulty);
        std::cout << printByteVector(target) << std::endl;

        std::vector<uint8_t> expected_target{
                0xa9, 0x9e, 0xcc, 0x3f, 0xfa, 0x26, 0x4d, 0x83,
                0xa2, 0x79, 0x00, 0x8b, 0xfc, 0xfa, 0x21, 0x36,
                0x58, 0x38, 0x49, 0xf3, 0xc7, 0xb4, 0x36, 0x8d,
                0xed, 0xb5, 0xa0, 0xf7, 0xc6, 0x10, 0x00, 0x00
        };

        EXPECT_EQ(expected_target, target);
    }

    TEST(PoWHelper, TargetCalculationDifficulty2) {
        PoWHelper ph;

        auto difficulty = toByteVector(1000488);
        auto target = ph.getTarget(difficulty);

        std::vector<uint8_t> expected_target{
                0xf5, 0xf2, 0xf9, 0x1b, 0x39, 0x81, 0x48, 0x54,
                0xa2, 0xbf, 0xc3, 0x31, 0xb5, 0xbf, 0xef, 0xe6,
                0xc3, 0xa2, 0xae, 0x73, 0xf3, 0xc9, 0xd3, 0x45,
                0xc0, 0xeb, 0x53, 0xdf, 0xc4, 0x10, 0x00, 0x00
        };

        std::cout << printByteVector(target) << std::endl;

        EXPECT_EQ(expected_target, target);
    }

    TEST(PoWHelper, DifficultyOne) {
        PoWHelper ph;

        std::vector<uint8_t> answer;

        answer = ph.getDifficulty(30, toByteVector(1000000) );
        EXPECT_EQ(1048828, fromByteVector(answer));

        answer = ph.getDifficulty(40, toByteVector(1000000) );
        EXPECT_EQ(1032226, fromByteVector(answer));

        answer = ph.getDifficulty(55, toByteVector(1000000) );
        EXPECT_EQ(1007812, fromByteVector(answer));

        answer = ph.getDifficulty(60, toByteVector(1000000) );
        EXPECT_EQ(1000000, fromByteVector(answer));

        answer = ph.getDifficulty(70, toByteVector(1000000) );
        EXPECT_EQ(984375, fromByteVector(answer));

        answer = ph.getDifficulty(80, toByteVector(1000000) );
        EXPECT_EQ(967774, fromByteVector(answer));

        answer = ph.getDifficulty(90, toByteVector(1000000) );
        EXPECT_EQ(951172, fromByteVector(answer));
    }

    TEST(PoWHelper, DifficultyExtreme) {
        PoWHelper ph;

        std::vector<uint8_t> answer = ph.getDifficulty(187, toByteVector(10727) );

        EXPECT_EQ(8517, fromByteVector(answer));
    }

    TEST(PoWHelper, DifficultyFlatQuantization) {
        PoWHelper ph;
        std::vector<uint8_t> answer;

        answer = ph.getDifficulty(10, toByteVector(2) );
        EXPECT_EQ(3, fromByteVector(answer));

        answer = ph.getDifficulty(10, toByteVector(3) );
        EXPECT_EQ(4, fromByteVector(answer));

        answer = ph.getDifficulty(10, toByteVector(500) );
        EXPECT_EQ(540, fromByteVector(answer));

        answer = ph.getDifficulty(70, toByteVector(5) );
        EXPECT_EQ(4, fromByteVector(answer));
    }

    TEST(PoWHelper, DifficultyTarget) {
        PoWHelper ph;

        std::vector<uint8_t> difficulty;

        difficulty = ph.getDifficulty(30, toByteVector(1000000) );
        auto target = ph.getTarget(difficulty);

        EXPECT_EQ(1048828, fromByteVector(difficulty));

        difficulty = ph.getDifficulty(40, toByteVector(1000000) );
        EXPECT_EQ(1032226, fromByteVector(difficulty));

        difficulty = ph.getDifficulty(55, toByteVector(1000000) );
        EXPECT_EQ(1007812, fromByteVector(difficulty));

        difficulty = ph.getDifficulty(60, toByteVector(1000000) );
        EXPECT_EQ(1000000, fromByteVector(difficulty));

        difficulty = ph.getDifficulty(70, toByteVector(1000000) );
        EXPECT_EQ(984375, fromByteVector(difficulty));

        difficulty = ph.getDifficulty(80, toByteVector(1000000) );
        EXPECT_EQ(967774, fromByteVector(difficulty));

        difficulty = ph.getDifficulty(90, toByteVector(1000000) );
        EXPECT_EQ(951172, fromByteVector(difficulty));
    }

}
