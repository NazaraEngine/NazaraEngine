#include <Nazara/Core/MemoryView.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstring>

SCENARIO("Buffering", "[CORE][BUFFERING]")
{
	const char mem[] = "abcdefghijklmnopqrstuvwxyz";

	for (std::size_t bufferSize : { 1, 2, 3, 4, 6, 7, 0xFFFF })
	{
		Nz::MemoryView memView(mem, sizeof(mem));
		memView.EnableBuffering(true, bufferSize);

		WHEN("Using a buffer size of " + std::to_string(bufferSize))
		{
			CHECK(memView.GetCursorPos() == 0);

			std::vector<char> readBuffer;

			WHEN("Reading the full buffer")
			{
				readBuffer.resize(sizeof(mem));
				REQUIRE(memView.Read(readBuffer.data(), readBuffer.size()) == readBuffer.size());
				CHECK(std::memcmp(readBuffer.data(), mem, readBuffer.size()) == 0);
				CHECK(memView.GetCursorPos() == sizeof(mem));
			}

			WHEN("Reading 2 bytes")
			{
				readBuffer.resize(2);
				REQUIRE(memView.Read(readBuffer.data(), readBuffer.size()) == readBuffer.size());
				CHECK(std::memcmp(readBuffer.data(), "ab", readBuffer.size()) == 0);
				CHECK(memView.GetCursorPos() == 2);

				REQUIRE(memView.Read(readBuffer.data(), readBuffer.size()) == readBuffer.size());
				CHECK(std::memcmp(readBuffer.data(), "cd", readBuffer.size()) == 0);
				CHECK(memView.GetCursorPos() == 4);

				WHEN("Reading 4 bytes and reading")
				{
					readBuffer.resize(4);
					REQUIRE(memView.Read(readBuffer.data(), readBuffer.size()) == readBuffer.size());
					CHECK(std::memcmp(readBuffer.data(), "efgh", readBuffer.size()) == 0);
					CHECK(memView.GetCursorPos() == 8);

					AND_WHEN("Seeking at 10")
					{
						memView.SetCursorPos(10);
						CHECK(memView.GetCursorPos() == 10);

						readBuffer.resize(2);
						REQUIRE(memView.Read(readBuffer.data(), readBuffer.size()) == readBuffer.size());
						CHECK(std::memcmp(readBuffer.data(), "kl", readBuffer.size()) == 0);
						CHECK(memView.GetCursorPos() == 12);
					}

					AND_WHEN("Seeking at 6 and reading")
					{
						memView.SetCursorPos(6);
						CHECK(memView.GetCursorPos() == 6);

						readBuffer.resize(2);
						REQUIRE(memView.Read(readBuffer.data(), readBuffer.size()) == readBuffer.size());
						CHECK(std::memcmp(readBuffer.data(), "ghij", readBuffer.size()) == 0);
						CHECK(memView.GetCursorPos() == 8);
					}
				}
			}

			WHEN("Reading 6 then 2 bytes")
			{
				readBuffer.resize(6);
				REQUIRE(memView.Read(readBuffer.data(), readBuffer.size()) == readBuffer.size());
				CHECK(std::memcmp(readBuffer.data(), "abcdef", readBuffer.size()) == 0);
				CHECK(memView.GetCursorPos() == 6);

				readBuffer.resize(2);
				REQUIRE(memView.Read(readBuffer.data(), readBuffer.size()) == readBuffer.size());
				CHECK(std::memcmp(readBuffer.data(), "gh", readBuffer.size()) == 0);
				CHECK(memView.GetCursorPos() == 8);
			}
		}
	}
}
