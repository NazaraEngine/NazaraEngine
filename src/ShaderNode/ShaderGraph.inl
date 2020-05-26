#include <ShaderNode/ShaderGraph.hpp>

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

