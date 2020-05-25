#pragma once

#ifndef NAZARA_SHADERNODES_VECBINOP_HPP
#define NAZARA_SHADERNODES_VECBINOP_HPP

#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataModels/VecValue.hpp>

template<typename Data, Nz::ShaderAst::BinaryType BinOp>
class VecBinOp : public ShaderNode
{
	public:
		VecBinOp(ShaderGraph& graph);
		~VecBinOp() = default;

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const override;

		QWidget* embeddedWidget() override;
		unsigned int nPorts(QtNodes::PortType portType) const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

	private:
		virtual void ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount) = 0;
		void UpdatePreview();

		QLabel* m_pixmapLabel;
		QPixmap m_preview;
		std::shared_ptr<Data> m_lhs;
		std::shared_ptr<Data> m_rhs;
		std::shared_ptr<Data> m_output;
};

template<typename Data>
class VecAdd : public VecBinOp<Data, Nz::ShaderAst::BinaryType::Add>
{
	public:
		using VecBinOp<Data, Nz::ShaderAst::BinaryType::Add>::VecBinOp;

		QString caption() const override;
		QString name() const override;

		void ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount) override;
};

template<typename Data>
class VecMul : public VecBinOp<Data, Nz::ShaderAst::BinaryType::Multiply>
{
	public:
		using VecBinOp<Data, Nz::ShaderAst::BinaryType::Multiply>::VecBinOp;

		QString caption() const override;
		QString name() const override;

		void ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount) override;
};

template<typename Data>
class VecSub : public VecBinOp<Data, Nz::ShaderAst::BinaryType::Substract>
{
	public:
		using VecBinOp<Data, Nz::ShaderAst::BinaryType::Substract>::VecBinOp;

		QString caption() const override;
		QString name() const override;

		void ApplyOp(const std::uint8_t* left, const std::uint8_t* right, std::uint8_t* output, std::size_t pixelCount) override;
};

using Vec2Add = VecAdd<Vec2Data>;
using Vec3Add = VecAdd<Vec3Data>;
using Vec4Add = VecAdd<Vec4Data>;

using Vec2Mul = VecMul<Vec2Data>;
using Vec3Mul = VecMul<Vec3Data>;
using Vec4Mul = VecMul<Vec4Data>;

using Vec2Sub = VecSub<Vec2Data>;
using Vec3Sub = VecSub<Vec3Data>;
using Vec4Sub = VecSub<Vec4Data>;

#include <ShaderNode/DataModels/VecBinOp.inl>

#endif
