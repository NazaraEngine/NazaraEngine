// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_SCROLLBARBUTTONWIDGET_HPP
#define NAZARA_WIDGETS_SCROLLBARBUTTONWIDGET_HPP

#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>

namespace Nz
{
	class AbstractTextDrawer;
	class MaterialPass;

	class NAZARA_WIDGETS_API ScrollbarButtonWidget : public BaseWidget
	{
		public:
			inline ScrollbarButtonWidget(BaseWidget* parent);
			ScrollbarButtonWidget(const ScrollbarButtonWidget&) = delete;
			ScrollbarButtonWidget(ScrollbarButtonWidget&&) = default;
			~ScrollbarButtonWidget() = default;

			ScrollbarButtonWidget& operator=(const ScrollbarButtonWidget&) = delete;
			ScrollbarButtonWidget& operator=(ScrollbarButtonWidget&&) = default;

			NazaraSignal(OnButtonGrabbed, ScrollbarButtonWidget* /*emitter*/, int /*x*/, int /*y*/);
			NazaraSignal(OnButtonMoved, ScrollbarButtonWidget* /*emitter*/, int /*x*/, int /*y*/);
			NazaraSignal(OnButtonReleased, ScrollbarButtonWidget* /*emitter*/);

		private:
			void Layout() override;

			void OnMouseEnter() override;
			bool OnMouseButtonPress(int x, int y, Mouse::Button button) override;
			bool OnMouseButtonRelease(int x, int y, Mouse::Button button) override;
			void OnMouseExit() override;
			bool OnMouseMoved(int x, int y, int deltaX, int deltaY) override;

			void OnRenderLayerUpdated(int baseRenderLayer) override;

			std::unique_ptr<ScrollbarButtonWidgetStyle> m_style;
			bool m_isGrabbed;
	};
}

#include <Nazara/Widgets/ScrollbarButtonWidget.inl>

#endif // NAZARA_WIDGETS_SCROLLBARBUTTONWIDGET_HPP
