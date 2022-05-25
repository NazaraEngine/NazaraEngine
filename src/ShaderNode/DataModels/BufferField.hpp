#pragma once

#ifndef NAZARA_SHADERNODES_BUFFERFIELD_HPP
#define NAZARA_SHADERNODES_BUFFERFIELD_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <optional>
#include <string>
#include <vector>

class BufferField : public ShaderNode
{
	public:
		BufferField(ShaderGraph& graph);
		~BufferField() = default;

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;
		void BuildNodeEdition(QFormLayout* layout) override;

		QString caption() const override { return "BufferField"; }
		QString name() const override { return "BufferField"; }

		unsigned int nPorts(QtNodes::PortType portType) const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		QString portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
		bool portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void restore(const QJsonObject& data) override;
		QJsonObject save() const override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;
		void PopulateFieldList(std::size_t structIndex, const std::string& prefix = "");
		const ShaderGraph::StructMemberEntry& RetrieveNestedMember() const;
		void UpdateBufferIndex();
		void UpdateBufferText();
		void UpdateFieldIndex();
		void UpdateFieldList();

		NazaraSlot(ShaderGraph, OnBufferListUpdate, m_onBufferListUpdateSlot);
		NazaraSlot(ShaderGraph, OnBufferUpdate, m_onBufferUpdateSlot);

		NazaraSlot(ShaderGraph, OnStructListUpdate, m_onStructListUpdateSlot);
		NazaraSlot(ShaderGraph, OnStructUpdate, m_onStructUpdateSlot);

		struct CurrentField
		{
			std::vector<std::size_t> nestedFields;
			std::size_t finalFieldIndex;
		};

		std::optional<std::size_t> m_currentBufferIndex;
		std::optional<CurrentField> m_currentFieldIndex;
		std::string m_currentBufferText;
		std::string m_currentFieldText;
		std::vector<std::string> m_fieldList;
};

#include <ShaderNode/DataModels/BufferField.inl>

#endif
