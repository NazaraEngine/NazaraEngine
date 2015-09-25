#include <Nazara/Core/Directory.hpp>
#include <Catch/catch.hpp>

SCENARIO("Directory", "[CORE][DIRECTORY]")
{
	GIVEN("The current directory")
	{
		Nz::Directory currentDirectory(Nz::Directory::GetCurrent());
		CHECK(currentDirectory.Exists());
		currentDirectory.Open();

		WHEN("We create a new directory Test Directory")
		{
			Nz::Directory::Create("Test Directory");

			THEN("A new directory has been created")
			{
				CHECK(Nz::Directory::Exists(currentDirectory.GetCurrent() + "/Test Directory"));
				CHECK(currentDirectory.IsOpen());
			}
		}

		AND_WHEN("We delete it")
		{
			Nz::Directory::Remove(currentDirectory.GetCurrent() + "/Test Directory", true);

			THEN("It doesn't exist anymore")
			{
				CHECK(!Nz::Directory::Exists(currentDirectory.GetCurrent() + "/Test Directory"));
			}
		}
	}
}

