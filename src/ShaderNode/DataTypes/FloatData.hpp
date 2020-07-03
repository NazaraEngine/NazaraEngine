#pragma once

#ifndef NAZARA_SHADERNODES_FLOATDATA_HPP
#define NAZARA_SHADERNODES_FLOATDATA_HPP

#include <ShaderNode/Previews/PreviewValues.hpp>
#include <nodes/NodeData>

struct FloatData : public QtNodes::NodeData
{
	inline FloatData();

	QtNodes::NodeDataType type() const override
	{
		return Type();
	}

	static QtNodes::NodeDataType Type()
	{
		return { "float", "Float" };
	}

	PreviewValues preview;
};

#include <ShaderNode/DataTypes/FloatData.inl>

#endif
