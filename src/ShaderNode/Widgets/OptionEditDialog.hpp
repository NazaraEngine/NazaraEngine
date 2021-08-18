#pragma once

#ifndef NAZARA_SHADERNODES_OPTIONEDITDIALOG_HPP
#define NAZARA_SHADERNODES_OPTIONEDITDIALOG_HPP

#include <ShaderNode/Enums.hpp>
#include <QtWidgets/QDialog>

class QComboBox;
class QLineEdit;
class QSpinBox;

struct OptionInfo
{
	std::string name;
};

class OptionEditDialog : public QDialog
{
	public:
		OptionEditDialog(QWidget* parent = nullptr);
		OptionEditDialog(const OptionInfo& input, QWidget* parent = nullptr);
		~OptionEditDialog() = default;

		OptionInfo GetOptionInfo() const;

	private:
		void OnAccept();

		QLineEdit* m_optionName;
};

#include <ShaderNode/Widgets/OptionEditDialog.inl>

#endif
