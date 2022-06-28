#pragma once

#ifndef NAZARA_SHADERNODES_COMPOP_HPP
#define NAZARA_SHADERNODES_COMPOP_HPP

#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/BoolData.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

template<typename DataType, nzsl::Ast::BinaryType Op>
class CompOp : public ShaderNode
{
	public:
		CompOp(ShaderGraph& graph);
		~CompOp() = default;

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;

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
		virtual void ApplyOp(const nzsl::Vector4f32* left, const nzsl::Vector4f32* right, nzsl::Vector4f32* output, std::size_t pixelCount) = 0;

		bool ComputePreview(QPixmap& pixmap) override;
		void UpdateOutput();

		std::shared_ptr<BoolData> m_output;
		std::shared_ptr<DataType> m_lhs;
		std::shared_ptr<DataType> m_rhs;
};


template<typename DataType>
class CompEq : public CompOp<DataType, nzsl::Ast::BinaryType::CompEq>
{
	public:
		using CompOp<DataType, nzsl::Ast::BinaryType::CompEq>::CompOp;

		void ApplyOp(const nzsl::Vector4f32* left, const nzsl::Vector4f32* right, nzsl::Vector4f32* output, std::size_t pixelCount) override;
		QString GetOperationString() const final;
};

template<typename DataType>
class CompGe : public CompOp<DataType, nzsl::Ast::BinaryType::CompGe>
{
	public:
		using CompOp<DataType, nzsl::Ast::BinaryType::CompGe>::CompOp;

		void ApplyOp(const nzsl::Vector4f32* left, const nzsl::Vector4f32* right, nzsl::Vector4f32* output, std::size_t pixelCount) override;
		QString GetOperationString() const final;
};

template<typename DataType>
class CompGt : public CompOp<DataType, nzsl::Ast::BinaryType::CompGt>
{
	public:
		using CompOp<DataType, nzsl::Ast::BinaryType::CompGt>::CompOp;

		void ApplyOp(const nzsl::Vector4f32* left, const nzsl::Vector4f32* right, nzsl::Vector4f32* output, std::size_t pixelCount) override;
		QString GetOperationString() const final;
};

template<typename DataType>
class CompLe : public CompOp<DataType, nzsl::Ast::BinaryType::CompLe>
{
	public:
		using CompOp<DataType, nzsl::Ast::BinaryType::CompLe>::CompOp;

		void ApplyOp(const nzsl::Vector4f32* left, const nzsl::Vector4f32* right, nzsl::Vector4f32* output, std::size_t pixelCount) override;
		QString GetOperationString() const final;
};

template<typename DataType>
class CompLt : public CompOp<DataType, nzsl::Ast::BinaryType::CompLt>
{
	public:
		using CompOp<DataType, nzsl::Ast::BinaryType::CompLt>::CompOp;

		void ApplyOp(const nzsl::Vector4f32* left, const nzsl::Vector4f32* right, nzsl::Vector4f32* output, std::size_t pixelCount) override;
		QString GetOperationString() const final;
};

template<typename DataType>
class CompNe : public CompOp<DataType, nzsl::Ast::BinaryType::CompNe>
{
	public:
		using CompOp<DataType, nzsl::Ast::BinaryType::CompNe>::CompOp;

		void ApplyOp(const nzsl::Vector4f32* left, const nzsl::Vector4f32* right, nzsl::Vector4f32* output, std::size_t pixelCount) override;
		QString GetOperationString() const final;
};


class FloatEq : public CompEq<FloatData>
{
	public:
		using CompEq<FloatData>::CompEq;

		QString caption() const override { return "Float equality"; }
		QString name() const override { return "float_eq"; }
};

class FloatGe : public CompGe<FloatData>
{
	public:
		using CompGe<FloatData>::CompGe;

		QString caption() const override { return "Float greater than or equal"; }
		QString name() const override { return "float_ge"; }
};

class FloatGt : public CompGt<FloatData>
{
	public:
		using CompGt<FloatData>::CompGt;

		QString caption() const override { return "Float greater than"; }
		QString name() const override { return "float_gt"; }
};

class FloatLe : public CompLe<FloatData>
{
	public:
		using CompLe<FloatData>::CompLe;

		QString caption() const override { return "Float less than or equal"; }
		QString name() const override { return "float_le"; }
};

class FloatLt : public CompLt<FloatData>
{
	public:
		using CompLt<FloatData>::CompLt;

		QString caption() const override { return "Float less than"; }
		QString name() const override { return "float_lt"; }
};

class FloatNe : public CompNe<FloatData>
{
	public:
		using CompNe<FloatData>::CompNe;

		QString caption() const override { return "Float inequality"; }
		QString name() const override { return "float_ne"; }
};


class VecEq : public CompEq<VecData>
{
	public:
		using CompEq<VecData>::CompEq;

		QString caption() const override { return "Vector equality"; }
		QString name() const override { return "vec_eq"; }
};

class VecGe : public CompGe<VecData>
{
	public:
		using CompGe<VecData>::CompGe;

		QString caption() const override { return "Vector greater than or equal"; }
		QString name() const override { return "vec_ge"; }
};

class VecGt : public CompGt<VecData>
{
	public:
		using CompGt<VecData>::CompGt;

		QString caption() const override { return "Vector greater than"; }
		QString name() const override { return "vec_gt"; }
};

class VecLe : public CompLe<VecData>
{
	public:
		using CompLe<VecData>::CompLe;

		QString caption() const override { return "Vector less than or equal"; }
		QString name() const override { return "vec_le"; }
};

class VecLt : public CompLt<VecData>
{
	public:
		using CompLt<VecData>::CompLt;

		QString caption() const override { return "Vector less than"; }
		QString name() const override { return "vec_lt"; }
};

class VecNe : public CompNe<VecData>
{
	public:
		using CompNe<VecData>::CompNe;

		QString caption() const override { return "Vector inequality"; }
		QString name() const override { return "vec_ne"; }
};

#include <ShaderNode/DataModels/CompOp.inl>

#endif
