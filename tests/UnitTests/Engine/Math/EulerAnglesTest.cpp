#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("EulerAngles", "[MATH][EULERANGLES]")
{
	GIVEN("Two zero euler angles")
	{
		Nz::EulerAnglesf firstZero(0.f, 0.f, 0.f);
		Nz::EulerAnglesf secondZero(Nz::EulerAngles<int>::Zero());

		THEN("They should be equal")
		{
			REQUIRE(firstZero == secondZero);
		}

		WHEN("We do some operations")
		{
			Nz::EulerAnglesf euler90(Nz::DegreeAnglef(90.f), Nz::DegreeAnglef(90.f), Nz::DegreeAnglef(90.f));
			Nz::EulerAnglesf euler270(Nz::DegreeAnglef(270.f), Nz::DegreeAnglef(270.f), Nz::DegreeAnglef(270.f));

			Nz::EulerAnglesf euler360 = euler90 + euler270;
			euler360.Normalize();
			Nz::EulerAnglesf euler0 = euler270 - euler90;
			euler0 -= euler90;
			euler0 -= euler90;

			THEN("They should still be equal")
			{
				CHECK(euler360 == firstZero);
				CHECK(euler0 == secondZero);
			}
		}

		WHEN("We ask for conversion to quaternion")
		{
			THEN("They are the same")
			{
				CHECK(firstZero.ToQuaternion() == secondZero.ToQuaternion());
				CHECK(firstZero.ToQuaternion() == Nz::EulerAnglesf(Nz::Quaternionf(1.f, 0.f, 0.f, 0.f)));
				CHECK(secondZero.ToQuaternion() == Nz::EulerAnglesf(Nz::Quaternionf(1.f, 0.f, 0.f, 0.f)));
			}
		}
	}

	GIVEN("Three rotation of 90 on each axis")
	{
		Nz::EulerAnglesf euler90P(Nz::DegreeAnglef(90.f), 0.f, 0.f);
		Nz::EulerAnglesf euler90Y(0.f, Nz::DegreeAnglef(90.f), 0.f);
		Nz::EulerAnglesf euler90R(0.f, 0.f, Nz::DegreeAnglef(90.f));

		WHEN("We transform the axis")
		{
			THEN("This is supposed to be left-handed")
			{
				Nz::Vector3f rotation90P = euler90P.ToQuaternion() * Nz::Vector3f::UnitY();
				Nz::Vector3f rotation90Y = euler90Y.ToQuaternion() * Nz::Vector3f::UnitZ();
				Nz::Vector3f rotation90R = euler90R.ToQuaternion() * Nz::Vector3f::UnitX();

				CHECK(rotation90P == Nz::Vector3f::UnitZ());
				CHECK(rotation90Y == Nz::Vector3f::UnitX());
				CHECK(rotation90R == Nz::Vector3f::UnitY());
			}
		}
	}

	GIVEN("Euler angles with rotation 45 on each axis")
	{
		WHEN("We convert to quaternion")
		{
			THEN("These results are expected")
			{
				CHECK(Nz::EulerAnglesf(Nz::DegreeAnglef(45.f), 0.f, 0.f) == Nz::EulerAnglesf(Nz::Quaternionf(0.923879504204f, 0.382683455944f, 0.f, 0.f).ToEulerAngles()));
				CHECK(Nz::EulerAnglesf(0.f, Nz::DegreeAnglef(45.f), 0.f) == Nz::EulerAnglesf(Nz::Quaternionf(0.923879504204f, 0.f, 0.382683455944f, 0.f).ToEulerAngles()));
				CHECK(Nz::EulerAnglesf(0.f, 0.f, Nz::DegreeAnglef(45.f)) == Nz::EulerAnglesf(Nz::Quaternionf(0.923879504204f, 0.f, 0.f, 0.382683455944f).ToEulerAngles()));
			}
		}
	}

	GIVEN("Three euler angles: (0, 22.5, 22.5), (90, 90, 0) and (30, 0, 30)")
	{
		Nz::EulerAnglesf euler45(Nz::DegreeAnglef(0.f), Nz::DegreeAnglef(22.5f), Nz::DegreeAnglef(22.5f));
		Nz::EulerAnglesf euler90(Nz::DegreeAnglef(90.f), Nz::DegreeAnglef(90.f), Nz::DegreeAnglef(0.f));
		Nz::EulerAnglesf euler30(Nz::DegreeAnglef(30.f), Nz::DegreeAnglef(0.f), Nz::DegreeAnglef(30.f));

		WHEN("We convert them to quaternion")
		{
			THEN("And then convert to euler angles, we have identity")
			{
				Nz::EulerAnglesf tmp = Nz::Quaternionf(euler45.ToQuaternion()).ToEulerAngles();
				CHECK(tmp.pitch.ToDegrees() == Catch::Approx(0.f));
				CHECK(tmp.yaw.ToDegrees() == Catch::Approx(22.5f));
				CHECK(tmp.roll.ToDegrees() == Catch::Approx(22.5f));

				tmp = Nz::Quaternionf(euler90.ToQuaternion()).ToEulerAngles();
				CHECK(tmp.pitch.ToDegrees() == Catch::Approx(90.f));
				CHECK(tmp.yaw.ToDegrees() == Catch::Approx(90.f));
				CHECK(tmp.roll.ToDegrees() == Catch::Approx(0.f));

				tmp = Nz::Quaternionf(euler30.ToQuaternion()).ToEulerAngles();
				CHECK(tmp.pitch.ToDegrees() == Catch::Approx(30.f));
				CHECK(tmp.yaw.ToDegrees() == Catch::Approx(0.f).margin(0.0001f));
				CHECK(tmp.roll.ToDegrees() == Catch::Approx(30.f));
			}
		}
	}

	GIVEN("An angle of 45 degrees")
	{
		Nz::DegreeAnglef angle(45.f);

		WHEN("We convert it to Euler angles")
		{
			Nz::EulerAnglesf eulerAngles(angle);

			THEN("It should be equal to a 2D rotation of 45 degrees")
			{
				CHECK(eulerAngles == Nz::EulerAnglesf(0.f, 0.f, 45.f));
			}
		}
	}
}
