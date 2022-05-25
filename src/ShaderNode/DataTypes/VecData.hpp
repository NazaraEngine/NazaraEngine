#pragma once

#ifndef NAZARA_SHADERNODES_VECDATA_HPP
#define NAZARA_SHADERNODES_VECDATA_HPP

#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <ShaderNode/Previews/PreviewValues.hpp>
#include <NZSL/Ast/Nodes.hpp>
#include <nodes/NodeData>

struct VecData : public QtNodes::NodeData
{
	inline VecData(std::size_t componentCount);

	inline QtNodes::NodeDataType type() const override;

	static inline QtNodes::NodeDataType Type();

	std::size_t componentCount;
	PreviewValues preview;
};

template<std::size_t N> using VecType = nzsl::Vector<float, N>;

constexpr std::array<char, 4> s_vectorComponents = { 'X', 'Y', 'Z', 'W' };

#include <ShaderNode/DataTypes/VecData.inl>

#endif
