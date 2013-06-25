// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PRIMITIVELIST_HPP
#define NAZARA_PRIMITIVELIST_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Math/Quaternion.hpp>

class NAZARA_API NzPrimitiveList
{
	public:
		NzPrimitiveList() = default;
		~NzPrimitiveList() = default;

		void AddBox(const NzVector3f& lengths, const NzVector3ui& subdivision = NzVector3ui(0U), const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		void AddBox(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity());
		void AddCubicSphere(float size, unsigned int subdivision = 4, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		void AddCubicSphere(float size, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity());
		void AddIcoSphere(float size, unsigned int recursionLevel = 3, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		void AddIcoSphere(float size, unsigned int recursionLevel, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity());
		void AddPlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		void AddPlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzPlanef& planeInfo);
		void AddPlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity());
		void AddUVSphere(float size, unsigned int sliceCount = 4, unsigned int stackCount = 4, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity());
		void AddUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity());

		NzPrimitive& GetPrimitive(unsigned int i);
		const NzPrimitive& GetPrimitive(unsigned int i) const;
		unsigned int GetSize() const;

		NzPrimitive& operator()(unsigned int i);
		const NzPrimitive& operator()(unsigned int i) const;

	private:
		std::vector<NzPrimitive> m_primitives;
};

#endif // NAZARA_PRIMITIVELIST_HPP
