#include <DataModels/VecBinOp.hpp>

void Vec4Add::ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		unsigned int lValue = left[i];
		unsigned int rValue = right[i];

		output[i] = static_cast<std::uint8_t>(std::min(lValue + rValue, 255U));
	}
}

void Vec4Mul::ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		unsigned int lValue = left[i];
		unsigned int rValue = right[i];

		output[i] = static_cast<std::uint8_t>(lValue * rValue / 255);
	}
}
