// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BILLBOARD_HPP
#define NAZARA_BILLBOARD_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>

namespace Nz
{
	class Billboard;

	using BillboardConstRef = ObjectRef<const Billboard>;
	using BillboardLibrary = ObjectLibrary<Billboard>;
	using BillboardRef = ObjectRef<Billboard>;

	class NAZARA_GRAPHICS_API Billboard : public InstancedRenderable
	{
		public:
			inline Billboard();
			inline Billboard(MaterialRef material);
			inline Billboard(Texture* texture);
			inline Billboard(const Billboard& billboard);
			Billboard(Billboard&&) = delete;
			~Billboard() = default;

			void AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const override;

			inline const Color& GetColor() const;
			inline const MaterialRef& GetMaterial() const;
			inline float GetRotation() const;
			inline const Vector2f& GetSize() const;

			inline void SetColor(const Color& color);
			inline void SetDefaultMaterial();
			inline void SetMaterial(MaterialRef material, bool resizeBillboard = true);
			inline void SetRotation(float rotation);
			inline void SetSize(const Vector2f& size);
			inline void SetSize(float sizeX, float sizeY);
			inline void SetTexture(TextureRef texture, bool resizeBillboard = true);

			inline Billboard& operator=(const Billboard& billboard);
			Billboard& operator=(Billboard&&) = delete;

			template<typename... Args> static BillboardRef New(Args&&... args);

		private:
			void MakeBoundingVolume() const override;

			Color m_color;
			MaterialRef m_material;
			Vector2f m_sinCos;
			Vector2f m_size;
			float m_rotation;

			static BillboardLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Graphics/Billboard.inl>

#endif // NAZARA_BILLBOARD_HPP
