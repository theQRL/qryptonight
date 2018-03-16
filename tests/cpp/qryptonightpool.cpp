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

    TEST(QryptoNightPool, AcquireAndRunSingleHash) {
        auto pool = std::make_shared<QryptonightPool>(factory);

        auto qn = pool->acquire();
        EXPECT_EQ(pool->size(), 0);
		EXPECT_EQ(QryptonightWithRefCount::_instances, 1);
        EXPECT_TRUE(qn->isValid());

        std::vector<uint8_t> input {
                0x03, 0x05, 0x07, 0x09
        };

        std::vector<uint8_t> output_expected {
                0x3E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
                0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
                0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
                0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xF6
        };

        auto output = qn->hash(input);

        EXPECT_EQ(output_expected, output);

		qn.reset();
        EXPECT_EQ(pool->size(), 1);
		EXPECT_EQ(QryptonightWithRefCount::_instances, 1);
		
		pool.reset();
		
		EXPECT_EQ(QryptonightWithRefCount::_instances, 0);
    }

    TEST(QryptoNightPool, AcquireAndRunSingleHashTwice) {
        auto pool = std::make_shared<QryptonightPool>(factory);

        auto qn = pool->acquire();
        EXPECT_EQ(pool->size(), 0);
		EXPECT_EQ(QryptonightWithRefCount::_instances, 1);
        EXPECT_TRUE(qn->isValid());

        std::vector<uint8_t> input {
                0x03, 0x05, 0x07, 0x09
        };

        std::vector<uint8_t> output_expected {
                0x3E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
                0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
                0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
                0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xF6
        };

        auto output = qn->hash(input);

        EXPECT_EQ(output_expected, output);

		qn.reset();
        EXPECT_EQ(pool->size(), 1);
		EXPECT_EQ(QryptonightWithRefCount::_instances, 1);

        qn = std::move(pool->acquire());
        EXPECT_EQ(pool->size(), 0);
		EXPECT_EQ(QryptonightWithRefCount::_instances, 1);
        EXPECT_TRUE(qn->isValid());

        output = qn->hash(input);

        EXPECT_EQ(output_expected, output);

		qn.reset();
        EXPECT_EQ(pool->size(), 1);
		EXPECT_EQ(QryptonightWithRefCount::_instances, 1);
		
		pool.reset();
		EXPECT_EQ(QryptonightWithRefCount::_instances, 0);
    }

    TEST(QryptoNightPool, AcquireAndDeletePool) {
        auto pool = std::make_shared<QryptonightPool>(factory);

        auto qn = pool->acquire();
        EXPECT_EQ(pool->size(), 0);
		EXPECT_EQ(QryptonightWithRefCount::_instances, 1);
        EXPECT_TRUE(qn->isValid());

		pool.reset();

		EXPECT_EQ(QryptonightWithRefCount::_instances, 1);

        std::vector<uint8_t> input {
                0x03, 0x05, 0x07, 0x09
        };

        std::vector<uint8_t> output_expected {
                0x3E, 0xE5, 0x3F, 0xE1, 0xAC, 0xF3, 0x55, 0x92,
                0x66, 0xD8, 0x43, 0x89, 0xCE, 0xDE, 0x99, 0x33,
                0xC6, 0x8F, 0xC5, 0x1E, 0xD0, 0xA6, 0xC7, 0x91,
                0xF8, 0xF9, 0xE8, 0x9D, 0xB6, 0x23, 0xF0, 0xF6
        };

        auto output = qn->hash(input);

        EXPECT_EQ(output_expected, output);

		qn.reset();
		
		EXPECT_EQ(QryptonightWithRefCount::_instances, 0);
    }

}
