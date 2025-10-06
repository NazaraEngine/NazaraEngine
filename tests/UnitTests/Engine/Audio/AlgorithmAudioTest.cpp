#include <Nazara/Audio/Algorithm.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <array>

TEST_CASE("MixToMono", "[AUDIO][ALGORITHM]")
{
	SECTION("Mix two channels together")
	{
		std::array<Nz::Int16, 4> input = { { 1, 3, 5, 3 } };
		std::array<Nz::Int16, 2> output = { { 0, 0 } };

		// Two channels and two frames !
		Nz::UInt64 outputFrame = Nz::ConvertFrames(Nz::AudioFormat::Signed16, 2, 100, input.data(), Nz::AudioFormat::Signed16, 1, 100, output.data(), 2);
		CHECK(outputFrame == 2);

		std::array<Nz::Int16, 2> theoric = { { 2, 4 } }; // It's the mean of the two channels
		CHECK(output == theoric);
	}
}
