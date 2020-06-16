#pragma once

#ifndef NAZARA_SHADERNODES_VECBINOP_HPP
#define NAZARA_SHADERNODES_VECBINOP_HPP

#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

template<Nz::ShaderAst::BinaryType BinOp>
class VecBinOp : public ShaderNode
{
	public:
		VecBinOp(ShaderGraph& graph);
		~VecBinOp() = default;

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const override;

		unsigned int nPorts(QtNodes::PortType portType) const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	private:
		virtual void ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount) = 0;

		bool ComputePreview(QPixmap& pixmap) override;
		void UpdateOutput();

		std::shared_ptr<VecData> m_lhs;
		std::shared_ptr<VecData> m_rhs;
		std::shared_ptr<VecData> m_output;
};

class VecAdd : public VecBinOp<Nz::ShaderAst::BinaryType::Add>
{
	public:
		using VecBinOp<Nz::ShaderAst::BinaryType::Add>::VecBinOp;

		QString caption() const override;
		QString name() const override;

		void ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount) override;
};

class VecMul : public VecBinOp<Nz::ShaderAst::BinaryType::Multiply>
{
	public:
		using VecBinOp<Nz::ShaderAst::BinaryType::Multiply>::VecBinOp;

		QString caption() const override;
		QString name() const override;

		void ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount) override;
};

class VecSub : public VecBinOp<Nz::ShaderAst::BinaryType::Substract>
{
	public:
		using VecBinOp<Nz::ShaderAst::BinaryType::Substract>::VecBinOp;

		QString caption() const override;
		QString name() const override;

		void ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount) override;
};

class VecDiv : public VecBinOp<Nz::ShaderAst::BinaryType::Divide>
{
	public:
		using VecBinOp<Nz::ShaderAst::BinaryType::Divide>::VecBinOp;

		QString caption() const override;
		QString name() const override;

		void ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount) override;
};

#include <ShaderNode/DataModels/VecBinOp.inl>

#endif
