#include <Nazara/Core/Mesh.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

std::filesystem::path GetAssetDir();

SCENARIO("Meshes", "[Core][Mesh]")
{
	WHEN("Loading OJB files")
	{
		GIVEN("Spaceship/spaceship.obj")
		{
			std::shared_ptr<Nz::Mesh> spaceship = Nz::Mesh::LoadFromFile(GetAssetDir() / "Utility/Spaceship/spaceship.obj");
			REQUIRE(spaceship);

			CHECK(!spaceship->IsAnimable());
			CHECK(spaceship->GetSubMeshCount() == 2);
			CHECK(spaceship->GetMaterialCount() == 2);
			CHECK(spaceship->GetTriangleCount() == 6814);
			CHECK(spaceship->GetVertexCount() == 8713);
		}

		GIVEN("SpaceStation/space_station.obj")
		{
			std::shared_ptr<Nz::Mesh> spacestation = Nz::Mesh::LoadFromFile(GetAssetDir() / "Utility/SpaceStation/space_station.obj");
			REQUIRE(spacestation);

			CHECK(!spacestation->IsAnimable());
			CHECK(spacestation->GetSubMeshCount() == 1);
			CHECK(spacestation->GetMaterialCount() == 1);
			CHECK(spacestation->GetTriangleCount() == 422);
			CHECK(spacestation->GetVertexCount() == 516);
		}
	}

	WHEN("Loading MD2 files")
	{
		GIVEN("drfreak.md2")
		{
			std::shared_ptr<Nz::Mesh> drfreak = Nz::Mesh::LoadFromFile(GetAssetDir() / "Utility/drfreak/drfreak.md2");
			CHECK(drfreak);

			CHECK(!drfreak->IsAnimable()); //< non-skeletal animations are not supported
			CHECK(drfreak->GetSubMeshCount() == 1);
			CHECK(drfreak->GetMaterialCount() == 1);
			CHECK(drfreak->GetTriangleCount() == 593);
			CHECK(drfreak->GetVertexCount() == 496);
		}
	}
}
