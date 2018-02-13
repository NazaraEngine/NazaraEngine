// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPRITE_HPP
#define NAZARA_SPRITE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/InstancedRenderable.hpp>
#include <Nazara/Graphics/Material.hpp>
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
			inline const Color& GetCornerColor(RectCorner corner) const;
			inline const Vector3f& GetOrigin() const;
			inline const Vector2f& GetSize() const;
			inline const Rectf& GetTextureCoords() const;

			inline void SetColor(const Color& color);
			inline void SetCornerColor(RectCorner corner, const Color& color);
			inline void SetDefaultMaterial();
			inline void SetMaterial(MaterialRef material, bool resizeSprite = true);
			bool SetMaterial(String materialName, bool resizeSprite = true);
			inline void SetMaterial(std::size_t skinIndex, MaterialRef material, bool resizeSprite = true);
			bool SetMaterial(std::size_t skinIndex, String materialName, bool resizeSprite = true);
			inline void SetOrigin(const Vector3f& origin);
			inline void SetSize(const Vector2f& size);
			inline void SetSize(float sizeX, float sizeY);
			bool SetTexture(String textureName, bool resizeSprite = true);
			inline void SetTexture(TextureRef texture, bool resizeSprite = true);
			bool SetTexture(std::size_t skinIndex, String textureName, bool resizeSprite = true);
			inline void SetTexture(std::size_t skinIndex, TextureRef texture, bool resizeSprite = true);
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

			std::array<Color, 4> m_cornerColor;
			Color m_color;
			Rectf m_textureCoords;
			Vector2f m_size;
			Vector3f m_origin;

			static SpriteLibrary::LibraryMap s_library;
	};
}

#include <Nazara/Graphics/Sprite.inl>

#endif // NAZARA_SPRITE_HPP
