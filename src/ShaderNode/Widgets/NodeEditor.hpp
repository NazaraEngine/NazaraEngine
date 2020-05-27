#pragma once

#ifndef NAZARA_SHADERNODES_NODEEDITOR_HPP
#define NAZARA_SHADERNODES_NODEEDITOR_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QWidget>
#include <optional>

class NodeEditor : public QWidget
{
	public:
		NodeEditor();
		~NodeEditor() = default;

		void Clear();

		template<typename F> void UpdateContent(QString nodeName, F&& callback);

	private:
		QFormLayout* m_layout;
};

#include <ShaderNode/Widgets/NodeEditor.inl>

#endif
