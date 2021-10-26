// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_BUTTONWIDGET_HPP
#define NAZARA_WIDGETS_BUTTONWIDGET_HPP

#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>

namespace Nz
{
	class AbstractTextDrawer;

	class NAZARA_WIDGETS_API ButtonWidget : public BaseWidget
	{
		public:
			ButtonWidget(BaseWidget* parent);
			ButtonWidget(const ButtonWidget&) = delete;
			ButtonWidget(ButtonWidget&&) = default;
			~ButtonWidget() = default;

			inline const Color& GetColor() const;
			inline const Color& GetCornerColor() const;
			inline const Color& GetHoverColor() const;
			inline const Color& GetHoverCornerColor() const;
			inline const Color& GetPressColor() const;
			inline const Color& GetPressCornerColor() const;

			inline const std::shared_ptr<Texture>& GetTexture() const;
			inline const std::shared_ptr<Texture>& GetHoverTexture() const;
			inline const std::shared_ptr<Texture>& GetPressTexture() const;

			inline void SetColor(const Color& color, const Color& cornerColor);
			inline void SetHoverColor(const Color& color, const Color& cornerColor);
			inline void SetPressColor(const Color& color, const Color& cornerColor);

			inline void UpdateText(const AbstractTextDrawer& drawer);

			ButtonWidget& operator=(const ButtonWidget&) = delete;
			ButtonWidget& operator=(ButtonWidget&&) = default;

			NazaraSignal(OnButtonTrigger, const ButtonWidget* /*button*/);

		private:
			void Layout() override;

			void OnMouseEnter() override;
			void OnMouseButtonPress(int x, int y, Mouse::Button button) override;
			void OnMouseButtonRelease(int x, int y, Mouse::Button button) override;
			void OnMouseExit() override;

			std::shared_ptr<Sprite> m_gradientSprite;
			std::shared_ptr<TextSprite> m_textSprite;
			entt::entity m_textEntity;
			entt::entity m_gradientEntity;
			Color m_color;
			Color m_cornerColor;
			Color m_hoverColor;
			Color m_hoverCornerColor;
			Color m_pressColor;
			Color m_pressCornerColor;
	};
}

#include <Nazara/Widgets/ButtonWidget.inl>

#endif // NAZARA_WIDGETS_BUTTONWIDGET_HPP
