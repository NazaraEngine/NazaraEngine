#pragma once

#ifndef NAZARA_SHADERNODES_STRUCTMEMBEREDITODIALOG_HPP
#define NAZARA_SHADERNODES_STRUCTMEMBEREDITODIALOG_HPP

#include <ShaderNode/Enums.hpp>
#include <QtWidgets/QDialog>
#include <variant>

class QComboBox;
class QLineEdit;
class ShaderGraph;

struct StructMemberInfo
{
	std::string name;
	std::variant<PrimitiveType, std::size_t> type;
};

class StructMemberEditDialog : public QDialog
{
	public:
		StructMemberEditDialog(const ShaderGraph& shaderGraph, QWidget* parent = nullptr);
		StructMemberEditDialog(const ShaderGraph& shaderGraph, const StructMemberInfo& output, QWidget* parent = nullptr);
		~StructMemberEditDialog() = default;

		StructMemberInfo GetMemberInfo() const;

	private:
		void OnAccept();

		const ShaderGraph& m_shaderGraph;
		QComboBox* m_typeList;
		QLineEdit* m_memberName;
};

#include <ShaderNode/Widgets/StructMemberEditDialog.inl>

#endif
