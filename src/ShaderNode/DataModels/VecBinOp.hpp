#pragma once

#ifndef NAZARA_SHADERNODES_VECBINOP_HPP
#define NAZARA_SHADERNODES_VECBINOP_HPP

#include <DataModels/ShaderNode.hpp>
#include <DataModels/VecValue.hpp>

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
		void UpdatePreview();

		using InternalType = typename Data::InternalType;

		InternalType GetValue() const;

		QLabel* m_pixmapLabel;
		QPixmap m_preview;
		std::shared_ptr<Vec4Data> m_lhs;
		std::shared_ptr<Vec4Data> m_rhs;
};

class Vec4Add : public VecBinOp<Vec4Data, Nz::ShaderAst::BinaryType::Add>
{
	public:
		using VecBinOp::VecBinOp;

		QString caption() const override { return "Vec4 addition"; }
		QString name() const override { return "Vec4Add"; }
};

class Vec4Mul : public VecBinOp<Vec4Data, Nz::ShaderAst::BinaryType::Multiply>
{
	public:
		using VecBinOp::VecBinOp;

		QString caption() const override { return "Vec4 multiplication"; }
		QString name() const override { return "Vec4Mul"; }
};

#include <DataModels/VecBinOp.inl>

#endif
