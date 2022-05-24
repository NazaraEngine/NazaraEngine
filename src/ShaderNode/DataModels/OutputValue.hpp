#pragma once

#ifndef NAZARA_SHADERNODES_OUTPUTVALUE_HPP
#define NAZARA_SHADERNODES_OUTPUTVALUE_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

class QFormLayout;

class OutputValue : public ShaderNode
{
	public:
		OutputValue(ShaderGraph& graph);

		void BuildNodeEdition(QFormLayout* layout) override;

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;

		QString caption() const override { return "Output"; }
		QString name() const override { return "Output"; }

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		QString portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
		bool portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;
		void OnOutputListUpdate();
		void UpdateOutputText();

		void restore(const QJsonObject& data) override;
		QJsonObject save() const override;

		NazaraSlot(ShaderGraph, OnOutputListUpdate, m_onOutputListUpdateSlot);
		NazaraSlot(ShaderGraph, OnOutputUpdate, m_onOutputUpdateSlot);

		std::optional<std::size_t> m_currentOutputIndex;
		std::shared_ptr<QtNodes::NodeData> m_input;
		std::string m_currentOutputText;
};

#include <ShaderNode/DataModels/OutputValue.inl>

#endif
