#pragma once

#ifndef NAZARA_SHADERNODES_SHADERINFODIALOG_HPP
#define NAZARA_SHADERNODES_SHADERINFODIALOG_HPP

#include <ShaderNode/Enums.hpp>
#include <QtWidgets/QDialog>

class QComboBox;

struct ShaderInfo
{
	ShaderType type;
};

class ShaderInfoDialog : public QDialog
{
	public:
		ShaderInfoDialog(QWidget* parent = nullptr);
		ShaderInfoDialog(const ShaderInfo& shader, QWidget* parent = nullptr);
		~ShaderInfoDialog() = default;

		ShaderInfo GetShaderInfo() const;

	private:
		void OnAccept();

		QComboBox* m_typeList;
};

#include <ShaderNode/Widgets/ShaderInfoDialog.inl>

#endif
