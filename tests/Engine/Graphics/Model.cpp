#include <Nazara/Graphics/Model.hpp>
#include <Catch/catch.hpp>

SCENARIO("Model", "[GRAPHICS][MODEL]")
{
	GIVEN("The standford dragon model")
	{
		WHEN("We get general informations")
		{
			THEN("These results are expected")
			{
				Nz::ModelRef model = Nz::Model::New();
				REQUIRE(model->LoadFromFile("resources/Engine/Graphics/dragon_recon/dragon_vrip_res4.obj"));

				REQUIRE(model->GetMaterialCount() == 2);
				REQUIRE(model->GetSkin() == 0);
				REQUIRE(model->GetSkinCount() == 1);

				Nz::Material* material = model->GetMaterial(0);
				REQUIRE(material->GetAmbientColor() == Nz::Color(128));
			}
		}
	}
}
