#pragma once

#ifndef NAZARA_SHADERNODES_VECVALUE_HPP
#define NAZARA_SHADERNODES_VECVALUE_HPP

#include <QtGui/QImage>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <DataModels/ShaderNode.hpp>
#include <array>

template<std::size_t N>
struct VecTypeHelper;

template<>
struct VecTypeHelper<2>
{
	using Type = Nz::Vector2f;
};

template<>
struct VecTypeHelper<3>
{
	using Type = Nz::Vector3f;
};

template<>
struct VecTypeHelper<4>
{
	using Type = Nz::Vector4f;
};

template<std::size_t N> using VecType = typename VecTypeHelper<N>::template Type;

template<std::size_t N, typename Data>
class VecValue : public ShaderNode
{
	public:
		VecValue(ShaderGraph& graph);
		~VecValue() = default;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		QWidget* embeddedWidget() override;
		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const override;

	protected:
		QColor ToColor() const;
		VecType<N> ToVector() const;
		void UpdatePreview();

		QLabel* m_pixmapLabel;
		QPixmap m_pixmap;
		QWidget* m_widget;
		QFormLayout* m_layout;
		std::array<QDoubleSpinBox*, N> m_spinboxes;
};

struct VecData : public QtNodes::NodeData
{
	inline VecData();

	QImage preview;
};

struct Vec2Data : public VecData
{
	QtNodes::NodeDataType type() const override
	{
		return Type();
	}

	static QtNodes::NodeDataType Type()
	{
		return { "vec2", "Vec2" };
	}
};

struct Vec4Data : public VecData
{
	QtNodes::NodeDataType type() const override
	{
		return Type();
	}

	static QtNodes::NodeDataType Type()
	{
		return { "vec4", "Vec4" };
	}
};

class Vec2Value : public VecValue<2, Vec2Data>
{
	public:
		using VecValue::VecValue;

		QString caption() const override { return "Vec2 value"; }
		QString name() const override { return "Vec2Value"; }
};

class Vec4Value : public VecValue<4, Vec4Data>
{
	public:
		using VecValue::VecValue;

		QString caption() const override { return "Vec4 value"; }
		QString name() const override { return "Vec4Value"; }
};

#include <DataModels/VecValue.inl>

#endif
