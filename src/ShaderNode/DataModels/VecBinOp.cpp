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

void VecAdd::ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
		output[i] = left[i] + right[i];
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

void VecMul::ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
		output[i] = left[i] * right[i];
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

void VecSub::ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
		output[i] = left[i] - right[i];
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

void VecDiv::ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount)
{
	for (std::size_t i = 0; i < pixelCount; ++i)
		output[i] = left[i] / right[i];
}
