#include <Editor/MaterialEditor.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <QtWidgets/QtWidgets>

MaterialEditor::MaterialEditor(QWidget* parent) :
QDockWidget("Material editor", parent),
m_fieldLocked(false)
{
	resize(350, 450);
	setWindowTitle("Material editor");
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_colorDialog = new QColorDialog(this);
	connect(m_colorDialog, &QColorDialog::colorSelected, this, &MaterialEditor::OnColorSelected);

	QWidget* mainWidget = new QWidget;

	QVBoxLayout* verticalLayout = new QVBoxLayout;

	QTabWidget* tabs = new QTabWidget;

	std::unordered_map<Nz::String, QFormLayout*> pages;

	for (const MaterialField& field : s_fields)
	{
		auto it = pages.find(field.group);
		if (it == pages.end())
		{
			QFormLayout* form = new QFormLayout;

			QWidget* page = new QWidget;
			page->setLayout(form);

			tabs->addTab(page, field.group);

			it = pages.emplace(field.group, form).first;
		}

		QWidget* editWidget = nullptr;
		QLabel* label = new QLabel(field.name);

		switch (field.type)
		{
			case FieldType::Boolean:
			{
				QCheckBox* checkbox = new QCheckBox;
				connect(checkbox, &QCheckBox::stateChanged, [this, index = m_fields.size()](int)
				{
					OnFieldUpdate(index);
				});

				editWidget = checkbox;
				break;
			}

			case FieldType::Color:
			{
				QPushButton* button = new QPushButton;
				connect(button, &QPushButton::clicked, [this, index = m_fields.size()](bool)
				{
					PrepareColorPicker(index);
				});

				editWidget = button;
				break;
			}

			case FieldType::FilePath:
				break;

			case FieldType::String:
			{
				QLineEdit* lineEdit = new QLineEdit;
				connect(lineEdit, &QLineEdit::textChanged, [this, index = m_fields.size()](const QString&)
				{
					OnFieldUpdate(index);
				});

				editWidget = lineEdit;
				break;
			}
		}

		QPushButton* resetButton = new QPushButton("R");
		connect(resetButton, &QPushButton::clicked, [this, index = m_fields.size()](bool)
		{
			OnFieldReset(index);
		});
		resetButton->setMaximumWidth(resetButton->fontMetrics().boundingRect('R').width() * 5);

		QHBoxLayout* horizontalLayout = new QHBoxLayout;
		horizontalLayout->addWidget(editWidget);
		horizontalLayout->addWidget(resetButton);

		it->second->addRow(label, horizontalLayout);

		m_fields.push_back({&field, label, editWidget});
	}
	
	verticalLayout->addWidget(tabs);

	QPushButton* updateButton = new QPushButton("Update");
	connect(updateButton, &QPushButton::clicked, this, &MaterialEditor::OnUpdatePressed);

	verticalLayout->addWidget(updateButton);

	QPushButton* resetButton = new QPushButton("Reset");
	verticalLayout->addWidget(resetButton);

	mainWidget->setLayout(verticalLayout);
	setWidget(mainWidget);
}

MaterialEditor::~MaterialEditor()
{
}

void MaterialEditor::FillValues(std::size_t matIndex, Nz::ParameterList list)
{
	m_materialIndex = matIndex;
	m_parameters = std::move(list);

	for (std::size_t i = 0; i < m_fields.size(); ++i)
		OnFieldReset(i);
}

void MaterialEditor::OnColorSelected(const QColor& color)
{
	OnFieldUpdate(m_currentField);

	FieldData& data = m_fields[m_currentField];
	QPushButton* button = static_cast<QPushButton*>(data.widget);

	button->setText(color.name(QColor::HexArgb).toUpper());
	button->setStyleSheet("background-color:" + button->text() + ';');
}

void MaterialEditor::OnFieldReset(std::size_t fieldIndex)
{
	m_fieldLocked = true;
	Nz::CallOnExit resetLock([this] () { m_fieldLocked = false; });

	FieldData& data = m_fields[fieldIndex];
	data.label->setText(data.matField->name);

	switch (data.matField->type)
	{
		case FieldType::Boolean:
		{
			bool value = false;
			m_parameters.GetBooleanParameter(data.matField->key, &value);

			QCheckBox* checkbox = static_cast<QCheckBox*>(data.widget);
			checkbox->setChecked(value);
			break;
		}

		case FieldType::Color:
		{
			Nz::Color value;
			m_parameters.GetColorParameter(data.matField->key, &value);

			QColor color(value.r, value.g, value.b, value.a);

			QPushButton* button = static_cast<QPushButton*>(data.widget);
			button->setText(color.name(QColor::HexArgb).toUpper());
			button->setStyleSheet("background-color:" + button->text() + ';');
			break;
		}

		case FieldType::FilePath:
			break;

		case FieldType::String:
		{
			Nz::String value;
			m_parameters.GetStringParameter(data.matField->key, &value);

			QLineEdit* lineEdit = static_cast<QLineEdit*>(data.widget);
			lineEdit->setText(value.GetConstBuffer());
			break;
		}
	}

	m_changedFields.UnboundedReset(fieldIndex);
}

