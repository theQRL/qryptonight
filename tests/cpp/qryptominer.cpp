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
#include <misc/bignum.h>
#include <pow/powhelper.h>
#include "gtest/gtest.h"

namespace {
    TEST(Qryptominer, PassesTarget) {
        Qryptominer qm;

        std::vector<uint8_t> target {
            0x3E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xF6
        };

        ASSERT_FALSE(PoWHelper::passesTarget(target, target));

        std::cout << std::endl;
        std::cout << printByteVector2(target) << std::endl;

        // Iterate changing a single byte
        for (int i=0; i<32; i++)
        {
            std::vector<uint8_t> below_1 = target;
            below_1[31-i]--;

            std::cout << printByteVector2(below_1) << std::endl;
            ASSERT_TRUE(PoWHelper::passesTarget(below_1, target));

            std::vector<uint8_t> over_1 = target;
            over_1[31-i]++;
            std::cout << printByteVector2(over_1) << std::endl;
            ASSERT_FALSE(PoWHelper::passesTarget(over_1, target));
        }
    }

    TEST(Qryptominer, Run1Thread) {
        Qryptominer qm;

        std::vector<uint8_t> input {
            0x03, 0x05, 0x07, 0x09, 0x19
        };

        std::vector<uint8_t> boundary = {
                0x0F, 0xFF, 0xFF, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
                0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
                0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
                0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0x0F
        };

        qm.start(input, 0, boundary);

        std::this_thread::sleep_for(std::chrono::seconds(3));

        ASSERT_TRUE(qm.solutionAvailable());

        EXPECT_EQ(4, qm.solutionNonce());
        std::cout << printByteVector(qm.solutionHash()) << std::endl;

        std::vector<uint8_t> expected_winner {
                0x00, 0x00, 0x00, 0x04, 0x19
        };

        std::vector<uint8_t> expected_hash {
            0xfc, 0xef, 0x73, 0x7b, 0x00, 0x22, 0x91, 0x3a,
            0x3e, 0x85, 0xc3, 0x05, 0x69, 0x4c, 0x0b, 0xe6,
            0xf9, 0x65, 0xec, 0x0e, 0x17, 0x40, 0xe0, 0x9a,
            0x0a, 0xb5, 0xda, 0x34, 0x78, 0x0e, 0x7e, 0x02
        };

        EXPECT_EQ(expected_winner, qm.solutionInput());

        std::cout << printByteVector2(qm.solutionHash()) << std::endl;

        EXPECT_EQ(expected_hash, qm.solutionHash());
    }

    TEST(Qryptominer, RunAndRestart) {
        Qryptominer qm;

        std::vector<uint8_t> input {
                0x03, 0x05, 0x07, 0x09, 0x19
        };

        std::vector<uint8_t> boundary = {
                0x0F, 0xFF, 0xFF, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
                0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
                0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
                0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0x0F
        };

        qm.start(input, 0, boundary);
        qm.start(input, 0, boundary);
        std::this_thread::sleep_for(std::chrono::seconds(3));

        ASSERT_TRUE(qm.solutionAvailable());
        EXPECT_EQ(4, qm.solutionNonce());

        std::vector<uint8_t> expected_winner {
                0x00, 0x00, 0x00, 0x04, 0x19
        };

        EXPECT_EQ(expected_winner, qm.solutionInput());
    }

    TEST(Qryptominer, MeasureHashRate) {
        Qryptominer qm;

        std::vector<uint8_t> input {
                0x03, 0x05, 0x07, 0x09, 0x18
        };

        std::vector<uint8_t> target {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
                0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };

        qm.start(input, 0, target, 4);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cout << std::endl << "hashes/sec: " << qm.hashRate() << std::endl;

        EXPECT_FALSE(qm.solutionAvailable());
        qm.cancel();
        ASSERT_FALSE(qm.isRunning());
    }

    TEST(Qryptominer, RunAndCancel) {
        Qryptominer qm;

        std::vector<uint8_t> input {
                0x03, 0x05, 0x07, 0x09, 0x19
        };

        std::vector<uint8_t> boundary = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
                0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };

        qm.start(input, 0, boundary);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        qm.cancel();
        ASSERT_FALSE(qm.isRunning());

        ASSERT_FALSE(qm.solutionAvailable());
    }

    TEST(Qryptominer, RunCancelSafety) {
        Qryptominer qm;

        std::vector<uint8_t> input {
                0x03, 0x05, 0x07, 0x09, 0x19
        };

        std::vector<uint8_t> boundary = {
                0x9F, 0xFF, 0xFF, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
                0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
                0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
                0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xFF
        };

        for(int i=0; i<20; i++)
        {
            using namespace std::chrono_literals;
            boundary[0]-=10;
            qm.start(input, 0, boundary);
            std::this_thread::sleep_for(500ms);
            qm.cancel();
        }

        ASSERT_FALSE(qm.isRunning());
    }

}
