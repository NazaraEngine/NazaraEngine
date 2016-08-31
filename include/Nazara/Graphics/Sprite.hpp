// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPRITE_HPP
#define NAZARA_SPRITE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <array>

namespace Nz
{
	class Sprite;

	using SpriteConstRef = ObjectRef<const Sprite>;
	using SpriteLibrary = ObjectLibrary<Sprite>;
	using SpriteRef = ObjectRef<Sprite>;

	class NAZARA_GRAPHICS_API Sprite : public InstancedRenderable
	{
		friend SpriteLibrary;
		friend class Graphics;

		public:
			inline Sprite();
			inline Sprite(MaterialRef material);
			inline Sprite(Texture* texture);
			inline Sprite(const Sprite& sprite);
			Sprite(Sprite&&) = delete;
			~Sprite() = default;

			void AddToRenderQueue(AbstractRenderQueue* renderQueue, const InstanceData& instanceData) const override;

			inline const Color& GetColor() const;
			inline const MaterialRef& GetMaterial() const;
			inline const Vector3f& GetOrigin() const;
			inline const Vector2f& GetSize() const;
			inline const Rectf& GetTextureCoords() const;

			inline void SetColor(const Color& color);
			inline void SetDefaultMaterial();
			inline void SetMaterial(MaterialRef material, bool resizeSprite = true);
			inline void SetOrigin(const Vector3f& origin);
			inline void SetSize(const Vector2f& size);
			inline void SetSize(float sizeX, float sizeY);
			inline void SetTexture(TextureRef texture, bool resizeSprite = true);
			inline void SetTextureCoords(const Rectf& coords);
			inline void SetTextureRect(const Rectui& rect);

			inline Sprite& operator=(const Sprite& sprite);
			Sprite& operator=(Sprite&& sprite) = delete;

			template<typename... Args> static SpriteRef New(Args&&... args);

		private:
			inline void InvalidateVertices();
			void MakeBoundingVolume() const override;
			void UpdateData(InstanceData* instanceData) const override;

			static bool Initialize();
			static void Uninitialize();

			Color m_color;
			MaterialRef m_material;
			Rectf m_textureCoords;
			Vector2f m_size;
			Vector3f m_origin;

			static SpriteLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Graphics/Sprite.inl>

#endif // NAZARA_SPRITE_HPP
