#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Catch/catch.hpp>

SCENARIO("File", "[CORE][FILE]")
{
	GIVEN("One file")
	{
		WHEN("We create a new file")
		{
			Nz::File file("Test File.txt", Nz::OpenMode_ReadWrite);
			REQUIRE(file.GetDirectory() == Nz::Directory::GetCurrent() + NAZARA_DIRECTORY_SEPARATOR);
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
				REQUIRE(Nz::String(message) == "Test String");

				REQUIRE(file.Read(message, 11) == 11);
				message[11] = '\0';
				REQUIRE(Nz::String(message) == "Test String");
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
			Nz::File::Delete("Test File.txt");

			THEN("It doesn't exist anymore")
			{
				CHECK(!Nz::File::Exists("Test File.txt"));
			}
		}
	}

	GIVEN("The test file")
	{
		REQUIRE(Nz::File::Exists("resources/Engine/Core/FileTest.txt"));

		Nz::File fileTest("resources/Engine/Core/FileTest.txt", Nz::OpenMode_ReadOnly | Nz::OpenMode_Text);

		WHEN("We read the first line of the file")
		{
			REQUIRE(fileTest.IsOpen());
			Nz::String content = fileTest.ReadLine();

			THEN("The content must be 'Test'")
			{
				REQUIRE(content == "Test");
			}
		}
	}

	GIVEN("Nothing")
	{
		WHEN("We get the absolute path of something containing relative path")
		{
			Nz::String containingDot = "/resources/Spaceship/./spaceship.mtl";
			Nz::String containingDoubleDot = "/resources/Spaceship/../Spaceship/spaceship.mtl";

			THEN("The relative positioning should disappear")
			{
				Nz::String containingNoMoreDot = Nz::File::NormalizePath("/resources/Spaceship/spaceship.mtl");
				REQUIRE(Nz::File::AbsolutePath(containingDot).EndsWith(containingNoMoreDot));
				REQUIRE(Nz::File::AbsolutePath(containingDoubleDot).EndsWith(containingNoMoreDot));
			}
		}
	}
}
