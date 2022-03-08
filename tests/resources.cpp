#include <filesystem>

std::filesystem::path GetResourceDir()
{
	static std::filesystem::path resourceDir = []
	{
		std::filesystem::path dir = "resources";
		if (!std::filesystem::is_directory(dir) && std::filesystem::is_directory(".." / dir))
			return ".." / dir;
		else
			return dir;

	}();
	return resourceDir;
}
