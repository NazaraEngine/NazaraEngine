// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_BUTTONWIDGET_HPP
#define NAZARA_WIDGETS_BUTTONWIDGET_HPP

#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>

namespace Nz
{
	class AbstractTextDrawer;
	class MaterialPass;

	class NAZARA_WIDGETS_API ButtonWidget : public BaseWidget
	{
		public:
			ButtonWidget(BaseWidget* parent);
			ButtonWidget(const ButtonWidget&) = delete;
			ButtonWidget(ButtonWidget&&) = default;
			~ButtonWidget() = default;

			void UpdateText(const AbstractTextDrawer& drawer);

			ButtonWidget& operator=(const ButtonWidget&) = delete;
			ButtonWidget& operator=(ButtonWidget&&) = default;

			NazaraSignal(OnButtonTrigger, const ButtonWidget* /*button*/);

		private:
			void Layout() override;

			void OnMouseEnter() override;
			void OnMouseButtonPress(int x, int y, Mouse::Button button) override;
			void OnMouseButtonRelease(int x, int y, Mouse::Button button) override;
			void OnMouseExit() override;

			std::unique_ptr<ButtonWidgetStyle> m_style;
	};
}

#include <Nazara/Widgets/ButtonWidget.inl>

#endif // NAZARA_WIDGETS_BUTTONWIDGET_HPP
