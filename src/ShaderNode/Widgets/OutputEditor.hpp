#pragma once

#ifndef NAZARA_SHADERNODES_OUTPUTEDITOR_HPP
#define NAZARA_SHADERNODES_OUTPUTEDITOR_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QWidget>
#include <optional>

class QLabel;
class QListWidget;
class QVBoxLayout;

class OutputEditor : public QWidget
{
	public:
		OutputEditor(ShaderGraph& graph);
		~OutputEditor() = default;

	private:
		void OnAddOutput();
		void OnEditOutput(int inputIndex);
		void OnOutputSelectionUpdate(int inputIndex);
		void OnOutputListUpdate(ShaderGraph* graph);
		void OnOutputUpdate(ShaderGraph* graph, std::size_t inputIndex);
		void RefreshOutputs();

		NazaraSlot(ShaderGraph, OnOutputListUpdate, m_onOutputListUpdateSlot);
		NazaraSlot(ShaderGraph, OnOutputUpdate, m_onOutputUpdateSlot);

		std::optional<std::size_t> m_currentOutputIndex;
		ShaderGraph& m_shaderGraph;
		QListWidget* m_outputList;
		QVBoxLayout* m_layout;
};

#include <ShaderNode/Widgets/OutputEditor.inl>

#endif
