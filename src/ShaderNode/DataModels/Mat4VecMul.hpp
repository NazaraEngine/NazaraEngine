#pragma once

#ifndef NAZARA_SHADERNODES_MAT4VECMUL_HPP
#define NAZARA_SHADERNODES_MAT4VECMUL_HPP

#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/Matrix4Data.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

class Mat4VecMul : public ShaderNode
{
	public:
		Mat4VecMul(ShaderGraph& graph);
		~Mat4VecMul() = default;

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

		std::shared_ptr<Matrix4Data> m_lhs;
		std::shared_ptr<VecData> m_rhs;
		std::shared_ptr<VecData> m_output;
};

#include <ShaderNode/DataModels/VecFloatMul.inl>

#endif
