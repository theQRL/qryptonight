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

#if !defined(__linux__) && !defined(__APPLE__)

#include <xmrstak/backend/cpu/crypto/cryptonight.h>
#include <xmrstak/backend/cpu/crypto/cryptonight_aesni.h>
#include <xmrstak/jconf.hpp>

#endif

#include <iostream>
#include "qryptonight.h"

#if defined(__linux__) || defined(__APPLE__)

#include "hash-ops.h"

#endif

Qryptonight::Qryptonight()
{
	#if !defined(__linux__) && !defined(__APPLE__)
	
    size_t init_res;
    init();

    // First try fast mem
    init_res = cryptonight_init(1, 1, &_last_msg);

    if (init_res)
    {
        // get context
        _context = cryptonight_alloc_ctx(1, 1, &_last_msg);
        if (_context!= nullptr)
            return;
    }

    // If something failed.. go for basic settings
    init_res = cryptonight_init(0, 1, &_last_msg);
    _context = cryptonight_alloc_ctx(0, 1, &_last_msg);
	
	#endif
}

std::atomic_bool Qryptonight::_jconf_initialized { false };

#if !defined(__linux__) && !defined(__APPLE__)

Qryptonight::cn_hash_fn Qryptonight::_hash_fn { NULL };

#endif

void Qryptonight::init()
{
	#if !defined(__linux__) && !defined(__APPLE__)
	
    static const cn_hash_fn hash_impls[] = {
        cryptonight_aesni_hash_kernel,
        cryptonight_hash_kernel
    };
    
    if (!_jconf_initialized)
    {
        _jconf_initialized = true;
        jconf::inst()->parse_config("", "");
	
	_hash_fn = hash_impls[jconf::inst()->HaveHardwareAes()];
    }
	
	#endif
}

Qryptonight::~Qryptonight()
{
	#if !defined(__linux__) && !defined(__APPLE__)
	
    if (_context!= nullptr)
    {
        cryptonight_free_ctx(_context);
    }
	
	#endif
}

std::vector<uint8_t> Qryptonight::hash(const std::vector<uint8_t>& input)
{
    std::vector<uint8_t> output(32);

    // cryptonight hash does not support less than 43 bytes
    const uint8_t minimum_input_size = 43;

    if (input.size()<minimum_input_size)
    {
        throw std::invalid_argument("input length should be > 42 bytes");
    }

	#if !defined(__linux__) && !defined(__APPLE__)
	
    _hash_fn(input.data(), input.size(),
	    output.data(),
	    _context);
		
	#else
		
	cn_slow_hash(input.data(), input.size(), (char*)output.data(), 1, 0, 0);
	
	#endif

    return output;
};

