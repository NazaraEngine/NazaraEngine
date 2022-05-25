#pragma once

#ifndef NAZARA_SHADERNODES_TEXTUREDATA_HPP
#define NAZARA_SHADERNODES_TEXTUREDATA_HPP

#include <ShaderNode/Previews/PreviewValues.hpp>
#include <NZSL/Ast/Nodes.hpp>
#include <nodes/NodeData>

struct TextureData : public QtNodes::NodeData
{
	inline TextureData();

	PreviewValues preview;
};

struct Texture2Data : public TextureData
{
	QtNodes::NodeDataType type() const override
	{
		return Type();
	}

	static QtNodes::NodeDataType Type()
	{
		return { "tex2d", "Texture2D" };
	}
};

#include <ShaderNode/DataTypes/TextureData.inl>

#endif
