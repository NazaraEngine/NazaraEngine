#include <Nazara/Utility/Font.hpp>
#include <catch2/catch.hpp>
#include <array>
#include <filesystem>

std::filesystem::path GetResourceDir();

SCENARIO("Fonts", "[Utility][Font]")
{
	WHEN("Loading default font")
	{
		std::shared_ptr<Nz::Font> font = Nz::Font::GetDefault();

		CHECK(font->GetFamilyName() == "Open Sans");
		CHECK(font->GetStyleName() == "Regular");

		for (std::size_t i = 0; i < 3; ++i)
		{
			WHEN("Inspecting size info")
			{
				const auto& sizeInfo24 = font->GetSizeInfo(24);
				CHECK(sizeInfo24.lineHeight == 33);
				CHECK(sizeInfo24.spaceAdvance == 6);
				CHECK(sizeInfo24.underlinePosition == Approx(-2.40625f));
				CHECK(sizeInfo24.underlineThickness == Approx(1.20312f));

				const auto& sizeInfo72 = font->GetSizeInfo(72);
				CHECK(sizeInfo72.lineHeight == 98);
				CHECK(sizeInfo72.spaceAdvance == 19);
				CHECK(sizeInfo72.underlinePosition == Approx(-7.20312f));
				CHECK(sizeInfo72.underlineThickness == Approx(3.59375f));
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

			WHEN("Retrieving a glyph existing in the font")
			{
				const auto& glyph = font->GetGlyph(72, Nz::TextStyle_Regular, 0.f, 'L');
				REQUIRE(glyph.valid);
				CHECK(glyph.advance == 37);
				CHECK(glyph.aabb.IsValid());
				CHECK(glyph.atlasRect.IsValid());
				CHECK(glyph.fauxOutlineThickness == 0.f);
				CHECK_FALSE(glyph.requireFauxBold);
				CHECK_FALSE(glyph.requireFauxItalic);
			}

			WHEN("Retrieving a glyph existing in the font with italic")
			{
				const auto& glyph = font->GetGlyph(72, Nz::TextStyle::Italic, 0.f, 'y');
				REQUIRE(glyph.valid);
				CHECK(glyph.advance == 36);
				CHECK(glyph.aabb.IsValid());
				CHECK(glyph.atlasRect.IsValid());
				CHECK(glyph.fauxOutlineThickness == 0.f);
				CHECK_FALSE(glyph.requireFauxBold);
				CHECK(glyph.requireFauxItalic);
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
