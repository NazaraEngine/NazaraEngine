#pragma once

#ifndef NAZARA_SHADERNODES_TEXTUREVALUE_HPP
#define NAZARA_SHADERNODES_TEXTUREVALUE_HPP

#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <array>

class TextureValue : public ShaderNode
{
	public:
		TextureValue(ShaderGraph& graph);
		~TextureValue() = default;

		void BuildNodeEdition(QFormLayout* layout) override;

		Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* /*expressions*/, std::size_t count) const override;

		QString caption() const override { return "Texture"; }
		QString name() const override { return "Texture"; }

		unsigned int nPorts(QtNodes::PortType portType) const override;

		QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

		std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

		QtNodes::NodeValidationState validationState() const override;
		QString validationMessage() const override;

	protected:
		bool ComputePreview(QPixmap& pixmap) override;
		void OnTextureListUpdate();

		NazaraSlot(ShaderGraph, OnTextureListUpdate, m_onTextureListUpdateSlot);
		NazaraSlot(ShaderGraph, OnTexturePreviewUpdate, m_onTexturePreviewUpdateSlot);

		std::optional<std::size_t> m_currentTextureIndex;
		std::string m_currentTextureText;
};

#include <ShaderNode/DataModels/TextureValue.inl>

#endif
