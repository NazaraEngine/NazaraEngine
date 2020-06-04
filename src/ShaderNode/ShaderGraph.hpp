#pragma once

#ifndef NAZARA_SHADERNODES_SHADERGRAPH_HPP
#define NAZARA_SHADERNODES_SHADERGRAPH_HPP

#include <Nazara/Core/Signal.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <nodes/FlowScene>
#include <ShaderNode/Enums.hpp>
#include <ShaderNode/Previews/PreviewModel.hpp>
#include <string>
#include <vector>

class ShaderNode;

class ShaderGraph
{
	public:
		struct InputEntry;
		struct OutputEntry;
		struct TextureEntry;

		ShaderGraph();
		~ShaderGraph();

		std::size_t AddInput(std::string name, InOutType type, InputRole role, std::size_t roleIndex);
		std::size_t AddOutput(std::string name, InOutType type);
		std::size_t AddTexture(std::string name, TextureType type);

		void Clear();

		inline const InputEntry& GetInput(std::size_t inputIndex) const;
		inline std::size_t GetInputCount() const;
		inline const std::vector<InputEntry>& GetInputs() const;
		inline const OutputEntry& GetOutput(std::size_t outputIndex) const;
		inline std::size_t GetOutputCount() const;
		inline const std::vector<OutputEntry>& GetOutputs() const;
		inline const PreviewModel& GetPreviewModel() const;
		inline QtNodes::FlowScene& GetScene();
		inline const TextureEntry& GetTexture(std::size_t textureIndex) const;
		inline std::size_t GetTextureCount() const;
		inline const std::vector<TextureEntry>& GetTextures() const;

		void Load(const QJsonObject& data);
		QJsonObject Save();

		Nz::ShaderAst::StatementPtr ToAst();

		void UpdateInput(std::size_t inputIndex, std::string name, InOutType type, InputRole role, std::size_t roleIndex);
		void UpdateOutput(std::size_t outputIndex, std::string name, InOutType type);
		void UpdateTexturePreview(std::size_t texture, QImage preview);

		struct InputEntry
		{
			std::size_t roleIndex;
			std::string name;
			InputRole role;
			InOutType type;
		};

		struct OutputEntry
		{
			std::string name;
			InOutType type;
		};

		struct TextureEntry
		{
			std::string name;
			TextureType type;
			QImage preview;
		};

		NazaraSignal(OnInputListUpdate, ShaderGraph*);
		NazaraSignal(OnInputUpdate, ShaderGraph*, std::size_t /*inputIndex*/);
		NazaraSignal(OnOutputListUpdate, ShaderGraph*);
		NazaraSignal(OnOutputUpdate, ShaderGraph*, std::size_t /*outputIndex*/);
		NazaraSignal(OnSelectedNodeUpdate, ShaderGraph*, ShaderNode* /*node*/);
		NazaraSignal(OnTextureListUpdate, ShaderGraph*);
		NazaraSignal(OnTexturePreviewUpdate, ShaderGraph*, std::size_t /*textureIndex*/);

	private:
		std::shared_ptr<QtNodes::DataModelRegistry> BuildRegistry();

		QtNodes::FlowScene m_flowScene;
		std::vector<InputEntry> m_inputs;
		std::vector<OutputEntry> m_outputs;
		std::vector<TextureEntry> m_textures;
		std::unique_ptr<PreviewModel> m_previewModel;
};

#include <ShaderNode/ShaderGraph.inl>

#endif
