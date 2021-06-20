#pragma once

#ifndef NAZARA_SHADERNODES_CONDITIONEDITOR_HPP
#define NAZARA_SHADERNODES_CONDITIONEDITOR_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QWidget>
#include <optional>

class QStandardItemModel;
class QVBoxLayout;

class ConditionEditor : public QWidget
{
	public:
		ConditionEditor(ShaderGraph& graph);
		~ConditionEditor() = default;

	private:
		void OnAddCondition();
		void OnConditionListUpdate(ShaderGraph* graph);
		void OnConditionUpdate(ShaderGraph* graph, std::size_t conditionIndex);
		void OnEditCondition(int inputIndex);
		void RefreshConditions();

		NazaraSlot(ShaderGraph, OnStructListUpdate, m_onConditionListUpdateSlot);
		NazaraSlot(ShaderGraph, OnStructUpdate, m_onConditionUpdateSlot);

		ShaderGraph& m_shaderGraph;
		QStandardItemModel* m_model;
		QVBoxLayout* m_layout;
};

#include <ShaderNode/Widgets/ConditionEditor.inl>

#endif
