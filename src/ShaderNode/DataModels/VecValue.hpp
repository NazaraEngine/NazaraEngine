#pragma once

#ifndef NAZARA_SHADERNODES_VECVALUE_HPP
#define NAZARA_SHADERNODES_VECVALUE_HPP

#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <DataModels/ShaderNode.hpp>
#include <array>

template<std::size_t N>
class VecValue : public ShaderNode
{
	public:
		VecValue(ShaderGraph& graph);
		~VecValue() = default;

		QWidget* embeddedWidget() override;
		unsigned int nPorts(QtNodes::PortType portType) const override;

	protected:
		void UpdatePreview();

		virtual void ComputePreview(QPixmap& pixmap) const = 0;

		QLabel* m_pixmapLabel;
		QPixmap m_pixmap;
		QWidget* m_widget;
		QFormLayout* m_layout;
		std::array<QDoubleSpinBox*, N> m_values;
};

class Vec2Data : public QtNodes::NodeData
{
	public:
		using InternalType = Nz::Vector2f;

		inline Vec2Data(const InternalType& vec);

		QtNodes::NodeDataType type() const override
		{
			return Type();
		}

		static QtNodes::NodeDataType Type()
		{
			return { "vec2", "Vec2" };
		}

		InternalType value;
};

class Vec4Data : public QtNodes::NodeData
{
	public:
		using InternalType = Nz::Vector4f;

		inline Vec4Data(const InternalType& vec);

		QtNodes::NodeDataType type() const override
		{
			return Type();
		}

		static QtNodes::NodeDataType Type()
		{
			return { "vec4", "Vec4" };
		}

		InternalType value;
};

class Vec2Value : public VecValue<2>
{
	public:
		Vec2Value(ShaderGraph& graph);

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const override;

		QString caption() const override { return "Vec2 value"; }
		QString name() const override { return "Vec2Value"; }

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

	private:
		void ComputePreview(QPixmap& pixmap) const override;

		Nz::Vector2f GetValue() const;
};

class Vec4Value : public VecValue<4>
{
	public:
		Vec4Value(ShaderGraph& graph);

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const override;

		QString caption() const override { return "Vec4 value"; }
		QString name() const override { return "Vec4Value"; }

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

	private:
		void ComputePreview(QPixmap& pixmap) const override;

		Nz::Vector4f GetValue() const;
};

#include <DataModels/VecValue.inl>

#endif
