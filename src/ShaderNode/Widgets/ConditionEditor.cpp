#include <ShaderNode/Widgets/ConditionEditor.hpp>
#include <ShaderNode/Widgets/ConditionEditDialog.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

ConditionEditor::ConditionEditor(ShaderGraph& graph) :
m_shaderGraph(graph)
{
	QTableView* tableView = new QTableView;
	m_model = new QStandardItemModel(0, 2, tableView);
	tableView->setModel(m_model);

	m_model->setHorizontalHeaderLabels({ tr("Condition"), tr("Enabled") });

	connect(tableView, &QTableView::doubleClicked, [this](const QModelIndex& index)
	{
		if (index.column() == 0)
			OnEditCondition(index.row());
	});

	connect(m_model, &QStandardItemModel::itemChanged, [this](QStandardItem* item)
	{
		if (item->column() == 1)
		{
			std::size_t conditionIndex = static_cast<std::size_t>(item->row());
			bool value = item->checkState() == Qt::Checked;
			m_shaderGraph.EnableCondition(conditionIndex, value);
		}
	});


	QPushButton* addStructButton = new QPushButton(tr("Add condition..."));
	connect(addStructButton, &QPushButton::released, this, &ConditionEditor::OnAddCondition);

	m_layout = new QVBoxLayout;
	m_layout->addWidget(tableView);
	m_layout->addWidget(addStructButton);

	setLayout(m_layout);

	m_onConditionListUpdateSlot.Connect(m_shaderGraph.OnConditionListUpdate, this, &ConditionEditor::OnConditionListUpdate);
	m_onConditionUpdateSlot.Connect(m_shaderGraph.OnConditionUpdate, this, &ConditionEditor::OnConditionUpdate);

	RefreshConditions();
}

void ConditionEditor::OnAddCondition()
{
	ConditionEditDialog* dialog = new ConditionEditDialog(this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog]
	{
		ConditionInfo conditionInfo = dialog->GetConditionInfo();
		m_shaderGraph.AddCondition(std::move(conditionInfo.name));
	});

	dialog->open();
}

void ConditionEditor::OnEditCondition(int conditionIndex)
{
	const auto& conditionInfo = m_shaderGraph.GetCondition(conditionIndex);

	ConditionInfo info;
	info.name = conditionInfo.name;

	ConditionEditDialog* dialog = new ConditionEditDialog(info, this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog, conditionIndex]
	{
		ConditionInfo conditionInfo = dialog->GetConditionInfo();

		m_shaderGraph.UpdateCondition(conditionIndex, std::move(conditionInfo.name));
	});

	dialog->open();
}

void ConditionEditor::OnConditionListUpdate(ShaderGraph* /*graph*/)
{
	RefreshConditions();
}

void ConditionEditor::OnConditionUpdate(ShaderGraph* /*graph*/, std::size_t conditionIndex)
{
	const auto& conditionEntry = m_shaderGraph.GetCondition(conditionIndex);

	int row = int(conditionIndex);
	m_model->item(row, 0)->setText(QString::fromStdString(conditionEntry.name));
	m_model->item(row, 1)->setCheckState((conditionEntry.enabled) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void ConditionEditor::RefreshConditions()
{
	m_model->setRowCount(int(m_shaderGraph.GetConditionCount()));

	int rowIndex = 0;
	for (const auto& conditionEntry : m_shaderGraph.GetConditions())
	{
		QStandardItem* label = new QStandardItem(1);
		label->setEditable(false);
		label->setText(QString::fromStdString(conditionEntry.name));

		m_model->setItem(rowIndex, 0, label);

		QStandardItem* checkbox = new QStandardItem(1);
		checkbox->setCheckable(true);
		checkbox->setCheckState((conditionEntry.enabled) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

		m_model->setItem(rowIndex, 1, checkbox);
	}
}
