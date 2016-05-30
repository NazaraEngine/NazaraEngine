#include <Nazara/Graphics/Light.hpp>
#include <Catch/catch.hpp>

SCENARIO("Light", "[GRAPHICS][LIGHT]")
{
	GIVEN("Different light")
	{
		Nz::Light directionalLight(Nz::LightType_Directional);
		Nz::Light pointLight(Nz::LightType_Point);
		Nz::Light spotLight(Nz::LightType_Spot);

		WHEN("We try to cull")
		{
			Nz::Frustumf frustum;
			frustum.Build(90.f, 16.f / 9.f, 1.f, 1000.f, Nz::Vector3f::Zero(), Nz::Vector3f::UnitX());
			Nz::Matrix4f Unit3InX = Nz::Matrix4f::Translate(Nz::Vector3f::UnitX() * 3.f);
			Nz::Matrix4f rotationTowardsY = Unit3InX * Nz::Matrix4f::Rotate(Nz::EulerAnglesf(Nz::FromDegrees(90.f), 0.f, 0.f).ToQuaternion());

			THEN("These results are expected")
			{
				REQUIRE(directionalLight.Cull(frustum, Unit3InX));
				REQUIRE(pointLight.Cull(frustum, Unit3InX));
				REQUIRE(!spotLight.Cull(frustum, Unit3InX));

				REQUIRE(directionalLight.Cull(frustum, rotationTowardsY));
				REQUIRE(pointLight.Cull(frustum, rotationTowardsY));
				REQUIRE(!spotLight.Cull(frustum, rotationTowardsY));
			}
		}
	}
}
