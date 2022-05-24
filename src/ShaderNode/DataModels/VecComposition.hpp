#pragma once

#ifndef NAZARA_SHADERNODES_VECTOR_COMPOSITION_HPP
#define NAZARA_SHADERNODES_VECTOR_COMPOSITION_HPP

#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

template<std::size_t ComponentCount>
class VecComposition : public ShaderNode
{
	public:
		VecComposition(ShaderGraph& graph);
		~VecComposition() = default;

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;

		QString caption() const override;
		QString name() const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
		unsigned int nPorts(QtNodes::PortType portType) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	private:
		bool ComputePreview(QPixmap& pixmap) override;
		void UpdateOutput();

		std::array<std::shared_ptr<FloatData>, ComponentCount> m_inputs;
		std::shared_ptr<VecData> m_output;
};

using Vec2Composition = VecComposition<2>;
using Vec3Composition = VecComposition<3>;
using Vec4Composition = VecComposition<4>;

#include <ShaderNode/DataModels/VecComposition.inl>

#endif
