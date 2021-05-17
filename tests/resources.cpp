#include <filesystem>

std::filesystem::path GetResourceDir()
{
	static std::filesystem::path resourceDir = []
	{
		std::filesystem::path resourceDir = "resources";
		if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory(".." / resourceDir))
			return ".." / resourceDir;
		else
			return resourceDir;

	}();
	return resourceDir;
}
