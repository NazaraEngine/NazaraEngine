#include <ShaderNode/Widgets/StructEditDialog.hpp>
#include <ShaderNode/Widgets/StructMemberEditDialog.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

StructEditDialog::StructEditDialog(ShaderGraph& graph, QWidget* parent) :
QDialog(parent),
m_shaderGraph(graph)
{
	setWindowTitle(tr("Struct edit dialog"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_structName = new QLineEdit;
	connect(m_structName, &QLineEdit::textEdited, [this](QString newText)
	{
		m_info.name = newText.toStdString();
	});

	m_memberList = new QListWidget;
	connect(m_memberList, &QListWidget::currentRowChanged, this, &StructEditDialog::OnMemberSelected);
	connect(m_memberList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item)
	{
		OnEditMember(m_memberList->row(item));
	});

	m_memberMoveUpButton = new QPushButton(tr("Move up"));
	m_memberMoveUpButton->setEnabled(false);
	connect(m_memberMoveUpButton, &QPushButton::released, this, &StructEditDialog::OnMemberMoveUp);

	m_memberMoveDownButton = new QPushButton(tr("Move down"));
	m_memberMoveDownButton->setEnabled(false);
	connect(m_memberMoveDownButton, &QPushButton::released, this, &StructEditDialog::OnMemberMoveDown);

	m_deleteMemberButton = new QPushButton(tr("Delete member"));
	m_deleteMemberButton->setEnabled(false);
	connect(m_deleteMemberButton, &QPushButton::released, this, &StructEditDialog::OnDeleteMember);

	QPushButton* addMemberButton = new QPushButton(tr("Add member..."));
	connect(addMemberButton, &QPushButton::released, this, &StructEditDialog::OnAddMember);

	QVBoxLayout* arrowLayout = new QVBoxLayout;
	arrowLayout->addWidget(m_memberMoveUpButton);
	arrowLayout->addWidget(m_memberMoveDownButton);
	arrowLayout->addWidget(m_deleteMemberButton);
	arrowLayout->addWidget(addMemberButton);

	QHBoxLayout* entityListLayout = new QHBoxLayout;
	entityListLayout->addWidget(m_memberList);
	entityListLayout->addLayout(arrowLayout);

	QFormLayout* formLayout = new QFormLayout;
	formLayout->addRow(tr("Name"), m_structName);
	formLayout->addRow(tr("Members"), entityListLayout);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &StructEditDialog::OnAccept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	verticalLayout->addLayout(formLayout);
	verticalLayout->addWidget(buttonBox);

	setLayout(verticalLayout);
}

StructEditDialog::StructEditDialog(ShaderGraph& graph, const StructInfo& structInfo, QWidget* parent) :
StructEditDialog(graph, parent)
{
	m_info = structInfo;

	m_structName->setText(QString::fromStdString(m_info.name));
	UpdateMemberList();
}

const StructInfo& StructEditDialog::GetStructInfo() const
{
	return m_info;
}

QString StructEditDialog::GetMemberName(const StructInfo::Member& member)
{
	QString name = QString::fromStdString(member.name) + " (";

	std::visit([&](auto&& arg)
	{
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, PrimitiveType>)
			name += QString(EnumToString(arg));
		else if constexpr (std::is_same_v<T, std::size_t>)
			name += QString::fromStdString(m_shaderGraph.GetStruct(arg).name);
		else
			static_assert(Nz::AlwaysFalse<T>::value, "non-exhaustive visitor");
	},
	member.type);

	name += ")";

	return name;
}

void StructEditDialog::OnAccept()
{
	if (m_info.name.empty())
	{
		QMessageBox::critical(this, tr("Empty name"), tr("Struct name must be set"), QMessageBox::Ok);
		return;
	}

	accept();
}

void StructEditDialog::OnAddMember()
{
	StructMemberEditDialog* dialog = new StructMemberEditDialog(m_shaderGraph, this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog]
	{
		const StructMemberInfo& structInfo = dialog->GetMemberInfo();

		auto& memberInfo = m_info.members.emplace_back();
		memberInfo.name = structInfo.name;
		memberInfo.type = structInfo.type;

		UpdateMemberList(true);
	});

	dialog->open();
}

void StructEditDialog::OnDeleteMember()
{
	int memberIndex = m_memberList->currentRow();
	if (memberIndex < 0)
		return;

	m_info.members.erase(m_info.members.begin() + memberIndex);
	UpdateMemberList();
}

void StructEditDialog::OnEditMember(int memberIndex)
{
	assert(memberIndex >= 0);

	auto& memberInfo = m_info.members[memberIndex];

	StructMemberInfo info;
	info.name = memberInfo.name;
	info.type = memberInfo.type;

	StructMemberEditDialog* dialog = new StructMemberEditDialog(m_shaderGraph, std::move(info), this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog, &memberInfo]
	{
		const StructMemberInfo& structInfo = dialog->GetMemberInfo();
		memberInfo.name = structInfo.name;
		memberInfo.type = structInfo.type;

		UpdateMemberList(true);
	});

	dialog->open();
}

void StructEditDialog::OnMemberMoveUp()
{
	int memberIndex = m_memberList->currentRow();
	if (memberIndex <= 0)
		return;

	std::size_t newMemberIndex = static_cast<std::size_t>(memberIndex - 1);
	std::swap(m_info.members[memberIndex], m_info.members[newMemberIndex]);
	UpdateMemberList();

	m_memberList->setCurrentRow(int(newMemberIndex));
}

void StructEditDialog::OnMemberMoveDown()
{
	int memberIndex = m_memberList->currentRow();
	if (memberIndex < 0 || memberIndex + 1 >= m_memberList->count())
		return;

	std::size_t newMemberIndex = static_cast<std::size_t>(memberIndex + 1);
	std::swap(m_info.members[memberIndex], m_info.members[newMemberIndex]);
	UpdateMemberList();

	m_memberList->setCurrentRow(int(newMemberIndex));
}

void StructEditDialog::OnMemberSelected(int memberIndex)
{
	if (memberIndex >= 0)
	{
		m_deleteMemberButton->setEnabled(true);
		m_memberMoveDownButton->setEnabled(memberIndex + 1 < m_memberList->count());
		m_memberMoveUpButton->setEnabled(memberIndex != 0);
	}
	else
	{
		m_deleteMemberButton->setEnabled(false);
		m_memberMoveDownButton->setEnabled(false);
		m_memberMoveUpButton->setEnabled(false);
	}
}

void StructEditDialog::UpdateMemberList(bool keepSelection)
{
	int selectionIndex = m_memberList->currentRow();

	m_memberList->clear();
	for (const auto& memberInfo : m_info.members)
		m_memberList->addItem(GetMemberName(memberInfo));

	if (keepSelection && selectionIndex >= 0 && selectionIndex < m_memberList->count())
		m_memberList->setCurrentRow(selectionIndex);
}
