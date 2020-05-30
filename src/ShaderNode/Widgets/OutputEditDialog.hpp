#pragma once

#ifndef NAZARA_SHADERNODES_OUTPUTEDITDIALOG_HPP
#define NAZARA_SHADERNODES_OUTPUTEDITDIALOG_HPP

#include <ShaderNode/Enums.hpp>
#include <QtWidgets/QDialog>

class QComboBox;
class QLineEdit;

struct OutputInfo
{
	std::string name;
	InOutType type;
};

class OutputEditDialog : public QDialog
{
	public:
		OutputEditDialog(QWidget* parent = nullptr);
		OutputEditDialog(const OutputInfo& input, QWidget* parent = nullptr);
		~OutputEditDialog() = default;

		OutputInfo GetOutputInfo() const;

	private:
		void OnAccept();

		QComboBox* m_typeList;
		QLineEdit* m_outputName;
};

#include <ShaderNode/Widgets/OutputEditDialog.inl>

#endif
