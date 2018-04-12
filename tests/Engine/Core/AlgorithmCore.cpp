#include <Nazara/Core/Algorithm.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Math/Vector2.hpp>

TEST_CASE("Apply", "[CORE][ALGORITHM]")
{
	SECTION("Apply lambda to two vector2")
	{
		Nz::Vector2<int> vector = Nz::Vector2<int>::Unit();
		auto lambda = [](const Nz::Vector2<int>& vec1, const Nz::Vector2<int>& vec2)
		{
			return vec1 + vec2;
		};

		Nz::Vector2<int> result = Nz::Apply(lambda, std::make_tuple(vector, vector));

		REQUIRE(result == (Nz::Vector2<int>::Unit() * 2));
	}

	SECTION("Apply member function to vector2")
	{
		Nz::Vector2<int> vector = Nz::Vector2<int>::Unit();

		int result = Nz::Apply(vector, &Nz::Vector2<int>::Distance, std::make_tuple(vector));

		REQUIRE(result == 0);
	}
}

TEST_CASE("ComputeHash", "[CORE][ALGORITHM]")
{
	/*SECTION("Compute CRC32 of '1234'")
	{
		auto result = Nz::ComputeHash(Nz::HashType_CRC32, "1234");
		REQUIRE(result.ToHex().ToUpper() == "596A3B55");
	}

	SECTION("Compute CRC64 of '1234'")
	{
		auto result = Nz::ComputeHash(Nz::HashType_CRC64, "1234");
		REQUIRE(result.ToHex().ToUpper() == "33302B9FC23855A8");
	}

	SECTION("Compute Fletcher16 of '1234'")
	{
		auto result = Nz::ComputeHash(Nz::HashType_Fletcher16, "1234");
		REQUIRE(result.ToHex().ToUpper() == "F5CA");
	}*/

	SECTION("Compute MD5 of '1234'")
	{
		auto result = Nz::ComputeHash(Nz::HashType_MD5, "1234");
		REQUIRE(result.ToHex().ToUpper() == "81DC9BDB52D04DC20036DBD8313ED055");
	}

	SECTION("Compute SHA1 of '1234'")
	{
		auto result = Nz::ComputeHash(Nz::HashType_SHA1, "1234");
		REQUIRE(result.ToHex().ToUpper() == "7110EDA4D09E062AA5E4A390B0A572AC0D2C0220");
	}

	SECTION("Compute SHA224 of '1234'")
	{
		auto result = Nz::ComputeHash(Nz::HashType_SHA224, "1234");
		REQUIRE(result.ToHex().ToUpper() == "99FB2F48C6AF4761F904FC85F95EB56190E5D40B1F44EC3A9C1FA319");
	}

	SECTION("Compute SHA256 of '1234'")
	{
		auto result = Nz::ComputeHash(Nz::HashType_SHA256, "1234");
		REQUIRE(result.ToHex().ToUpper() == "03AC674216F3E15C761EE1A5E255F067953623C8B388B4459E13F978D7C846F4");
	}

	SECTION("Compute SHA384 of '1234'")
	{
		auto result = Nz::ComputeHash(Nz::HashType_SHA384, "1234");
		REQUIRE(result.ToHex().ToUpper() == "504F008C8FCF8B2ED5DFCDE752FC5464AB8BA064215D9C5B5FC486AF3D9AB8C81B14785180D2AD7CEE1AB792AD44798C");
	}

	SECTION("Compute SHA512 of '1234'")
	{
		auto result = Nz::ComputeHash(Nz::HashType_SHA512, "1234");
		REQUIRE(result.ToHex().ToUpper() == "D404559F602EAB6FD602AC7680DACBFAADD13630335E951F097AF3900E9DE176B6DB28512F2E000B9D04FBA5133E8B1C6E8DF59DB3A8AB9D60BE4B97CC9E81DB");
	}

	SECTION("Compute Whirlpool of '1234'")
	{
		auto result = Nz::ComputeHash(Nz::HashType_Whirlpool, "1234");
		REQUIRE(result.ToHex().ToUpper() == "2F9959B230A44678DD2DC29F037BA1159F233AA9AB183CE3A0678EAAE002E5AA6F27F47144A1A4365116D3DB1B58EC47896623B92D85CB2F191705DAF11858B8");
	}
}
