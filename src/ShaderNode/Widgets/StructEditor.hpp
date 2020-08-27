#pragma once

#ifndef NAZARA_SHADERNODES_STRUCTEDITOR_HPP
#define NAZARA_SHADERNODES_STRUCTEDITOR_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QWidget>
#include <optional>

class QLabel;
class QListWidget;
class QVBoxLayout;

class StructEditor : public QWidget
{
	public:
		StructEditor(ShaderGraph& graph);
		~StructEditor() = default;

	private:
		void OnAddStruct();
		void OnEditStruct(int inputIndex);
		void OnStructListUpdate(ShaderGraph* graph);
		void OnStructUpdate(ShaderGraph* graph, std::size_t inputIndex);
		void RefreshStructs();

		NazaraSlot(ShaderGraph, OnStructListUpdate, m_onStructListUpdateSlot);
		NazaraSlot(ShaderGraph, OnStructUpdate, m_onStructUpdateSlot);

		ShaderGraph& m_shaderGraph;
		QListWidget* m_structList;
		QVBoxLayout* m_layout;
};

#include <ShaderNode/Widgets/StructEditor.inl>

#endif
