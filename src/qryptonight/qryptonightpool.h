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

#ifndef QRYPTONIGHT_QRYPTONIGHTPOOL_H
#define QRYPTONIGHT_QRYPTONIGHTPOOL_H

#include "qryptonight.h"
#include <mutex>
#include <stack>
#include <memory>
#include <functional>

// An RAII-style object pool for memory-intensive Qryptonight objects
// Warning! This class is not swig-compatible but does not need to be
// exposed so make sure this class is not #included from a swig-included
// header file
class QryptonightPool : public std::enable_shared_from_this<QryptonightPool>
{
public:

    // a factory function to create new Qryptonight objects
    using QryptonightFactory = std::function<Qryptonight*()>;

    QryptonightPool(QryptonightFactory factory = [](){ return new Qryptonight(); });

    virtual ~QryptonightPool();

    // helper functor to return pointers back to the pool
    // or delete the pointer if the pool no longer exists
    class ReturnToPoolDeleter
    {
    public:
        explicit ReturnToPoolDeleter(std::weak_ptr<QryptonightPool> ptrToOwnerPool);
        void operator()(Qryptonight* ptrToReleasedObject);
        void detachFromPool();
    private:
        std::weak_ptr<QryptonightPool> _ptrToOwnerPool;
    };

    // a std::unique_ptr with a custome deleter that the client will use
    using uniqueQryptonightPtr = std::unique_ptr<Qryptonight, ReturnToPoolDeleter>;

    // obtain an unused Qryptonight instance from the pool or
    // create a new one if there are none available
    uniqueQryptonightPtr acquire();

    bool empty() const;

    size_t size() const;

protected:

    // return the Qryptonight instance back to the pool
    void add(uniqueQryptonightPtr ptr);

    // factory function to create the Qryptonight objects
    QryptonightFactory _factory;

    // allow mutually exclusive access to _poolContainer
    mutable std::mutex _mutex;

    // container for the unused Qryptonight instances
    std::stack<uniqueQryptonightPtr> _poolContainer;
};

#endif //QRYPTONIGHT_QRYPTONIGHTPOOL_H
