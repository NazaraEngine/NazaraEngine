#pragma once

#ifndef NAZARA_SHADERNODES_BOOLDATA_HPP
#define NAZARA_SHADERNODES_BOOLDATA_HPP

#include <ShaderNode/Previews/PreviewValues.hpp>
#include <nodes/NodeData>

struct BoolData : public QtNodes::NodeData
{
	inline BoolData();

	QtNodes::NodeDataType type() const override
	{
		return Type();
	}

	static QtNodes::NodeDataType Type()
	{
		return { "bool", "Bool" };
	}

	PreviewValues preview;
};

#include <ShaderNode/DataTypes/BoolData.inl>

#endif
