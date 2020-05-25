#pragma once

#ifndef NAZARA_SHADERNODES_FRAGMENTOUTPUT_HPP
#define NAZARA_SHADERNODES_FRAGMENTOUTPUT_HPP

#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>

class FragmentOutput : public ShaderNode
{
	public:
		inline FragmentOutput(ShaderGraph& graph);

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const override;

		QString caption() const override { return "Fragment shader output"; }
		QString name() const override { return "FragmentShaderOutput"; }

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		QWidget* embeddedWidget() override;
		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData>, int) override {};

};

#include <ShaderNode/DataModels/FragmentOutput.inl>

#endif
