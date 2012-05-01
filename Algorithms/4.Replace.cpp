#include <cstring>
#include <string>

int test1(const std::string& str, const std::string& oldString, const std::string& replaceString)
{
	unsigned int count = 0;
	unsigned int newSize = str.size()+15; // Pour éviter le Count, on considère que la chaine finale aura quinze caractères de plus
	char* newString = new char[newSize+1];

	char* ptr = &newString[0];
	for (unsigned int i = 0; i < str.size(); ++i)
	{
		if (str.c_str()[i] == oldString[0] && std::memcmp(&str.c_str()[i], oldString.c_str(), oldString.size()*sizeof(char)) == 0)
		{
			std::memcpy(ptr, replaceString.c_str(), replaceString.size()*sizeof(char));
			ptr += replaceString.size();
			i += oldString.size()-1;
			++count;
		}
		else
			*ptr++ = str.c_str()[i];
	}

	newString[newSize] = '\0';

	delete[] newString;

	return count;
}

int test2(const std::string& str, const std::string& oldString, const std::string& replaceString)
{
	unsigned int count = 0;
	unsigned int newSize = str.size()+15; // Pour éviter le Count, on considère que la chaine finale aura quinze caractères de plus
	char* newString = new char[newSize+1];

	char* ptr = newString;
	const char* p = str.c_str();
	char* r = strstr(p, oldString.c_str());
	while (true)
	{
		std::memcpy(ptr, p, (r-p)*sizeof(char));
		ptr += r-p;
		std::memcpy(ptr, replaceString.c_str(), (replaceString.size())*sizeof(char));
		ptr += replaceString.size();
		p = r+oldString.size();

		count++;

		r = strstr(p+1, oldString.c_str());
		if (!r)
			break;
	}

	std::strcpy(ptr, p);

	delete[] newString;

	return count;
}