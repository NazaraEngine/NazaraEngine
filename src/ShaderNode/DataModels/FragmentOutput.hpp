#pragma once

#ifndef NAZARA_SHADERNODES_FRAGMENTOUTPUT_HPP
#define NAZARA_SHADERNODES_FRAGMENTOUTPUT_HPP

#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
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

		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;

		std::shared_ptr<Vec4Data> m_input;
};

#include <ShaderNode/DataModels/FragmentOutput.inl>

#endif
