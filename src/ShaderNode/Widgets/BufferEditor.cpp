#include <ShaderNode/Widgets/BufferEditor.hpp>
#include <ShaderNode/Widgets/BufferEditDialog.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>

BufferEditor::BufferEditor(ShaderGraph& graph) :
m_shaderGraph(graph)
{
	m_bufferList = new QListWidget(this);
	connect(m_bufferList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item)
	{
		OnEditBuffer(m_bufferList->row(item));
	});

	QPushButton* addBufferButton = new QPushButton(tr("Add buffer..."));
	connect(addBufferButton, &QPushButton::released, this, &BufferEditor::OnAddBuffer);

	m_layout = new QVBoxLayout;
	m_layout->addWidget(m_bufferList);
	m_layout->addWidget(addBufferButton);

	setLayout(m_layout);

	m_onBufferListUpdateSlot.Connect(m_shaderGraph.OnBufferListUpdate, this, &BufferEditor::OnBufferListUpdate);
	m_onBufferUpdateSlot.Connect(m_shaderGraph.OnBufferUpdate, this, &BufferEditor::OnBufferUpdate);

	RefreshBuffers();
}

void BufferEditor::OnAddBuffer()
{
	BufferEditDialog* dialog = new BufferEditDialog(m_shaderGraph, this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog]
	{
		BufferInfo bufferInfo = dialog->GetBufferInfo();
		m_shaderGraph.AddBuffer(std::move(bufferInfo.name), bufferInfo.type, bufferInfo.structIndex, bufferInfo.setIndex, bufferInfo.bindingIndex);
	});

	dialog->open();
}

void BufferEditor::OnEditBuffer(int inputIndex)
{
	const auto& buffer = m_shaderGraph.GetBuffer(inputIndex);

	BufferInfo info;
	info.bindingIndex = buffer.bindingIndex;
	info.name = buffer.name;
	info.setIndex = buffer.setIndex;
	info.structIndex = buffer.structIndex;
	info.type = buffer.type;

	BufferEditDialog* dialog = new BufferEditDialog(m_shaderGraph, std::move(info), this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog, inputIndex]
	{
		BufferInfo bufferInfo = dialog->GetBufferInfo();
		m_shaderGraph.UpdateBuffer(inputIndex, std::move(bufferInfo.name), bufferInfo.type, bufferInfo.structIndex, bufferInfo.setIndex, bufferInfo.bindingIndex);
	});

	dialog->open();
}

void BufferEditor::OnBufferListUpdate(ShaderGraph* /*graph*/)
{
	RefreshBuffers();
}

void BufferEditor::OnBufferUpdate(ShaderGraph* /*graph*/, std::size_t bufferIndex)
{
	const auto& bufferEntry = m_shaderGraph.GetBuffer(bufferIndex);
	m_bufferList->item(int(bufferIndex))->setText(QString::fromStdString(bufferEntry.name));
}

void BufferEditor::RefreshBuffers()
{
	m_bufferList->clear();
	m_bufferList->setCurrentRow(-1);

	for (const auto& bufferEntry : m_shaderGraph.GetBuffers())
		m_bufferList->addItem(QString::fromStdString(bufferEntry.name));
}
