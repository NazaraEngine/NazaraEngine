#include <ShaderGraph.hpp>

inline QtNodes::FlowScene& ShaderGraph::GetScene()
{
	return m_flowScene;
}

inline auto ShaderGraph::GetTexture(std::size_t textureIndex) const -> const TextureEntry&
{
	assert(textureIndex < m_textures.size());
	return m_textures[textureIndex];
}

inline auto ShaderGraph::GetTextures() -> const std::vector<TextureEntry>&
{
	return m_textures;
}

