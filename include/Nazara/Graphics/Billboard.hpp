// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BILLBOARD_HPP
#define NAZARA_BILLBOARD_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>

class NzBillboard;

using NzBillboardConstRef = NzObjectRef<const NzBillboard>;
using NzBillboardLibrary = NzObjectLibrary<NzBillboard>;
using NzBillboardRef = NzObjectRef<NzBillboard>;

class NAZARA_GRAPHICS_API NzBillboard : public NzInstancedRenderable
{
	public:
		inline NzBillboard();
		inline NzBillboard(NzMaterialRef material);
		inline NzBillboard(NzTexture* texture);
		inline NzBillboard(const NzBillboard& billboard);
		~NzBillboard() = default;

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const InstanceData& instanceData) const override;

		inline const NzColor& GetColor() const;
		inline const NzMaterialRef& GetMaterial() const;
		inline float GetRotation() const;
		inline const NzVector2f& GetSize() const;

		inline void SetColor(const NzColor& color);
		inline void SetDefaultMaterial();
		inline void SetMaterial(NzMaterialRef material, bool resizeBillboard = true);
		inline void SetRotation(float rotation);
		inline void SetSize(const NzVector2f& size);
		inline void SetSize(float sizeX, float sizeY);
		inline void SetTexture(NzTextureRef texture, bool resizeBillboard = true);

		inline NzBillboard& operator=(const NzBillboard& billboard);

		template<typename... Args> static NzBillboardRef New(Args&&... args);

	private:
		void MakeBoundingVolume() const override;

		NzColor m_color;
		NzMaterialRef m_material;
		NzVector2f m_sinCos;
		NzVector2f m_size;
		float m_rotation;

		static NzBillboardLibrary::LibraryMap s_library;
};

#include <Nazara/Graphics/Billboard.inl>

#endif // NAZARA_BILLBOARD_HPP
