#include <ShaderNode/DataModels/Discard.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataTypes/BoolData.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/Matrix4Data.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <limits>

Discard::Discard(ShaderGraph& graph) :
ShaderNode(graph)
{
	DisablePreview();
	DisableCustomVariableName();
}

Nz::ShaderNodes::NodePtr Discard::BuildNode(Nz::ShaderNodes::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	using namespace Nz::ShaderBuilder;

	assert(count == 1);
	assert(outputIndex == 0);

	return Branch(Equal(expressions[0], Constant(true)), Nz::ShaderBuilder::Discard(), nullptr);
}

int Discard::GetOutputOrder() const
{
	return std::numeric_limits<int>::lowest();
}

QtNodes::NodeDataType Discard::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::In);
	assert(portIndex == 0);

	return BoolData::Type();
}

unsigned int Discard::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return 1;
		case QtNodes::PortType::Out: return 0;
	}

	return 0;
}

std::shared_ptr<QtNodes::NodeData> Discard::outData(QtNodes::PortIndex port)
{
	return {};
}
