// Copyright (C) 2017 Jérôme Leclercq
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

namespace Nz
{
	struct Primitive
	{
		void MakeBox(const Vector3f& lengths, const Vector3ui& subdivision = Vector3ui(0U), const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakeBox(const Vector3f& lengths, const Vector3ui& subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakeCone(float length, float radius, unsigned int subdivision = 4, const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakeCone(float length, float radius, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakeCubicSphere(float size, unsigned int subdivision = 4, const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakeCubicSphere(float size, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakeIcoSphere(float size, unsigned int recursionLevel = 3, const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakeIcoSphere(float size, unsigned int recursionLevel, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakePlane(const Vector2f& size, const Vector2ui& subdivision, const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakePlane(const Vector2f& size, const Vector2ui& subdivision, const Planef& plane, const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakePlane(const Vector2f& size, const Vector2ui& subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakeUVSphere(float size, unsigned int sliceCount = 4, unsigned int stackCount = 4, const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		void MakeUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));

		static Primitive Box(const Vector3f& lengths, const Vector3ui& subdivision = Vector3ui(0U), const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive Box(const Vector3f& lengths, const Vector3ui& subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive Cone(float length, float radius, unsigned int subdivision = 4, const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive Cone(float length, float radius, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive CubicSphere(float size, unsigned int subdivision = 4, const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive CubicSphere(float size, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive IcoSphere(float size, unsigned int recursionLevel = 3, const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive IcoSphere(float size, unsigned int recursionLevel, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive Plane(const Vector2f& size, const Vector2ui& subdivision = Vector2ui(0U), const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive Plane(const Vector2f& size, const Vector2ui& subdivision, const Planef& plane, const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive Plane(const Vector2f& size, const Vector2ui& subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive UVSphere(float size, unsigned int sliceCount = 4, unsigned int stackCount = 4, const Matrix4f& transformMatrix = Matrix4f::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));
		static Primitive UVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity(), const Rectf& uvCoords = Rectf(0.f, 0.f, 1.f, 1.f));

		Matrix4f matrix;
		PrimitiveType type;
		Rectf textureCoords;

		union
		{
			struct
			{
				Vector3f lengths;
				Vector3ui subdivision;
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
				Vector2f size;
				Vector2ui subdivision;
			}
			plane;

			struct
			{
				SphereType type;
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
}

#include <Nazara/Core/Primitive.inl>

#endif // NAZARA_PRIMITIVE_HPP
