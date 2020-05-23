#pragma once

#ifndef NAZARA_SHADERNODES_SHADERGRAPH_HPP
#define NAZARA_SHADERNODES_SHADERGRAPH_HPP

#include <Nazara/Core/Signal.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <nodes/FlowScene>
#include <Enums.hpp>
#include <Previews/PreviewModel.hpp>
#include <string>
#include <vector>

class ShaderGraph
{
	public:
		struct InputEntry;
		struct TextureEntry;

		ShaderGraph();
		~ShaderGraph();

		std::size_t AddInput(std::string name, InputType type, InputRole role, std::size_t roleIndex);
		std::size_t AddTexture(std::string name, TextureType type);

		inline const InputEntry& GetInput(std::size_t inputIndex) const;
		inline const std::vector<InputEntry>& GetInputs() const;
		inline const PreviewModel& GetPreviewModel() const;
		inline QtNodes::FlowScene& GetScene();
		inline const TextureEntry& GetTexture(std::size_t textureIndex) const;
		inline const std::vector<TextureEntry>& GetTextures() const;

		Nz::ShaderAst::StatementPtr ToAst();

		void UpdateInput(std::size_t inputIndex, std::string name, InputType type, InputRole role, std::size_t roleIndex);
		void UpdateTexturePreview(std::size_t texture, QImage preview);

		struct InputEntry
		{
			std::size_t roleIndex;
			std::string name;
			InputRole role;
			InputType type;
		};

		struct TextureEntry
		{
			std::string name;
			TextureType type;
			QImage preview;
		};

		NazaraSignal(OnInputListUpdate, ShaderGraph*);
		NazaraSignal(OnInputUpdate, ShaderGraph*, std::size_t /*inputIndex*/);
		NazaraSignal(OnTextureListUpdate, ShaderGraph*);
		NazaraSignal(OnTexturePreviewUpdate, ShaderGraph*, std::size_t /*textureIndex*/);

	private:
		std::shared_ptr<QtNodes::DataModelRegistry> BuildRegistry();

		QtNodes::FlowScene m_flowScene;
		std::vector<InputEntry> m_inputs;
		std::vector<TextureEntry> m_textures;
		std::unique_ptr<PreviewModel> m_previewModel;
};

#include <ShaderGraph.inl>

#endif
