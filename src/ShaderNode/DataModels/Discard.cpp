#include <ShaderNode/DataModels/Discard.hpp>
#include <NZSL/ShaderBuilder.hpp>
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

nzsl::Ast::NodePtr Discard::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 1);
	assert(outputIndex == 0);

	using namespace nzsl;

	auto condition = ShaderBuilder::Binary(nzsl::Ast::BinaryType::CompEq, std::move(expressions[0]), ShaderBuilder::ConstantValue(true));
	return ShaderBuilder::Branch(std::move(condition), ShaderBuilder::Discard());
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
		case QtNodes::PortType::None:
			break;

		case QtNodes::PortType::In: return 1;
		case QtNodes::PortType::Out: return 0;
	}

	return 0;
}

std::shared_ptr<QtNodes::NodeData> Discard::outData(QtNodes::PortIndex /*port*/)
{
	return {};
}
