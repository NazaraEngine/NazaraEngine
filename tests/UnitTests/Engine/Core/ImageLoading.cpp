#include <Nazara/Core/Image.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

std::filesystem::path GetAssetDir();

SCENARIO("Images", "[Core][Image]")
{
	WHEN("Loading PNG files")
	{
		GIVEN("Logo.png")
		{
			std::shared_ptr<Nz::Image> logo = Nz::Image::LoadFromFile(GetAssetDir() / "Logo.png");
			REQUIRE(logo);

			CHECK(logo->GetWidth() == 765);
			CHECK(logo->GetHeight() == 212);
			CHECK(logo->GetLevelCount() == 1);
			CHECK(logo->GetPixelColor(165, 139) == Nz::Color::FromRGB8(51, 58, 100));
			CHECK(logo->GetFormat() == Nz::PixelFormat::RGBA8);
			CHECK(logo->HasAlpha());
		}
	}

	WHEN("Loading JPG files")
	{
		GIVEN("stars-background.jpg")
		{
			std::shared_ptr<Nz::Image> background = Nz::Image::LoadFromFile(GetAssetDir() / "Utility/stars-background.jpg");
			REQUIRE(background);

			CHECK(background->GetWidth() == 1920);
			CHECK(background->GetHeight() == 1200);
			CHECK(background->GetLevelCount() == 1);
			CHECK(background->GetPixelColor(1377, 372) == Nz::Color::FromRGB8(171, 152, 233));
			CHECK(background->GetFormat() == Nz::PixelFormat::RGBA8);
			CHECK(!background->HasAlpha());
		}
	}
}
