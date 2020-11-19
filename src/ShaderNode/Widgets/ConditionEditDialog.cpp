#include <ShaderNode/Widgets/ConditionEditDialog.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

ConditionEditDialog::ConditionEditDialog(QWidget* parent) :
QDialog(parent)
{
	setWindowTitle(tr("Condition edit dialog"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_conditionName = new QLineEdit;

	QFormLayout* formLayout = new QFormLayout;
	formLayout->addRow(tr("Name"), m_conditionName);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &ConditionEditDialog::OnAccept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	verticalLayout->addLayout(formLayout);
	verticalLayout->addWidget(buttonBox);

	setLayout(verticalLayout);
}

ConditionEditDialog::ConditionEditDialog(const ConditionInfo& condition, QWidget* parent) :
ConditionEditDialog(parent)
{
	m_conditionName->setText(QString::fromStdString(condition.name));
}

ConditionInfo ConditionEditDialog::GetConditionInfo() const
{
	ConditionInfo inputInfo;
	inputInfo.name = m_conditionName->text().toStdString();

	return inputInfo;
}

void ConditionEditDialog::OnAccept()
{
	if (m_conditionName->text().isEmpty())
	{
		QMessageBox::critical(this, tr("Empty name"), tr("Condition name must be set"), QMessageBox::Ok);
		return;
	}

	accept();
}
