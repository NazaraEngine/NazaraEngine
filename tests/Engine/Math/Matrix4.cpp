#include <Nazara/Math/Matrix4.hpp>
#include <Catch/catch.hpp>

SCENARIO("Matrix4", "[MATH][MATRIX4]")
{
	GIVEN("Two identity matrix")
	{
		NzMatrix4f firstIdentity(NzMatrix4<int>::Identity());
		NzMatrix4f secondIdentity(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f);

		WHEN("We compare them")
		{
			THEN("They are equal")
			{
				REQUIRE(firstIdentity == secondIdentity);
			}
		}

		WHEN("We multiply the first with a vector")
		{
			THEN("Vector stay the same")
			{
				REQUIRE(firstIdentity.Transform(NzVector2f::Unit()) == NzVector2f::Unit());
				REQUIRE(firstIdentity.Transform(NzVector3f::Unit()) == NzVector3f::Unit());
				REQUIRE(firstIdentity.Transform(NzVector4f(1.f, 1.f, 1.f, 1.f)) == NzVector4f(1.f, 1.f, 1.f, 1.f));
			}
		}

		WHEN("We multiply them")
		{
			THEN("It keeps being a identity")
			{
				REQUIRE(firstIdentity.Concatenate(secondIdentity) == firstIdentity);
				REQUIRE(firstIdentity.ConcatenateAffine(secondIdentity) == firstIdentity);
				REQUIRE((firstIdentity * secondIdentity) == firstIdentity);
				REQUIRE((1.f * firstIdentity) == firstIdentity);
				REQUIRE(firstIdentity.Inverse() == secondIdentity.InverseAffine());
			}
		}
	}

	GIVEN("Two different matrix")
	{
		NzMatrix4f matrix1(1.0f, 0.0f, 0.0f, 0.0f,
		                   7.0f, 2.0f, 0.0f, 0.0f,
		                   1.0f, 5.0f, 3.0f, 0.0f,
		                   8.0f, 9.0f, 2.0f, 4.0f);

		NzMatrix4f matrix2(1.0f,  1.0f,  2.0f, -1.0f,
		                  -2.0f, -1.0f, -2.0f,  2.0f,
		                   4.0f,  2.0f,  5.0f, -4.0f,
		                   5.0f, -3.0f, -7.0f, -6.0f);

		WHEN("We ask for determinant")
		{
			THEN("These results are expected")
			{
				REQUIRE(matrix1.GetDeterminant() == Approx(24.f));
				REQUIRE(matrix2.GetDeterminant() == Approx(-1.f));
			}
		}

		WHEN("We multiply the matrix and its inverse")
		{
			NzMatrix4f invMatrix1;
			matrix1.GetInverse(&invMatrix1);

			NzMatrix4f invMatrix2;
			matrix2.GetInverse(&invMatrix2);

			THEN("We get the identity")
			{
				NzMatrix4f tmp = matrix1 * invMatrix1;
				REQUIRE(tmp.m32 == Approx(0.f));
				REQUIRE(tmp.m42 == Approx(0.f));
				tmp.m32 = 0.f;
				tmp.m42 = 0.f;
				REQUIRE(tmp == NzMatrix4f::Identity());
				REQUIRE((matrix2 * invMatrix2) == NzMatrix4f::Identity());
			}
		}
	}

	GIVEN("One transformed matrix from rotation 45 and translation 0")
	{
		NzMatrix4f transformedMatrix = NzMatrix4f::Transform(NzVector3f::Zero(), NzQuaternionf::Identity());
		REQUIRE(transformedMatrix == NzMatrix4f::Identity());

		WHEN("We compare with the right matrix")
		{
			THEN("Rotation around X")
			{
				transformedMatrix.MakeTransform(NzVector3f::Zero(), NzEulerAnglesf(NzFromDegrees(45.f), 0.f, 0.f).ToQuaternion());
				NzMatrix4f rotation45X(1.f,  0.f,                  0.f,                  0.f,
				                       0.f,  std::sqrt(2.f) / 2.f, std::sqrt(2.f) / 2.f, 0.f,
				                       0.f, -std::sqrt(2.f) / 2.f, std::sqrt(2.f) / 2.f, 0.f,
				                       0.f,  0.f,                  0.f,                  1.f);

				REQUIRE(transformedMatrix == rotation45X);
				transformedMatrix.MakeTransform(NzVector3f::Unit(), NzEulerAnglesf(NzFromDegrees(45.f), 0.f, 0.f).ToQuaternion());
				rotation45X.ApplyTranslation(NzVector3f::Unit());
				REQUIRE(transformedMatrix == rotation45X);
			}

			THEN("Rotation around Y")
			{
				transformedMatrix.MakeTransform(NzVector3f::Zero(), NzEulerAnglesf(0.f, NzFromDegrees(45.f), 0.f).ToQuaternion());
				NzMatrix4f rotation45Y(std::sqrt(2.f) / 2.f, 0.f, -std::sqrt(2.f) / 2.f, 0.f,
				                       0.f,                  1.f,  0.f,                  0.f,
				                       std::sqrt(2.f) / 2.f, 0.f,  std::sqrt(2.f) / 2.f, 0.f,
				                       0.f,                  0.f,  0.f,                  1.f);

				REQUIRE(transformedMatrix == rotation45Y);
				transformedMatrix.MakeTransform(NzVector3f::Unit(), NzEulerAnglesf(0.f, NzFromDegrees(45.f), 0.f).ToQuaternion());
				rotation45Y.ApplyTranslation(NzVector3f::Unit());
				REQUIRE(transformedMatrix == rotation45Y);
			}

			THEN("Rotation around Z")
			{
				transformedMatrix.MakeTransform(NzVector3f::Zero(), NzEulerAnglesf(0.f, 0.f, NzFromDegrees(45.f)).ToQuaternion());
				NzMatrix4f rotation45Z( std::sqrt(2.f) / 2.f, std::sqrt(2.f) / 2.f, 0.f, 0.f,
				                       -std::sqrt(2.f) / 2.f, std::sqrt(2.f) / 2.f, 0.f, 0.f,
				                        0.f,                  0.f,                  1.f, 0.f,
				                        0.f,                  0.f,                  0.f, 1.f);

				REQUIRE(transformedMatrix == rotation45Z);
				transformedMatrix.MakeTransform(NzVector3f::Unit(), NzEulerAnglesf(NzEulerAnglesf(0.f, 0.f, NzFromDegrees(45.f)).ToQuaternion()));
				rotation45Z.ApplyTranslation(NzVector3f::Unit());
				REQUIRE(transformedMatrix == rotation45Z);
			}
		}
	}
}
