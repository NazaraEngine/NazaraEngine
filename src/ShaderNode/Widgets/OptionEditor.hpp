#pragma once

#ifndef NAZARA_SHADERNODES_OPTIONEDITOR_HPP
#define NAZARA_SHADERNODES_OPTIONEDITOR_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QWidget>
#include <optional>

class QStandardItemModel;
class QVBoxLayout;

class OptionEditor : public QWidget
{
	public:
		OptionEditor(ShaderGraph& graph);
		~OptionEditor() = default;

	private:
		void OnAddOption();
		void OnOptionListUpdate(ShaderGraph* graph);
		void OnOptionUpdate(ShaderGraph* graph, std::size_t optionIndex);
		void OnEditOption(int optionIndex);
		void RefreshOptions();

		NazaraSlot(ShaderGraph, OnStructListUpdate, m_onOptionListUpdateSlot);
		NazaraSlot(ShaderGraph, OnStructUpdate, m_onOptionUpdateSlot);

		ShaderGraph& m_shaderGraph;
		QStandardItemModel* m_model;
		QVBoxLayout* m_layout;
};

#include <ShaderNode/Widgets/OptionEditor.inl>

#endif
