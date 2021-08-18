#include <ShaderNode/Widgets/OptionEditDialog.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

OptionEditDialog::OptionEditDialog(QWidget* parent) :
QDialog(parent)
{
	setWindowTitle(tr("Option edit dialog"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_optionName = new QLineEdit;

	QFormLayout* formLayout = new QFormLayout;
	formLayout->addRow(tr("Name"), m_optionName);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &OptionEditDialog::OnAccept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	verticalLayout->addLayout(formLayout);
	verticalLayout->addWidget(buttonBox);

	setLayout(verticalLayout);
}

OptionEditDialog::OptionEditDialog(const OptionInfo& option, QWidget* parent) :
OptionEditDialog(parent)
{
	m_optionName->setText(QString::fromStdString(option.name));
}

OptionInfo OptionEditDialog::GetOptionInfo() const
{
	OptionInfo inputInfo;
	inputInfo.name = m_optionName->text().toStdString();

	return inputInfo;
}

void OptionEditDialog::OnAccept()
{
	if (m_optionName->text().isEmpty())
	{
		QMessageBox::critical(this, tr("Empty name"), tr("Option name must be set"), QMessageBox::Ok);
		return;
	}

	accept();
}
