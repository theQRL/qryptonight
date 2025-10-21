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

#ifndef QRYPTONIGHT_QRYPTONIGHT_H
#define QRYPTONIGHT_QRYPTONIGHT_H

#include <vector>
#include <string>
#include <stdexcept>
#include <atomic>

#if defined(__linux__)

#include "hash-ops.h"

#else
	
#include <xmrstak/backend/cpu/crypto/cryptonight.h>

#endif

class Qryptonight {
public:
    Qryptonight();
    virtual ~Qryptonight();

    bool isValid() { 		
		#if defined(__linux__)
		return true;
		#else
		return _context != nullptr; 
		#endif
	}
	
	
    std::string lastError()	{ 
		#if defined(__linux__)
		    return "";
		#else
		    return std::string(_last_msg.warning ? _last_msg.warning : ""); 
		#endif
	}
	

    std::vector<uint8_t> hash(const std::vector<uint8_t>& input);

protected:
	#if !defined(__linux__)
    //Protected variables are prefixed with an underscore
    typedef void (*cn_hash_fn)(const void* input, size_t len, void* output, cryptonight_ctx* ctx0);
    static cn_hash_fn _hash_fn;
	#endif
    
    static void init();
    static std::atomic_bool _jconf_initialized;

	#if !defined(__linux__)
    alloc_msg _last_msg = { nullptr };
    cryptonight_ctx *_context;
	#endif
};

#endif //QRYPTONIGHT_QRYPTONIGHT_H


