#include <Nazara/Core/Error.hpp>
#include <Catch/catch.hpp>

SCENARIO("Error", "[CORE][ERROR]")
{
	Nz::UInt32 oldFlags = Nz::Error::GetFlags();

	GIVEN("Multiple errors")
	{
		WHEN("Calling to error")
		{
			THEN("These errors should be written in the log file")
			{
				Nz::Error::Trigger(Nz::ErrorType_Internal, "ErrorType_Internal");
				Nz::Error::Trigger(Nz::ErrorType_Internal, "ErrorType_Internal", 2, "Error.cpp", "2nd place Internal");
				REQUIRE("ErrorType_Internal" == Nz::Error::GetLastError());
				Nz::Error::Trigger(Nz::ErrorType_Normal, "ErrorType_Normal");
				Nz::Error::Trigger(Nz::ErrorType_Normal, "ErrorType_Normal", 2, "Error.cpp", "2nd place Normal");
				REQUIRE("ErrorType_Normal" == Nz::Error::GetLastError());
				Nz::Error::Trigger(Nz::ErrorType_Warning, "ErrorType_Warning");
				Nz::Error::Trigger(Nz::ErrorType_Warning, "ErrorType_Warning", 2, "Error.cpp", "2nd place Warning");
				REQUIRE("ErrorType_Warning" == Nz::Error::GetLastError());
			}
		}
	}

	Nz::Error::SetFlags(oldFlags);
}
