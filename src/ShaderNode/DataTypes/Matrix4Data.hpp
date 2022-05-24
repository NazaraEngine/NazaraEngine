#pragma once

#ifndef NAZARA_SHADERNODES_MATRIXDATA_HPP
#define NAZARA_SHADERNODES_MATRIXDATA_HPP

#include <NZSL/Ast/Nodes.hpp>
#include <ShaderNode/Previews/PreviewValues.hpp>
#include <nodes/NodeData>

struct Matrix4Data : public QtNodes::NodeData
{
	inline QtNodes::NodeDataType type() const override;

	static inline QtNodes::NodeDataType Type();
};

#include <ShaderNode/DataTypes/Matrix4Data.inl>

#endif
