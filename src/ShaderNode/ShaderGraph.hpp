#pragma once

#ifndef NAZARA_SHADERNODES_SHADERGRAPH_HPP
#define NAZARA_SHADERNODES_SHADERGRAPH_HPP

#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>
#include <nodes/FlowScene>
#include <ShaderNode/Enums.hpp>
#include <ShaderNode/Previews/PreviewModel.hpp>
#include <optional>
#include <string>
#include <vector>

class ShaderNode;

class ShaderGraph
{
	public:
		struct BufferEntry;
		struct ConditionEntry;
		struct InputEntry;
		struct OutputEntry;
		struct StructEntry;
		struct StructMemberEntry;
		struct TextureEntry;

		ShaderGraph();
		~ShaderGraph();

		std::size_t AddBuffer(std::string name, BufferType bufferType, std::size_t structIndex, std::size_t bindingIndex);
		std::size_t AddCondition(std::string name);
		std::size_t AddInput(std::string name, PrimitiveType type, InputRole role, std::size_t roleIndex, std::size_t locationIndex);
		std::size_t AddOutput(std::string name, PrimitiveType type, std::size_t locationIndex);
		std::size_t AddStruct(std::string name, std::vector<StructMemberEntry> members);
		std::size_t AddTexture(std::string name, TextureType type, std::size_t bindingIndex);

		void Clear();

		void EnableCondition(std::size_t conditionIndex, bool enable);

		inline const BufferEntry& GetBuffer(std::size_t bufferIndex) const;
		inline std::size_t GetBufferCount() const;
		inline const std::vector<BufferEntry>& GetBuffers() const;
		inline const ConditionEntry& GetCondition(std::size_t conditionIndex) const;
		inline std::size_t GetConditionCount() const;
		inline const std::vector<ConditionEntry>& GetConditions() const;
		inline const InputEntry& GetInput(std::size_t bufferIndex) const;
		inline std::size_t GetInputCount() const;
		inline const std::vector<InputEntry>& GetInputs() const;
		inline const OutputEntry& GetOutput(std::size_t outputIndex) const;
		inline std::size_t GetOutputCount() const;
		inline const std::vector<OutputEntry>& GetOutputs() const;
		inline const StructEntry& GetStruct(std::size_t structIndex) const;
		inline std::size_t GetStructCount() const;
		inline const std::vector<StructEntry>& GetStructs() const;
		inline const PreviewModel& GetPreviewModel() const;
		inline QtNodes::FlowScene& GetScene();
		inline const TextureEntry& GetTexture(std::size_t textureIndex) const;
		inline std::size_t GetTextureCount() const;
		inline const std::vector<TextureEntry>& GetTextures() const;
		inline ShaderType GetType() const;

		inline bool IsConditionEnabled(std::size_t conditionIndex) const;

		void Load(const QJsonObject& data);
		QJsonObject Save();

		Nz::ShaderAst::StatementPtr ToAst() const;
		Nz::ShaderAst::ExpressionType ToShaderExpressionType(const std::variant<PrimitiveType, std::size_t>& type) const;

		void UpdateBuffer(std::size_t bufferIndex, std::string name, BufferType bufferType, std::size_t structIndex, std::size_t bindingIndex);
		void UpdateCondition(std::size_t conditionIndex, std::string condition);
		void UpdateInput(std::size_t inputIndex, std::string name, PrimitiveType type, InputRole role, std::size_t roleIndex, std::size_t locationIndex);
		void UpdateOutput(std::size_t outputIndex, std::string name, PrimitiveType type, std::size_t locationIndex);
		void UpdateStruct(std::size_t structIndex, std::string name, std::vector<StructMemberEntry> members);
		void UpdateTexture(std::size_t textureIndex, std::string name, TextureType type, std::size_t bindingIndex);
		void UpdateTexturePreview(std::size_t texture, QImage preview);
		void UpdateType(ShaderType type);

		struct BufferEntry
		{
			std::size_t bindingIndex;
			std::size_t structIndex;
			std::string name;
			BufferType type;
		};

		struct ConditionEntry
		{
			std::string name;
			bool enabled = false;
		};

		struct InputEntry
		{
			std::size_t locationIndex;
			std::size_t roleIndex;
			std::string name;
			InputRole role;
			PrimitiveType type;
		};

		struct OutputEntry
		{
			std::size_t locationIndex;
			std::string name;
			PrimitiveType type;
		};

		struct StructEntry
		{
			std::string name;
			std::vector<StructMemberEntry> members;
		};

		struct StructMemberEntry
		{
			std::string name;
			std::variant<PrimitiveType, std::size_t /*structIndex*/> type;
		};

		struct TextureEntry
		{
			std::size_t bindingIndex;
			std::string name;
			TextureType type;
			QImage preview;
		};

		NazaraSignal(OnBufferListUpdate, ShaderGraph*);
		NazaraSignal(OnBufferUpdate, ShaderGraph*, std::size_t /*bufferIndex*/);
		NazaraSignal(OnConditionListUpdate, ShaderGraph*);
		NazaraSignal(OnConditionUpdate, ShaderGraph*, std::size_t /*conditionIndex*/);
		NazaraSignal(OnInputListUpdate, ShaderGraph*);
		NazaraSignal(OnInputUpdate, ShaderGraph*, std::size_t /*inputIndex*/);
		NazaraSignal(OnOutputListUpdate, ShaderGraph*);
		NazaraSignal(OnOutputUpdate, ShaderGraph*, std::size_t /*outputIndex*/);
		NazaraSignal(OnSelectedNodeUpdate, ShaderGraph*, ShaderNode* /*node*/);
		NazaraSignal(OnStructListUpdate, ShaderGraph*);
		NazaraSignal(OnStructUpdate, ShaderGraph*, std::size_t /*structIndex*/);
		NazaraSignal(OnTextureListUpdate, ShaderGraph*);
		NazaraSignal(OnTexturePreviewUpdate, ShaderGraph*, std::size_t /*textureIndex*/);
		NazaraSignal(OnTextureUpdate, ShaderGraph*, std::size_t /*textureIndex*/);
		NazaraSignal(OnTypeUpdated, ShaderGraph*);

		static QtNodes::NodeDataType ToNodeDataType(PrimitiveType type);
		static Nz::ShaderAst::ExpressionType ToShaderExpressionType(PrimitiveType type);
		static Nz::ShaderAst::ExpressionType ToShaderExpressionType(TextureType type);
		static Nz::ShaderStageType ToShaderStageType(ShaderType type);

	private:
		std::shared_ptr<QtNodes::DataModelRegistry> BuildRegistry();
		std::unique_ptr<Nz::ShaderAst::DeclareFunctionStatement> ToFunction() const;

		mutable std::optional<QtNodes::FlowScene> m_flowScene;
		std::vector<BufferEntry> m_buffers;
		std::vector<ConditionEntry> m_conditions;
		std::vector<InputEntry> m_inputs;
		std::vector<OutputEntry> m_outputs;
		std::vector<StructEntry> m_structs;
		std::vector<TextureEntry> m_textures;
		std::unique_ptr<PreviewModel> m_previewModel;
		ShaderType m_type;
};

#include <ShaderNode/ShaderGraph.inl>

#endif
