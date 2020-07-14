#include <ShaderNode/Widgets/InputEditDialog.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

InputEditDialog::InputEditDialog(QWidget* parent) :
QDialog(parent)
{
	setWindowTitle(tr("Input edit dialog"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_inputName = new QLineEdit;

	m_typeList = new QComboBox;
	for (std::size_t i = 0; i < PrimitiveTypeCount; ++i)
		m_typeList->addItem(EnumToString(static_cast<PrimitiveType>(i)));

	m_roleList = new QComboBox;
	for (std::size_t i = 0; i < InputRoleCount; ++i)
		m_roleList->addItem(EnumToString(static_cast<InputRole>(i)));

	m_locationIndex = new QSpinBox;

	m_roleIndex = new QSpinBox;

	QFormLayout* formLayout = new QFormLayout;
	formLayout->addRow(tr("Name"), m_inputName);
	formLayout->addRow(tr("Type"), m_typeList);
	formLayout->addRow(tr("Role"), m_roleList);
	formLayout->addRow(tr("Role index"), m_roleIndex);
	formLayout->addRow(tr("Input index"), m_locationIndex);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &InputEditDialog::OnAccept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	verticalLayout->addLayout(formLayout);
	verticalLayout->addWidget(buttonBox);

	setLayout(verticalLayout);
}

InputEditDialog::InputEditDialog(const InputInfo& input, QWidget* parent) :
InputEditDialog(parent)
{
	m_inputName->setText(QString::fromStdString(input.name));
	m_locationIndex->setValue(int(input.locationIndex));
	m_roleIndex->setValue(int(input.roleIndex));
	m_roleList->setCurrentText(EnumToString(input.role));
	m_typeList->setCurrentText(EnumToString(input.type));
}

InputInfo InputEditDialog::GetInputInfo() const
{
	InputInfo inputInfo;
	inputInfo.locationIndex = static_cast<std::size_t>(m_locationIndex->value());
	inputInfo.name = m_inputName->text().toStdString();
	inputInfo.role = static_cast<InputRole>(m_roleList->currentIndex());
	inputInfo.roleIndex = static_cast<std::size_t>(m_roleIndex->value());
	inputInfo.type = static_cast<PrimitiveType>(m_typeList->currentIndex());

	return inputInfo;
}

void InputEditDialog::OnAccept()
{
	if (m_inputName->text().isEmpty())
	{
		QMessageBox::critical(this, tr("Empty name"), tr("Input name must be set"), QMessageBox::Ok);
		return;
	}

	if (m_typeList->currentIndex() < 0)
	{
		QMessageBox::critical(this, tr("Invalid type"), tr("You must select a type"), QMessageBox::Ok);
		return;
	}

	accept();
}
