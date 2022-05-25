#pragma once

#ifndef NAZARA_SHADERNODES_CONDITIONALEXPRESSION_HPP
#define NAZARA_SHADERNODES_CONDITIONALEXPRESSION_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <optional>
#include <string>
#include <vector>

class ConditionalExpression : public ShaderNode
{
	public:
		ConditionalExpression(ShaderGraph& graph);
		~ConditionalExpression() = default;

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;
		void BuildNodeEdition(QFormLayout* layout) override;

		QString caption() const override;
		QString name() const override;

		unsigned int nPorts(QtNodes::PortType portType) const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		QString portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
		bool portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void restore(const QJsonObject& data) override;
		QJsonObject save() const override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;
		void OnOptionListUpdate();
		void UpdateOptionText();

		NazaraSlot(ShaderGraph, OnOptionListUpdate, m_onOptionListUpdateSlot);
		NazaraSlot(ShaderGraph, OnOptionUpdate, m_onOptionUpdateSlot);

		std::optional<std::size_t> m_currentOptionIndex;
		std::shared_ptr<QtNodes::NodeData> m_falsePath;
		std::shared_ptr<QtNodes::NodeData> m_truePath;
		std::string m_currentOptionText;
};

#include <ShaderNode/DataModels/BufferField.inl>

#endif
