#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("Angle", "[MATH][ANGLE]")
{
	GIVEN("A degree angle of 90deg")
	{
		Nz::DegreeAnglef angle(90.f);

		WHEN("We convert it to degrees")
		{
			Nz::DegreeAnglef copyAngle = angle.ToDegreeAngle();

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
				Nz::RadianAnglef expectedResult(Nz::HalfPi<float>);

				CHECK(radAngle == expectedResult);
				CHECK(angle.ToRadianAngle() == expectedResult);
			}
		}

		WHEN("We convert it to turns")
		{
			Nz::TurnAnglef turnAngle(angle);

			THEN("It should be equal to pi/2")
			{
				Nz::TurnAnglef expectedResult(1.f / 4.f);

				CHECK(turnAngle == expectedResult);
				CHECK(angle.ToTurnAngle() == expectedResult);
			}
		}

		WHEN("We compute its sinus/cosinus separatly")
		{
			THEN("It should be equal to 1 and 0")
			{
				CHECK(angle.GetSin() == Catch::Approx(1.f).margin(0.0001f));
				CHECK(angle.GetCos() == Catch::Approx(0.f).margin(0.0001f));
			}
			AND_WHEN("We compute sin/cos at the same time")
			{
				auto sincos = angle.GetSinCos();

				THEN("It should also be equal to 1 and 0")
				{
					CHECK(sincos.first == Catch::Approx(1.f).margin(0.0001f));
					CHECK(sincos.second == Catch::Approx(0.f).margin(0.0001f));
				}
			}
		}

		WHEN("We get the Euler Angles representation of this angle")
		{
			Nz::EulerAnglesf eulerAngles = angle;
			THEN("It should be equivalent to a 2D rotation by this angle")
			{
				CHECK(eulerAngles == Nz::EulerAnglesf(0.f, 0.f, 90.f));
			}
			AND_WHEN("We get the Quaternion representation of this angle")
			{
				Nz::Quaternionf quat = angle;

				THEN("It should be equivalent to a 2D rotation by this angle")
				{
					CHECK(quat.ApproxEqual(eulerAngles.ToQuaternion()));
				}
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

	GIVEN("A radian angle of -Pi")
	{
		Nz::RadianAnglef angle(-Nz::Pi<float>);

		WHEN("We convert it to radians")
		{
			Nz::RadianAnglef copyAngle = angle.ToRadianAngle();

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
				CHECK(angle.ToDegreeAngle() == expectedResult);
			}
		}

		WHEN("We compute its sinus/cosinus separatly")
		{
			THEN("It should be equal to 0 and -1")
			{
				CHECK(angle.GetSin() == Catch::Approx(0.f).margin(0.0001f));
				CHECK(angle.GetCos() == Catch::Approx(-1.f).margin(0.0001f));
			}

		}
		AND_WHEN("We compute it at the same time")
		{
			auto sincos = angle.GetSinCos();

			THEN("It should also be equal to 0 and -1")
			{
				CHECK(sincos.first == Catch::Approx(0.f).margin(0.0001f));
				CHECK(sincos.second == Catch::Approx(-1.f).margin(0.0001f));
			}
		}

		WHEN("We get the Euler Angles representation of this angle")
		{
			Nz::EulerAnglesf eulerAngles = angle;
			THEN("It should be equivalent to a 2D rotation by this angle")
			{
				CHECK(eulerAngles == Nz::EulerAnglesf(0.f, 0.f, -180.f));
			}
			AND_WHEN("We get the Quaternion representation of this angle")
			{
				Nz::Quaternionf quat = angle;

				THEN("It should be equivalent to a 2D rotation by this angle")
				{
					CHECK(quat.ApproxEqual(eulerAngles.ToQuaternion()));
				}
			}
		}
	}

	GIVEN("A radian angle of 7pi")
	{
		Nz::RadianAnglef angle(7.f * Nz::Pi<float>);

		WHEN("We normalize it")
		{
			angle.Normalize();

			THEN("It should be equal to a normalized version of itself")
			{
				Nz::RadianAnglef expectedResult(-Nz::Pi<float>);

				INFO(angle);
				CHECK(angle.ApproxEqual(expectedResult));
			}
		}
	}

	GIVEN("A radian angle of -4pi")
	{
		Nz::RadianAnglef angle(-4.f * Nz::Pi<float>);

		WHEN("We normalize it")
		{
			angle.Normalize();

			THEN("It should be equal to a normalized version of itself")
			{
				Nz::RadianAnglef expectedResult(0.f);

				INFO(angle);
				CHECK(angle.ApproxEqual(expectedResult));
			}
		}
	}

	GIVEN("A turn angle of 1.5f")
	{
		Nz::TurnAnglef angle(1.5f);

		WHEN("We normalize it")
		{
			angle.Normalize();

			THEN("It should be equal to a normalized version of itself")
			{
				Nz::TurnAnglef expectedResult(-0.5f);

				INFO(angle);
				CHECK(angle.ApproxEqual(expectedResult));
			}
		}
	}
}
