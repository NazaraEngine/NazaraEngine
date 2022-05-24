#pragma once

#ifndef NAZARA_SHADERNODES_VECDOT_HPP
#define NAZARA_SHADERNODES_VECDOT_HPP

#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

class VecDot : public ShaderNode
{
	public:
		VecDot(ShaderGraph& graph);
		~VecDot() = default;

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;

		QString caption() const override;
		QString name() const override;

		unsigned int nPorts(QtNodes::PortType portType) const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;
		void UpdateOutput();

		std::shared_ptr<VecData> m_lhs;
		std::shared_ptr<VecData> m_rhs;
		std::shared_ptr<FloatData> m_output;
};

#include <ShaderNode/DataModels/VecDot.inl>

#endif
