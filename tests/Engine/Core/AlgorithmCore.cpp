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
	SECTION("Compute hash of '0'")
	{
		auto result = Nz::ComputeHash(Nz::HashType_SHA512, "1234");
		REQUIRE(result.ToHex().ToUpper() == "D404559F602EAB6FD602AC7680DACBFAADD13630335E951F097AF3900E9DE176B6DB28512F2E000B9D04FBA5133E8B1C6E8DF59DB3A8AB9D60BE4B97CC9E81DB");
	}
}
