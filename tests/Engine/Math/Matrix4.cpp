#include <Nazara/Math/Matrix4.hpp>
#include <Catch/catch.hpp>

#include <array>

SCENARIO("Matrix4", "[MATH][MATRIX4]")
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
				CHECK(firstIdentity * Nz::Vector2f::Unit() == Nz::Vector2f::Unit());
				CHECK(firstIdentity * Nz::Vector3f::Unit() == Nz::Vector3f::Unit());
				CHECK(firstIdentity * Nz::Vector4f(1.f, 1.f, 1.f, 1.f) == Nz::Vector4f(1.f, 1.f, 1.f, 1.f));
			}
		}

		WHEN("We multiply them")
		{
			THEN("It keeps being a identity")
			{
				CHECK(firstIdentity.Concatenate(secondIdentity) == firstIdentity);
				CHECK(firstIdentity.ConcatenateAffine(secondIdentity) == firstIdentity);
				CHECK((firstIdentity * secondIdentity) == firstIdentity);
				CHECK((1.f * firstIdentity) == firstIdentity);
				CHECK(firstIdentity.Inverse() == secondIdentity.InverseAffine());
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
				CHECK(matrix1.GetDeterminant() == Approx(24.f));
				CHECK(matrix2.GetDeterminant() == Approx(-1.f));
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
				CHECK(tmp.m32 == Approx(0.f));
				CHECK(tmp.m42 == Approx(0.f));
				tmp.m32 = 0.f;
				tmp.m42 = 0.f;
				CHECK(tmp == Nz::Matrix4f::Identity());
				CHECK((matrix2 * invMatrix2) == Nz::Matrix4f::Identity());
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

				CHECK(transformedMatrix == rotation45X);
				transformedMatrix.MakeTransform(Nz::Vector3f::Unit(), Nz::EulerAnglesf(Nz::FromDegrees(45.f), 0.f, 0.f).ToQuaternion());
				rotation45X.ApplyTranslation(Nz::Vector3f::Unit());
				CHECK(transformedMatrix == rotation45X);
			}

			THEN("Rotation around Y")
			{
				transformedMatrix.MakeTransform(Nz::Vector3f::Zero(), Nz::EulerAnglesf(0.f, Nz::FromDegrees(45.f), 0.f).ToQuaternion());
				Nz::Matrix4f rotation45Y(std::sqrt(2.f) / 2.f, 0.f, -std::sqrt(2.f) / 2.f, 0.f,
				                         0.f,                  1.f,  0.f,                  0.f,
				                         std::sqrt(2.f) / 2.f, 0.f,  std::sqrt(2.f) / 2.f, 0.f,
				                         0.f,                  0.f,  0.f,                  1.f);

				CHECK(transformedMatrix == rotation45Y);
				transformedMatrix.MakeTransform(Nz::Vector3f::Unit(), Nz::EulerAnglesf(0.f, Nz::FromDegrees(45.f), 0.f).ToQuaternion());
				rotation45Y.ApplyTranslation(Nz::Vector3f::Unit());
				CHECK(transformedMatrix == rotation45Y);
			}

			THEN("Rotation around Z")
			{
				transformedMatrix.MakeTransform(Nz::Vector3f::Zero(), Nz::EulerAnglesf(0.f, 0.f, Nz::FromDegrees(45.f)).ToQuaternion());
				Nz::Matrix4f rotation45Z( std::sqrt(2.f) / 2.f, std::sqrt(2.f) / 2.f, 0.f, 0.f,
				                          -std::sqrt(2.f) / 2.f, std::sqrt(2.f) / 2.f, 0.f, 0.f,
				                          0.f,                  0.f,                  1.f, 0.f,
				                          0.f,                  0.f,                  0.f, 1.f);

				CHECK(transformedMatrix == rotation45Z);
				transformedMatrix.MakeTransform(Nz::Vector3f::Unit(), Nz::EulerAnglesf(Nz::EulerAnglesf(0.f, 0.f, Nz::FromDegrees(45.f)).ToQuaternion()));
				rotation45Z.ApplyTranslation(Nz::Vector3f::Unit());
				CHECK(transformedMatrix == rotation45Z);
			}
		}
	}

	GIVEN("An identity matrix")
	{
		std::array<float, 16> content{{ 1.f, 0.f, 0.f, 0.f,
		                                0.f, 1.f, 0.f, 0.f,
		                                0.f, 0.f, 1.f, 0.f,
		                                0.f, 0.f, 0.f, 1.f
		}};

		Nz::Matrix4f identity(content.data());
		REQUIRE(identity.IsIdentity());

		WHEN("We rotate it from pitch 30")
		{
			Nz::Quaternionf rotation(Nz::EulerAnglesf(Nz::FromDegrees(30.f), 0.f, 0.f));
			identity.ApplyRotation(rotation);

			THEN("We should retrieve it")
			{
				REQUIRE(identity.GetRotation() == rotation);
			}
		}

		WHEN("We rotate it from yaw 30")
		{
			Nz::Quaternionf rotation(Nz::EulerAnglesf(0.f, Nz::FromDegrees(30.f), 0.f));
			identity.ApplyRotation(rotation);

			THEN("We should retrieve it")
			{
				REQUIRE(identity.GetRotation() == rotation);
			}
		}

		WHEN("We rotate it from roll 30")
		{
			Nz::Quaternionf rotation(Nz::EulerAnglesf(0.f, 0.f, Nz::FromDegrees(30.f)));
			identity.ApplyRotation(rotation);

			THEN("We should retrieve it")
			{
				REQUIRE(identity.GetRotation() == rotation);
			}
		}

		WHEN("We rotate it from a strange rotation")
		{
			Nz::Quaternionf rotation(Nz::EulerAnglesf(Nz::FromDegrees(10.f), Nz::FromDegrees(20.f), Nz::FromDegrees(30.f)));
			identity.ApplyRotation(rotation);

			THEN("We should retrieve it")
			{
				REQUIRE(identity.GetRotation() == rotation);
			}
		}

		WHEN("We scale it")
		{
			Nz::Vector3f scale(1.f, 2.f, 3.f);
			Nz::Vector3f squaredScale(scale.x * scale.x, scale.y * scale.y, scale.z * scale.z);
			identity.ApplyScale(scale);

			THEN("We should retrieve it")
			{
				CHECK(identity.GetScale() == scale);
				CHECK(identity.GetSquaredScale() == squaredScale);
			}

			AND_THEN("With a rotation")
			{
				identity.ApplyRotation(Nz::EulerAnglesf(Nz::FromDegrees(10.f), Nz::FromDegrees(20.f), Nz::FromDegrees(30.f)));
				Nz::Vector3f retrievedScale = identity.GetScale();
				CHECK(retrievedScale.x == Approx(scale.x));
				CHECK(retrievedScale.y == Approx(scale.y));
				CHECK(retrievedScale.z == Approx(scale.z));
			}
		}
	}

	GIVEN("A matrix with a negative determinant")
	{
		Nz::Matrix4f negativeDeterminant( -1.f, 0.f, 0.f, 0.f,
		                                   0.f, 1.f, 0.f, 0.f,
		                                   0.f, 0.f, 1.f, 0.f,
		                                   0.f, 0.f, 0.f, 1.f);

		WHEN("We ask information about determinant")
		{
			THEN("We expect those to be true")
			{
				CHECK(negativeDeterminant.GetDeterminant() == Approx(-1.f));
				CHECK(!negativeDeterminant.HasScale());
				CHECK(negativeDeterminant.HasNegativeScale());
			}
		}
	}

	GIVEN("Some transformed matrices")
	{
		Nz::Vector3f simpleTranslation = Nz::Vector3f::Zero();
		Nz::Quaternionf simpleRotation = Nz::Quaternionf::Identity();
		Nz::Vector3f simpleScale = Nz::Vector3f::Unit();
		Nz::Matrix4f simple = Nz::Matrix4f::Transform(simpleTranslation, simpleRotation, simpleScale);

		Nz::Vector3f complexTranslation = Nz::Vector3f(-5.f, 7.f, 3.5f);
		Nz::Quaternionf complexRotation = Nz::EulerAnglesf(Nz::FromDegrees(-22.5f), Nz::FromDegrees(30.f), Nz::FromDegrees(15.f));
		Nz::Vector3f complexScale = Nz::Vector3f(1.f, 2.f, 0.5f);
		Nz::Matrix4f complex = Nz::Matrix4f::Transform(complexTranslation, complexRotation, complexScale);

		Nz::Vector3f oppositeTranslation = Nz::Vector3f(-5.f, 7.f, 3.5f);
		Nz::Quaternionf oppositeRotation = Nz::EulerAnglesf(Nz::FromDegrees(-90.f), Nz::FromDegrees(0.f), Nz::FromDegrees(0.f));
		Nz::Vector3f oppositeScale = Nz::Vector3f(1.f, 2.f, 0.5f);
		Nz::Matrix4f opposite = Nz::Matrix4f::Transform(oppositeTranslation, oppositeRotation, oppositeScale);

		WHEN("We retrieve the different components")
		{
			THEN("It should be the original ones")
			{
				CHECK(simple.GetTranslation() == simpleTranslation);
				CHECK(simple.GetRotation() == simpleRotation);
				CHECK(simple.GetScale() == simpleScale);

				/*CHECK(complex.GetTranslation() == complexTranslation);
				CHECK(complex.GetRotation() == complexRotation);
				CHECK(complex.GetScale() == complexScale);

				CHECK(opposite.GetTranslation() == oppositeTranslation);
				CHECK(opposite.GetRotation() == oppositeRotation);
				CHECK(opposite.GetScale() == oppositeScale);*/
			}
		}
	}

	GIVEN("Some defined matrix and its opposite")
	{
		Nz::Vector3f translation(-5.f, 3.f, 0.5);
		Nz::Matrix4f initial = Nz::Matrix4f::Translate(translation);
		Nz::Quaternionf rotation = Nz::EulerAnglesf(Nz::FromDegrees(30.f), Nz::FromDegrees(-90.f), 0.f);
		initial.ApplyRotation(rotation);

		Nz::Matrix4f simple = Nz::Matrix4f::Transform(-translation, rotation.GetInverse(), Nz::Vector3f::Unit());

		WHEN("We multiply them together")
		{
			Nz::Matrix4f result = Nz::Matrix4f::Concatenate(simple, initial);

			THEN("We should get the identity")
			{
				Nz::Matrix4f identity = Nz::Matrix4f::Identity();
				for (int i = 0; i != 4; ++i)
				{
					Nz::Vector4f row = result.GetRow(i);
					Nz::Vector4f column = result.GetColumn(i);
					for (int j = 0; j != 4; ++j)
					{
						CHECK(Nz::NumberEquals(row[j], identity(i, j), 0.00001f));
						CHECK(Nz::NumberEquals(column[j], identity(i, j), 0.00001f));
					}
				}
			}
		}
	}
}
