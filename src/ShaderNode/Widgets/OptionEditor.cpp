#include <ShaderNode/Widgets/OptionEditor.hpp>
#include <ShaderNode/Widgets/OptionEditDialog.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

OptionEditor::OptionEditor(ShaderGraph& graph) :
m_shaderGraph(graph)
{
	QTableView* tableView = new QTableView;
	m_model = new QStandardItemModel(0, 2, tableView);
	tableView->setModel(m_model);

	m_model->setHorizontalHeaderLabels({ tr("Option"), tr("Enabled") });

	connect(tableView, &QTableView::doubleClicked, [this](const QModelIndex& index)
	{
		if (index.column() == 0)
			OnEditOption(index.row());
	});

	connect(m_model, &QStandardItemModel::itemChanged, [this](QStandardItem* item)
	{
		if (item->column() == 1)
		{
			std::size_t optionIndex = static_cast<std::size_t>(item->row());
			bool value = item->checkState() == Qt::Checked;
			m_shaderGraph.EnableOption(optionIndex, value);
		}
	});


	QPushButton* addStructButton = new QPushButton(tr("Add option..."));
	connect(addStructButton, &QPushButton::released, this, &OptionEditor::OnAddOption);

	m_layout = new QVBoxLayout;
	m_layout->addWidget(tableView);
	m_layout->addWidget(addStructButton);

	setLayout(m_layout);

	m_onOptionListUpdateSlot.Connect(m_shaderGraph.OnOptionListUpdate, this, &OptionEditor::OnOptionListUpdate);
	m_onOptionUpdateSlot.Connect(m_shaderGraph.OnOptionUpdate, this, &OptionEditor::OnOptionUpdate);

	RefreshOptions();
}

void OptionEditor::OnAddOption()
{
	OptionEditDialog* dialog = new OptionEditDialog(this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog]
	{
		OptionInfo optionInfo = dialog->GetOptionInfo();
		m_shaderGraph.AddOption(std::move(optionInfo.name));
	});

	dialog->open();
}

void OptionEditor::OnEditOption(int optionIndex)
{
	const auto& optionInfo = m_shaderGraph.GetOption(optionIndex);

	OptionInfo info;
	info.name = optionInfo.name;

	OptionEditDialog* dialog = new OptionEditDialog(info, this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog, optionIndex]
	{
		OptionInfo optionInfo = dialog->GetOptionInfo();

		m_shaderGraph.UpdateOption(optionIndex, std::move(optionInfo.name));
	});

	dialog->open();
}

void OptionEditor::OnOptionListUpdate(ShaderGraph* /*graph*/)
{
	RefreshOptions();
}

void OptionEditor::OnOptionUpdate(ShaderGraph* /*graph*/, std::size_t optionIndex)
{
	const auto& optionEntry = m_shaderGraph.GetOption(optionIndex);

	int row = int(optionIndex);
	m_model->item(row, 0)->setText(QString::fromStdString(optionEntry.name));
	m_model->item(row, 1)->setCheckState((optionEntry.enabled) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void OptionEditor::RefreshOptions()
{
	m_model->setRowCount(int(m_shaderGraph.GetOptionCount()));

	int rowIndex = 0;
	for (const auto& optionEntry : m_shaderGraph.GetOptions())
	{
		QStandardItem* label = new QStandardItem(1);
		label->setEditable(false);
		label->setText(QString::fromStdString(optionEntry.name));

		m_model->setItem(rowIndex, 0, label);

		QStandardItem* checkbox = new QStandardItem(1);
		checkbox->setCheckable(true);
		checkbox->setCheckState((optionEntry.enabled) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

		m_model->setItem(rowIndex, 1, checkbox);

		rowIndex++;
	}
}
