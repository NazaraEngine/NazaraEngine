#pragma once

#ifndef NAZARA_SHADERNODES_VECVALUE_HPP
#define NAZARA_SHADERNODES_VECVALUE_HPP

#include <QtGui/QImage>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataModels/VecData.hpp>
#include <array>

template<typename Data>
class VecValue : public ShaderNode
{
	public:
		VecValue(ShaderGraph& graph);
		~VecValue() = default;

		QString caption() const override;
		QString name() const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void BuildNodeEdition(QVBoxLayout* layout) override;

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;

		static constexpr std::size_t ComponentCount = Data::ComponentCount;

		QColor ToColor() const;

		VecType<ComponentCount> m_value;
};

using Vec2Value = VecValue<Vec2Data>;
using Vec3Value = VecValue<Vec3Data>;
using Vec4Value = VecValue<Vec4Data>;

#include <ShaderNode/DataModels/VecValue.inl>

#endif
