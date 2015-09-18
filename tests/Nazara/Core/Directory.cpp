#include <Nazara/Core/Directory.hpp>
#include <Catch/catch.hpp>

SCENARIO("Directory", "[CORE][DIRECTORY]")
{
	GIVEN("The current directory")
	{
		NzDirectory currentDirectory(NzDirectory::GetCurrent());
		CHECK(currentDirectory.Exists());
		currentDirectory.Open();

		WHEN("We create a new directory Test Directory")
		{
			NzDirectory::Create("Test Directory");

			THEN("A new directory has been created")
			{
				CHECK(NzDirectory::Exists(currentDirectory.GetCurrent() + "/Test Directory"));
				CHECK(currentDirectory.IsOpen());
			}
		}

		AND_WHEN("We delete it")
		{
			NzDirectory::Remove(currentDirectory.GetCurrent() + "/Test Directory", true);

			THEN("It doesn't exist anymore")
			{
				CHECK(!NzDirectory::Exists(currentDirectory.GetCurrent() + "/Test Directory"));
			}
		}
	}
}

