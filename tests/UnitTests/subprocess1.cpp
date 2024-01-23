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
		Nz::File file(Nz::Utf8Path("step1_success.txt"), Nz::OpenMode::Write);
		file.Write("1");
	}

	for (int i = 1; i < argc; ++i)
	{
		Nz::File file(Nz::Utf8Path(Nz::Format("step2_param{}.txt", i)), Nz::OpenMode::Write);
		file.Write(argv[i]);
	}

	std::vector<std::string> parameters;
	parameters.push_back(std::to_string(Nz::Process::GetCurrentPid()));

	Nz::Result result = Nz::Process::SpawnDetached("../UnitTests_sub2", parameters);
	if (result)
	{
		Nz::File file(Nz::Utf8Path("step3_pid.txt"), Nz::OpenMode::Write);
		file.Write(std::to_string(result.GetValue()));
	}
	else
	{
		Nz::File file(Nz::Utf8Path("step3_failure.txt"), Nz::OpenMode::Write);
		file.Write(result.GetError());
	}

	// Wait for our child process to start and check for our existence
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
