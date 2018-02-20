// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTRENDERQUEUE_HPP
#define NAZARA_ABSTRACTRENDERQUEUE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <vector>

namespace Nz
{
	class Drawable;
	class Material;
	class Texture;
	struct MeshData;
	struct VertexStruct_XYZ_Color_UV;

	class NAZARA_GRAPHICS_API AbstractRenderQueue
	{
		public:
			struct DirectionalLight;
			struct PointLight;
			struct SpotLight;

			AbstractRenderQueue() = default;
			AbstractRenderQueue(const AbstractRenderQueue&) = delete;
			AbstractRenderQueue(AbstractRenderQueue&&) = default;
			virtual ~AbstractRenderQueue();

			// Je ne suis vraiment pas fan du nombre de surcharges pour AddBillboards,
			// mais je n'ai pas d'autre solution tout aussi performante pour le moment...
			virtual void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr = nullptr, SparsePtr<const Color> colorPtr = nullptr) = 0;
			virtual void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr) = 0;
			virtual void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr = nullptr) = 0;
			virtual void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const Vector2f> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr) = 0;
			virtual void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr = nullptr, SparsePtr<const Color> colorPtr = nullptr) = 0;
			virtual void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const Vector2f> sinCosPtr, SparsePtr<const float> alphaPtr) = 0;
			virtual void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const Color> colorPtr = nullptr) = 0;
			virtual void AddBillboards(int renderOrder, const Material* material, unsigned int count, SparsePtr<const Vector3f> positionPtr, SparsePtr<const float> sizePtr, SparsePtr<const float> anglePtr, SparsePtr<const float> alphaPtr) = 0;
			virtual void AddDrawable(int renderOrder, const Drawable* drawable) = 0;
			virtual void AddDirectionalLight(const DirectionalLight& light);
			virtual void AddMesh(int renderOrder, const Material* material, const MeshData& meshData, const Boxf& meshAABB, const Matrix4f& transformMatrix, const Recti& scissorRect) = 0;
			virtual void AddPointLight(const PointLight& light);
			virtual void AddSpotLight(const SpotLight& light);
			virtual void AddSprites(int renderOrder, const Material* material, const VertexStruct_XYZ_Color_UV* vertices, std::size_t spriteCount, const Recti& scissorRect, const Texture* overlay = nullptr) = 0;

			virtual void Clear(bool fully = false);

			AbstractRenderQueue& operator=(const AbstractRenderQueue&) = delete;
			AbstractRenderQueue& operator=(AbstractRenderQueue&&) = default;

			struct DirectionalLight
			{
				Color color;
				Matrix4f transformMatrix;
				Vector3f direction;
				Texture* shadowMap;
				float ambientFactor;
				float diffuseFactor;
			};

			struct PointLight
			{
				Color color;
				Vector3f position;
				Texture* shadowMap;
				float ambientFactor;
				float attenuation;
				float diffuseFactor;
				float invRadius;
				float radius;
			};

			struct SpotLight
			{
				Color color;
				Matrix4f transformMatrix;
				Vector3f direction;
				Vector3f position;
				Texture* shadowMap;
				float ambientFactor;
				float attenuation;
				float diffuseFactor;
				float innerAngleCosine;
				float invRadius;
				float outerAngleCosine;
				float outerAngleTangent;
				float radius;
			};

			std::vector<DirectionalLight> directionalLights;
			std::vector<PointLight> pointLights;
			std::vector<SpotLight> spotLights;
	};
}

#endif // NAZARA_ABSTRACTRENDERQUEUE_HPP
