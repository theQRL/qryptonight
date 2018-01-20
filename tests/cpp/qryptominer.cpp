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

        EXPECT_FALSE(PoWHelper::passesTarget(target, target));

        // Iterate changing a single byte
        for (int i=0; i<32; i++)
        {
            std::vector<uint8_t> below_1 = target;
            below_1[i]--;

            EXPECT_TRUE(PoWHelper::passesTarget(below_1, target));

            std::vector<uint8_t> over_1 = target;
            over_1[i]++;
            EXPECT_FALSE(PoWHelper::passesTarget(over_1, target));
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
                0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xFF
        };

        qm.start(input, 0, boundary);

        sleep(3);

        ASSERT_TRUE(qm.solutionAvailable());

        EXPECT_EQ(24, qm.solutionNonce());
        std::cout << printByteVector(qm.solutionHash()) << std::endl;

        std::vector<uint8_t> expected_winner {
                0x00, 0x00, 0x00, 0x18, 0x19
        };

        std::vector<uint8_t> expected_hash {
                0x0e, 0xe1, 0xb7, 0x24, 0x60, 0xb9, 0x97, 0x88,
                0xd6, 0xbc, 0x5a, 0x74, 0x11, 0x08, 0xa5, 0xfa,
                0x8f, 0x56, 0xa7, 0x67, 0xd0, 0xdc, 0x72, 0xc7,
                0xba, 0xdc, 0x39, 0x93, 0x12, 0x3e, 0x71, 0x76
        };

        EXPECT_EQ(expected_winner, qm.solutionInput());
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
                0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xFF
        };

        qm.start(input, 0, boundary);
        qm.start(input, 0, boundary);
        sleep(3);

        ASSERT_TRUE(qm.solutionAvailable());
        EXPECT_EQ(24, qm.solutionNonce());

        std::vector<uint8_t> expected_winner {
                0x00, 0x00, 0x00, 0x18, 0x19
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
                0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xFF
        };

        qm.start(input, 0, target, 4);
        sleep(3);
        std::cout << std::endl << "hashes/sec: " << qm.hashRate() << std::endl;

        EXPECT_FALSE(qm.solutionAvailable());
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
                0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xFF
        };

        qm.start(input, 0, boundary);
        sleep(1);
        qm.cancel();

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
    }

}
