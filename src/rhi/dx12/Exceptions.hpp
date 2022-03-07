
#pragma once

#include <stdexcept>
#include <Windows.h>

namespace rhi {

class COMException : public std::exception
{
public:
	COMException(HRESULT hr) noexcept : result(hr) {}

	const char* what() const override
	{
		static char s_str[64] = {};
		sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
		return s_str;
	}

private:
	HRESULT result;
};

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw COMException(hr);
	}
}

}