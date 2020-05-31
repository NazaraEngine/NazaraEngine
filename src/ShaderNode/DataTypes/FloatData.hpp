#pragma once

#ifndef NAZARA_SHADERNODES_FLOATDATA_HPP
#define NAZARA_SHADERNODES_FLOATDATA_HPP

#include <nodes/NodeData>
#include <QtGui/QImage>

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

	QImage preview;
};

#include <ShaderNode/DataTypes/FloatData.inl>

#endif
