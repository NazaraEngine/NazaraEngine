#pragma once

#ifndef NAZARA_SHADERNODES_BINOP_HPP
#define NAZARA_SHADERNODES_BINOP_HPP

#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

template<typename DataType, Nz::ShaderAst::BinaryType Op>
class BinOp : public ShaderNode
{
	public:
		BinOp(ShaderGraph& graph);
		~BinOp() = default;

		Nz::ShaderAst::NodePtr BuildNode(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;

		virtual QString GetOperationString() const = 0;

		unsigned int nPorts(QtNodes::PortType portType) const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		QString portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
		bool portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	private:
		virtual void ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount) = 0;

		bool ComputePreview(QPixmap& pixmap) override;
		void UpdateOutput();

		std::shared_ptr<DataType> m_lhs;
		std::shared_ptr<DataType> m_rhs;
		std::shared_ptr<DataType> m_output;
};


template<typename DataType>
class BinAdd : public BinOp<DataType, Nz::ShaderAst::BinaryType::Add>
{
	public:
		using BinOp<DataType, Nz::ShaderAst::BinaryType::Add>::BinOp;

		void ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount) override;
		QString GetOperationString() const final;
};

template<typename DataType>
class BinMul : public BinOp<DataType, Nz::ShaderAst::BinaryType::Multiply>
{
	public:
		using BinOp<DataType, Nz::ShaderAst::BinaryType::Multiply>::BinOp;

		void ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount) override;
		QString GetOperationString() const final;
};

template<typename DataType>
class BinSub : public BinOp<DataType, Nz::ShaderAst::BinaryType::Subtract>
{
	public:
		using BinOp<DataType, Nz::ShaderAst::BinaryType::Subtract>::BinOp;

		void ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount) override;
		QString GetOperationString() const final;
};

template<typename DataType>
class BinDiv : public BinOp<DataType, Nz::ShaderAst::BinaryType::Divide>
{
	public:
		using BinOp<DataType, Nz::ShaderAst::BinaryType::Divide>::BinOp;

		void ApplyOp(const Nz::Vector4f* left, const Nz::Vector4f* right, Nz::Vector4f* output, std::size_t pixelCount) override;
		QString GetOperationString() const final;
};


class FloatAdd : public BinAdd<FloatData>
{
	public:
		using BinAdd<FloatData>::BinAdd;

		QString caption() const override { return "Float addition"; }
		QString name() const override { return "float_add"; }
};

class FloatMul : public BinMul<FloatData>
{
	public:
		using BinMul<FloatData>::BinMul;

		QString caption() const override { return "Float multiplication"; }
		QString name() const override { return "float_mul"; }
};

class FloatSub : public BinMul<FloatData>
{
	public:
		using BinMul<FloatData>::BinMul;

		QString caption() const override { return "Float subtraction"; }
		QString name() const override { return "float_sub"; }
};

class FloatDiv : public BinDiv<FloatData>
{
	public:
		using BinDiv<FloatData>::BinDiv;

		QString caption() const override { return "Float division"; }
		QString name() const override { return "float_div"; }
};


class VecAdd : public BinAdd<VecData>
{
	public:
		using BinAdd<VecData>::BinAdd;

		QString caption() const override { return "Vector addition"; }
		QString name() const override { return "vec_add"; }
};

class VecMul : public BinMul<VecData>
{
	public:
		using BinMul<VecData>::BinMul;

		QString caption() const override { return "Vector multiplication"; }
		QString name() const override { return "vec_mul"; }
};

class VecSub : public BinMul<VecData>
{
	public:
		using BinMul<VecData>::BinMul;

		QString caption() const override { return "Vector subtraction"; }
		QString name() const override { return "vec_sub"; }
};

class VecDiv : public BinDiv<VecData>
{
	public:
		using BinDiv<VecData>::BinDiv;

		QString caption() const override { return "Vector division"; }
		QString name() const override { return "vec_div"; }
};

#include <ShaderNode/DataModels/BinOp.inl>

#endif
