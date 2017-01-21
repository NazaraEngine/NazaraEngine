// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_WIDGETS_BUTTONWIDGET_HPP
#define NDK_WIDGETS_BUTTONWIDGET_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/BaseWidget.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>

namespace Ndk
{
	class World;

	class NDK_API ButtonWidget : public BaseWidget
	{
		public:
			ButtonWidget(BaseWidget* parent = nullptr);
			ButtonWidget(const ButtonWidget&) = delete;
			ButtonWidget(ButtonWidget&&) = default;
			~ButtonWidget() = default;

			//virtual ButtonWidget* Clone() const = 0;

			void ResizeToContent();

			inline void UpdateText(const Nz::AbstractTextDrawer& drawer);

			ButtonWidget& operator=(const ButtonWidget&) = delete;
			ButtonWidget& operator=(ButtonWidget&&) = default;

			NazaraSignal(OnButtonTrigger, const ButtonWidget* /*button*/);

		private:
			void Layout() override;

			void OnMouseEnter() override;
			void OnMouseMoved(int x, int y, int deltaX, int deltaY) override;
			void OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button) override;
			void OnMouseExit() override;

			EntityHandle m_textEntity;
			EntityHandle m_gradientEntity;
			Nz::SpriteRef m_gradientSprite;
			Nz::TextSpriteRef m_textSprite;
	};
}

#include <NDK/Widgets/ButtonWidget.inl>

#endif // NDK_WIDGETS_BUTTONWIDGET_HPP
