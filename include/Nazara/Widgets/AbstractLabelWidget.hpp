// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_ABSTRACTLABELWIDGET_HPP
#define NAZARA_WIDGETS_ABSTRACTLABELWIDGET_HPP

#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>

namespace Nz
{
	class AbstractTextDrawer;

	class NAZARA_WIDGETS_API AbstractLabelWidget : public BaseWidget
	{
		public:
			AbstractLabelWidget(BaseWidget* parent);
			AbstractLabelWidget(const AbstractLabelWidget&) = delete;
			AbstractLabelWidget(AbstractLabelWidget&&) = delete;
			~AbstractLabelWidget() = default;

			AbstractLabelWidget& operator=(const AbstractLabelWidget&) = delete;
			AbstractLabelWidget& operator=(AbstractLabelWidget&&) = delete;

		protected:
			std::unique_ptr<LabelWidgetStyle> m_style;

		private:
			void OnMouseEnter() override;
			void OnMouseExit() override;
			void OnRenderLayerUpdated(int baseRenderLayer) override;
	};
}

#include <Nazara/Widgets/AbstractLabelWidget.inl>

#endif // NAZARA_WIDGETS_ABSTRACTLABELWIDGET_HPP
