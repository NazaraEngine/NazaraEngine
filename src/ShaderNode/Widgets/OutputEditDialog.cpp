#include <ShaderNode/Widgets/OutputEditDialog.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

OutputEditDialog::OutputEditDialog(QWidget* parent) :
QDialog(parent)
{
	setWindowTitle(tr("Output edit dialog"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_outputName = new QLineEdit;

	m_typeList = new QComboBox;
	for (std::size_t i = 0; i < PrimitiveTypeCount; ++i)
		m_typeList->addItem(EnumToString(static_cast<PrimitiveType>(i)));

	m_locationIndex = new QSpinBox;

	QFormLayout* formLayout = new QFormLayout;
	formLayout->addRow(tr("Name"), m_outputName);
	formLayout->addRow(tr("Type"), m_typeList);
	formLayout->addRow(tr("Output index"), m_locationIndex);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &OutputEditDialog::OnAccept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	verticalLayout->addLayout(formLayout);
	verticalLayout->addWidget(buttonBox);

	setLayout(verticalLayout);
}

OutputEditDialog::OutputEditDialog(const OutputInfo& output, QWidget* parent) :
OutputEditDialog(parent)
{
	m_locationIndex->setValue(int(output.locationIndex));
	m_outputName->setText(QString::fromStdString(output.name));
	m_typeList->setCurrentText(EnumToString(output.type));
}

OutputInfo OutputEditDialog::GetOutputInfo() const
{
	OutputInfo inputInfo;
	inputInfo.locationIndex = static_cast<std::size_t>(m_locationIndex->value());
	inputInfo.name = m_outputName->text().toStdString();
	inputInfo.type = static_cast<PrimitiveType>(m_typeList->currentIndex());

	return inputInfo;
}

void OutputEditDialog::OnAccept()
{
	if (m_outputName->text().isEmpty())
	{
		QMessageBox::critical(this, tr("Empty name"), tr("Output name must be set"), QMessageBox::Ok);
		return;
	}

	if (m_typeList->currentIndex() < 0)
	{
		QMessageBox::critical(this, tr("Invalid type"), tr("You must select a type"), QMessageBox::Ok);
		return;
	}

	accept();
}
