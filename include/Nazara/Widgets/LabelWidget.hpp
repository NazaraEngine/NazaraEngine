// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_LABELWIDGET_HPP
#define NAZARA_WIDGETS_LABELWIDGET_HPP

#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>

namespace Nz
{
	class AbstractTextDrawer;
	class TextSprite;

	class NAZARA_WIDGETS_API LabelWidget : public BaseWidget
	{
		public:
			LabelWidget(BaseWidget* parent);
			LabelWidget(const LabelWidget&) = delete;
			LabelWidget(LabelWidget&&) = delete;
			~LabelWidget() = default;

			void UpdateText(const AbstractTextDrawer& drawer);

			LabelWidget& operator=(const LabelWidget&) = delete;
			LabelWidget& operator=(LabelWidget&&) = delete;

		private:
			void OnMouseEnter() override;
			void OnMouseExit() override;

			void OnRenderLayerUpdated(int baseRenderLayer) override;

			std::unique_ptr<LabelWidgetStyle> m_style;
	};
}

#include <Nazara/Widgets/LabelWidget.inl>

#endif // NAZARA_WIDGETS_LABELWIDGET_HPP
