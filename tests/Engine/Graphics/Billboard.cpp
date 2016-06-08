#include <Nazara/Graphics/Billboard.hpp>
#include <Catch/catch.hpp>

SCENARIO("Billboard", "[GRAPHICS][BILLBOARD]")
{
	GIVEN("A default billboard")
	{
		Nz::Billboard billboard;

		WHEN("We assign it to another")
		{
			Nz::MaterialRef materialRef = Nz::Material::New();
			materialRef->LoadFromFile("resources/Engine/Graphics/Nazara.png");
			Nz::Color materialColor = materialRef->GetDiffuseColor();
			Nz::BillboardRef otherBillboard = Nz::Billboard::New(materialRef);
		
			billboard = *otherBillboard;

			THEN("The old one has the same properties than the new one")
			{
				REQUIRE(billboard.GetColor() == materialColor);
				REQUIRE(billboard.GetMaterial().Get() == materialRef.Get());
				REQUIRE(billboard.GetRotation() == Approx(0.f));
				REQUIRE(billboard.GetSize() == Nz::Vector2f(64.f, 64.f)); // Default sizes
			}

			THEN("We set it with our new material and ask for its real size")
			{
				billboard.SetMaterial(materialRef, true);
				REQUIRE(billboard.GetSize() == Nz::Vector2f(765.f, 212.f)); // Nazara.png sizes
			}
		}
	}
}
