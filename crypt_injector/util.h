#pragma once

namespace util
{
	static __forceinline bool is_valid_text(const char& x)
	{
		return true;
		return x == '~' || x == '_' || x == '-' || isalnum(x);
	}
}