#include <ShaderNode/DataModels/ShaderNode.hpp>

inline ShaderGraph& ShaderNode::GetGraph()
{
	return m_graph;
}

inline const ShaderGraph& ShaderNode::GetGraph() const
{
	return m_graph;
}

inline const std::string& ShaderNode::GetVariableName() const
{
	return m_variableName;
}

inline void ShaderNode::SetPreviewSize(const Nz::Vector2i& size)
{
	m_previewSize = size;
	if (m_isPreviewEnabled)
	{
		UpdatePreview();
		embeddedWidgetSizeUpdated();
	}
}

inline void ShaderNode::SetVariableName(std::string variableName)
{
	m_variableName = std::move(variableName);
}

inline void ShaderNode::DisableCustomVariableName()
{
	return EnableCustomVariableName(false);
}

inline void ShaderNode::EnableCustomVariableName(bool enable)
{
	m_enableCustomVariableName = enable;
	if (!m_enableCustomVariableName)
		m_variableName.clear();
}
