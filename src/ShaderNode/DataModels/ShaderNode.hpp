#pragma once

#ifndef NAZARA_SHADERNODES_SHADERNODE_HPP
#define NAZARA_SHADERNODES_SHADERNODE_HPP

#include <Nazara/Renderer/ShaderAst.hpp>
#include <nodes/NodeDataModel>

class ShaderNode : public QtNodes::NodeDataModel
{
	public:
		virtual Nz::ShaderAst::ExpressionPtr GetExpression(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count) const = 0;
};

#include <DataModels/ShaderNode.inl>

#endif