void MaterialEditor::OnFieldUpdate(std::size_t fieldIndex)
{
	if (m_fieldLocked || m_changedFields.UnboundedTest(fieldIndex))
		return;

	m_changedFields.UnboundedSet(fieldIndex);
	m_fields[fieldIndex].label->setText("<b>" + QString(m_fields[fieldIndex].matField->name) + "</b>");
}

void MaterialEditor::OnUpdatePressed(bool checked)
{
	NazaraUnused(checked);

	for (auto fieldIndex = m_changedFields.FindFirst(); fieldIndex != m_changedFields.npos; fieldIndex = m_changedFields.FindNext(fieldIndex))
	{
		FieldData& data = m_fields[fieldIndex];

		switch (data.matField->type)
		{
			case FieldType::Boolean:
			{
				QCheckBox* checkbox = static_cast<QCheckBox*>(data.widget);
				m_parameters.SetParameter(data.matField->key, checkbox->isChecked());
				break;
			}

			case FieldType::Color:
			{
				QPushButton* button = static_cast<QPushButton*>(data.widget);
				QColor color = button->palette().color(QPalette::Button);

				m_parameters.SetParameter(data.matField->key, Nz::Color(color.red(), color.green(), color.blue(), color.alpha()));
				break;
			}

			case FieldType::FilePath:
				break;

			case FieldType::String:
			{
				QLineEdit* lineEdit = static_cast<QLineEdit*>(data.widget);
				m_parameters.SetParameter(data.matField->key, Nz::String(lineEdit->text().toUtf8().constData()));
				break;
			}
		}
	}

	OnMaterialEditorSave(this, m_materialIndex, m_parameters);
}

void MaterialEditor::PrepareColorPicker(std::size_t fieldIndex)
{
	FieldData& data = m_fields[fieldIndex];
	QPushButton* button = static_cast<QPushButton*>(data.widget);

	m_currentField = fieldIndex;

	m_colorDialog->setCurrentColor(button->palette().color(QPalette::Button));
	m_colorDialog->open();
}

std::array<MaterialEditor::MaterialField, 20> MaterialEditor::s_fields =
{
	{
		{"general", Nz::MaterialData::Name, Nz::MaterialData::Name, FieldType::String},
		{"general", Nz::MaterialData::AlphaTest, Nz::MaterialData::AlphaTest, FieldType::Boolean},
		{"general", Nz::MaterialData::ColorWrite, Nz::MaterialData::ColorWrite, FieldType::Boolean},
		{"general", Nz::MaterialData::DepthBuffer, Nz::MaterialData::DepthBuffer, FieldType::Boolean},
		{"general", Nz::MaterialData::DepthWrite, Nz::MaterialData::DepthWrite, FieldType::Boolean},
		{"general", Nz::MaterialData::DepthSorting, Nz::MaterialData::DepthSorting, FieldType::Boolean},
		{"general", Nz::MaterialData::FaceCulling, Nz::MaterialData::FaceCulling, FieldType::Boolean},
		{"general", Nz::MaterialData::Lighting, Nz::MaterialData::Lighting, FieldType::Boolean},
		{"general", Nz::MaterialData::ScissorTest, Nz::MaterialData::ScissorTest, FieldType::Boolean},
		{"general", Nz::MaterialData::StencilTest, Nz::MaterialData::StencilTest, FieldType::Boolean},
		{"general", Nz::MaterialData::Transform, Nz::MaterialData::Transform, FieldType::Boolean},

		{"textures", Nz::MaterialData::AmbientColor, Nz::MaterialData::AmbientColor, FieldType::Color},
		{"textures", Nz::MaterialData::AlphaTexturePath, Nz::MaterialData::AlphaTexturePath, FieldType::String},
		{"textures", Nz::MaterialData::DiffuseColor, Nz::MaterialData::DiffuseColor, FieldType::Color},
		{"textures", Nz::MaterialData::DiffuseTexturePath, Nz::MaterialData::DiffuseTexturePath, FieldType::String},
		{"textures", Nz::MaterialData::EmissiveTexturePath, Nz::MaterialData::EmissiveTexturePath, FieldType::String},
		{"textures", Nz::MaterialData::HeightTexturePath, Nz::MaterialData::HeightTexturePath, FieldType::String},
		{"textures", Nz::MaterialData::NormalTexturePath, Nz::MaterialData::NormalTexturePath, FieldType::String},
		{"textures", Nz::MaterialData::SpecularColor, Nz::MaterialData::SpecularColor, FieldType::Color},
		{"textures", Nz::MaterialData::SpecularTexturePath, Nz::MaterialData::SpecularTexturePath, FieldType::String}
	}
};

#include <Editor/MaterialEditor_moc.inl>