#pragma once

#ifndef NAZARA_SHADERNODES_BUFFEREDITDIALOG_HPP
#define NAZARA_SHADERNODES_BUFFEREDITDIALOG_HPP

#include <ShaderNode/Enums.hpp>
#include <QtWidgets/QDialog>

class QComboBox;
class QLineEdit;
class QSpinBox;
class ShaderGraph;

struct BufferInfo
{
	std::size_t bindingIndex;
	std::size_t setIndex;
	std::size_t structIndex;
	std::string name;
	BufferType type;
};

class BufferEditDialog : public QDialog
{
	public:
		BufferEditDialog(const ShaderGraph& shaderGraph, QWidget* parent = nullptr);
		BufferEditDialog(const ShaderGraph& shaderGraph, const BufferInfo& output, QWidget* parent = nullptr);
		~BufferEditDialog() = default;

		BufferInfo GetBufferInfo() const;

	private:
		void OnAccept();

		const ShaderGraph& m_shaderGraph;
		QComboBox* m_typeList;
		QComboBox* m_structList;
		QLineEdit* m_outputName;
		QSpinBox* m_bindingIndex;
		QSpinBox* m_setIndex;
};

#include <ShaderNode/Widgets/BufferEditDialog.inl>

#endif
