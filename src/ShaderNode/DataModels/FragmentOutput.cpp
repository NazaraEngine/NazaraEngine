#include <DataModels/FragmentOutput.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <DataModels/VecValue.hpp>

Nz::ShaderAst::ExpressionPtr FragmentOutput::GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const
{
	using namespace Nz::ShaderAst;
	using namespace Nz::ShaderBuilder;

	assert(count == 1);

	auto output = Nz::ShaderBuilder::Output("RenderTarget0", ExpressionType::Float4);

	return Nz::ShaderBuilder::Assign(output, *expressions);
}

QtNodes::NodeDataType FragmentOutput::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::In);
	assert(portIndex == 0);

	return Vec4Data::Type();
}

QWidget* FragmentOutput::embeddedWidget()
{
	return nullptr;
}

unsigned int FragmentOutput::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return 1;
		case QtNodes::PortType::Out: return 0;
	}

	return 0;
}

std::shared_ptr<QtNodes::NodeData> FragmentOutput::outData(QtNodes::PortIndex /*port*/)
{
	return {};
}
