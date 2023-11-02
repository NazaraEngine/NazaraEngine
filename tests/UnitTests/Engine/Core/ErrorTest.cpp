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
				Nz::Error::Trigger(Nz::ErrorType::Internal, Nz::Format("ErrorType::{0}", "Internal"));
				Nz::Error::Trigger(Nz::ErrorType::Internal, 2, "Error.cpp", "2nd place Internal", Nz::Format("ErrorType::{0}", "Internal"));
				REQUIRE("ErrorType::Internal" == Nz::Error::GetLastError());
				Nz::Error::Trigger(Nz::ErrorType::Normal, Nz::Format("ErrorType::{1}{0}", "mal", "Nor"));
				Nz::Error::Trigger(Nz::ErrorType::Normal, 2, "Error.cpp", "2nd place Normal", Nz::Format("ErrorType::{1}{0}", "mal", "Nor"));
				REQUIRE("ErrorType::Normal" == Nz::Error::GetLastError());
				Nz::Error::Trigger(Nz::ErrorType::Warning, "ErrorType::Warning");
				Nz::Error::Trigger(Nz::ErrorType::Warning, Nz::Format("ErrorType::Warning", 2, "Error.cpp", "2nd place Warning"));
				REQUIRE("ErrorType::Normal" == Nz::Error::GetLastError()); //< Warning are not captured by GetLastError
			}
		}
	}

	Nz::Error::SetFlags(oldFlags);
}
