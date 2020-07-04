#pragma once

#ifndef NAZARA_SHADERNODES_TEXTUREEDITOR_HPP
#define NAZARA_SHADERNODES_TEXTUREEDITOR_HPP

#include <ShaderNode/ShaderGraph.hpp>
#include <QtWidgets/QWidget>
#include <optional>

class QLabel;
class QListWidget;
class QVBoxLayout;

class TextureEditor : public QWidget
{
	public:
		TextureEditor(ShaderGraph& graph);
		~TextureEditor() = default;

	private:
		void OnAddTexture();
		void OnEditTexture(int inputIndex);
		void OnLoadTexture();
		void OnTextureSelectionUpdate(int textureIndex);
		void OnTextureListUpdate(ShaderGraph* graph);
		void OnTexturePreviewUpdate(ShaderGraph* graph, std::size_t textureIndex);
		void OnTextureUpdate(ShaderGraph* graph, std::size_t textureIndex);
		void RefreshTextures();
		void UpdateTexturePreview();

		NazaraSlot(ShaderGraph, OnTextureListUpdate, m_onTextureListUpdateSlot);
		NazaraSlot(ShaderGraph, OnTexturePreviewUpdate, m_onTexturePreviewUpdateSlot);
		NazaraSlot(ShaderGraph, OnTextureUpdate, m_onTextureUpdateSlot);

		std::optional<std::size_t> m_currentTextureIndex;
		ShaderGraph& m_shaderGraph;
		QLabel* m_pixmapLabel;
		QListWidget* m_textureList;
		QVBoxLayout* m_layout;
};

#include <ShaderNode/Widgets/TextureEditor.inl>

#endif
