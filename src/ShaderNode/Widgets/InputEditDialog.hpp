#pragma once

#ifndef NAZARA_SHADERNODES_INPUTEDITDIALOG_HPP
#define NAZARA_SHADERNODES_INPUTEDITDIALOG_HPP

#include <Enums.hpp>
#include <QtWidgets/QDialog>

class QComboBox;
class QLineEdit;
class QSpinBox;

struct InputInfo
{
	std::size_t roleIndex;
	std::string name;
	InputRole role;
	InputType type;
};

class InputEditDialog : public QDialog
{
	public:
		InputEditDialog(QWidget* parent = nullptr);
		InputEditDialog(const InputInfo& input, QWidget* parent = nullptr);
		~InputEditDialog() = default;

		InputInfo GetInputInfo() const;

	private:
		void OnAccept();

		QComboBox* m_roleList;
		QComboBox* m_typeList;
		QLineEdit* m_inputName;
		QSpinBox* m_roleIndex;
};

#include <Widgets/InputEditor.inl>

#endif
