#include <ShaderNode/Widgets/TextureEditDialog.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

TextureEditDialog::TextureEditDialog(QWidget* parent) :
QDialog(parent)
{
	setWindowTitle(tr("Texture edit dialog"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_textureName = new QLineEdit;

	m_typeList = new QComboBox;
	for (std::size_t i = 0; i < TextureTypeCount; ++i)
		m_typeList->addItem(EnumToString(static_cast<TextureType>(i)));

	m_bindingIndex = new QSpinBox;
	m_setIndex = new QSpinBox;

	QFormLayout* formLayout = new QFormLayout;
	formLayout->addRow(tr("Name"), m_textureName);
	formLayout->addRow(tr("Type"), m_typeList);
	formLayout->addRow(tr("Set index"), m_setIndex);
	formLayout->addRow(tr("Binding index"), m_bindingIndex);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &TextureEditDialog::OnAccept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	verticalLayout->addLayout(formLayout);
	verticalLayout->addWidget(buttonBox);

	setLayout(verticalLayout);
}

TextureEditDialog::TextureEditDialog(const TextureInfo& texture, QWidget* parent) :
TextureEditDialog(parent)
{
	m_bindingIndex->setValue(int(texture.bindingIndex));
	m_setIndex->setValue(int(texture.setIndex));
	m_textureName->setText(QString::fromStdString(texture.name));
	m_typeList->setCurrentText(EnumToString(texture.type));
}

TextureInfo TextureEditDialog::GetTextureInfo() const
{
	TextureInfo inputInfo;
	inputInfo.bindingIndex = static_cast<std::size_t>(m_bindingIndex->value());
	inputInfo.name = m_textureName->text().toStdString();
	inputInfo.setIndex = static_cast<std::size_t>(m_setIndex->value());
	inputInfo.type = static_cast<TextureType>(m_typeList->currentIndex());

	return inputInfo;
}

void TextureEditDialog::OnAccept()
{
	if (m_textureName->text().isEmpty())
	{
		QMessageBox::critical(this, tr("Empty name"), tr("Texture name must be set"), QMessageBox::Ok);
		return;
	}

	if (m_typeList->currentIndex() < 0)
	{
		QMessageBox::critical(this, tr("Invalid type"), tr("You must select a type"), QMessageBox::Ok);
		return;
	}

	accept();
}
