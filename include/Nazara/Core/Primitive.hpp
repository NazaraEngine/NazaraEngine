// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PRIMITIVE_HPP
#define NAZARA_PRIMITIVE_HPP

#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Plane.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>

struct NzPrimitive
{
	void MakeBox(const NzVector3f& lengths, const NzVector3ui& subdivision = NzVector3ui(0U), const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakeBox(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakeCone(float length, float radius, unsigned int subdivision = 4, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakeCone(float length, float radius, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakeCubicSphere(float size, unsigned int subdivision = 4, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakeCubicSphere(float size, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakeIcoSphere(float size, unsigned int recursionLevel = 3, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakeIcoSphere(float size, unsigned int recursionLevel, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakePlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakePlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzPlanef& plane, const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakePlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakeUVSphere(float size, unsigned int sliceCount = 4, unsigned int stackCount = 4, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	void MakeUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));

	static NzPrimitive Box(const NzVector3f& lengths, const NzVector3ui& subdivision = NzVector3ui(0U), const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive Box(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive Cone(float length, float radius, unsigned int subdivision = 4, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive Cone(float length, float radius, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive CubicSphere(float size, unsigned int subdivision = 4, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive CubicSphere(float size, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive IcoSphere(float size, unsigned int recursionLevel = 3, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive IcoSphere(float size, unsigned int recursionLevel, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive Plane(const NzVector2f& size, const NzVector2ui& subdivision = NzVector2ui(0U), const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive Plane(const NzVector2f& size, const NzVector2ui& subdivision, const NzPlanef& plane, const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive Plane(const NzVector2f& size, const NzVector2ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive UVSphere(float size, unsigned int sliceCount = 4, unsigned int stackCount = 4, const NzMatrix4f& transformMatrix = NzMatrix4f::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));
	static NzPrimitive UVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzVector3f& position, const NzQuaternionf& rotation = NzQuaternionf::Identity(), const NzRectf& uvCoords = NzRectf(0.f, 0.f, 1.f, 1.f));

	NzMatrix4f matrix;
	nzPrimitiveType type;
	NzRectf textureCoords;

	union
	{
		struct
		{
			NzVector3f lengths;
			NzVector3ui subdivision;
		}
		box;

		struct
		{
			float length;
			float radius;
			unsigned int subdivision;
		}
		cone;

		struct
		{
			NzVector2f size;
			NzVector2ui subdivision;
		}
		plane;

		struct
		{
			nzSphereType type;
			float size;

			union
			{
				struct
				{
					unsigned int subdivision;
				}
				cubic;

				struct
				{
					unsigned int recursionLevel;
				}
				ico;

				struct
				{
					unsigned int sliceCount;
					unsigned int stackCount;
				}
				uv;
			};
		}
		sphere;
	};
};

#include <Nazara/Core/Primitive.inl>

#endif // NAZARA_PRIMITIVE_HPP
