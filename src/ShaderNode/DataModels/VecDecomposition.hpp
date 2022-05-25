#pragma once

#ifndef NAZARA_SHADERNODES_VECTOR_DECOMPOSITION_HPP
#define NAZARA_SHADERNODES_VECTOR_DECOMPOSITION_HPP

#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <array>

class VecDecomposition : public ShaderNode
{
	public:
		VecDecomposition(ShaderGraph& graph);
		~VecDecomposition() = default;

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;

		QString caption() const override;
		QString name() const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
		unsigned int nPorts(QtNodes::PortType portType) const override;

		QString portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
		bool portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	private:
		void UpdateOutputs();

		std::shared_ptr<VecData> m_input;
		std::array<std::shared_ptr<FloatData>, 4> m_outputs;
};

#include <ShaderNode/DataModels/VecDecomposition.inl>

#endif
