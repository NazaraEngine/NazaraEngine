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

		void BuildNodeEdition(QFormLayout* layout) override;

		Nz::ShaderNodes::ExpressionPtr GetExpression(Nz::ShaderNodes::ExpressionPtr* /*expressions*/, std::size_t count) const override;

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
		void OnConditionListUpdate();
		void UpdateConditionText();

		NazaraSlot(ShaderGraph, OnConditionListUpdate, m_onConditionListUpdateSlot);
		NazaraSlot(ShaderGraph, OnConditionUpdate, m_onConditionUpdateSlot);

		std::optional<std::size_t> m_currentConditionIndex;
		std::shared_ptr<QtNodes::NodeData> m_falsePath;
		std::shared_ptr<QtNodes::NodeData> m_truePath;
		std::string m_currentConditionText;
};

#include <ShaderNode/DataModels/BufferField.inl>

#endif
