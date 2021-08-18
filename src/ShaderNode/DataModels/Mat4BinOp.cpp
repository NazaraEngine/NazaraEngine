#include <ShaderNode/DataModels/Mat4BinOp.hpp>

QString Mat4Add::caption() const
{
	static QString caption = "Matrix4 addition";
	return caption;
}

QString Mat4Add::name() const
{
	static QString name = "mat4_add";
	return name;
}

QString Mat4Mul::caption() const
{
	static QString caption = "Matrix4 multiplication";
	return caption;
}

QString Mat4Mul::name() const
{
	static QString name = "mat4_mul";
	return name;
}

QString Mat4Sub::caption() const
{
	static QString caption = "Matrix4 subtraction";
	return caption;
}

QString Mat4Sub::name() const
{
	static QString name = "mat4_sub";
	return name;
}
