#pragma once

#ifndef NAZARA_SHADERNODES_CONDITIONEDITDIALOG_HPP
#define NAZARA_SHADERNODES_CONDITIONEDITDIALOG_HPP

#include <ShaderNode/Enums.hpp>
#include <QtWidgets/QDialog>

class QComboBox;
class QLineEdit;
class QSpinBox;

struct ConditionInfo
{
	std::string name;
};

class ConditionEditDialog : public QDialog
{
	public:
		ConditionEditDialog(QWidget* parent = nullptr);
		ConditionEditDialog(const ConditionInfo& input, QWidget* parent = nullptr);
		~ConditionEditDialog() = default;

		ConditionInfo GetConditionInfo() const;

	private:
		void OnAccept();

		QLineEdit* m_conditionName;
};

#include <ShaderNode/Widgets/ConditionEditDialog.inl>

#endif
