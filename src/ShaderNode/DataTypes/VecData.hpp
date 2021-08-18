#pragma once

#ifndef NAZARA_SHADERNODES_VECDATA_HPP
#define NAZARA_SHADERNODES_VECDATA_HPP

#include <Nazara/Shader/Ast/Nodes.hpp>
#include <ShaderNode/Previews/PreviewValues.hpp>
#include <nodes/NodeData>

struct VecData : public QtNodes::NodeData
{
	inline VecData(std::size_t componentCount);

	inline QtNodes::NodeDataType type() const override;

	static inline QtNodes::NodeDataType Type();

	std::size_t componentCount;
	PreviewValues preview;
};


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

template<std::size_t N> using VecType = typename VecTypeHelper<N>::Type;

constexpr std::array<char, 4> s_vectorComponents = { 'X', 'Y', 'Z', 'W' };

#include <ShaderNode/DataTypes/VecData.inl>

#endif
