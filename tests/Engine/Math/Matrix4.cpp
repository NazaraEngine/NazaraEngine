#include <Nazara/Math/Matrix4.hpp>
#include <Catch/catch.hpp>

SCENARIO("Matrix4", "[MATH][Matrix4]")
{
	GIVEN("Two identity matrix")
	{
		Nz::Matrix4f firstIdentity(Nz::Matrix4<int>::Identity());
		Nz::Matrix4f secondIdentity(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f);

		WHEN("We compare them")
		{
			THEN("They are equal")
			{
				REQUIRE(firstIdentity == secondIdentity);
			}
		}

		WHEN("We multiply the first with a Nz::Vector")
		{
			THEN("Nz::Vector stay the same")
			{
				REQUIRE(firstIdentity.Transform(Nz::Vector2f::Unit()) == Nz::Vector2f::Unit());
				REQUIRE(firstIdentity.Transform(Nz::Vector3f::Unit()) == Nz::Vector3f::Unit());
				REQUIRE(firstIdentity.Transform(Nz::Vector4f(1.f, 1.f, 1.f, 1.f)) == Nz::Vector4f(1.f, 1.f, 1.f, 1.f));
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

		WHEN("We transpose one of this matrix")
		{
			THEN("Identity transposed is the same than identity")
			{
				Nz::Matrix4f transposedIdentity;
				firstIdentity.GetTransposed(&transposedIdentity);
				REQUIRE(firstIdentity == transposedIdentity);
			}
		}
	}

	GIVEN("Two different matrix")
	{
		Nz::Matrix4f matrix1(1.0f, 0.0f, 0.0f, 0.0f,
		                     7.0f, 2.0f, 0.0f, 0.0f,
		                     1.0f, 5.0f, 3.0f, 0.0f,
		                     8.0f, 9.0f, 2.0f, 4.0f);

		Nz::Matrix4f matrix2(1.0f,  1.0f,  2.0f, -1.0f,
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
			Nz::Matrix4f invMatrix1;
			matrix1.GetInverse(&invMatrix1);

			Nz::Matrix4f invMatrix2;
			matrix2.GetInverse(&invMatrix2);

			THEN("We get the identity")
			{
				Nz::Matrix4f tmp = matrix1 * invMatrix1;
				REQUIRE(tmp.m32 == Approx(0.f));
				REQUIRE(tmp.m42 == Approx(0.f));
				tmp.m32 = 0.f;
				tmp.m42 = 0.f;
				REQUIRE(tmp == Nz::Matrix4f::Identity());
				REQUIRE((matrix2 * invMatrix2) == Nz::Matrix4f::Identity());
			}
		}
	}

	GIVEN("One transformed matrix from rotation 45 and translation 0")
	{
		Nz::Matrix4f transformedMatrix = Nz::Matrix4f::Transform(Nz::Vector3f::Zero(), Nz::Quaternionf::Identity());
		REQUIRE(transformedMatrix == Nz::Matrix4f::Identity());

		WHEN("We compare with the right matrix")
		{
			THEN("Rotation around X")
			{
				transformedMatrix.MakeTransform(Nz::Vector3f::Zero(), Nz::EulerAnglesf(Nz::FromDegrees(45.f), 0.f, 0.f).ToQuaternion());
				Nz::Matrix4f rotation45X(1.f,  0.f,                  0.f,                  0.f,
				                         0.f,  std::sqrt(2.f) / 2.f, std::sqrt(2.f) / 2.f, 0.f,
				                         0.f, -std::sqrt(2.f) / 2.f, std::sqrt(2.f) / 2.f, 0.f,
				                         0.f,  0.f,                  0.f,                  1.f);

				REQUIRE(transformedMatrix == rotation45X);
				transformedMatrix.MakeTransform(Nz::Vector3f::Unit(), Nz::EulerAnglesf(Nz::FromDegrees(45.f), 0.f, 0.f).ToQuaternion());
				rotation45X.ApplyTranslation(Nz::Vector3f::Unit());
				REQUIRE(transformedMatrix == rotation45X);
			}

			THEN("Rotation around Y")
			{
				transformedMatrix.MakeTransform(Nz::Vector3f::Zero(), Nz::EulerAnglesf(0.f, Nz::FromDegrees(45.f), 0.f).ToQuaternion());
				Nz::Matrix4f rotation45Y(std::sqrt(2.f) / 2.f, 0.f, -std::sqrt(2.f) / 2.f, 0.f,
				                         0.f,                  1.f,  0.f,                  0.f,
				                         std::sqrt(2.f) / 2.f, 0.f,  std::sqrt(2.f) / 2.f, 0.f,
				                         0.f,                  0.f,  0.f,                  1.f);

				REQUIRE(transformedMatrix == rotation45Y);
				transformedMatrix.MakeTransform(Nz::Vector3f::Unit(), Nz::EulerAnglesf(0.f, Nz::FromDegrees(45.f), 0.f).ToQuaternion());
				rotation45Y.ApplyTranslation(Nz::Vector3f::Unit());
				REQUIRE(transformedMatrix == rotation45Y);
			}

			THEN("Rotation around Z")
			{
				transformedMatrix.MakeTransform(Nz::Vector3f::Zero(), Nz::EulerAnglesf(0.f, 0.f, Nz::FromDegrees(45.f)).ToQuaternion());
				Nz::Matrix4f rotation45Z( std::sqrt(2.f) / 2.f, std::sqrt(2.f) / 2.f, 0.f, 0.f,
				                          -std::sqrt(2.f) / 2.f, std::sqrt(2.f) / 2.f, 0.f, 0.f,
				                          0.f,                  0.f,                  1.f, 0.f,
				                          0.f,                  0.f,                  0.f, 1.f);

				REQUIRE(transformedMatrix == rotation45Z);
				transformedMatrix.MakeTransform(Nz::Vector3f::Unit(), Nz::EulerAnglesf(Nz::EulerAnglesf(0.f, 0.f, Nz::FromDegrees(45.f)).ToQuaternion()));
				rotation45Z.ApplyTranslation(Nz::Vector3f::Unit());
				REQUIRE(transformedMatrix == rotation45Z);
			}
		}
	}
}
