#include <cstring>
#include <string>

int test1(const std::string& str, const char* pattern)
{
	const char* s = str.c_str();
	const char* ch = s;
	unsigned int size = std::strlen(pattern);

	while (true)
	{
		char* r = strstr(ch, pattern);
		if (!r)
			return -1;

		// Si le pattern est bien isolé
		if ((r == s || std::isspace(*(r-1))) && (*(r+size) == '\0' || std::isspace(*(r+size))))
			return r-s;

		ch = r+1;
	}
}

int test2(const std::string& str, const std::string& pattern)
{
	const char* s = str.c_str();
	const char* ch = s;
	unsigned int size = pattern.size();

	while (true)
	{
		char* r = strstr(ch, pattern.c_str());
		if (!r)
			return -1;

		// Si le pattern est bien isolé
		if ((r == s || std::isspace(*(r-1))) && (*(r+size) == '\0' || std::isspace(*(r+size))))
			return r-s;

		ch = r+1;
	}
}

int test3(const std::string& str, const char* pattern)
{
	const char* s = str.c_str();
	const char* ptr = s;

	do
	{
		if (*ptr == pattern[0])
		{
			if (ptr != s && !std::isspace(*(ptr-1)))
				continue;

			const char* p = &pattern[1];
			const char* tPtr = ptr+1;
			while (true)
			{
				if (*p == '\0')
				{
					if (*tPtr == '\0' || std::isspace(*tPtr))
						return ptr-s;
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
	while (*ptr++ != '\0');

	return -1;
}
