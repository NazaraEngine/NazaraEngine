// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_WIDGETS_IMAGEWIDGET_HPP
#define NDK_WIDGETS_IMAGEWIDGET_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/BaseWidget.hpp>
#include <NDK/Entity.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Math/Vector2.hpp>

namespace Ndk
{
	class NDK_API ImageWidget : public BaseWidget
	{
		public:
			ImageWidget(BaseWidget* parent = nullptr);
			ImageWidget(const ImageWidget&) = delete;
			ImageWidget(ImageWidget&&) = default;
			~ImageWidget() = default;

			//virtual ImageWidget* Clone() const = 0;

			void ResizeToContent() override;

			inline const Nz::TextureRef& GetTexture() const;
			inline void SetTexture(const Nz::TextureRef& texture, bool resizeToContent = true);

			ImageWidget& operator=(const ImageWidget&) = delete;
			ImageWidget& operator=(ImageWidget&&) = default;

		private:
			void Layout() override;

			Ndk::EntityHandle m_entity;
			Nz::SpriteRef m_sprite;
	};
}

#include <NDK/Widgets/ImageWidget.inl>

#endif // NDK_WIDGETS_IMAGEWIDGET_HPP
