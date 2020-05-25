#pragma once

#ifndef NAZARA_SHADERNODES_VECDATA_HPP
#define NAZARA_SHADERNODES_VECDATA_HPP

#include <Nazara/Renderer/ShaderAst.hpp>
#include <nodes/NodeData>
#include <QtGui/QImage>

struct VecData : public QtNodes::NodeData
{
	inline VecData();

	QImage preview;
};

struct Vec2Data : public VecData
{
	static constexpr std::size_t ComponentCount = 2;
	static constexpr Nz::ShaderAst::ExpressionType ExpressionType = Nz::ShaderAst::ExpressionType::Float2;

	QtNodes::NodeDataType type() const override
	{
		return Type();
	}

	static QtNodes::NodeDataType Type()
	{
		return { "vec2", "Vec2" };
	}
};

struct Vec3Data : public VecData
{
	static constexpr std::size_t ComponentCount = 3;
	static constexpr Nz::ShaderAst::ExpressionType ExpressionType = Nz::ShaderAst::ExpressionType::Float3;

	QtNodes::NodeDataType type() const override
	{
		return Type();
	}

	static QtNodes::NodeDataType Type()
	{
		return { "vec3", "Vec3" };
	}
};

struct Vec4Data : public VecData
{
	static constexpr std::size_t ComponentCount = 4;
	static constexpr Nz::ShaderAst::ExpressionType ExpressionType = Nz::ShaderAst::ExpressionType::Float4;

	QtNodes::NodeDataType type() const override
	{
		return Type();
	}

	static QtNodes::NodeDataType Type()
	{
		return { "vec4", "Vec4" };
	}
};

#include <ShaderNode/DataModels/VecData.inl>

#endif
