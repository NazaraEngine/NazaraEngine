#include <Nazara/Core/AbstractImage.hpp>
#include <Nazara/TextRenderer/Font.hpp>
#include <Nazara/Core/GuillotineImageAtlas.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <array>
#include <filesystem>

std::filesystem::path GetAssetDir();

SCENARIO("Fonts", "[TextRenderer][Font]")
{
	WHEN("Loading default font")
	{
		std::shared_ptr<Nz::Font> font = Nz::Font::GetDefault();

		std::shared_ptr<Nz::GuillotineImageAtlas> imageAtlas = std::make_shared<Nz::GuillotineImageAtlas>();
		imageAtlas->SetMaxLayerSize(1024);

		font->SetAtlas(imageAtlas);

		CHECK(font->GetFamilyName() == "Open Sans");
		CHECK(font->GetStyleName() == "Regular");

		for (std::size_t i = 0; i < 3; ++i)
		{
			WHEN("Inspecting size info")
			{
				const auto& sizeInfo24 = font->GetSizeInfo(24);
				CHECK(sizeInfo24.lineHeight == 33);
				CHECK(sizeInfo24.spaceAdvance == 6);
				CHECK(sizeInfo24.underlinePosition == Catch::Approx(-2.40625f));
				CHECK(sizeInfo24.underlineThickness == Catch::Approx(1.20312f));

				const auto& sizeInfo72 = font->GetSizeInfo(72);
				CHECK(sizeInfo72.lineHeight == 98);
				CHECK(sizeInfo72.spaceAdvance == 19);
				CHECK(sizeInfo72.underlinePosition == Catch::Approx(-7.20312f));
				CHECK(sizeInfo72.underlineThickness == Catch::Approx(3.59375f));
			}

			WHEN("Retrieving kerning")
			{
				struct Test
				{
					unsigned int characterSize;
					char firstChar;
					char secondChar;
					int expectedKerning;
				};

				std::array<Test, 16> tests{
					{
						Test { 16, 'A', 'A', 0 },
						Test { 24, 'A', 'A', 0 },
						Test { 32, 'A', 'A', 0 },
						Test { 48, 'A', 'A', 0 },

						Test { 16, 'A', 'M', 0 },
						Test { 24, 'A', 'M', 0 },
						Test { 32, 'A', 'M', 0 },
						Test { 48, 'A', 'M', 0 },

						Test { 16, 'A', 'T', -1 },
						Test { 24, 'A', 'T', -2 },
						Test { 32, 'A', 'T', -2 },
						Test { 48, 'A', 'T', -3 },

						Test { 16, 'A', 'V', 0 },
						Test { 24, 'A', 'V', -1 },
						Test { 32, 'A', 'V', -1 },
						Test { 48, 'A', 'V', -2 },
					}
				};

				for (const Test& test : tests)
				{
					INFO("between " << test.firstChar << " and " << test.secondChar << " at character size " << test.characterSize);
					CHECK(font->GetKerning(test.characterSize, test.firstChar, test.secondChar) == test.expectedKerning);
				}
			}

			WHEN("Retrieving glyphs")
			{
				const auto& glyph1 = font->GetGlyph(72, Nz::TextStyle_Regular, 0.f, 'L');
				REQUIRE(glyph1.valid);
				CHECK(glyph1.advance == 37);
				CHECK(glyph1.aabb.IsValid());
				CHECK(glyph1.atlasRect.IsValid());
				CHECK(glyph1.fauxOutlineThickness == 0.f);
				CHECK_FALSE(glyph1.requireFauxBold);
				CHECK_FALSE(glyph1.requireFauxItalic);

				const auto& glyphYItalic = font->GetGlyph(72, Nz::TextStyle::Italic, 0.f, 'y');
				REQUIRE(glyphYItalic.valid);
				CHECK(glyphYItalic.advance == 36);
				CHECK(glyphYItalic.aabb.IsValid());
				CHECK(glyphYItalic.atlasRect.IsValid());
				CHECK(glyphYItalic.fauxOutlineThickness == 0.f);
				CHECK_FALSE(glyphYItalic.requireFauxBold);
				CHECK(glyphYItalic.requireFauxItalic);

				const auto& glyphYRegular = font->GetGlyph(72, Nz::TextStyle_Regular, 0.f, 'y');
				REQUIRE(glyphYRegular.valid);
				CHECK(glyphYRegular.advance == 36);
				CHECK(glyphYRegular.aabb == glyphYItalic.aabb);
				CHECK(glyphYRegular.atlasRect == glyphYItalic.atlasRect);
				CHECK(glyphYRegular.fauxOutlineThickness == 0.f);
				CHECK_FALSE(glyphYRegular.requireFauxBold);
				CHECK_FALSE(glyphYRegular.requireFauxItalic);

				CHECK(font->GetCachedGlyphCount() == 3);
				CHECK(font->GetAtlas()->GetLayerCount() == 1);
				CHECK(font->GetAtlas()->GetLayer(0)->GetLevelCount() == 1);
			}

			WHEN("Precaching a lot of glyphs")
			{
				std::string characterSet;
				for (char c = 'a'; c <= 'z'; ++c)
					characterSet += c;

				for (char c = 'A'; c <= 'Z'; ++c)
					characterSet += c;

				for (char c = '0'; c <= '9'; ++c)
					characterSet += c;

				for (unsigned int fontSize : {24, 36, 48, 72, 140})
				{
					for (float outlineThickness : { 0.f, 1.f, 2.f, 5.f })
					{
						font->Precache(fontSize, Nz::TextStyle_Regular, outlineThickness, characterSet);
					}
				}

				CHECK(font->GetAtlas()->GetLayerCount() > 1);
				for (std::size_t layerIndex = 0; layerIndex < font->GetAtlas()->GetLayerCount(); ++layerIndex)
				{
					CHECK(font->GetAtlas()->GetLayer(layerIndex)->GetLevelCount() == 1);
				}
			}

			if (i == 1)
			{
				font->ClearGlyphCache();
				font->ClearKerningCache();
				font->ClearSizeInfoCache();
			}
		}
	}
}
