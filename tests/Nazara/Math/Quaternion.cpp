#include <Nazara/Math/Quaternion.hpp>
#include <catch.hpp>

SCENARIO("Quaternion", "[MATH][QUATERNION]")
{
	GIVEN("Two quaternions (0, 1, 0, 0)")
	{
		NzQuaternionf firstQuaternion(NzFromDegrees(180.f), NzVector3f::UnitX());
		NzQuaternionf secondQuaternion(0.f, 1.f, 0.f, 0.f);

		WHEN("We compare them")
		{
			THEN("They are the same and the proprieties of quaternions are respected")
			{
				REQUIRE(firstQuaternion == secondQuaternion);
				REQUIRE(firstQuaternion.ComputeW() == secondQuaternion.Normalize());
				REQUIRE(firstQuaternion.Conjugate() == secondQuaternion.Inverse());
				REQUIRE(firstQuaternion.DotProduct(secondQuaternion) == Approx(1.f));
			}
		}

		WHEN("We do some operations")
		{
			THEN("Multiply with a vectorX is identity")
			{
				REQUIRE((firstQuaternion * NzVector3f::UnitX()) == NzVector3f::UnitX());
			}

			AND_THEN("Multiply with a vectorY or Z is opposite")
			{
				REQUIRE((firstQuaternion * NzVector3f::UnitY()) == -NzVector3f::UnitY());
				REQUIRE((firstQuaternion * NzVector3f::UnitZ()) == -NzVector3f::UnitZ());
			}
		}
	}

	GIVEN("The four unit quaternions")
	{
		NzQuaternionf w(1.f, 0.f, 0.f, 0.f);
		NzQuaternionf x(0.f, 1.f, 0.f, 0.f);
		NzQuaternionf y(0.f, 0.f, 1.f, 0.f);
		NzQuaternionf z(0.f, 0.f, 0.f, 1.f);

		NzQuaternionf xyzw = x * y * z * w;

		WHEN("We ask for the norm")
		{
			THEN("They are all equal to 1")
			{
				REQUIRE(w.Magnitude() == Approx(1.f));
				REQUIRE(x.Magnitude() == Approx(1.f));
				REQUIRE(y.Magnitude() == Approx(1.f));
				REQUIRE(z.Magnitude() == Approx(1.f));
				REQUIRE(xyzw.Magnitude() == Approx(1.f));
			}
		}

		WHEN("We multiply them")
		{
			THEN("Results shoud follow")
			{
				NzQuaternionf oppositeOfW(-1.f, 0.f, 0.f, 0.f);
				NzQuaternionf oppositeOfX = x.GetConjugate();
				NzQuaternionf oppositeOfY = y.GetConjugate();
				NzQuaternionf oppositeOfZ = z.GetConjugate();

				REQUIRE((x * x) == oppositeOfW);
				REQUIRE((y * y) == oppositeOfW);
				REQUIRE((z * z) == oppositeOfW);
				REQUIRE((x * y * z) == oppositeOfW);

				REQUIRE((x * y) == z);
				REQUIRE((y * x) == oppositeOfZ);
				REQUIRE((y * z) == x);
				REQUIRE((z * y) == oppositeOfX);
				REQUIRE((z * x) == y);
				REQUIRE((x * z) == oppositeOfY);
			}
		}
	}

	GIVEN("Two different quaternions (10, (1, 0, 0) and (20, (1, 0, 0))")
	{
		NzQuaternionf x10 = NzQuaternionf(NzFromDegrees(10.f), NzVector3f::UnitX());
		NzQuaternionf x20 = x10 * x10;

		NzQuaternionf x30a = x10 * x20;
		NzQuaternionf x30b = x20 * x10;

		WHEN("We multiply them")
		{
			THEN("These results are expected")
			{
				REQUIRE(x20 == NzQuaternionf(NzFromDegrees(20.f), NzVector3f::UnitX()));
				REQUIRE(x30a == x30b);
			}
		}

		WHEN("Convert euler to quaternion")
		{
			NzQuaternionf X45(NzEulerAnglesf(NzFromDegrees(45.f), 0.f, 0.f));
			NzQuaternionf Y45(NzEulerAnglesf(0.f, NzFromDegrees(45.f), 0.f));
			NzQuaternionf Z45(NzEulerAnglesf(0.f, 0.f, NzFromDegrees(45.f)));

			THEN("They must be equal")
			{
				REQUIRE(X45 == NzQuaternionf(0.9238795f, 0.38268346f, 0.f, 0.f));
				REQUIRE(Y45 == NzQuaternionf(0.9238795f, 0.f, 0.38268346f, 0.f));
				REQUIRE(Z45 == NzQuaternionf(0.9238795f, 0.f, 0.f, 0.38268346f));
			}
		}

		WHEN("We convert to euler angles and then to quaternions")
		{
			THEN("These results are expected")
			{
				REQUIRE(x30a.ToEulerAngles() == x30b.ToEulerAngles());
				REQUIRE(x30a.ToEulerAngles().ToQuaternion() == x30b.ToEulerAngles().ToQuaternion());

				NzQuaternionf tmp(1.f, 1.f, 0.f, 0.f);
				tmp.Normalize();
				REQUIRE(tmp == tmp.ToEulerAngles().ToQuaternion());
			}
		}

		WHEN("We slerp")
		{
			THEN("The half of 10 and 30 is 20")
			{
				NzQuaternionf slerpx10x30a = NzQuaternionf::Slerp(x10, x30a, 0.5f);
				REQUIRE(slerpx10x30a.w == Approx(x20.w));
				REQUIRE(slerpx10x30a.x == Approx(x20.x));
				REQUIRE(slerpx10x30a.y == Approx(x20.y));
				REQUIRE(slerpx10x30a.z == Approx(x20.z));
				NzQuaternionf slerpx10x30b = NzQuaternionf::Slerp(x10, x30b, 0.5f);
				REQUIRE(slerpx10x30b.w == Approx(x20.w));
				REQUIRE(slerpx10x30b.x == Approx(x20.x));
				REQUIRE(slerpx10x30b.y == Approx(x20.y));
				REQUIRE(slerpx10x30b.z == Approx(x20.z));
				REQUIRE(NzQuaternionf::Slerp(x10, x30a, 0.f) == x10);
				REQUIRE(NzQuaternionf::Slerp(x10, x30a, 1.f) == x30a);
			}

			AND_THEN("The half of 45 is 22.5")
			{
				NzQuaternionf quaterionA(NzFromDegrees(0.f), NzVector3f::UnitZ());
				NzQuaternionf quaterionB(NzFromDegrees(45.f), NzVector3f::UnitZ());
				NzQuaternionf quaternionC = NzQuaternionf::Slerp(quaterionA, quaterionB, 0.5f);

				NzQuaternionf unitZ225(NzFromDegrees(22.5f), NzVector3f::UnitZ());
				REQUIRE(quaternionC.w == Approx(unitZ225.w));
				REQUIRE(quaternionC.x == Approx(unitZ225.x));
				REQUIRE(quaternionC.y == Approx(unitZ225.y));
				REQUIRE(quaternionC.z == Approx(unitZ225.z));
			}
		}
	}
}
