#pragma once

#ifndef NAZARA_SHADERNODES_SHADERGRAPH_HPP
#define NAZARA_SHADERNODES_SHADERGRAPH_HPP

#include <Nazara/Core/Signal.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <string>
#include <vector>

class ShaderGraph
{
	public:
		struct TextureEntry;

		ShaderGraph();
		~ShaderGraph() = default;

		std::size_t AddTexture(std::string name, Nz::ShaderAst::ExpressionType type);

		inline QtNodes::FlowScene& GetScene();
		inline const TextureEntry& GetTexture(std::size_t textureIndex) const;
		inline const std::vector<TextureEntry>& GetTextures();

		Nz::ShaderAst::StatementPtr ToAst();

		void UpdateTexturePreview(std::size_t texture, QImage preview);

		struct TextureEntry
		{
			std::string name;
			Nz::ShaderAst::ExpressionType type;
			QImage preview;
		};

		NazaraSignal(OnTextureListUpdate, ShaderGraph*);
		NazaraSignal(OnTexturePreviewUpdate, ShaderGraph*, std::size_t /*textureIndex*/);

	private:
		std::shared_ptr<QtNodes::DataModelRegistry> BuildRegistry();

		QtNodes::FlowScene m_flowScene;
		std::vector<TextureEntry> m_textures;
};

#include <ShaderGraph.inl>

#endif
