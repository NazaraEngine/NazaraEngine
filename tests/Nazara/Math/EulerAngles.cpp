#include <Nazara/Math/EulerAngles.hpp>
#include <catch.hpp>

SCENARIO("EulerAngles", "[MATH][EULERANGLES]")
{
	GIVEN("Two zero euler angles")
	{
		NzEulerAnglesf firstZero(0.f, 0.f, 0.f);
		NzEulerAnglesf secondZero(NzEulerAngles<int>::Zero());

		THEN("They should be equal")
		{
			REQUIRE(firstZero == secondZero);
		}

		WHEN("We do some operations")
		{
			NzEulerAnglesf euler90(90.f, 90.f, 90.f);
			NzEulerAnglesf euler270(270.f, 270.f, 270.f);

			NzEulerAnglesf euler360 = euler90 + euler270;
			euler360.Normalize();
			NzEulerAnglesf euler0 = euler270 - euler90;
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
				REQUIRE(firstZero.ToQuaternion() == NzEulerAnglesf(NzQuaternionf(1.f, 0.f, 0.f, 0.f)));
				REQUIRE(secondZero.ToQuaternion() == NzEulerAnglesf(NzQuaternionf(1.f, 0.f, 0.f, 0.f)));
			}
		}
	}

	GIVEN("Euler angles with rotation 45 on each axis")
	{
		WHEN("We convert to quaternion")
		{
			THEN("These results are expected")
			{
				REQUIRE(NzEulerAngles<int>(NzFromDegrees(45.f), 0.f, 0.f) == NzEulerAngles<int>(NzQuaternionf(0.923879504204f, 0.382683455944f, 0.f, 0.f).ToEulerAngles()));
				REQUIRE(NzEulerAngles<int>(0.f, NzFromDegrees(45.f), 0.f) == NzEulerAngles<int>(NzQuaternionf(0.923879504204f, 0.f, 0.382683455944f, 0.f).ToEulerAngles()));
				REQUIRE(NzEulerAngles<int>(0.f, 0.f, NzFromDegrees(45.f)) == NzEulerAngles<int>(NzQuaternionf(0.923879504204f, 0.f, 0.f, 0.382683455944f).ToEulerAngles()));
			}
		}
	}

	GIVEN("Three euler angles: (0, 22.5, 22.5), (90, 90, 0) and (30, 0, 30)")
	{
		NzEulerAnglesf euler45(NzFromDegrees(0.f), NzFromDegrees(22.5f), NzFromDegrees(22.5f));
		NzEulerAnglesf euler90(NzFromDegrees(90.f), NzFromDegrees(90.f), NzFromDegrees(0.f));
		NzEulerAnglesf euler30(NzFromDegrees(30.f), NzFromDegrees(0.f), NzFromDegrees(30.f));

		WHEN("We convert them to quaternion")
		{
			THEN("And then convert to euler angles, we have identity")
			{
				NzEulerAnglesf tmp = NzQuaternionf(euler45.ToQuaternion()).ToEulerAngles();
				REQUIRE(tmp.pitch == Approx(0.f));
				REQUIRE(tmp.yaw == Approx(22.5f));
				REQUIRE(tmp.roll == Approx(22.5f));
				tmp = NzQuaternionf(euler90.ToQuaternion()).ToEulerAngles();
				REQUIRE(tmp.pitch == Approx(90.f));
				REQUIRE(tmp.yaw == Approx(90.f));
				REQUIRE(tmp.roll == Approx(0.f));
				tmp = NzQuaternionf(euler30.ToQuaternion()).ToEulerAngles();
				REQUIRE(tmp.pitch == Approx(30.f));
				REQUIRE(tmp.yaw == Approx(0.f));
				REQUIRE(tmp.roll == Approx(30.f));
			}
		}
	}
}
