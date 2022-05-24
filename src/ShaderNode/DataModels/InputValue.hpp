#pragma once

#ifndef NAZARA_SHADERNODES_INPUTVALUE_HPP
#define NAZARA_SHADERNODES_INPUTVALUE_HPP

#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <array>
#include <optional>

class InputValue : public ShaderNode
{
	public:
		InputValue(ShaderGraph& graph);
		~InputValue() = default;

		void BuildNodeEdition(QFormLayout* layout) override;

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;

		QString caption() const override { return "Input"; }
		QString name() const override { return "Input"; }

		unsigned int nPorts(QtNodes::PortType portType) const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		QString portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
		bool portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;
		void OnInputListUpdate();
		void UpdateInputText();

		void restore(const QJsonObject& data) override;
		QJsonObject save() const override;

		NazaraSlot(ShaderGraph, OnInputListUpdate, m_onInputListUpdateSlot);
		NazaraSlot(ShaderGraph, OnInputUpdate, m_onInputUpdateSlot);

		std::optional<std::size_t> m_currentInputIndex;
		std::string m_currentInputText;
};

#include <ShaderNode/DataModels/InputValue.inl>

#endif
