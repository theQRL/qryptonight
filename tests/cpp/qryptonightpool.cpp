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
#include <qryptonight/qryptonightpool.h>
#include "gtest/gtest.h"

namespace {

    class QryptonightWithRefCount : public Qryptonight
    {
    public:
        QryptonightWithRefCount() : Qryptonight() { ++_instances; }
        virtual ~QryptonightWithRefCount() { --_instances; }
        static size_t _instances;
    };

    size_t QryptonightWithRefCount::_instances = 0;

    QryptonightPool::QryptonightFactory factory =
        [](){ return new QryptonightWithRefCount(); };

    void ValidateHash(QryptonightPool::uniqueQryptonightPtr& qn)
    {
        EXPECT_TRUE(qn->isValid());

        std::vector<uint8_t> input {
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09,
            0x03, 0x05, 0x07, 0x09, 0x03, 0x05, 0x07, 0x09
        };

        std::vector<uint8_t> output_expected {
            0xd7, 0xf6, 0x86, 0xcc, 0xdf, 0xb4, 0xe8, 0x59,
            0xe1, 0x62, 0xf9, 0x6d, 0xdd, 0x6a, 0x3b, 0x75,
            0x79, 0xf2, 0x00, 0xf2, 0xf0, 0xe4, 0x26, 0xae,
            0x14, 0x32, 0x74, 0xbe, 0x06, 0x1a, 0x8c, 0xf0
        };

        auto output = qn->hash(input);

        EXPECT_EQ(output_expected, output);
    }

    TEST(QryptoNightPool, Init) {
        auto pool = std::make_shared<QryptonightPool>(factory);
        EXPECT_TRUE(pool->empty());
        EXPECT_EQ(pool->size(), 0);
        EXPECT_EQ(QryptonightWithRefCount::_instances, 0);
    }

    TEST(QryptoNightPool, Empty) {
        auto pool = std::make_shared<QryptonightPool>(factory);

        auto qn = pool->acquire();
        EXPECT_TRUE(pool->empty());
        EXPECT_EQ(QryptonightWithRefCount::_instances, 1);

        qn.reset();
        EXPECT_FALSE(pool->empty());
        EXPECT_EQ(QryptonightWithRefCount::_instances, 1);
    }

    TEST(QryptoNightPool, AcquireHashReleaseCycle) {
        auto pool = std::make_shared<QryptonightPool>(factory);

        auto qn = pool->acquire();
        EXPECT_EQ(pool->size(), 0);
        EXPECT_EQ(QryptonightWithRefCount::_instances, 1);

        ValidateHash(qn);

        qn.reset();
        EXPECT_EQ(pool->size(), 1);
        EXPECT_EQ(QryptonightWithRefCount::_instances, 1);

        qn = pool->acquire();
        EXPECT_EQ(pool->size(), 0);
        EXPECT_EQ(QryptonightWithRefCount::_instances, 1);

        ValidateHash(qn);

        qn.reset();
        EXPECT_EQ(pool->size(), 1);
        EXPECT_EQ(QryptonightWithRefCount::_instances, 1);

        pool.reset();
        EXPECT_EQ(QryptonightWithRefCount::_instances, 0);
    }

    TEST(QryptoNightPool, AcquireHashReleaseFour) {
        auto pool = std::make_shared<QryptonightPool>(factory);

        auto qn1 = pool->acquire();
        auto qn2 = pool->acquire();
        auto qn3 = pool->acquire();
        auto qn4 = pool->acquire();
        EXPECT_EQ(pool->size(), 0);
        EXPECT_EQ(QryptonightWithRefCount::_instances, 4);

        ValidateHash(qn1);
        ValidateHash(qn2);
        ValidateHash(qn3);
        ValidateHash(qn4);

        qn1.reset();
        qn2.reset();
        qn3.reset();
        qn4.reset();
        EXPECT_EQ(pool->size(), 4);
        EXPECT_EQ(QryptonightWithRefCount::_instances, 4);

        pool.reset();
        EXPECT_EQ(QryptonightWithRefCount::_instances, 0);
    }

    TEST(QryptoNightPool, AcquireAndDeletePool) {
        auto pool = std::make_shared<QryptonightPool>(factory);

        auto qn = pool->acquire();
        EXPECT_EQ(pool->size(), 0);
        EXPECT_EQ(QryptonightWithRefCount::_instances, 1);

        pool.reset();

        EXPECT_EQ(QryptonightWithRefCount::_instances, 1);

        ValidateHash(qn);

        qn.reset();

        EXPECT_EQ(QryptonightWithRefCount::_instances, 0);
    }

}
