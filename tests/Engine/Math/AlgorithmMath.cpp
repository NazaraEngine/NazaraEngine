#include <Nazara/Math/Algorithm.hpp>
#include <Catch/catch.hpp>
#include <limits>

TEST_CASE("Approach", "[MATH][ALGORITHM]")
{
	SECTION("Approach 8 with 5 by 2")
	{
		REQUIRE(Nz::Approach(5, 8, 2) == 7);
	}

	SECTION("Approach 5 with 8 by 2")
	{
		REQUIRE(Nz::Approach(8, 5, 2) == 6);
	}

	SECTION("Approach 8 with 8 by 2")
	{
		REQUIRE(Nz::Approach(8, 8, 2) == 8);
	}
}

TEST_CASE("Clamp", "[MATH][ALGORITHM]")
{
	SECTION("Clamp 8 between 5 and 10")
	{
		REQUIRE(Nz::Clamp(8, 5, 10) == 8);
	}

	SECTION("Clamp 4 between 5 and 10")
	{
		REQUIRE(Nz::Clamp(4, 5, 10) == 5);
	}

	SECTION("Clamp 12 between 5 and 10")
	{
		REQUIRE(Nz::Clamp(12, 5, 10) == 10);
	}
}

TEST_CASE("CountBits", "[MATH][ALGORITHM]")
{
	SECTION("Number 10 has 2 bits set to 1")
	{
		REQUIRE(Nz::CountBits(10) == 2);
	}

	SECTION("Number 0 has 0 bit set to 1")
	{
		REQUIRE(Nz::CountBits(0) == 0);
	}

	SECTION("Number 0xFFFFFFFF has 32 bit set to 1")
	{
		REQUIRE(Nz::CountBits(0xFFFFFFFF) == 32);
	}
}

TEST_CASE("DegreeToRadian", "[MATH][ALGORITHM]")
{
	SECTION("Convert 45.f degree to radian")
	{
		REQUIRE(Nz::DegreeToRadian(45.f) == Approx(M_PI / 4));
	}
}

TEST_CASE("GetNearestPowerOfTwo", "[MATH][ALGORITHM]")
{
	SECTION("Nearest power of two of 0 = 1")
	{
		REQUIRE(Nz::GetNearestPowerOfTwo(0) == 1);
	}

	SECTION("Nearest power of two of 16 = 16")
	{
		REQUIRE(Nz::GetNearestPowerOfTwo(16) == 16);
	}

	SECTION("Nearest power of two of 17 = 32")
	{
		REQUIRE(Nz::GetNearestPowerOfTwo(17) == 32);
	}
}

TEST_CASE("GetNumberLength", "[MATH][ALGORITHM]")
{
	SECTION("GetNumberLength of -127 signed char")
	{
		signed char minus127 = -127;
		REQUIRE(Nz::GetNumberLength(minus127) == 4);
	}

	SECTION("GetNumberLength of 255 unsigned char")
	{
		unsigned char plus255 = 255;
		REQUIRE(Nz::GetNumberLength(plus255) == 3);
	}

	SECTION("GetNumberLength of -1270 signed int")
	{
		signed int minus1270 = -1270;
		REQUIRE(Nz::GetNumberLength(minus1270) == 5);
	}

	SECTION("GetNumberLength of 2550 unsigned int")
	{
		unsigned int plus2550 = 2550;
		REQUIRE(Nz::GetNumberLength(plus2550) == 4);
	}

	SECTION("GetNumberLength of -1270 signed long long")
	{
		signed long long minus12700 = -12700;
		REQUIRE(Nz::GetNumberLength(minus12700) == 6);
	}

	SECTION("GetNumberLength of 2550 unsigned long long")
	{
		unsigned long long plus25500 = 25500;
		REQUIRE(Nz::GetNumberLength(plus25500) == 5);
	}

	SECTION("GetNumberLength of -2.456f float")
	{
		float minus2P456 = -2.456f;
		REQUIRE(Nz::GetNumberLength(minus2P456, 3) == 6);
	}

	SECTION("GetNumberLength of -2.456 double")
	{
		double minus2P456 = -2.456;
		REQUIRE(Nz::GetNumberLength(minus2P456, 3) == 6);
	}

	SECTION("GetNumberLength of -2.456 long double")
	{
		long double minus2P456 = -2.456L;
		REQUIRE(Nz::GetNumberLength(minus2P456, 3) == 6);
	}
}

TEST_CASE("IntegralLog2", "[MATH][ALGORITHM]")
{
	SECTION("According to implementation, log in base 2 of 0 = 0")
	{
		REQUIRE(Nz::IntegralLog2(0) == 0);
	}

	SECTION("Log in base 2 of 1 = 0")
	{
		REQUIRE(Nz::IntegralLog2(1) == 0);
	}

	SECTION("Log in base 2 of 4 = 2")
	{
		REQUIRE(Nz::IntegralLog2(4) == 2);
	}

	SECTION("Log in base 2 of 5 = 2")
	{
		REQUIRE(Nz::IntegralLog2(5) == 2);
	}
}

