#include <cstring>
#include <string>

int test1(const std::string& str, const char* pattern)
{
	const char* strPtr = str.c_str();
	const char* ptr = &strPtr[str.size()-1];

	do
	{
		if (*ptr == pattern[0])
		{
			if (ptr != strPtr && !std::isspace(*(ptr-1)))
				continue;

			const char* p = &pattern[1];
			const char* tPtr = ptr+1;
			while (true)
			{
				if (*p == '\0')
				{
					if (*tPtr == '\0' || std::isspace(*tPtr))
						return ptr-strPtr;
					else
						break;
				}

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


int test2(const std::string& str, const std::string& pattern)
{
	const char* s = str.c_str();

	unsigned int size = pattern.size();
	const char* ptr = &s[str.size()-1];
	const char* limit = &s[size-1];

	do
	{
		if (*ptr == pattern[size-1])
		{
			if (*(ptr+1) != '\0' && !std::isspace(*(ptr+1)))
				continue;

			const char* p = &pattern[size-1];
			for (; p >= &pattern[0]; --p, --ptr)
			{
				if (*ptr != *p)
					break;

				if (p == &pattern[0])
				{
					if (ptr == s || std::isspace(*(ptr-1)))
						return ptr-s;
					else
						break;
				}


				if (ptr == s)
					return -1;
			}
		}
	}
	while (ptr-- > limit);

	return -1;
}
