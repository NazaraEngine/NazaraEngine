#include <ShaderGraph.hpp>

inline QtNodes::FlowScene& ShaderGraph::GetScene()
{
	return m_flowScene;
}

inline auto ShaderGraph::GetTextures() -> const std::vector<TextureEntry>&
{
	return m_textures;
}
