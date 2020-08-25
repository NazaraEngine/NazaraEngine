#include <ShaderNode/Widgets/StructEditor.hpp>
#include <ShaderNode/Widgets/StructEditDialog.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>

StructEditor::StructEditor(ShaderGraph& graph) :
m_shaderGraph(graph)
{
	m_structList = new QListWidget(this);
	connect(m_structList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item)
	{
		OnEditStruct(m_structList->row(item));
	});

	QPushButton* addStructButton = new QPushButton(tr("Add struct..."));
	connect(addStructButton, &QPushButton::released, this, &StructEditor::OnAddStruct);

	m_layout = new QVBoxLayout;
	m_layout->addWidget(m_structList);
	m_layout->addWidget(addStructButton);

	setLayout(m_layout);

	m_onStructListUpdateSlot.Connect(m_shaderGraph.OnStructListUpdate, this, &StructEditor::OnStructListUpdate);
	m_onStructUpdateSlot.Connect(m_shaderGraph.OnStructUpdate, this, &StructEditor::OnStructUpdate);

	RefreshStructs();
}

void StructEditor::OnAddStruct()
{
	StructEditDialog* dialog = new StructEditDialog(m_shaderGraph, this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog]
	{
		const StructInfo& structInfo = dialog->GetStructInfo();

		std::vector<ShaderGraph::StructMemberEntry> members;
		for (const auto& memberInfo : structInfo.members)
		{
			auto& member = members.emplace_back();
			member.name = memberInfo.name;
			member.type = memberInfo.type;
		}

		m_shaderGraph.AddStruct(std::move(structInfo.name), std::move(members));
	});

	dialog->open();
}

void StructEditor::OnEditStruct(int inputIndex)
{
	const auto& structInfo = m_shaderGraph.GetStruct(inputIndex);

	StructInfo info;
	info.name = structInfo.name;
	for (const auto& memberInfo : structInfo.members)
	{
		auto& member = info.members.emplace_back();
		member.name = memberInfo.name;
		member.type = memberInfo.type;
	}

	StructEditDialog* dialog = new StructEditDialog(m_shaderGraph, std::move(info), this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog, inputIndex]
	{
		const StructInfo& structInfo = dialog->GetStructInfo();

		std::vector<ShaderGraph::StructMemberEntry> members;
		for (const auto& memberInfo : structInfo.members)
		{
			auto& member = members.emplace_back();
			member.name = memberInfo.name;
			member.type = memberInfo.type;
		}

		m_shaderGraph.UpdateStruct(inputIndex, std::move(structInfo.name), std::move(members));
	});

	dialog->open();
}

void StructEditor::OnStructListUpdate(ShaderGraph* /*graph*/)
{
	RefreshStructs();
}

void StructEditor::OnStructUpdate(ShaderGraph* /*graph*/, std::size_t structIndex)
{
	const auto& structEntry = m_shaderGraph.GetStruct(structIndex);
	m_structList->item(int(structIndex))->setText(QString::fromStdString(structEntry.name));
}

void StructEditor::RefreshStructs()
{
	m_structList->clear();
	m_structList->setCurrentRow(-1);

	for (const auto& structEntry : m_shaderGraph.GetStructs())
		m_structList->addItem(QString::fromStdString(structEntry.name));
}
