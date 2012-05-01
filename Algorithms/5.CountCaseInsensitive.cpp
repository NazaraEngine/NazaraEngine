#include <cstring>
#include <string>

inline char lower(char c)
{
	return ((c >= 65 && c <= 91) ? c+32 : c);
}

inline char upper(char c)
{
	return ((c >= 97 && c <= 123) ? c-32 : c);
}

int test1(const std::string& str, char character)
{
	char character_lower = lower(character);
	char character_upper = upper(character);
	unsigned int count = 0;
	const char* ptr = str.c_str();
	do
	{
		if (*ptr == character_lower || *ptr == character_upper)
			count++;
	}
	while (*ptr++);

	return count;
}

int test2(const std::string& str, char character)
{
	static char tosearch[3];
	char character_lower = lower(character);
	char character_upper = upper(character);
	tosearch[0] = character_lower;
	tosearch[1] = character_upper;
	tosearch[2] = '\0';

	const char* ptr = str.c_str();
	unsigned int count = 0;
	while (true)
	{
		ptr = std::strpbrk(ptr, tosearch);
		if (!ptr)
			break;

		count++;
		ptr++;
	}

	return count;
}

int test3(const std::string& str, char character)
{
	char character_lower = lower(character);
	unsigned int count = 0;
	const char* ptr = str.c_str();
	do
	{
		if (lower(*ptr) == character_lower)
			count++;
	}
	while (*ptr++);

	return count;
}
