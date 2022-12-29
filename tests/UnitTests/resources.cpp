#include <filesystem>

std::filesystem::path GetAssetDir()
{
	static std::filesystem::path resourceDir = []
	{
		std::filesystem::path dir = "assets";
		if (!std::filesystem::is_directory(dir) && std::filesystem::is_directory("../.." / dir))
			dir = "../.." / dir;

		return dir / "unittests";
	}();

	return resourceDir;
}
