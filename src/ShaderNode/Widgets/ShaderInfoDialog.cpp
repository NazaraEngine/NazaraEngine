#include <ShaderNode/Widgets/ShaderInfoDialog.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

ShaderInfoDialog::ShaderInfoDialog(QWidget* parent) :
QDialog(parent)
{
	setWindowTitle(tr("Shader edit dialog"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_typeList = new QComboBox;
	for (std::size_t i = 0; i < ShaderTypeCount; ++i)
		m_typeList->addItem(EnumToString(static_cast<ShaderType>(i)));

	QFormLayout* formLayout = new QFormLayout;
	formLayout->addRow(tr("Type"), m_typeList);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &ShaderInfoDialog::OnAccept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	verticalLayout->addLayout(formLayout);
	verticalLayout->addWidget(buttonBox);

	setLayout(verticalLayout);
}

ShaderInfoDialog::ShaderInfoDialog(const ShaderInfo& shader, QWidget* parent) :
ShaderInfoDialog(parent)
{
	m_typeList->setCurrentText(QString(EnumToString(shader.type)));
}

ShaderInfo ShaderInfoDialog::GetShaderInfo() const
{
	ShaderInfo bufferInfo;
	bufferInfo.type = static_cast<ShaderType>(m_typeList->currentIndex());

	return bufferInfo;
}

void ShaderInfoDialog::OnAccept()
{
	if (m_typeList->currentIndex() < 0)
	{
		QMessageBox::critical(this, tr("Invalid shader type"), tr("You must select a shader type"), QMessageBox::Ok);
		return;
	}

	accept();
}
