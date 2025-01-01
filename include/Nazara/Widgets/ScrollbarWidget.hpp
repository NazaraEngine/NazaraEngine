// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_SCROLLBARWIDGET_HPP
#define NAZARA_WIDGETS_SCROLLBARWIDGET_HPP

#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/WidgetStyleFactory.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>

namespace Nz
{
	class NAZARA_WIDGETS_API ScrollbarWidget : public BaseWidget
	{
		public:
			using StyleFactory = WidgetStyleFactory<ScrollbarWidget, ScrollbarWidgetStyle>;

			ScrollbarWidget(BaseWidget* parent, ScrollbarOrientation orientation, const StyleFactory& styleFactory = nullptr);
			ScrollbarWidget(const ScrollbarWidget&) = delete;
			ScrollbarWidget(ScrollbarWidget&&) = delete;
			~ScrollbarWidget() = default;

			inline ScrollbarOrientation GetOrientation() const;
			inline float GetMaximumValue() const;
			inline float GetMinimumValue() const;
			inline float GetStep() const;
			inline float GetValue() const;

			inline void SetValue(float newValue);

			ScrollbarWidget& operator=(const ScrollbarWidget&) = delete;
			ScrollbarWidget& operator=(ScrollbarWidget&&) = delete;

			NazaraSignal(OnScrollbarValueUpdate, ScrollbarWidget* /*emitter*/, float /*newValue*/);

		private:
			void Layout() override;

			void OnMouseEnter() override;
			void OnMouseExit() override;

			void OnRenderLayerUpdated(int baseRenderLayer) override;

			std::unique_ptr<ScrollbarWidgetStyle> m_style;
			ImageButtonWidget* m_scrollBackButton;
			ImageButtonWidget* m_scrollForwardButton;
			ScrollbarButtonWidget* m_scrollCenterButton;
			ScrollbarOrientation m_orientation;
			float m_maximumValue;
			float m_minimumValue;
			float m_step;
			float m_value;
			float m_grabbedValue;
			int m_grabbedPosition;
	};
}

#include <Nazara/Widgets/ScrollbarWidget.inl>

#endif // NAZARA_WIDGETS_SCROLLBARWIDGET_HPP
