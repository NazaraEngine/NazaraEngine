#pragma once

#ifndef NAZARA_SHADERNODES_SAMPLETEXTURE_HPP
#define NAZARA_SHADERNODES_SAMPLETEXTURE_HPP

#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataTypes/TextureData.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>

class SampleTexture : public ShaderNode
{
	public:
		SampleTexture(ShaderGraph& graph);
		~SampleTexture() = default;

		nzsl::Ast::NodePtr BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const override;

		QString caption() const override { return "Sample texture"; }
		QString name() const override { return "SampleTexture"; }

		unsigned int nPorts(QtNodes::PortType portType) const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		QString portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		bool portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		void setInData(std::shared_ptr<QtNodes::NodeData> value, int index) override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	protected:
		bool ComputePreview(QPixmap& pixmap) override;
		void UpdateOutput();

		std::shared_ptr<Texture2Data> m_texture;
		std::shared_ptr<VecData> m_uv;
		std::shared_ptr<VecData> m_output;
};

#include <ShaderNode/DataModels/SampleTexture.inl>

#endif
