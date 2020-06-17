#pragma once

#ifndef NAZARA_SHADERNODES_FLOATVALUE_HPP
#define NAZARA_SHADERNODES_FLOATVALUE_HPP

#include <QtGui/QImage>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <array>

class FloatValue : public ShaderNode
{
	public:
		FloatValue(ShaderGraph& graph);
		~FloatValue() = default;

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

		float m_value;
};

#include <ShaderNode/DataModels/FloatValue.inl>

#endif
