#include <DataModels/ShaderNode.hpp>

inline ShaderNode::ShaderNode(ShaderGraph& graph) :
m_graph(graph)
{
}

inline ShaderGraph& ShaderNode::GetGraph()
{
	return m_graph;
}

inline const ShaderGraph& ShaderNode::GetGraph() const
{
	return m_graph;
}
