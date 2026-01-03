// Copyright (C) 2026 Samy Bensaid
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_IMAGEWIDGET_HPP
#define NAZARA_WIDGETS_IMAGEWIDGET_HPP

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>

namespace Nz
{
	class NAZARA_WIDGETS_API ImageWidget : public BaseWidget
	{
		public:
			ImageWidget(BaseWidget* parent, std::shared_ptr<MaterialInstance> material);
			ImageWidget(const ImageWidget&) = delete;
			ImageWidget(ImageWidget&&) = delete;
			~ImageWidget() = default;

			inline const Color& GetColor() const;
			inline const std::shared_ptr<MaterialInstance>& GetMaterial() const;
			inline const Rectf& GetTextureCoords() const;

			inline void SetColor(const Color& color);
			inline void SetMaterial(const std::shared_ptr<MaterialInstance>& texture);
			inline void SetTextureCoords(const Rectf& coords);
			inline void SetTextureRect(const Rectf& rect);

			ImageWidget& operator=(const ImageWidget&) = delete;
			ImageWidget& operator=(ImageWidget&&) = delete;

		private:
			void Layout() override;
			inline void UpdatePreferredSize();

			entt::entity m_entity;
			std::shared_ptr<Sprite> m_sprite;
	};
}

#include <Nazara/Widgets/ImageWidget.inl>

#endif // NAZARA_WIDGETS_IMAGEWIDGET_HPP
