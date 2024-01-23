#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Format.hpp>
#include <Nazara/Core/Process.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <charconv>
#include <chrono>
#include <cstring>
#include <string>
#include <thread>

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		Nz::File errFile(Nz::Utf8Path("step4_failure.txt"), Nz::OpenMode::Write);
		errFile.Write(Nz::Format("unexpected argc: {}", argc));

		return EXIT_FAILURE;
	}

	Nz::Pid pid;
	if (auto pidParse = std::from_chars(argv[1], argv[1] + std::strlen(argv[1]), pid); pidParse.ec != std::errc())
	{
		Nz::File errFile(Nz::Utf8Path("step4_failure.txt"), Nz::OpenMode::Write);
		errFile.Write(Nz::Format("invalid pid: {}", argv[1]));

		return EXIT_FAILURE;
	}

	Nz::File successFile4(Nz::Utf8Path("step4_success.txt"), Nz::OpenMode::Write);
	successFile4.Write("1");

	Nz::Result result = Nz::Process::Exists(pid);
	if (!result)
	{
		Nz::File errFile(Nz::Utf8Path("step5_failure.txt"), Nz::OpenMode::Write);
		errFile.Write(Nz::Format("failed to retrieve parent process status: {}", result.GetError()));

		return EXIT_FAILURE;
	}

	if (!result.GetValue())
	{
		Nz::File file(Nz::Utf8Path("step5_failure.txt"), Nz::OpenMode::Write);
		file.Write("parent process is already dead");

		return EXIT_FAILURE;
	}

	Nz::File successFile5(Nz::Utf8Path("step5_success.txt"), Nz::OpenMode::Write);
	successFile5.Write("1");

	// Wait until parent dies
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	result = Nz::Process::Exists(pid);
	if (!result)
	{
		Nz::File errFile(Nz::Utf8Path("step6_failure.txt"), Nz::OpenMode::Write);
		errFile.Write(Nz::Format("failed to retrieve parent process status after waiting: {}", result.GetError()));

		return EXIT_FAILURE;
	}

	if (result.GetValue())
	{
		Nz::File errFile(Nz::Utf8Path("step6_failure.txt"), Nz::OpenMode::Write);
		errFile.Write("parent process is still alive");

		return EXIT_FAILURE;
	}

	Nz::File successFile6(Nz::Utf8Path("step6_success.txt"), Nz::OpenMode::Write);
	successFile6.Write("1");

	// Wait for the unit tests process to check our existence
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}
