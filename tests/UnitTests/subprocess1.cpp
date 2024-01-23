#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Format.hpp>
#include <Nazara/Core/Process.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <chrono>
#include <string>
#include <thread>

int main(int argc, char* argv[])
{
	{
		Nz::File file(Nz::Utf8Path("step1_success.txt"), Nz::OpenMode::Write | Nz::OpenMode::Unbuffered);
		file.Write("1");
		file.Flush();
	}

	for (int i = 1; i < argc; ++i)
	{
		Nz::File file(Nz::Utf8Path(Nz::Format("step2_param{}.txt", i)), Nz::OpenMode::Write | Nz::OpenMode::Unbuffered);
		file.Write(argv[i]);
		file.Flush();
	}

	std::vector<std::string> parameters;
	parameters.push_back(std::to_string(Nz::Process::GetCurrentPid()));

	Nz::Result result = Nz::Process::SpawnDetached("../UnitTests_sub2", parameters);
	if (result)
	{
		Nz::File file(Nz::Utf8Path("step3_pid.txt"), Nz::OpenMode::Write | Nz::OpenMode::Unbuffered);
		file.Write(std::to_string(result.GetValue()));
		file.Flush();
	}
	else
	{
		Nz::File errFile(Nz::Utf8Path("step3_failure.txt"), Nz::OpenMode::Write | Nz::OpenMode::Unbuffered);
		errFile.Write(result.GetError());
		errFile.Flush();
	}

	// Wait for our child process to start and check for our existence
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
