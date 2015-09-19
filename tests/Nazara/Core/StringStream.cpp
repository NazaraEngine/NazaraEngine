#include <Nazara/Core/StringStream.hpp>
#include <catch.hpp>

SCENARIO("StringStream", "[CORE][STRINGSTREAM]")
{
	GIVEN("A string stream")
	{
		NzStringStream stringstream("default");

		WHEN("We add bool and char")
		{
			stringstream << true;

			char valueCharSigned = 64;
			stringstream << valueCharSigned;
			unsigned char valueCharUnsigned = 64;
			stringstream << valueCharUnsigned;

			REQUIRE(stringstream.ToString() == "defaulttrue@@");
		}

		AND_WHEN("We add short and int")
		{
			short valueShortSigned = -3;
			stringstream << valueShortSigned;
			unsigned short valueShortUnsigned = 3;
			stringstream << valueShortUnsigned;

			int valueIntSigned = -3;
			stringstream << valueIntSigned;
			unsigned int valueIntUnsigned = 3;
			stringstream << valueIntUnsigned;

			REQUIRE(stringstream.ToString() == "default-33-33");
		}

		AND_WHEN("We add long and long long")
		{
			long valueLongSigned = -3;
			stringstream << valueLongSigned;
			unsigned long valueLongUnsigned = 3;
			stringstream << valueLongUnsigned;

			long long valueLongLongSigned = -3;
			stringstream << valueLongLongSigned;
			unsigned long long valueLongLongUnsigned = 3;
			stringstream << valueLongLongUnsigned;

			REQUIRE(stringstream.ToString() == "default-33-33");
		}

		AND_WHEN("We add floating points")
		{
			stringstream << 3.f;
			stringstream << 3.0;
			stringstream << 3.0L;

			REQUIRE(stringstream.ToString() == "default333");
		}

		AND_WHEN("We add string and pointer")
		{
			stringstream << "3";
			stringstream << std::string("3");
			stringstream << NzString("3");
			stringstream << static_cast<void*>(nullptr);

			REQUIRE(stringstream.ToString() == (NzString("default333") + NzString::Pointer(nullptr)));
		}
	}
}
