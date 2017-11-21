#include <Nazara/Math/Quaternion.hpp>
#include <Catch/catch.hpp>

SCENARIO("Quaternion", "[MATH][QUATERNION]")
{
	GIVEN("Two quaternions (0, 1, 0, 0)")
	{
		Nz::Quaternionf firstQuaternion(Nz::FromDegrees(180.f), Nz::Vector3f::UnitX());
		Nz::Quaternionf secondQuaternion(0.f, 1.f, 0.f, 0.f);

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
				REQUIRE((firstQuaternion * Nz::Vector3f::UnitX()) == Nz::Vector3f::UnitX());
			}

			AND_THEN("Multiply with a vectorY or Z is opposite")
			{
				REQUIRE((firstQuaternion * Nz::Vector3f::UnitY()) == -Nz::Vector3f::UnitY());
				REQUIRE((firstQuaternion * Nz::Vector3f::UnitZ()) == -Nz::Vector3f::UnitZ());
			}
		}

		WHEN("We invert or normalize Zero quaternion")
		{
			Nz::Quaternionf zero = Nz::Quaternionf::Zero();

			THEN("It's meant not to be changed")
			{
				Nz::Quaternionf inverted = zero.GetInverse();
				float tmp = -1.f;
				Nz::Quaternionf normalized = zero.GetNormal(&tmp);

				REQUIRE(inverted == zero);
				REQUIRE(normalized == zero);
				REQUIRE(tmp == Approx(0.f));
			}
		}
	}

	GIVEN("The four unit quaternions")
	{
		Nz::Quaternionf w(1.f, 0.f, 0.f, 0.f);
		Nz::Quaternionf x(0.f, 1.f, 0.f, 0.f);
		Nz::Quaternionf y(0.f, 0.f, 1.f, 0.f);
		Nz::Quaternionf z(0.f, 0.f, 0.f, 1.f);

		Nz::Quaternionf xyzw = x * y * z * w;

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
				Nz::Quaternionf oppositeOfW(-1.f, 0.f, 0.f, 0.f);
				Nz::Quaternionf oppositeOfX = x.GetConjugate();
				Nz::Quaternionf oppositeOfY = y.GetConjugate();
				Nz::Quaternionf oppositeOfZ = z.GetConjugate();

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
		Nz::Quaternionf x10 = Nz::Quaternionf(Nz::FromDegrees(10.f), Nz::Vector3f::UnitX());
		Nz::Quaternionf x20 = x10 * x10;

		Nz::Quaternionf x30a = x10 * x20;
		Nz::Quaternionf x30b = x20 * x10;

		WHEN("We multiply them")
		{
			THEN("These results are expected")
			{
				REQUIRE(x20 == Nz::Quaternionf(Nz::FromDegrees(20.f), Nz::Vector3f::UnitX()));
				REQUIRE(x30a == x30b);
			}
		}

		WHEN("Convert euler to quaternion")
		{
			Nz::Quaternionf X45(Nz::EulerAnglesf(Nz::FromDegrees(45.f), 0.f, 0.f));
			Nz::Quaternionf Y45(Nz::EulerAnglesf(0.f, Nz::FromDegrees(45.f), 0.f));
			Nz::Quaternionf Z45(Nz::EulerAnglesf(0.f, 0.f, Nz::FromDegrees(45.f)));

			THEN("They must be equal")
			{
				REQUIRE(X45 == Nz::Quaternionf(0.9238795f, 0.38268346f, 0.f, 0.f));
				REQUIRE(Y45 == Nz::Quaternionf(0.9238795f, 0.f, 0.38268346f, 0.f));
				REQUIRE(Z45 == Nz::Quaternionf(0.9238795f, 0.f, 0.f, 0.38268346f));
			}
		}

		WHEN("We convert to euler angles and then to quaternions")
		{
			THEN("These results are expected")
			{
				REQUIRE(x30a.ToEulerAngles() == x30b.ToEulerAngles());
				REQUIRE(x30a.ToEulerAngles().ToQuaternion() == x30b.ToEulerAngles().ToQuaternion());

				Nz::Quaternionf tmp(1.f, 1.f, 0.f, 0.f);
				tmp.Normalize();
				REQUIRE(tmp == tmp.ToEulerAngles().ToQuaternion());
			}
		}

		WHEN("We slerp")
		{
			THEN("The half of 10 and 30 is 20")
			{
				Nz::Quaternionf slerpx10x30a = Nz::Quaternionf::Slerp(x10, x30a, 0.5f);
				REQUIRE(slerpx10x30a.w == Approx(x20.w));
				REQUIRE(slerpx10x30a.x == Approx(x20.x));
				REQUIRE(slerpx10x30a.y == Approx(x20.y));
				REQUIRE(slerpx10x30a.z == Approx(x20.z));
				Nz::Quaternionf slerpx10x30b = Nz::Quaternionf::Slerp(x10, x30b, 0.5f);
				REQUIRE(slerpx10x30b.w == Approx(x20.w));
				REQUIRE(slerpx10x30b.x == Approx(x20.x));
				REQUIRE(slerpx10x30b.y == Approx(x20.y));
				REQUIRE(slerpx10x30b.z == Approx(x20.z));
				REQUIRE(Nz::Quaternionf::Slerp(x10, x30a, 0.f) == x10);
				REQUIRE(Nz::Quaternionf::Slerp(x10, x30a, 1.f) == x30a);
			}

			AND_THEN("The half of 45 is 22.5")
			{
				Nz::Quaternionf quaterionA(Nz::FromDegrees(0.f), Nz::Vector3f::UnitZ());
				Nz::Quaternionf quaterionB(Nz::FromDegrees(45.f), Nz::Vector3f::UnitZ());
				Nz::Quaternionf quaternionC = Nz::Quaternionf::Slerp(quaterionA, quaterionB, 0.5f);

				Nz::Quaternionf unitZ225(Nz::FromDegrees(22.5f), Nz::Vector3f::UnitZ());
				REQUIRE(quaternionC.w == Approx(unitZ225.w));
				REQUIRE(quaternionC.x == Approx(unitZ225.x));
				REQUIRE(quaternionC.y == Approx(unitZ225.y));
				REQUIRE(quaternionC.z == Approx(unitZ225.z));
			}
		}

		WHEN("We get the rotation between two vectors")
		{
			THEN("The rotation in right-handed is 90 degree on z")
			{
				Nz::Quaternionf rotationBetweenXY = Nz::Quaternionf::RotationBetween(Nz::Vector3f::UnitX(), Nz::Vector3f::UnitY());
				Nz::Quaternionf rotation90Z(Nz::FromDegrees(90.f), Nz::Vector3f::UnitZ());
				REQUIRE(rotation90Z == rotationBetweenXY);
			}

			THEN("The rotation in right-handed is 90 degree on y")
			{
				Nz::Quaternionf rotationBetweenXZ = Nz::Quaternionf::RotationBetween(Nz::Vector3f::UnitX(), Nz::Vector3f::UnitZ());
				Nz::Quaternionf rotation90Y(Nz::FromDegrees(-90.f), Nz::Vector3f::UnitY());
				REQUIRE(rotation90Y == rotationBetweenXZ);
			}

			THEN("The rotation in right-handed is 90 degree on x")
			{
				Nz::Quaternionf rotationBetweenYZ = Nz::Quaternionf::RotationBetween(Nz::Vector3f::UnitY(), Nz::Vector3f::UnitZ());
				Nz::Quaternionf rotation90X(Nz::FromDegrees(90.f), Nz::Vector3f::UnitX());
				REQUIRE(rotation90X == rotationBetweenYZ);
			}

			THEN("The rotation in right-handed is 90 degree on y with non-unit vectors")
			{
				Nz::Vector3f origin(1.f, 0.f, 1.f);
				Nz::Vector3f extremity(-1.f, 0.f, 1.f);
				Nz::Quaternionf rotation = Nz::Quaternionf::RotationBetween(origin, extremity);
				REQUIRE(rotation * origin == extremity);
			}
		}
	} 

	GIVEN("Different angles")
	{
		Nz::Quaternionf rotation90X(0.707f, 0.707f, 0.f, 0.f);
		Nz::Quaternionf rotation90Y(0.707f, 0.f, 0.707f, 0.f);
		Nz::Quaternionf rotation90Z(0.707f, 0.f, 0.f, 0.707f);

		Nz::Quaternionf rotation180X(0.f, 1.f, 0.f, 0.f);
		Nz::Quaternionf rotation180Y(0.f, 0.f, 1.f, 0.f);
		Nz::Quaternionf rotation180Z(0.f, 0.f, 0.f, 1.f);

		Nz::Quaternionf rotation270X(-0.707f, 0.707f, 0.f, 0.f);
		Nz::Quaternionf rotation270Y(-0.707f, 0.f, 0.707f, 0.f);
		Nz::Quaternionf rotation270Z(-0.707f, 0.f, 0.f, 0.707f);

		Nz::Quaternionf special(0.707f, 0.006f, 0.006f, 0.707f);

		WHEN("We convert them to euler angles")
		{
			THEN("Those are equal to")
			{
				CHECK(Nz::NumberEquals(rotation90X.ToEulerAngles().pitch, Nz::FromDegrees(90.f), 0.1f));
				CHECK(Nz::NumberEquals(rotation90Y.ToEulerAngles().yaw, Nz::FromDegrees(90.f), 0.1f));
				CHECK(Nz::NumberEquals(rotation90Z.ToEulerAngles().roll, Nz::FromDegrees(90.f), 0.1f));

				CHECK(rotation180X == Nz::EulerAnglesf(180.f, 0.f, 0.f));
				CHECK(rotation180Y == Nz::EulerAnglesf(0.f, 180.f, 0.f));
				CHECK(rotation180Z == Nz::EulerAnglesf(0.f, 0.f, 180.f));

				CHECK(Nz::NumberEquals(rotation270X.ToEulerAngles().pitch, Nz::FromDegrees(-90.f), 0.1f));
				CHECK(Nz::NumberEquals(rotation270Y.ToEulerAngles().yaw, Nz::FromDegrees(-90.f), 0.1f));
				CHECK(Nz::NumberEquals(rotation270Z.ToEulerAngles().roll, Nz::FromDegrees(-90.f), 0.1f));

				CHECK(Nz::NumberEquals(special.ToEulerAngles().pitch, Nz::FromDegrees(0.f), 0.1f));
				CHECK(Nz::NumberEquals(special.ToEulerAngles().yaw, Nz::FromDegrees(1.f), 0.1f));
				CHECK(Nz::NumberEquals(special.ToEulerAngles().roll, Nz::FromDegrees(90.f), 0.1f));
			}
		}
	}
}
