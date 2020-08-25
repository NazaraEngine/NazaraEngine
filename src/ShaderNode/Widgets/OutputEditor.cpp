#include <ShaderNode/Widgets/OutputEditor.hpp>
#include <ShaderNode/Widgets/OutputEditDialog.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>

OutputEditor::OutputEditor(ShaderGraph& graph) :
m_shaderGraph(graph)
{
	m_outputList = new QListWidget(this);
	connect(m_outputList, &QListWidget::currentRowChanged, this, &OutputEditor::OnOutputSelectionUpdate);
	connect(m_outputList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item)
	{
		OnEditOutput(m_outputList->row(item));
	});

	QPushButton* addOutputButton = new QPushButton(tr("Add output..."));
	connect(addOutputButton, &QPushButton::released, this, &OutputEditor::OnAddOutput);

	m_layout = new QVBoxLayout;
	m_layout->addWidget(m_outputList);
	m_layout->addWidget(addOutputButton);

	setLayout(m_layout);

	m_onOutputListUpdateSlot.Connect(m_shaderGraph.OnOutputListUpdate, this, &OutputEditor::OnOutputListUpdate);
	m_onOutputUpdateSlot.Connect(m_shaderGraph.OnOutputUpdate, this, &OutputEditor::OnOutputUpdate);

	RefreshOutputs();
}

void OutputEditor::OnAddOutput()
{
	OutputEditDialog* dialog = new OutputEditDialog(this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog]
	{
		OutputInfo outputInfo = dialog->GetOutputInfo();
		m_shaderGraph.AddOutput(std::move(outputInfo.name), outputInfo.type, outputInfo.locationIndex);
	});

	dialog->open();
}

void OutputEditor::OnEditOutput(int inputIndex)
{
	const auto& output = m_shaderGraph.GetOutput(inputIndex);

	OutputInfo info;
	info.locationIndex = output.locationIndex;
	info.name = output.name;
	info.type = output.type;

	OutputEditDialog* dialog = new OutputEditDialog(std::move(info), this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog, inputIndex]
	{
		OutputInfo outputInfo = dialog->GetOutputInfo();
		m_shaderGraph.UpdateOutput(inputIndex, std::move(outputInfo.name), outputInfo.type, outputInfo.locationIndex);
	});

	dialog->open();
}

void OutputEditor::OnOutputSelectionUpdate(int inputIndex)
{
	if (inputIndex >= 0)
		m_currentOutputIndex = inputIndex;
	else
		m_currentOutputIndex.reset();
}

void OutputEditor::OnOutputListUpdate(ShaderGraph* /*graph*/)
{
	RefreshOutputs();
}

void OutputEditor::OnOutputUpdate(ShaderGraph* /*graph*/, std::size_t inputIndex)
{
	const auto& inputEntry = m_shaderGraph.GetOutput(inputIndex);
	m_outputList->item(int(inputIndex))->setText(QString::fromStdString(inputEntry.name));
}

void OutputEditor::RefreshOutputs()
{
	m_outputList->clear();
	m_outputList->setCurrentRow(-1);

	for (const auto& inputEntry : m_shaderGraph.GetOutputs())
		m_outputList->addItem(QString::fromStdString(inputEntry.name));
}
