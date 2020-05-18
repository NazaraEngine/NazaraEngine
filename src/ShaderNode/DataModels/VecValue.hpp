#pragma once

#ifndef NAZARA_SHADERNODES_VECVALUE_HPP
#define NAZARA_SHADERNODES_VECVALUE_HPP

#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <DataModels/ShaderNode.hpp>
#include <array>

template<std::size_t N>
class VecValue : public ShaderNode
{
	public:
		VecValue();
		~VecValue() = default;

		QWidget* embeddedWidget() override;
		unsigned int nPorts(QtNodes::PortType portType) const override;

	protected:
		QWidget* m_widget;
		QFormLayout* m_layout;
		std::array<QDoubleSpinBox*, N> m_values;
};

class Vec4Data : public QtNodes::NodeData
{
	public:
		inline Vec4Data(const Nz::Vector4f& vec);

		QtNodes::NodeDataType type() const override
		{
			return Type();
		}

		static QtNodes::NodeDataType Type()
		{
			return { "vec4", "Vec4" };
		}

		Nz::Vector4f value;
};

class Vec4Value : public VecValue<4>
{
	public:
		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const override;

		QString caption() const override { return "Vec4 value"; }
		bool captionVisible() const override { return true; }
		QString name() const override { return "Vec4Value"; }

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData>, int) override {};

	private:
		Nz::Vector4f GetValue() const;
};

#include <DataModels/VecValue.inl>

#endif
