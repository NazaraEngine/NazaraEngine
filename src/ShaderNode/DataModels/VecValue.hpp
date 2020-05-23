#pragma once

#ifndef NAZARA_SHADERNODES_VECVALUE_HPP
#define NAZARA_SHADERNODES_VECVALUE_HPP

#include <QtGui/QImage>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <DataModels/ShaderNode.hpp>
#include <DataModels/VecData.hpp>
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

template<typename Data>
class VecValue : public ShaderNode
{
	public:
		VecValue(ShaderGraph& graph);
		~VecValue() = default;

		QString caption() const override;
		QString name() const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		QWidget* embeddedWidget() override;
		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const override;

	protected:
		static constexpr std::size_t ComponentCount = Data::ComponentCount;

		QColor ToColor() const;
		VecType<ComponentCount> ToVector() const;
		void UpdatePreview();

		QLabel* m_pixmapLabel;
		QPixmap m_pixmap;
		QWidget* m_widget;
		QFormLayout* m_layout;
		std::array<QDoubleSpinBox*, ComponentCount> m_spinboxes;
};

using Vec2Value = VecValue<Vec2Data>;
using Vec3Value = VecValue<Vec3Data>;
using Vec4Value = VecValue<Vec4Data>;

#include <DataModels/VecValue.inl>

#endif
