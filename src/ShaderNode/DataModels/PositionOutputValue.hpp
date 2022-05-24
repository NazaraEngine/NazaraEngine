#pragma once

#ifndef NAZARA_SHADERNODES_POSITIONOUTPUTVALUE_HPP
#define NAZARA_SHADERNODES_POSITIONOUTPUTVALUE_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

class QFormLayout;

class PositionOutputValue : public ShaderNode
{
	public:
		PositionOutputValue(ShaderGraph& graph);

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;

		QString caption() const override { return "PositionOutputValue"; }
		QString name() const override { return "PositionOutputValue"; }

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	private:
		std::shared_ptr<VecData> m_input;
};

#include <ShaderNode/DataModels/OutputValue.inl>

#endif
