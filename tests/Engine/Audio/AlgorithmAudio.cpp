#include <Nazara/Audio/Algorithm.hpp>
#include <Catch/catch.hpp>

#include <array>

TEST_CASE("MixToMono", "[AUDIO][ALGORITHM]")
{
	SECTION("Mix two channels together")
	{
		std::array<int, 4> input = { { 1, 3, 5, 3 } };
		std::array<int, 2> output = { { 0, 0 } };

		// Two channels and two frames !
		Nz::MixToMono(input.data(), output.data(), 2, 2);

		std::array<int, 2> theoric = { { 2, 4 } }; // It's the mean of the two channels
		CHECK(output == theoric);
	}
}
