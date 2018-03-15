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

#include "qryptonightpool.h"

QryptonightPool::ReturnToPoolDeleter::ReturnToPoolDeleter(std::weak_ptr<QryptonightPool> pool)
	: _pool(pool) { }

void QryptonightPool::ReturnToPoolDeleter::operator()(Qryptonight* ptrToReleasedObject)
{
	if (auto pool = _pool.lock())
	{
		// the pool still exists so attempt to return it back to the pool
		try
		{
			pool->add(uniqueQryptonightPtr{ptrToReleasedObject, ReturnToPoolDeleter(pool)});
			return;
		}
		catch(const std::bad_alloc&) { }
	}
	// the pool is gone or cannot be added back so delete the object
	delete ptrToReleasedObject;
}

void QryptonightPool::ReturnToPoolDeleter::detachFromPool()
{
	_pool.reset();
}

QryptonightPool::QryptonightPool()
	: _mutex()
	, _hashers() { }

QryptonightPool::~QryptonightPool()
{
	std::unique_lock<std::mutex> lock(_mutex);
	while (!_hashers.empty())
	{
		_hashers.top().get_deleter().detachFromPool();
		_hashers.pop();
	}
}

QryptonightPool::uniqueQryptonightPtr QryptonightPool::acquire()
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_hashers.empty())
	{
		return uniqueQryptonightPtr{new Qryptonight(), ReturnToPoolDeleter(shared_from_this())};
	}
	else
	{
		auto ptr = std::move(_hashers.top());
		_hashers.pop();
		return ptr;
	}
}

void QryptonightPool::add(QryptonightPool::uniqueQryptonightPtr ptr)
{
	std::unique_lock<std::mutex> lock(_mutex);
	_hashers.push(std::move(ptr));
}

bool QryptonightPool::empty() const
{
	std::unique_lock<std::mutex> lock(_mutex);
	return _hashers.empty();
}

size_t QryptonightPool::size() const
{
	std::unique_lock<std::mutex> lock(_mutex);
	return _hashers.size();
}