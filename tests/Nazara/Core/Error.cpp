#include <Nazara/Core/Error.hpp>
#include <catch.hpp>

SCENARIO("Error", "[CORE][ERROR]")
{
	nzUInt32 oldFlags = NzError::GetFlags();

	GIVEN("Multiple errors")
	{
		WHEN("Calling to error")
		{
			THEN("These errors should be written in the log file")
			{
				NzError::Error(nzErrorType_Internal, "nzErrorType_Internal");
				NzError::Error(nzErrorType_Internal, "nzErrorType_Internal", 2, "Error.cpp", "2nd place Internal");
				REQUIRE("nzErrorType_Internal" == NzError::GetLastError());
				NzError::Error(nzErrorType_Normal, "nzErrorType_Normal");
				NzError::Error(nzErrorType_Normal, "nzErrorType_Normal", 2, "Error.cpp", "2nd place Normal");
				REQUIRE("nzErrorType_Normal" == NzError::GetLastError());
				NzError::Error(nzErrorType_Warning, "nzErrorType_Warning");
				NzError::Error(nzErrorType_Warning, "nzErrorType_Warning", 2, "Error.cpp", "2nd place Warning");
				REQUIRE("nzErrorType_Warning" == NzError::GetLastError());
			}
		}
	}

	NzError::SetFlags(oldFlags);
}
