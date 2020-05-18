#include <DataModels/VecValue.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>

Nz::ShaderAst::ExpressionPtr Vec4Value::GetExpression(Nz::ShaderAst::ExpressionPtr* /*expressions*/, std::size_t count) const
{
	assert(count == 0);

	return Nz::ShaderBuilder::Constant(GetValue());
}

auto Vec4Value::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const -> QtNodes::NodeDataType
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex == 0);

	return Vec4Data::Type();
}

std::shared_ptr<QtNodes::NodeData> Vec4Value::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	return std::make_shared<Vec4Data>(GetValue());
}

Nz::Vector4f Vec4Value::GetValue() const
{
	float x = float(m_values[0]->value());
	float y = float(m_values[1]->value());
	float z = float(m_values[2]->value());
	float w = float(m_values[3]->value());

	return Nz::Vector4f(x, y, z, w);
}
