// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTRENDERQUEUE_HPP
#define NAZARA_ABSTRACTRENDERQUEUE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <vector>

class NzDrawable;
class NzMaterial;
class NzTexture;
struct NzMeshData;

class NAZARA_GRAPHICS_API NzAbstractRenderQueue : NzNonCopyable
{
	public:
		struct DirectionalLight;
		struct PointLight;
		struct SpotLight;

		NzAbstractRenderQueue() = default;
		virtual ~NzAbstractRenderQueue();

		// Je ne suis vraiment pas fan du nombre de surcharges pour AddBillboards,
		// mais je n'ai pas d'autre solution tout aussi performante pour le moment...
		virtual void AddBillboard(const NzMaterial* material, const NzVector3f& position, const NzVector2f& size, const NzVector2f& sinCos = NzVector2f(0.f, 1.f), const NzColor& color = NzColor::White) = 0;
		virtual void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr = nullptr, NzSparsePtr<const NzColor> colorPtr = nullptr) = 0;
		virtual void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr) = 0;
		virtual void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr = nullptr) = 0;
		virtual void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const NzVector2f> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr) = 0;
		virtual void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr = nullptr, NzSparsePtr<const NzColor> colorPtr = nullptr) = 0;
		virtual void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const NzVector2f> sinCosPtr, NzSparsePtr<const float> alphaPtr) = 0;
		virtual void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const NzColor> colorPtr = nullptr) = 0;
		virtual void AddBillboards(const NzMaterial* material, unsigned int count, NzSparsePtr<const NzVector3f> positionPtr, NzSparsePtr<const float> sizePtr, NzSparsePtr<const float> anglePtr, NzSparsePtr<const float> alphaPtr) = 0;
		virtual void AddDrawable(const NzDrawable* drawable) = 0;
		virtual void AddDirectionalLight(const DirectionalLight& light);
		virtual void AddMesh(const NzMaterial* material, const NzMeshData& meshData, const NzBoxf& meshAABB, const NzMatrix4f& transformMatrix) = 0;
		virtual void AddPointLight(const PointLight& light);
		virtual void AddSpotLight(const SpotLight& light);
		virtual void AddSprites(const NzMaterial* material, const NzVertexStruct_XYZ_Color_UV* vertices, unsigned int spriteCount, const NzTexture* overlay = nullptr) = 0;

		virtual void Clear(bool fully = false);


		struct DirectionalLight
		{
			NzColor color;
			NzVector3f direction;
			float ambientFactor;
			float diffuseFactor;
		};

		struct PointLight
		{
			NzColor color;
			NzVector3f position;
			float ambientFactor;
			float attenuation;
			float diffuseFactor;
			float invRadius;
			float radius;
		};

		struct SpotLight
		{
			NzColor color;
			NzVector3f direction;
			NzVector3f position;
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

#endif // NAZARA_ABSTRACTRENDERQUEUE_HPP
