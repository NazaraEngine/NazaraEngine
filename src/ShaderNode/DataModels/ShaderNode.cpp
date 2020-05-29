#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

ShaderNode::ShaderNode(ShaderGraph& graph) :
m_previewSize(64, 64),
m_pixmapLabel(nullptr),
m_graph(graph),
m_enableCustomVariableName(true),
m_isPreviewEnabled(false)
{
	m_pixmapLabel = new QLabel;
	m_pixmapLabel->setStyleSheet("background-color: rgba(0,0,0,0)");
}

void ShaderNode::BuildNodeEdition(QFormLayout* layout)
{
	QCheckBox* checkbox = new QCheckBox;
	checkbox->setCheckState((m_isPreviewEnabled) ? Qt::Checked : Qt::Unchecked);

	connect(checkbox, &QCheckBox::stateChanged, [&](int state)
	{
		EnablePreview(state == Qt::Checked);
	});

	layout->addRow(tr("Enable preview"), checkbox);

	QComboBox* previewSize = new QComboBox;

	int index = 0;
	for (int size : { 32, 64, 128, 256, 512 })
	{
		QString sizeStr = QString::number(size);
		previewSize->addItem(sizeStr + "x" + sizeStr, size);

		if (m_previewSize.x == size)
			previewSize->setCurrentIndex(index);

		index++;
	}
	
	connect(previewSize, qOverload<int>(&QComboBox::currentIndexChanged), [=](int index)
	{
		if (index < 0)
			return;

		int size = previewSize->itemData(index).toInt();
		SetPreviewSize({ size, size });
	});

	layout->addRow(tr("Preview size"), previewSize);

	if (m_enableCustomVariableName)
	{
		QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(m_variableName));
		connect(lineEdit, &QLineEdit::textChanged, [&](const QString& text)
		{
			SetVariableName(text.toStdString());
		});
		layout->addRow(tr("Variable name"), lineEdit);
	}
}

void ShaderNode::EnablePreview(bool enable)
{
	if (m_isPreviewEnabled != enable)
	{
		m_isPreviewEnabled = enable;

		if (m_isPreviewEnabled)
		{
			m_pixmap.emplace(m_previewSize.x, m_previewSize.y);

			UpdatePreview();
		}
		else
		{
			m_pixmapLabel->clear();
			m_pixmap.reset();
		}

		embeddedWidgetSizeUpdated();
	}
}

QWidget* ShaderNode::embeddedWidget()
{
	return m_pixmapLabel;
}

void ShaderNode::setInData(std::shared_ptr<QtNodes::NodeData>, int)
{
}

bool ShaderNode::ComputePreview(QPixmap& /*pixmap*/)
{
	return false;
}

void ShaderNode::UpdatePreview()
{
	if (!m_pixmap)
		return;

	QPixmap& pixmap = *m_pixmap;

	if (!ComputePreview(pixmap))
	{
		pixmap = QPixmap(m_previewSize.x, m_previewSize.y);
		pixmap.fill(QColor::fromRgb(255, 255, 255, 0));
	}
	else
		pixmap = pixmap.scaled(m_previewSize.x, m_previewSize.y);

	m_pixmapLabel->setPixmap(pixmap);
}
