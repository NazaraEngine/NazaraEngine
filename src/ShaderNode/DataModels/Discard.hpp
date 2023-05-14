#pragma once

#ifndef NAZARA_SHADERNODES_DISCARD_HPP
#define NAZARA_SHADERNODES_DISCARD_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

class QFormLayout;

class Discard : public ShaderNode
{
	public:
		Discard(ShaderGraph& graph);

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;
		int GetOutputOrder() const override;

		QString caption() const override { return "Discard"; }
		QString name() const override { return "Discard"; }

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;
};

#include <ShaderNode/DataModels/Discard.inl>

#endif
