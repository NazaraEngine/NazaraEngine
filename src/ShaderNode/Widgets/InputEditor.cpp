#include <ShaderNode/Widgets/InputEditor.hpp>
#include <ShaderNode/Widgets/InputEditDialog.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>

InputEditor::InputEditor(ShaderGraph& graph) :
m_shaderGraph(graph)
{
	m_inputList = new QListWidget(this);
	connect(m_inputList, &QListWidget::currentRowChanged, this, &InputEditor::OnInputSelectionUpdate);
	connect(m_inputList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item)
	{
		OnEditInput(m_inputList->row(item));
	});

	QPushButton* addInputButton = new QPushButton(tr("Add input..."));
	connect(addInputButton, &QPushButton::released, this, &InputEditor::OnAddInput);

	m_layout = new QVBoxLayout;
	m_layout->addWidget(m_inputList);
	m_layout->addWidget(addInputButton);

	setLayout(m_layout);

	m_onInputListUpdateSlot.Connect(m_shaderGraph.OnInputListUpdate, this, &InputEditor::OnInputListUpdate);
	m_onInputUpdateSlot.Connect(m_shaderGraph.OnInputUpdate, this, &InputEditor::OnInputUpdate);

	RefreshInputs();
}

void InputEditor::OnAddInput()
{
	InputEditDialog* dialog = new InputEditDialog(this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog]
	{
		InputInfo inputInfo = dialog->GetInputInfo();
		m_shaderGraph.AddInput(std::move(inputInfo.name), inputInfo.type, inputInfo.role, inputInfo.roleIndex, inputInfo.locationIndex);
	});

	dialog->open();
}

void InputEditor::OnEditInput(int inputIndex)
{
	const auto& input = m_shaderGraph.GetInput(inputIndex);

	InputInfo info;
	info.locationIndex = input.locationIndex;
	info.name = input.name;
	info.type = input.type;
	info.role = input.role;
	info.roleIndex = input.roleIndex;

	InputEditDialog* dialog = new InputEditDialog(std::move(info), this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog, inputIndex]
	{
		InputInfo inputInfo = dialog->GetInputInfo();
		m_shaderGraph.UpdateInput(inputIndex, std::move(inputInfo.name), inputInfo.type, inputInfo.role, inputInfo.roleIndex, inputInfo.locationIndex);
	});

	dialog->open();
}

void InputEditor::OnInputSelectionUpdate(int inputIndex)
{
	if (inputIndex >= 0)
	{
		m_currentInputIndex = inputIndex;
	}
	else
		m_currentInputIndex.reset();
}

void InputEditor::OnInputListUpdate(ShaderGraph* /*graph*/)
{
	RefreshInputs();
}

void InputEditor::OnInputUpdate(ShaderGraph* /*graph*/, std::size_t inputIndex)
{
	const auto& inputEntry = m_shaderGraph.GetInput(inputIndex);
	m_inputList->item(int(inputIndex))->setText(QString::fromStdString(inputEntry.name));
}

void InputEditor::RefreshInputs()
{
	m_inputList->clear();
	m_inputList->setCurrentRow(-1);

	for (const auto& inputEntry : m_shaderGraph.GetInputs())
		m_inputList->addItem(QString::fromStdString(inputEntry.name));
}
