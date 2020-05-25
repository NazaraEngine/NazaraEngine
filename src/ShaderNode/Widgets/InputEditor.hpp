#pragma once

#ifndef NAZARA_SHADERNODES_INPUTEDITOR_HPP
#define NAZARA_SHADERNODES_INPUTEDITOR_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QWidget>
#include <optional>

class QLabel;
class QListWidget;
class QVBoxLayout;

class InputEditor : public QWidget
{
	public:
		InputEditor(ShaderGraph& graph);
		~InputEditor() = default;

	private:
		void OnAddInput();
		void OnEditInput(int inputIndex);
		void OnInputSelectionUpdate(int inputIndex);
		void OnInputListUpdate(ShaderGraph* graph);
		void OnInputUpdate(ShaderGraph* graph, std::size_t inputIndex);
		void RefreshInputs();

		NazaraSlot(ShaderGraph, OnInputListUpdate, m_onInputListUpdateSlot);
		NazaraSlot(ShaderGraph, OnInputUpdate, m_onInputUpdateSlot);

		std::optional<std::size_t> m_currentInputIndex;
		ShaderGraph& m_shaderGraph;
		QListWidget* m_inputList;
		QVBoxLayout* m_layout;
};

#include <ShaderNode/Widgets/InputEditor.inl>

#endif
