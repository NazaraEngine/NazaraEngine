#pragma once

#ifndef NAZARA_SHADERNODES_STRUCTEDITDIALOG_HPP
#define NAZARA_SHADERNODES_STRUCTEDITDIALOG_HPP

#include <ShaderNode/Enums.hpp>
#include <QtWidgets/QDialog>
#include <variant>
#include <vector>

class QLineEdit;
class QListWidget;
class QPushButton;
class ShaderGraph;

struct StructInfo
{
	struct Member
	{
		std::string name;
		std::variant<PrimitiveType, std::size_t> type;
	};

	std::string name;
	std::vector<Member> members;
};

class StructEditDialog : public QDialog
{
	public:
		StructEditDialog(ShaderGraph& graph, QWidget* parent = nullptr);
		StructEditDialog(ShaderGraph& graph, const StructInfo& output, QWidget* parent = nullptr);
		~StructEditDialog() = default;

		const StructInfo& GetStructInfo() const;

	private:
		QString GetMemberName(const StructInfo::Member& member);
		void OnAccept();
		void OnAddMember();
		void OnDeleteMember();
		void OnEditMember(int memberIndex);
		void OnMemberMoveUp();
		void OnMemberMoveDown();
		void OnMemberSelected(int memberIndex);
		void UpdateMemberList(bool keepSelection = false);

		QLineEdit* m_structName;
		QListWidget* m_memberList;
		QPushButton* m_deleteMemberButton;
		QPushButton* m_memberMoveUpButton;
		QPushButton* m_memberMoveDownButton;
		ShaderGraph& m_shaderGraph;
		StructInfo m_info;
};

#include <ShaderNode/Widgets/StructEditDialog.inl>

#endif
