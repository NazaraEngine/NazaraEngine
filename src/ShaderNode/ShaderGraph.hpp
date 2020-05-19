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

		void AddTexture(std::string name, Nz::ShaderAst::ExpressionType type);

		Nz::ShaderAst::StatementPtr Generate();

		inline QtNodes::FlowScene& GetScene();
		inline const std::vector<TextureEntry>& GetTextures();

		NazaraSignal(OnTextureListUpdate, ShaderGraph*);

		struct TextureEntry
		{
			std::string name;
			Nz::ShaderAst::ExpressionType type;
		};

	private:
		std::shared_ptr<QtNodes::DataModelRegistry> BuildRegistry();

		QtNodes::FlowScene m_flowScene;
		std::vector<TextureEntry> m_textures;
};

#include <ShaderGraph.inl>

#endif
