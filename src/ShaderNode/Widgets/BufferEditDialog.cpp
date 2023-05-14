#include <ShaderNode/Widgets/BufferEditDialog.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

BufferEditDialog::BufferEditDialog(const ShaderGraph& shaderGraph, QWidget* parent) :
QDialog(parent),
m_shaderGraph(shaderGraph)
{
	setWindowTitle(tr("Buffer edit dialog"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_outputName = new QLineEdit;

	m_typeList = new QComboBox;
	for (std::size_t i = 0; i < BufferTypeCount; ++i)
		m_typeList->addItem(EnumToString(static_cast<BufferType>(i)));

	m_structList = new QComboBox;
	for (const auto& structEntry : m_shaderGraph.GetStructs())
		m_structList->addItem(QString::fromStdString(structEntry.name));

	m_bindingIndex = new QSpinBox;
	m_setIndex = new QSpinBox;

	QFormLayout* formLayout = new QFormLayout;
	formLayout->addRow(tr("Name"), m_outputName);
	formLayout->addRow(tr("Type"), m_typeList);
	formLayout->addRow(tr("Struct"), m_structList);
	formLayout->addRow(tr("Set index"), m_setIndex);
	formLayout->addRow(tr("Binding index"), m_bindingIndex);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &BufferEditDialog::OnAccept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	verticalLayout->addLayout(formLayout);
	verticalLayout->addWidget(buttonBox);

	setLayout(verticalLayout);
}

BufferEditDialog::BufferEditDialog(const ShaderGraph& shaderGraph, const BufferInfo& buffer, QWidget* parent) :
BufferEditDialog(shaderGraph, parent)
{
	m_bindingIndex->setValue(int(buffer.bindingIndex));
	m_setIndex->setValue(int(buffer.setIndex));
	m_outputName->setText(QString::fromStdString(buffer.name));
	m_structList->setCurrentIndex(int(buffer.structIndex));
	m_typeList->setCurrentIndex(int(buffer.type));
}

BufferInfo BufferEditDialog::GetBufferInfo() const
{
	BufferInfo bufferInfo;
	bufferInfo.bindingIndex = static_cast<std::size_t>(m_bindingIndex->value());
	bufferInfo.name = m_outputName->text().toStdString();
	bufferInfo.setIndex = static_cast<std::size_t>(m_setIndex->value());
	bufferInfo.structIndex = m_structList->currentIndex();
	bufferInfo.type = static_cast<BufferType>(m_typeList->currentIndex());

	return bufferInfo;
}

void BufferEditDialog::OnAccept()
{
	if (m_outputName->text().isEmpty())
	{
		QMessageBox::critical(this, tr("Empty name"), tr("Buffer name must be set"), QMessageBox::Ok);
		return;
	}

	if (m_structList->currentIndex() < 0)
	{
		QMessageBox::critical(this, tr("Invalid struct"), tr("You must select a struct"), QMessageBox::Ok);
		return;
	}

	if (m_typeList->currentIndex() < 0)
	{
		QMessageBox::critical(this, tr("Invalid type"), tr("You must select a type"), QMessageBox::Ok);
		return;
	}

	accept();
}
