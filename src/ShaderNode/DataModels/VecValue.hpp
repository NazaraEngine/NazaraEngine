#pragma once

#ifndef NAZARA_SHADERNODES_VECVALUE_HPP
#define NAZARA_SHADERNODES_VECVALUE_HPP

#include <QtGui/QImage>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <array>

template<std::size_t ComponentCount>
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

		void BuildNodeEdition(QFormLayout* layout) override;

		Nz::ShaderNodes::ExpressionPtr GetExpression(Nz::ShaderNodes::ExpressionPtr* expressions, std::size_t count) const override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;
		QColor ToColor() const;

		void restore(const QJsonObject& data) override;
		QJsonObject save() const override;

		VecType<ComponentCount> m_value;
};

using Vec2Value = VecValue<2>;
using Vec3Value = VecValue<3>;
using Vec4Value = VecValue<4>;

#include <ShaderNode/DataModels/VecValue.inl>

#endif
