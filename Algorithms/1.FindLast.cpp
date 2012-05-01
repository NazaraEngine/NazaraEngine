#include <cstring>
#include <string>

int test1(const std::string& str, const char* pattern)
{
	const char* ch = &str[0];
	unsigned int lastResult = -1;
	while (true)
	{
		char* r = strstr(ch, pattern);
		if (!r)
			return lastResult;

		lastResult = r - &str[0];

		ch = r+1;
	}
}

int test2(const std::string& str, const char* pattern)
{
	const char* s = str.c_str();

	unsigned int size = std::strlen(pattern);
	const char* ptr = &s[str.size()-1];
	const char* limit = &s[size-1];

	while (true)
	{
		if (*ptr == pattern[size-1])
		{
			const char* p = &pattern[size-1];
			for (; p >= &pattern[0]; --p, --ptr)
			{
				if (*ptr != *p)
					break;

				if (p == &pattern[0])
					return ptr-s;

				if (ptr == s)
					return -1;
			}
		}
		else if (ptr-- <= limit)
			break;
	}

	return -1;
}

int test3(const std::string& str, const char* pattern)
{
	const char* strPtr = str.c_str();
	const char* ptr = &strPtr[str.size()-1];

	do
	{
		if (*ptr == pattern[0])
		{
			const char* p = &pattern[1];
			const char* tPtr = ptr+1;
			while (true)
			{
				if (*p == '\0')
					return ptr-strPtr;

				if (*tPtr != *p)
					break;

				p++;
				tPtr++;
			}
		}
	}
	while (ptr-- != strPtr);

	return -1;
}

int test4(const std::string& str, const std::string& pattern)
{
	const char* s = str.c_str();

	unsigned int size = pattern.size();
	const char* ptr = &s[str.size()-1];
	const char* limit = &s[size-1];

	while (true)
	{
		if (*ptr == pattern[size-1])
		{
			const char* p = &pattern[size-1];
			for (; p >= &pattern[0]; --p, --ptr)
			{
				if (*ptr != *p)
					break;

				if (p == &pattern[0])
					return ptr-s;

				if (ptr == s)
					return -1;
			}
		}
		else if (ptr-- <= limit)
			break;
	}

	return -1;
}

int test5(const std::string& str, const std::string& pattern)
{
	unsigned int size = pattern.size();
	const char* ptr = &str[str.size()];
	const char* limit = &str[size-1];

	const char* patternStr = pattern.c_str();
	while (ptr-- > limit)
	{
		if (*ptr == pattern[size-1])
		{
			if (static_cast<unsigned int>(&patternStr[str.size()-1]-ptr) < size)
				continue;

			if (std::memcmp(ptr-size+1, patternStr, size*sizeof(char)) == 0)
				return (ptr-size+1) - &str[0];
		}
	}

	return -1;
}
