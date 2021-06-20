#pragma once

#ifndef NAZARA_SHADERNODES_CODEOUTPUTWIDGET_HPP
#define NAZARA_SHADERNODES_CODEOUTPUTWIDGET_HPP

#include <ShaderNode/Enums.hpp>
#include <QtWidgets/QWidget>

class QCheckBox;
class QComboBox;
class QTextEdit;
class ShaderGraph;

class CodeOutputWidget : public QWidget
{
	public:
		CodeOutputWidget(const ShaderGraph& shaderGraph);
		~CodeOutputWidget() = default;

		void Refresh();

	private:
		const ShaderGraph& m_shaderGraph;
		QCheckBox* m_optimisationCheckbox;
		QComboBox* m_outputLang;
		QTextEdit* m_textOutput;
};

#include <ShaderNode/Widgets/CodeOutputWidget.inl>

#endif
