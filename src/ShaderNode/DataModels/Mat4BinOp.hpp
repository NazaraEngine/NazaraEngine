#pragma once

#ifndef NAZARA_SHADERNODES_MAT4BINOP_HPP
#define NAZARA_SHADERNODES_MAT4BINOP_HPP

#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/Matrix4Data.hpp>

template<Nz::ShaderAst::BinaryType Op>
class Mat4BinOp : public ShaderNode
{
	public:
		Mat4BinOp(ShaderGraph& graph);
		~Mat4BinOp() = default;

		Nz::ShaderAst::NodePtr BuildNode(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const;

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
		std::shared_ptr<Matrix4Data> m_rhs;
		std::shared_ptr<Matrix4Data> m_output;
};

class Mat4Add : public Mat4BinOp<Nz::ShaderAst::BinaryType::Add>
{
	public:
		using Mat4BinOp<Nz::ShaderAst::BinaryType::Add>::Mat4BinOp;

		QString caption() const override;
		QString name() const override;
};

class Mat4Mul : public Mat4BinOp<Nz::ShaderAst::BinaryType::Multiply>
{
	public:
		using Mat4BinOp<Nz::ShaderAst::BinaryType::Multiply>::Mat4BinOp;

		QString caption() const override;
		QString name() const override;
};

class Mat4Sub : public Mat4BinOp<Nz::ShaderAst::BinaryType::Subtract>
{
	public:
		using Mat4BinOp<Nz::ShaderAst::BinaryType::Subtract>::Mat4BinOp;

		QString caption() const override;
		QString name() const override;
};

#include <ShaderNode/DataModels/Mat4BinOp.inl>

#endif
