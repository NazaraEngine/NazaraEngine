#pragma once

#ifndef NAZARA_SHADERNODES_SHADERNODE_HPP
#define NAZARA_SHADERNODES_SHADERNODE_HPP

#include <Nazara/Renderer/ShaderAst.hpp>
#include <nodes/NodeDataModel>

class ShaderGraph;

class ShaderNode : public QtNodes::NodeDataModel
{
	public:
		inline ShaderNode(ShaderGraph& graph);

		virtual Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const = 0;
		inline ShaderGraph& GetGraph();
		inline const ShaderGraph& GetGraph() const;

		void setInData(std::shared_ptr<QtNodes::NodeData>, int) override;

	private:
		ShaderGraph& m_graph;
};

#include <DataModels/ShaderNode.inl>

#endif
