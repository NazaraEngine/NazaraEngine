#include <ShaderNode/DataModels/VecBinOp.hpp>

QString VecAdd::caption() const
{
	static QString caption = "Vector addition";
	return caption;
}

QString VecAdd::name() const
{
	static QString name = "vec_add";
	return name;
}

void VecAdd::ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		unsigned int lValue = left[i];
		unsigned int rValue = right[i];

		output[i] = static_cast<std::uint8_t>(std::min(lValue + rValue, 255U));
	}
}

QString VecMul::caption() const
{
	static QString caption = "Vector multiplication";
	return caption;
}

QString VecMul::name() const
{
	static QString name = "vec_mul";
	return name;
}

void VecMul::ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		unsigned int lValue = left[i];
		unsigned int rValue = right[i];

		output[i] = static_cast<std::uint8_t>(lValue * rValue / 255);
	}
}

QString VecSub::caption() const
{
	static QString caption = "Vector subtraction";
	return caption;
}


QString VecSub::name() const
{
	static QString name = "vec_sub";
	return name;
}

void VecSub::ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		unsigned int lValue = left[i];
		unsigned int rValue = right[i];

		unsigned int sub = (lValue >= rValue) ? lValue - rValue : 0u;

		output[i] = static_cast<std::uint8_t>(sub);
	}
}

QString VecDiv::caption() const
{
	static QString caption = "Vector divide";
	return caption;
}


QString VecDiv::name() const
{
	static QString name = "vec_div";
	return name;
}

void VecDiv::ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
	{
		unsigned int lValue = left[i];
		unsigned int rValue = right[i];

		unsigned res;
		if (rValue != 0)
			res = lValue / rValue;
		else if (lValue != 0)
			res = 0xFF; //< positive / 0 = +inf, which we clamp to 0xFF
		else
			res = 0; //< 0 / 0 = NaN, which we set to zero

		output[i] = static_cast<std::uint8_t>(res);
	}
}
