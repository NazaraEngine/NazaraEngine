#include <Nazara/Math/Angle.hpp>
#include <Catch/catch.hpp>

SCENARIO("Angle", "[MATH][ANGLE]")
{
	GIVEN("A degree angle of 90deg")
	{
		Nz::DegreeAnglef angle(90.f);

		WHEN("We convert it to degrees")
		{
			Nz::DegreeAnglef copyAngle = angle.ToDegrees();

			THEN("It should compare to itself")
			{
				CHECK(angle == copyAngle);
			}
		}

		WHEN("We convert it to radians")
		{
			Nz::RadianAnglef radAngle(angle);

			THEN("It should be equal to pi/2")
			{
				Nz::RadianAnglef expectedResult(float(M_PI_2));

				CHECK(radAngle == expectedResult);
				CHECK(angle.ToRadians() == expectedResult);
			}
		}

		WHEN("We compute its sinus/cosinus separatly")
		{
			THEN("It should be equal to 1 and 0")
			{
				CHECK(angle.GetSin() == Approx(1.f).margin(0.0001f));
				CHECK(angle.GetCos() == Approx(0.f).margin(0.0001f));
			}

		}
		AND_WHEN("We compute it at the same time")
		{
			auto sincos = angle.GetSinCos();

			THEN("It should also be equal to 1 and 0")
			{
				CHECK(sincos.first == Approx(1.f).margin(0.0001f));
				CHECK(sincos.second == Approx(0.f).margin(0.0001f));
			}
		}
	}

	GIVEN("A degree angle of 480deg")
	{
		Nz::DegreeAnglef angle(480.f);

		WHEN("We normalize it")
		{
			angle.Normalize();

			THEN("It should be equal to a normalized version of itself")
			{
				Nz::DegreeAnglef expectedResult(120.f);

				CHECK(angle == expectedResult);
			}
		}
	}

	GIVEN("A degree angle of -300deg")
	{
		Nz::DegreeAnglef angle(-300.f);

		WHEN("We normalize it")
		{
			angle.Normalize();

			THEN("It should be equal to a normalized version of itself")
			{
				Nz::DegreeAnglef expectedResult(60.f);

				CHECK(angle == expectedResult);
			}
		}
	}

	GIVEN("A radian angle of -M_PI")
	{
		Nz::RadianAnglef angle(float(-M_PI));

		WHEN("We convert it to radians")
		{
			Nz::RadianAnglef copyAngle = angle.ToRadians();

			THEN("It should compare to itself")
			{
				CHECK(angle == copyAngle);
			}
		}

		WHEN("We convert it to degrees")
		{
			Nz::DegreeAnglef degAngle(angle);

			THEN("It should be equal to pi/2")
			{
				Nz::DegreeAnglef expectedResult(-180.f);

				CHECK(degAngle == expectedResult);
				CHECK(angle.ToDegrees() == expectedResult);
			}
		}

		WHEN("We compute its sinus/cosinus separatly")
		{
			THEN("It should be equal to 0 and -1")
			{
				CHECK(angle.GetSin() == Approx(0.f).margin(0.0001f));
				CHECK(angle.GetCos() == Approx(-1.f).margin(0.0001f));
			}

		}
		AND_WHEN("We compute it at the same time")
		{
			auto sincos = angle.GetSinCos();

			THEN("It should also be equal to 0 and -1")
			{
				CHECK(sincos.first == Approx(0.f).margin(0.0001f));
				CHECK(sincos.second == Approx(-1.f).margin(0.0001f));
			}
		}
	}

	GIVEN("A radian angle of 7pi")
	{
		Nz::RadianAnglef angle(float(7 * M_PI));

		WHEN("We normalize it")
		{
			angle.Normalize();

			THEN("It should be equal to a normalized version of itself")
			{
				Nz::RadianAnglef expectedResult(float(M_PI));

				CHECK(angle == expectedResult);
			}
		}
	}

	GIVEN("A radian angle of -4pi")
	{
		Nz::RadianAnglef angle(float(-4 * M_PI));

		WHEN("We normalize it")
		{
			angle.Normalize();

			THEN("It should be equal to a normalized version of itself")
			{
				Nz::RadianAnglef expectedResult(0.f);

				CHECK(angle == expectedResult);
			}
		}
	}
}
