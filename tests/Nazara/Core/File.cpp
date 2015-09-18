#include <Nazara/Core/File.hpp>
#include <Catch/catch.hpp>

SCENARIO("File", "[CORE][FILE]")
{
	GIVEN("One file")
	{
		WHEN("We create a new file")
		{
			NzFile file("Test File.txt", nzOpenMode_ReadWrite);
			REQUIRE(file.GetDirectory() == NzDirectory::GetCurrent() + NAZARA_DIRECTORY_SEPARATOR);
			CHECK(file.IsOpen());

			THEN("We are allowed to write 3 times 'Test String'")
			{
				const char message[12] = "Test String"; // 11 + '\0'
				NzByteArray byteArray(message, 11);
				file.Write("Test String");
				file.Write(byteArray);
				file.Write(message, sizeof(char), 11);
			}

			AND_THEN("We can retrieve 3 times 'Test String'")
			{
				char message[12];
				REQUIRE(file.Read(message, 11) == 11);
				message[11] = '\0';
				REQUIRE(NzString(message) == "Test String");

				REQUIRE(file.Read(message, sizeof(char), 11) == 11);
				message[11] = '\0';
				REQUIRE(NzString(message) == "Test String");
			}

			AND_THEN("We close it")
			{
				file.Close();
				REQUIRE(file.GetSize() == 33U);
				CHECK(!file.IsOpen());
			}
		}

		WHEN("We delete this file")
		{
			NzFile::Delete("Test File.txt");

			THEN("It doesn't exist anymore")
			{
				CHECK(!NzFile::Exists("Test File.txt"));
			}
		}
	}
}
