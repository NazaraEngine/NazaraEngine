#include <Nazara/Math/EulerAngles.hpp>
#include <Catch/catch.hpp>

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
			Nz::EulerAnglesf euler90(Nz::FromDegrees(90.f), Nz::FromDegrees(90.f), Nz::FromDegrees(90.f));
			Nz::EulerAnglesf euler270(Nz::FromDegrees(270.f), Nz::FromDegrees(270.f), Nz::FromDegrees(270.f));

			Nz::EulerAnglesf euler360 = euler90 + euler270;
			euler360.Normalize();
			Nz::EulerAnglesf euler0 = euler270 - euler90;
			euler0 -= euler90;
			euler0 -= euler90;

			THEN("They should still be equal")
			{
				REQUIRE(euler360 == firstZero);
				REQUIRE(euler0 == secondZero);
			}
		}

		WHEN("We ask for conversion to quaternion")
		{
			THEN("They are the same")
			{
				REQUIRE(firstZero.ToQuaternion() == secondZero.ToQuaternion());
				REQUIRE(firstZero.ToQuaternion() == Nz::EulerAnglesf(Nz::Quaternionf(1.f, 0.f, 0.f, 0.f)));
				REQUIRE(secondZero.ToQuaternion() == Nz::EulerAnglesf(Nz::Quaternionf(1.f, 0.f, 0.f, 0.f)));
			}
		}
	}

	GIVEN("Three rotation of 90 on each axis")
	{
		Nz::EulerAnglesf euler90P(Nz::FromDegrees(90.f), 0.f, 0.f);
		Nz::EulerAnglesf euler90Y(0.f, Nz::FromDegrees(90.f), 0.f);
		Nz::EulerAnglesf euler90R(0.f, 0.f, Nz::FromDegrees(90.f));

		WHEN("We transform the axis")
		{
			THEN("This is supposed to be left-handed")
			{
				Nz::Vector3f rotation90P = euler90P.ToQuaternion() * Nz::Vector3f::UnitY();
				Nz::Vector3f rotation90Y = euler90Y.ToQuaternion() * Nz::Vector3f::UnitZ();
				Nz::Vector3f rotation90R = euler90R.ToQuaternion() * Nz::Vector3f::UnitX();

				REQUIRE(rotation90P == Nz::Vector3f::UnitZ());
				REQUIRE(rotation90Y == Nz::Vector3f::UnitX());
				REQUIRE(rotation90R == Nz::Vector3f::UnitY());
			}
		}
	}

	GIVEN("Euler angles with rotation 45 on each axis")
	{
		WHEN("We convert to quaternion")
		{
			THEN("These results are expected")
			{
				REQUIRE(Nz::EulerAngles<int>(Nz::FromDegrees(45.f), 0.f, 0.f) == Nz::EulerAngles<int>(Nz::Quaternionf(0.923879504204f, 0.382683455944f, 0.f, 0.f).ToEulerAngles()));
				REQUIRE(Nz::EulerAngles<int>(0.f, Nz::FromDegrees(45.f), 0.f) == Nz::EulerAngles<int>(Nz::Quaternionf(0.923879504204f, 0.f, 0.382683455944f, 0.f).ToEulerAngles()));
				REQUIRE(Nz::EulerAngles<int>(0.f, 0.f, Nz::FromDegrees(45.f)) == Nz::EulerAngles<int>(Nz::Quaternionf(0.923879504204f, 0.f, 0.f, 0.382683455944f).ToEulerAngles()));
			}
		}
	}

	GIVEN("Three euler angles: (0, 22.5, 22.5), (90, 90, 0) and (30, 0, 30)")
	{
		Nz::EulerAnglesf euler45(Nz::FromDegrees(0.f), Nz::FromDegrees(22.5f), Nz::FromDegrees(22.5f));
		Nz::EulerAnglesf euler90(Nz::FromDegrees(90.f), Nz::FromDegrees(90.f), Nz::FromDegrees(0.f));
		Nz::EulerAnglesf euler30(Nz::FromDegrees(30.f), Nz::FromDegrees(0.f), Nz::FromDegrees(30.f));

		WHEN("We convert them to quaternion")
		{
			THEN("And then convert to euler angles, we have identity")
			{
				Nz::EulerAnglesf tmp = Nz::Quaternionf(euler45.ToQuaternion()).ToEulerAngles();
				REQUIRE(tmp.pitch == Approx(0.f));
				REQUIRE(tmp.yaw == Approx(22.5f));
				REQUIRE(tmp.roll == Approx(22.5f));
				tmp = Nz::Quaternionf(euler90.ToQuaternion()).ToEulerAngles();
				REQUIRE(tmp.pitch == Approx(90.f));
				REQUIRE(tmp.yaw == Approx(90.f));
				REQUIRE(tmp.roll == Approx(0.f));
				tmp = Nz::Quaternionf(euler30.ToQuaternion()).ToEulerAngles();
				REQUIRE(tmp.pitch == Approx(30.f));
				REQUIRE(tmp.yaw == Approx(0.f).margin(0.0001f));
				REQUIRE(tmp.roll == Approx(30.f));
			}
		}
	}
}
