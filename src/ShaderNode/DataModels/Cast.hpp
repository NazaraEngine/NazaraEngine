#pragma once

#ifndef NAZARA_SHADERNODES_CAST_HPP
#define NAZARA_SHADERNODES_CAST_HPP

#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

template<std::size_t ToComponentCount>
class CastVec : public ShaderNode
{
	public:
		CastVec(ShaderGraph& graph);
		~CastVec() = default;

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;
		void BuildNodeEdition(QFormLayout* layout) override;

		QString caption() const override;
		QString name() const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;
		void UpdateOutput();

		void restore(const QJsonObject& data) override;
		QJsonObject save() const override;

		std::shared_ptr<VecData> m_input;
		std::shared_ptr<VecData> m_output;
		VecType<ToComponentCount> m_overflowComponents;
};

using CastToVec2 = CastVec<2>;
using CastToVec3 = CastVec<3>;
using CastToVec4 = CastVec<4>;

#include <ShaderNode/DataModels/Cast.inl>

#endif
