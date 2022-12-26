#include <Nazara/Core/Error.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("Error", "[CORE][ERROR]")
{
	Nz::ErrorModeFlags oldFlags = Nz::Error::GetFlags();

	GIVEN("Multiple errors")
	{
		WHEN("Calling to error")
		{
			THEN("These errors should be written in the log file")
			{
				Nz::Error::Trigger(Nz::ErrorType::Internal, "ErrorType::Internal");
				Nz::Error::Trigger(Nz::ErrorType::Internal, "ErrorType::Internal", 2, "Error.cpp", "2nd place Internal");
				REQUIRE("ErrorType::Internal" == Nz::Error::GetLastError());
				Nz::Error::Trigger(Nz::ErrorType::Normal, "ErrorType::Normal");
				Nz::Error::Trigger(Nz::ErrorType::Normal, "ErrorType::Normal", 2, "Error.cpp", "2nd place Normal");
				REQUIRE("ErrorType::Normal" == Nz::Error::GetLastError());
				Nz::Error::Trigger(Nz::ErrorType::Warning, "ErrorType::Warning");
				Nz::Error::Trigger(Nz::ErrorType::Warning, "ErrorType::Warning", 2, "Error.cpp", "2nd place Warning");
				REQUIRE("ErrorType::Warning" == Nz::Error::GetLastError());
			}
		}
	}

	Nz::Error::SetFlags(oldFlags);
}
