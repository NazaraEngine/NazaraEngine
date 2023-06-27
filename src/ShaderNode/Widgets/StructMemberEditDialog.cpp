#include <ShaderNode/Widgets/StructMemberEditDialog.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QVBoxLayout>

StructMemberEditDialog::StructMemberEditDialog(const ShaderGraph& shaderGraph, QWidget* parent) :
QDialog(parent),
m_shaderGraph(shaderGraph)
{
	setWindowTitle(tr("Struct member edit dialog"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_memberName = new QLineEdit;

	m_typeList = new QComboBox;
	for (std::size_t i = 0; i < PrimitiveTypeCount; ++i)
		m_typeList->addItem(EnumToString(static_cast<PrimitiveType>(i)));

	for (const auto& structInfo : m_shaderGraph.GetStructs())
		m_typeList->addItem(QString::fromStdString(structInfo.name));

	QFormLayout* formLayout = new QFormLayout;
	formLayout->addRow(tr("Name"), m_memberName);
	formLayout->addRow(tr("Type"), m_typeList);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &StructMemberEditDialog::OnAccept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	QVBoxLayout* verticalLayout = new QVBoxLayout;
	verticalLayout->addLayout(formLayout);
	verticalLayout->addWidget(buttonBox);

	setLayout(verticalLayout);
}

StructMemberEditDialog::StructMemberEditDialog(const ShaderGraph& shaderGraph, const StructMemberInfo& member, QWidget* parent) :
StructMemberEditDialog(shaderGraph, parent)
{
	m_memberName->setText(QString::fromStdString(member.name));
	std::visit([&](auto&& arg)
	{
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, PrimitiveType>)
			m_typeList->setCurrentIndex(static_cast<int>(arg));
		else if constexpr (std::is_same_v<T, std::size_t>)
			m_typeList->setCurrentIndex(static_cast<int>(PrimitiveTypeCount + arg));
		else
			static_assert(Nz::AlwaysFalse<T>::value, "non-exhaustive visitor");
	},
	member.type);
}

StructMemberInfo StructMemberEditDialog::GetMemberInfo() const
{
	StructMemberInfo inputInfo;
	inputInfo.name = m_memberName->text().toStdString();

	std::size_t index = Nz::SafeCast<std::size_t>(m_typeList->currentIndex());

	if (index < PrimitiveTypeCount)
		inputInfo.type = static_cast<PrimitiveType>(index);
	else
		inputInfo.type = Nz::SafeCast<std::size_t>(index - PrimitiveTypeCount);

	return inputInfo;
}

void StructMemberEditDialog::OnAccept()
{
	if (m_memberName->text().isEmpty())
	{
		QMessageBox::critical(this, tr("Empty name"), tr("Struct member name must be set"), QMessageBox::Ok);
		return;
	}

	if (m_typeList->currentIndex() < 0)
	{
		QMessageBox::critical(this, tr("Invalid type"), tr("You must select a type"), QMessageBox::Ok);
		return;
	}

	accept();
}
