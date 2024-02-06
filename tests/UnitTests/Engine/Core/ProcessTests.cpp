#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Format.hpp>
#include <Nazara/Core/Process.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <array>
#include <charconv>
#include <chrono>
#include <string>
#include <thread>
#include <catch2/catch_test_macros.hpp>

SCENARIO("Process", "[CORE][PROCESS]")
{
	WHEN("Checking if the current process exists")
	{
		if (Nz::Result result = Nz::Process::Exists(Nz::Process::GetCurrentPid()))
			CHECK(result.GetValue());
		else
		{
			INFO(result.GetError());
			CHECK(false);
		}
	}

	WHEN("Creating a detached process")
	{
		std::array<std::string, 3> args = { "simple_arg", "space \\arg", "okay\n\\\\\"_wt\"f\\" };

		std::filesystem::path path = Nz::Utf8Path("ProcessTests");
		if (std::filesystem::exists(path))
			REQUIRE(std::filesystem::remove_all(path));

		REQUIRE(std::filesystem::create_directory(path));

		Nz::Result spawnResult = Nz::Process::SpawnDetached(Nz::Utf8Path("UnitTests_sub1"), args, path);
		if (!spawnResult)
		{
			INFO(spawnResult.GetError());
			REQUIRE(false);
		}

		std::this_thread::sleep_for(std::chrono::seconds(3));

		// Some files were written in the ProcessTests directory

		auto ReadFile = [&](std::string_view filename, std::string_view failureName = {})
		{
			std::filesystem::path filePath = path / Nz::Utf8Path(filename);

			auto fileContentOpt = Nz::File::ReadWhole(filePath);
			if (!fileContentOpt)
			{
				if (!failureName.empty())
				{
					fileContentOpt = Nz::File::ReadWhole(path / Nz::Utf8Path(failureName));
					if (fileContentOpt)
					{
						std::string_view error(reinterpret_cast<const char*>(fileContentOpt->data()), fileContentOpt->size());

						INFO(failureName << ": " << error);
						REQUIRE(false);
					}
					else
					{
						INFO(filename << " nor " << failureName << " exist");
						REQUIRE(false);
					}
				}
				else
				{
					INFO(filename << " doesn't exist");
					REQUIRE(false);
				}
			}

			return std::string(reinterpret_cast<const char*>(fileContentOpt->data()), fileContentOpt->size());
		};

		auto CheckStep = [&](std::string_view filename, std::string_view expected, std::string_view failureName = {})
		{
			std::string content = ReadFile(filename, failureName);
			INFO("checking " << filename << " content");
			REQUIRE(content == expected);
		};

		CheckStep("step1_success.txt", "1", "step1_failure.txt");

		// Parameter passing check
		for (std::size_t i = 0; i < args.size(); ++i)
			CheckStep(Nz::Format("step2_param{}.txt", i + 1), args[i]);
		CHECK(!std::filesystem::exists(path / Nz::Utf8Path(Nz::Format("step2_param{}.txt", args.size() + 1))));

		std::string pidStr = ReadFile("step3_pid.txt", "step3_failure.txt");
		Nz::Pid pid;
		if (auto pidParse = std::from_chars(pidStr.data(), pidStr.data() + pidStr.size(), pid); pidParse.ec == std::errc())
		{
			Nz::Result result = Nz::Process::Exists(pid);
			if (result)
			{
				INFO("checking that grand-child process still exists");
				CHECK(result.GetValue());
			}
			else
			{
				INFO("failed to retrieve grand-child status: " << result.GetError());
				CHECK(false);
			}
		}
		else
		{
			INFO("failed to parse pid from step3_pid.txt: " << pidStr);
			CHECK(false);
		}

		CheckStep("step4_success.txt", "1", "step4_failure.txt");
		CheckStep("step5_success.txt", "1", "step5_failure.txt");
		CheckStep("step6_success.txt", "1", "step6_failure.txt");
	}
}
