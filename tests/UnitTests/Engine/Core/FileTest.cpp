#include <Nazara/Core/File.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

std::filesystem::path GetAssetDir();

SCENARIO("File", "[CORE][FILE]")
{
	GIVEN("One file")
	{
		WHEN("We create a new file")
		{
			Nz::File file("Test File.txt", Nz::OpenMode_ReadWrite);
			REQUIRE(file.GetDirectory() == std::filesystem::current_path());
			REQUIRE(file.IsOpen());

			THEN("We are allowed to write 3 times 'Test String'")
			{
				const char message[12] = "Test String"; // 11 + '\0'
				Nz::ByteArray byteArray(message, 11);
				file.Write("Test String");
				file.Write(byteArray);
				file.Write(message, 11);
			}

			AND_THEN("We can retrieve 3 times 'Test String'")
			{
				char message[12];
				REQUIRE(file.Read(message, 11) == 11);
				message[11] = '\0';
				REQUIRE(std::string(message) == "Test String");

				REQUIRE(file.Read(message, 11) == 11);
				message[11] = '\0';
				REQUIRE(std::string(message) == "Test String");
			}

			AND_THEN("We can get its size")
			{
				REQUIRE(file.GetSize() == 33U);
			}

			AND_THEN("We close it")
			{
				file.Close();
				CHECK(!file.IsOpen());
			}

			AND_THEN("Change its size")
			{
				file.SetSize(50U);
				REQUIRE(file.GetSize() == 50U);
			}
		}

		WHEN("We delete this file")
		{
			std::filesystem::remove("Test File.txt");

			THEN("It doesn't exist anymore")
			{
				CHECK(!std::filesystem::exists("Test File.txt"));
			}
		}
	}

	GIVEN("The test file")
	{
		REQUIRE(std::filesystem::exists(GetAssetDir() / "Core/FileTest.txt"));

		Nz::File fileTest(GetAssetDir() / "Core/FileTest.txt", Nz::OpenMode::Read | Nz::OpenMode::Text);

		WHEN("We read the first line of the file")
		{
			REQUIRE(fileTest.IsOpen());
			std::string content = fileTest.ReadLine();

			THEN("The content must be 'Test'")
			{
				REQUIRE(content == "Test");
			}
		}
	}
}
