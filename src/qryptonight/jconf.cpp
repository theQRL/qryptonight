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

#include "jconf.hpp"

#include "xmrstak/misc/console.hpp"
#include "xmrstak/misc/jext.hpp"

#include <cmath>
#include <vector>
#include <numeric>

#ifdef _WIN32
#define strcasecmp _stricmp
#include <intrin.h>
#else
#include <cpuid.h>
#endif


using namespace rapidjson;

/*
 * This enum needs to match index in oConfigValues, otherwise we will get a runtime error
 */
enum configEnum {
	aPoolList, bTlsSecureAlgo, sCurrency, iCallTimeout, iNetRetry, iGiveUpLimit, iVerboseLevel, bPrintMotd, iAutohashTime, 
	bFlushStdout, bDaemonMode, sOutputFile, iHttpdPort, sHttpLogin, sHttpPass, bPreferIpv4, bAesOverride, sUseSlowMem 
};

struct configVal {
	configEnum iName;
	const char* sName;
	Type iType;
};

// Same order as in configEnum, as per comment above
// kNullType means any type
configVal oConfigValues[] = {
	{ aPoolList, "pool_list", kArrayType },
	{ bTlsSecureAlgo, "tls_secure_algo", kTrueType },
	{ sCurrency, "currency", kStringType },
	{ iCallTimeout, "call_timeout", kNumberType },
	{ iNetRetry, "retry_time", kNumberType },
	{ iGiveUpLimit, "giveup_limit", kNumberType },
	{ iVerboseLevel, "verbose_level", kNumberType },
	{ bPrintMotd, "print_motd", kTrueType },
	{ iAutohashTime, "h_print_time", kNumberType },
	{ bFlushStdout, "flush_stdout", kTrueType},
	{ bDaemonMode, "daemon_mode", kTrueType },
	{ sOutputFile, "output_file", kStringType },
	{ iHttpdPort, "httpd_port", kNumberType },
	{ sHttpLogin, "http_login", kStringType },
	{ sHttpPass, "http_pass", kStringType },
	{ bPreferIpv4, "prefer_ipv4", kTrueType },
	{ bAesOverride, "aes_override", kNullType },
	{ sUseSlowMem, "use_slow_memory", kStringType }
};

constexpr size_t iConfigCnt = (sizeof(oConfigValues)/sizeof(oConfigValues[0]));

inline bool checkType(Type have, Type want)
{
	if(want == have)
		return true;
	else if(want == kNullType)
		return true;
	else if(want == kTrueType && have == kFalseType)
		return true;
	else if(want == kFalseType && have == kTrueType)
		return true;
	else
		return false;
}

struct jconf::opaque_private
{
	Document jsonDoc;
	const Value* configValues[iConfigCnt]{}; //Compile time constant

	opaque_private() = default;
};

jconf::jconf()
{
	prv = new opaque_private();
}

const std::string jconf::GetCurrency()
{
    return "monero";
}

bool jconf::IsCurrencyMonero()
{
    return true;
}

void jconf::cpuid(uint32_t eax, int32_t ecx, int32_t val[4])
{
	memset(val, 0, sizeof(int32_t)*4);

#ifdef _WIN32
	__cpuidex(val, eax, ecx);
#else
	__cpuid_count(eax, ecx, val[0], val[1], val[2], val[3]);
#endif
}

bool jconf::check_cpu_features()
{
	constexpr int AESNI_BIT = 1 << 25;
	constexpr int SSE2_BIT = 1 << 26;
	int32_t cpu_info[4];
	bool bHaveSse2;

	cpuid(1, 0, cpu_info);

	bHaveAes = (cpu_info[2] & AESNI_BIT) != 0;
	bHaveSse2 = (cpu_info[3] & SSE2_BIT) != 0;

	return bHaveSse2;
}

jconf::slow_mem_cfg jconf::GetSlowMemSetting()
{
	const char* opt = prv->configValues[sUseSlowMem]->GetString();

	if(strcasecmp(opt, "always") == 0)
		return always_use;
	else if(strcasecmp(opt, "no_mlck") == 0)
		return no_mlck;
	else if(strcasecmp(opt, "warn") == 0)
		return print_warning;
	else if(strcasecmp(opt, "never") == 0)
		return never_use;
	else
		return unknown_value;
}
