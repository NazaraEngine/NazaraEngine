#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>

ShaderNode::ShaderNode(ShaderGraph& graph) :
m_previewSize(64, 64),
m_pixmapLabel(nullptr),
m_graph(graph),
m_forceVariable(false),
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