TEST_CASE("IntegralLog2Pot", "[MATH][ALGORITHM]")
{
	SECTION("According to implementation, log in base 2 of 0 = 0")
	{
		REQUIRE(Nz::IntegralLog2Pot(0) == 0);
	}

	SECTION("Log in base 2 of 1 = 0")
	{
		REQUIRE(Nz::IntegralLog2Pot(1) == 0);
	}

	SECTION("Log in base 2 of 4 = 2")
	{
		REQUIRE(Nz::IntegralLog2Pot(4) == 2);
	}
}

TEST_CASE("IntegralPow", "[MATH][ALGORITHM]")
{
	SECTION("2 to power 4")
	{
		REQUIRE(Nz::IntegralPow(2, 4) == 16);
	}
}

TEST_CASE("Lerp", "[MATH][ALGORITHM]")
{
	SECTION("Lerp 2 to 6 with 0.5")
	{
		REQUIRE(Nz::Lerp(2, 6, 0.5) == 4);
	}
}

TEST_CASE("MultiplyAdd", "[MATH][ALGORITHM]")
{
	SECTION("2 * 3 + 1")
	{
		REQUIRE(Nz::MultiplyAdd(2, 3, 1) == 7);
	}
}

TEST_CASE("NormalizeAngle", "[MATH][ALGORITHM]")
{
	SECTION("-90 should be normalized to +270")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(-90.f)) == Nz::FromDegrees(270.f));
	}

	SECTION("-540 should be normalized to +180")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(-540.f)) == Nz::FromDegrees(180.f));
	}

	SECTION("0 should remain 0")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(0.f)) == Nz::FromDegrees(0.f));
	}

	SECTION("90 should remain 90")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(90.f)) == Nz::FromDegrees(90.f));
	}

	SECTION("360 should be normalized to 0")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(360.f)) == Nz::FromDegrees(0.f));
	}

	SECTION("450 should be normalized to 90")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(450.f)) == Nz::FromDegrees(90.f));
	}

	SECTION("-90 should be normalized to +270")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(-90)) == Nz::FromDegrees(270));
	}

	SECTION("-540 should be normalized to +180")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(-540)) == Nz::FromDegrees(180));
	}

	SECTION("0 should remain 0")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(0)) == Nz::FromDegrees(0));
	}

	SECTION("90 should remain 90")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(90)) == Nz::FromDegrees(90));
	}

	SECTION("360 should be normalized to 0")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(360)) == Nz::FromDegrees(0));
	}

	SECTION("450 should be normalized to 90")
	{
		REQUIRE(Nz::NormalizeAngle(Nz::FromDegrees(450)) == Nz::FromDegrees(90));
	}
}

TEST_CASE("NumberEquals", "[MATH][ALGORITHM]")
{
	SECTION("2.35 and 2.351 should be the same at 0.01")
	{
		CHECK(Nz::NumberEquals(2.35, 2.35, 0.01));
	}

	SECTION("0 and 4 unsigned should be the same at 4")
	{
		CHECK(Nz::NumberEquals(0U, 4U, 4U));
	}

	SECTION("1 and -1 signed should be the same at 2")
	{
		CHECK(Nz::NumberEquals(1, -1, 2));
	}

	SECTION("Maximum integer and -1 should not be equal")
	{
		CHECK_FALSE(Nz::NumberEquals(std::numeric_limits<int>::max(), -1));
	}

	SECTION("Maximum integer and minimum integer should not be equal")
	{
		CHECK_FALSE(Nz::NumberEquals(std::numeric_limits<int>::max(), std::numeric_limits<int>::min()));
	}
}

TEST_CASE("NumberToString", "[MATH][ALGORITHM]")
{
	SECTION("0 to string")
	{
		REQUIRE(Nz::NumberToString(0) == "0");
	}

	SECTION("235 to string")
	{
		REQUIRE(Nz::NumberToString(235) == "235");
	}

	SECTION("-235 to string")
	{
		REQUIRE(Nz::NumberToString(-235) == "-235");
	}

	SECTION("16 in base 16 to string")
	{
		REQUIRE(Nz::NumberToString(16, 16) == "10");
	}
}

TEST_CASE("RadianToDegree", "[MATH][ALGORITHM]")
{
	SECTION("PI / 4 to degree")
	{
		REQUIRE(Nz::RadianToDegree(M_PI / 4) == Approx(45.f));
	}
}

TEST_CASE("StringToNumber", "[MATH][ALGORITHM]")
{
	SECTION("235 in string")
	{
		REQUIRE(Nz::StringToNumber("235") == 235);
	}

	SECTION("-235 in string")
	{
		REQUIRE(Nz::StringToNumber("-235") == -235);
	}

	SECTION("235 157 in string")
	{
		REQUIRE(Nz::StringToNumber("235 157") == 235157);
	}

	SECTION("16 in base 16 in string")
	{
		REQUIRE(Nz::StringToNumber("10", 16) == 16);
	}

	SECTION("8 in base 4 in string should not be valid")
	{
		bool ok = true;
		REQUIRE(Nz::StringToNumber("8", 4, &ok) == 0);
		REQUIRE(!ok);
	}
}
