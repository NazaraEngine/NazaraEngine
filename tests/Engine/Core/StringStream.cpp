#include <Nazara/Core/StringStream.hpp>
#include <Catch/catch.hpp>

SCENARIO("StringStream", "[CORE][STRINGSTREAM]")
{
	GIVEN("A string stream")
	{
		Nz::StringStream stringStream("default");

		WHEN("We add bool and char")
		{
			stringStream << true;

			char valueCharSigned = 64;
			stringStream << valueCharSigned;
			unsigned char valueCharUnsigned = 64;
			stringStream << valueCharUnsigned;

			REQUIRE(stringStream.ToString() == "defaulttrue@@");
		}

		AND_WHEN("We add short and int")
		{
			short valueShortSigned = -3;
			stringStream << valueShortSigned;
			unsigned short valueShortUnsigned = 3;
			stringStream << valueShortUnsigned;

			int valueIntSigned = -3;
			stringStream << valueIntSigned;
			unsigned int valueIntUnsigned = 3;
			stringStream << valueIntUnsigned;

			REQUIRE(stringStream.ToString() == "default-33-33");
		}

		AND_WHEN("We add long and long long")
		{
			long valueLongSigned = -3;
			stringStream << valueLongSigned;
			unsigned long valueLongUnsigned = 3;
			stringStream << valueLongUnsigned;

			long long valueLongLongSigned = -3;
			stringStream << valueLongLongSigned;
			unsigned long long valueLongLongUnsigned = 3;
			stringStream << valueLongLongUnsigned;

			REQUIRE(stringStream.ToString() == "default-33-33");
		}

		AND_WHEN("We add round floating points")
		{
			stringStream << 3.f;
			stringStream << 3.0;
			stringStream << 3.0L;

			REQUIRE(stringStream.ToString() == "default3.0000003.0000003.000000");
		}

		AND_WHEN("We add floating points")
		{
			stringStream << 3.5f << ' ';
			stringStream << 3.65 << ' ';
			stringStream << 3.6478L;

			REQUIRE(stringStream.ToString() == "default3.500000 3.650000 3.647800");
		}

		AND_WHEN("We add string and pointer")
		{
			stringStream << "3";
			stringStream << std::string("3");
			stringStream << Nz::String("3");
			stringStream << static_cast<void*>(nullptr);

			REQUIRE(stringStream.ToString() == (Nz::String("default333") + Nz::String::Pointer(nullptr)));
		}
	}
}
