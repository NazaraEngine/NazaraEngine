#pragma once

#ifndef NAZARA_SHADERNODES_VECDATA_HPP
#define NAZARA_SHADERNODES_VECDATA_HPP

#include <Nazara/Renderer/ShaderNodes.hpp>
#include <nodes/NodeData>
#include <QtGui/QImage>

struct VecData : public QtNodes::NodeData
{
	inline VecData(std::size_t componentCount);

	inline QtNodes::NodeDataType type() const override;

	Nz::ShaderNodes::ExpressionType GetExpressionType() const;

	static inline QtNodes::NodeDataType Type();

	std::size_t componentCount;
	QImage preview;
};

template<std::size_t N>
struct VecExpressionTypeHelper;

template<>
struct VecExpressionTypeHelper<1>
{
	static constexpr Nz::ShaderNodes::ExpressionType ExpressionType = Nz::ShaderNodes::ExpressionType::Float1;
};

template<>
struct VecExpressionTypeHelper<2>
{
	static constexpr Nz::ShaderNodes::ExpressionType ExpressionType = Nz::ShaderNodes::ExpressionType::Float2;
};

template<>
struct VecExpressionTypeHelper<3>
{
	static constexpr Nz::ShaderNodes::ExpressionType ExpressionType = Nz::ShaderNodes::ExpressionType::Float3;
};

template<>
struct VecExpressionTypeHelper<4>
{
	static constexpr Nz::ShaderNodes::ExpressionType ExpressionType = Nz::ShaderNodes::ExpressionType::Float4;
};

template<std::size_t N> constexpr Nz::ShaderNodes::ExpressionType VecExpressionType = VecExpressionTypeHelper<N>::template ExpressionType;


struct VecTypeDummy {};

template<std::size_t N>
struct VecTypeHelper;

template<>
struct VecTypeHelper<0>
{
	using Type = VecTypeDummy;
};

template<>
struct VecTypeHelper<1>
{
	using Type = std::array<float, 1>; //< To allow [0]
};

template<>
struct VecTypeHelper<2>
{
	using Type = Nz::Vector2f;
};

template<>
struct VecTypeHelper<3>
{
	using Type = Nz::Vector3f;
};

template<>
struct VecTypeHelper<4>
{
	using Type = Nz::Vector4f;
};

template<std::size_t N> using VecType = typename VecTypeHelper<N>::template Type;

constexpr std::array<char, 4> s_vectorComponents = { 'X', 'Y', 'Z', 'W' };

#include <ShaderNode/DataTypes/VecData.inl>

#endif
