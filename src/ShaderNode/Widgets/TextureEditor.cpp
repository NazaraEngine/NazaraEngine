#include <ShaderNode/Widgets/TextureEditor.hpp>
#include <ShaderNode/Widgets/TextureEditDialog.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>

TextureEditor::TextureEditor(ShaderGraph& graph) :
m_shaderGraph(graph)
{
	m_textureList = new QListWidget;
	connect(m_textureList, &QListWidget::currentRowChanged, this, &TextureEditor::OnTextureSelectionUpdate);
	connect(m_textureList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item)
	{
		OnEditTexture(m_textureList->row(item));
	});

	m_pixmapLabel = new QLabel;

	QPushButton* addTextureButton = new QPushButton(tr("Add texture..."));
	connect(addTextureButton, &QPushButton::released, this, &TextureEditor::OnAddTexture);

	QPushButton* updateTextureButton = new QPushButton(tr("Load texture preview..."));
	connect(updateTextureButton, &QPushButton::released, this, &TextureEditor::OnLoadTexture);

	m_layout = new QVBoxLayout;
	m_layout->addWidget(m_textureList);
	m_layout->addWidget(addTextureButton);
	m_layout->addWidget(updateTextureButton);
	m_layout->addWidget(m_pixmapLabel);

	setLayout(m_layout);

	m_onTextureListUpdateSlot.Connect(m_shaderGraph.OnTextureListUpdate, this, &TextureEditor::OnTextureListUpdate);
	m_onTexturePreviewUpdateSlot.Connect(m_shaderGraph.OnTexturePreviewUpdate, this, &TextureEditor::OnTexturePreviewUpdate);
	m_onTextureUpdateSlot.Connect(m_shaderGraph.OnTextureUpdate, this, &TextureEditor::OnTextureUpdate);

	RefreshTextures();
}

void TextureEditor::OnAddTexture()
{
	TextureEditDialog* dialog = new TextureEditDialog(this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog]
	{
		TextureInfo outputInfo = dialog->GetTextureInfo();
		m_shaderGraph.AddTexture(std::move(outputInfo.name), outputInfo.type, outputInfo.setIndex, outputInfo.bindingIndex);
	});

	dialog->open();
}

void TextureEditor::OnEditTexture(int inputIndex)
{
	const auto& output = m_shaderGraph.GetTexture(inputIndex);

	TextureInfo info;
	info.bindingIndex = output.bindingIndex;
	info.name = output.name;
	info.type = output.type;

	TextureEditDialog* dialog = new TextureEditDialog(std::move(info), this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog, inputIndex]
	{
		TextureInfo textureInfo = dialog->GetTextureInfo();
		m_shaderGraph.UpdateTexture(inputIndex, std::move(textureInfo.name), textureInfo.type, textureInfo.setIndex, textureInfo.bindingIndex);
	});

	dialog->open();
}

void TextureEditor::OnLoadTexture()
{
	if (!m_currentTextureIndex)
		return;

	QString fileName = QFileDialog::getOpenFileName(nullptr, tr("Open Image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp)"));
	if (fileName.isEmpty())
		return;

	m_shaderGraph.UpdateTexturePreview(m_currentTextureIndex.value(), QImage(fileName));
}

void TextureEditor::OnTextureSelectionUpdate(int textureIndex)
{
	if (textureIndex >= 0)
	{
		m_currentTextureIndex = textureIndex;
		UpdateTexturePreview();
	}
	else
		m_currentTextureIndex.reset();
}

void TextureEditor::OnTextureListUpdate(ShaderGraph* /*graph*/)
{
	RefreshTextures();
}

void TextureEditor::OnTexturePreviewUpdate(ShaderGraph* /*graph*/, std::size_t textureIndex)
{
	if (m_currentTextureIndex && *m_currentTextureIndex == textureIndex)
		UpdateTexturePreview();
}

void TextureEditor::OnTextureUpdate(ShaderGraph* /*graph*/, std::size_t inputIndex)
{
	const auto& inputEntry = m_shaderGraph.GetTexture(inputIndex);
	m_textureList->item(int(inputIndex))->setText(QString::fromStdString(inputEntry.name));
}

void TextureEditor::RefreshTextures()
{
	m_textureList->clear();
	m_textureList->setCurrentRow(-1);

	for (const auto& textureEntry : m_shaderGraph.GetTextures())
		m_textureList->addItem(QString::fromStdString(textureEntry.name));
}

void TextureEditor::UpdateTexturePreview()
{
	assert(m_currentTextureIndex);
	const auto& textureEntry = m_shaderGraph.GetTexture(*m_currentTextureIndex);
	m_pixmapLabel->setPixmap(QPixmap::fromImage(textureEntry.preview).scaled(128, 128, Qt::KeepAspectRatio));
}
