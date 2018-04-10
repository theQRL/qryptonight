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
#include <qryptonight/qryptonight.h>
#include <misc/bignum.h>
#include "gtest/gtest.h"

namespace {
TEST(QryptoNight, Init) {
  Qryptonight qn;
  EXPECT_TRUE(qn.isValid());
}

TEST(QryptoNight, RunSingleHash) {
  Qryptonight qn;
  EXPECT_TRUE(qn.isValid());

  std::vector<uint8_t> input{
      0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
      0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
      0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
      0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
      0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
      0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
      0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
      0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09
  };

  std::vector<uint8_t> output_expected{
      0xd7, 0xf6, 0x86, 0xcc, 0xdf, 0xb4, 0xe8, 0x59,
      0xe1, 0x62, 0xf9, 0x6d, 0xdd, 0x6a, 0x3b, 0x75,
      0x79, 0xf2, 0x00, 0xf2, 0xf0, 0xe4, 0x26, 0xae,
      0x14, 0x32, 0x74, 0xbe, 0x06, 0x1a, 0x8c, 0xf0
  };

  auto output = qn.hash(input);

  EXPECT_EQ(output_expected, output);
}

TEST(QryptoNight, RunSingleHashBigBlob) {
  Qryptonight qn;
  EXPECT_TRUE(qn.isValid());

  std::vector<uint8_t> input(10000);

  std::vector<uint8_t> output_expected{
      0xbf, 0x2f, 0xa0, 0x5d, 0x59, 0x67, 0x60, 0xa8,
      0x43, 0x19, 0xd9, 0xe8, 0x97, 0x5e, 0x80, 0xcc,
      0xa6, 0x70, 0xdd, 0x9e, 0x19, 0xc8, 0x37, 0x3d,
      0xc9, 0x05, 0x49, 0xf2, 0xd3, 0x09, 0x5e, 0x0c
  };

  auto output = qn.hash(input);

  std::cout << printByteVector(output) << std::endl;

  EXPECT_EQ(output_expected, output);
}

}
