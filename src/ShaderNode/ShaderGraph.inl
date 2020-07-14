#include <ShaderNode/ShaderGraph.hpp>

inline auto ShaderGraph::GetBuffer(std::size_t bufferIndex) const -> const BufferEntry&
{
	assert(bufferIndex < m_buffers.size());
	return m_buffers[bufferIndex];
}

inline std::size_t ShaderGraph::GetBufferCount() const
{
	return m_buffers.size();
}

inline auto ShaderGraph::GetBuffers() const -> const std::vector<BufferEntry>&
{
	return m_buffers;
}

inline auto ShaderGraph::GetInput(std::size_t inputIndex) const -> const InputEntry&
{
	assert(inputIndex < m_inputs.size());
	return m_inputs[inputIndex];
}

inline std::size_t ShaderGraph::GetInputCount() const
{
	return m_inputs.size();
}

inline auto ShaderGraph::GetInputs() const -> const std::vector<InputEntry>&
{
	return m_inputs;
}

inline auto ShaderGraph::GetOutput(std::size_t outputIndex) const -> const OutputEntry&
{
	assert(outputIndex < m_outputs.size());
	return m_outputs[outputIndex];
}

inline std::size_t ShaderGraph::GetOutputCount() const
{
	return m_outputs.size();
}

inline auto ShaderGraph::GetOutputs() const -> const std::vector<OutputEntry>&
{
	return m_outputs;
}

inline auto ShaderGraph::GetStruct(std::size_t structIndex) const -> const StructEntry&
{
	assert(structIndex < m_structs.size());
	return m_structs[structIndex];
}

inline std::size_t ShaderGraph::GetStructCount() const
{
	return m_structs.size();
}

inline auto ShaderGraph::GetStructs() const -> const std::vector<StructEntry>&
{
	return m_structs;
}

inline const PreviewModel& ShaderGraph::GetPreviewModel() const
{
	return *m_previewModel;
}

inline QtNodes::FlowScene& ShaderGraph::GetScene()
{
	return m_flowScene;
}

inline auto ShaderGraph::GetTexture(std::size_t textureIndex) const -> const TextureEntry&
{
	assert(textureIndex < m_textures.size());
	return m_textures[textureIndex];
}

inline std::size_t ShaderGraph::GetTextureCount() const
{
	return m_textures.size();
}

inline auto ShaderGraph::GetTextures() const -> const std::vector<TextureEntry>&
{
	return m_textures;
}

