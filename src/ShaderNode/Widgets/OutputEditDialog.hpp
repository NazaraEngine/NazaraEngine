#pragma once

#ifndef NAZARA_SHADERNODES_OUTPUTEDITDIALOG_HPP
#define NAZARA_SHADERNODES_OUTPUTEDITDIALOG_HPP

#include <ShaderNode/Enums.hpp>
#include <QtWidgets/QDialog>

class QComboBox;
class QLineEdit;
class QSpinBox;

struct OutputInfo
{
	std::size_t locationIndex;
	std::string name;
	PrimitiveType type;
};

class OutputEditDialog : public QDialog
{
	public:
		OutputEditDialog(QWidget* parent = nullptr);
		OutputEditDialog(const OutputInfo& output, QWidget* parent = nullptr);
		~OutputEditDialog() = default;

		OutputInfo GetOutputInfo() const;

	private:
		void OnAccept();

		QComboBox* m_typeList;
		QLineEdit* m_outputName;
		QSpinBox* m_locationIndex;
};

#include <ShaderNode/Widgets/OutputEditDialog.inl>

#endif
