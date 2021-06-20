#include <ShaderNode/DataTypes/Matrix4Data.hpp>

inline QtNodes::NodeDataType Matrix4Data::type() const
{
	return Type();
}

inline QtNodes::NodeDataType Matrix4Data::Type()
{
	return { "mat4", "Matrix4x4" };
}
