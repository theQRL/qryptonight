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
#include "gtest/gtest.h"

namespace {
    TEST(Qryptominer, PassesTarget) {
        Qryptominer qm;

        std::vector<uint8_t> input {
            0x03, 0x05, 0x07, 0x09
        };

        std::vector<uint8_t> target {
            0x3E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xF6
        };

        qm.setInput(input, 0, target);

        EXPECT_FALSE(qm.passesTarget(target));

        // Iterate changing a single byte
        for (int i=0; i<32; i++)
        {
            std::vector<uint8_t> below_1 = target;
            below_1[i]--;

            EXPECT_TRUE(qm.passesTarget(below_1));

            std::vector<uint8_t> over_1 = target;
            over_1[i]++;
            EXPECT_FALSE(qm.passesTarget(over_1));
        }
    }

    TEST(Qryptominer, Run1Thread) {
        Qryptominer qm;

        std::vector<uint8_t> input {
            0x03, 0x05, 0x07, 0x09, 0x19
        };

        std::vector<uint8_t> target {
            0x1E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xFF
        };

        qm.setInput(input, 0, target);
        qm.start(1);
        sleep(2);

        EXPECT_TRUE(qm.solutionFound());
        EXPECT_EQ(7, qm.solutionNonce());

        std::vector<uint8_t> expected_winner {
                0x07, 0x00, 0x00, 0x00, 0x19
        };

        EXPECT_EQ(expected_winner, qm.solutionInput());
    }

    TEST(Qryptominer, SolutionRace) {
        Qryptominer qm;

        std::vector<uint8_t> input {
            0x03, 0x05, 0x07, 0x09, 0x18
        };

        std::vector<uint8_t> target {
            0x1E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
            0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
            0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
            0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xFF
        };

        qm.setInput(input, 0, target);
        qm.start(2);
        sleep(1);

        EXPECT_TRUE(qm.solutionFound());

        // There are two possible solutions that are found at the same time

        if (qm.solutionNonce()==4)
        {
            EXPECT_EQ(4, qm.solutionNonce());

            std::vector<uint8_t> expected_winner {
                    0x04, 0x00, 0x00, 0x00, 0x18
            };

            EXPECT_EQ(expected_winner, qm.solutionInput());
        }
        else
        {
            EXPECT_EQ(5, qm.solutionNonce());

            std::vector<uint8_t> expected_winner {
                    0x05, 0x00, 0x00, 0x00, 0x18
            };

            EXPECT_EQ(expected_winner, qm.solutionInput());
        }
    }
}
