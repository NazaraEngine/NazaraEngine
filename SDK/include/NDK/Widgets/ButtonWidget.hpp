// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WIDGETS_BUTTONWIDGET_HPP
#define NDK_WIDGETS_BUTTONWIDGET_HPP

#include <NDK/Prerequisites.hpp>
#include <NDK/BaseWidget.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>

namespace Nz
{
	class AbstractTextDrawer;
}

namespace Ndk
{
	class NDK_API ButtonWidget : public BaseWidget
	{
		public:
			ButtonWidget(BaseWidget* parent);
			ButtonWidget(const ButtonWidget&) = delete;
			ButtonWidget(ButtonWidget&&) = default;
			~ButtonWidget() = default;

			inline const Nz::Color& GetColor() const;
			inline const Nz::Color& GetCornerColor() const;
			inline const Nz::Color& GetHoverColor() const;
			inline const Nz::Color& GetHoverCornerColor() const;
			inline const Nz::Color& GetPressColor() const;
			inline const Nz::Color& GetPressCornerColor() const;

			inline const Nz::TextureRef& GetTexture() const;
			inline const Nz::TextureRef& GetHoverTexture() const;
			inline const Nz::TextureRef& GetPressTexture() const;

			inline void SetColor(const Nz::Color& color, const Nz::Color& cornerColor);
			inline void SetHoverColor(const Nz::Color& color, const Nz::Color& cornerColor);
			inline void SetPressColor(const Nz::Color& color, const Nz::Color& cornerColor);

			inline void SetTexture(const Nz::TextureRef& texture);
			inline void SetHoverTexture(const Nz::TextureRef& texture);
			inline void SetPressTexture(const Nz::TextureRef& texture);

			inline void UpdateText(const Nz::AbstractTextDrawer& drawer);

			ButtonWidget& operator=(const ButtonWidget&) = delete;
			ButtonWidget& operator=(ButtonWidget&&) = default;

			static const Nz::Color& GetDefaultColor();
			static const Nz::Color& GetDefaultCornerColor();
			static const Nz::Color& GetDefaultHoverColor();
			static const Nz::Color& GetDefaultHoverCornerColor();
			static const Nz::Color& GetDefaultPressColor();
			static const Nz::Color& GetDefaultPressCornerColor();

			NazaraSignal(OnButtonTrigger, const ButtonWidget* /*button*/);

		private:
			void Layout() override;

			void OnMouseEnter() override;
			void OnMouseButtonPress(int x, int y, Nz::Mouse::Button button) override;
			void OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button) override;
			void OnMouseExit() override;

			EntityHandle m_textEntity;
			EntityHandle m_gradientEntity;
			Nz::SpriteRef m_gradientSprite;
			Nz::TextSpriteRef m_textSprite;

			Nz::Color m_color;
			Nz::Color m_cornerColor;
			Nz::Color m_hoverColor;
			Nz::Color m_hoverCornerColor;
			Nz::Color m_pressColor;
			Nz::Color m_pressCornerColor;

			Nz::TextureRef m_texture;
			Nz::TextureRef m_hoverTexture;
			Nz::TextureRef m_pressTexture;

			static Nz::Color s_color;
			static Nz::Color s_cornerColor;
			static Nz::Color s_hoverColor;
			static Nz::Color s_hoverCornerColor;
			static Nz::Color s_pressColor;
			static Nz::Color s_pressCornerColor;
	};
}

#include <NDK/Widgets/ButtonWidget.inl>

#endif // NDK_WIDGETS_BUTTONWIDGET_HPP
