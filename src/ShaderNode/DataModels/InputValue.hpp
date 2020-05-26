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

		void BuildNodeEdition(QVBoxLayout* layout) override;

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* /*expressions*/, std::size_t count) const override;

		QString caption() const override { return "Input"; }
		QString name() const override { return "Input"; }

		unsigned int nPorts(QtNodes::PortType portType) const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;
		void OnInputListUpdate();

		NazaraSlot(ShaderGraph, OnInputListUpdate, m_onInputListUpdateSlot);
		NazaraSlot(ShaderGraph, OnInputUpdate, m_onInputUpdateSlot);

		std::optional<std::size_t> m_currentInputIndex;
		std::string m_currentInputText;
};

#include <ShaderNode/DataModels/InputValue.inl>

#endif
