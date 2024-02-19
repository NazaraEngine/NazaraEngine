// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_PROGRESSBARWIDGET_HPP
#define NAZARA_WIDGETS_PROGRESSBARWIDGET_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>

namespace Nz
{
	class NAZARA_WIDGETS_API ProgressBarWidget : public BaseWidget
	{
		public:
			ProgressBarWidget(BaseWidget* parent);
			ProgressBarWidget(const ProgressBarWidget&) = delete;
			ProgressBarWidget(ProgressBarWidget&&) = delete;
			~ProgressBarWidget() = default;

			inline float GetFraction() const;

			void Layout() override;

			void OnRenderLayerUpdated(int baseRenderLayer) override;

			inline void SetFraction(float fraction);

			ProgressBarWidget& operator=(const ProgressBarWidget&) = delete;
			ProgressBarWidget& operator=(ProgressBarWidget&&) = delete;

		private:
			std::unique_ptr<ProgressBarWidgetStyle> m_style;
			float m_fraction;
	};
}

#include <Nazara/Widgets/ProgressBarWidget.inl>

#endif // NAZARA_WIDGETS_PROGRESSBARWIDGET_HPP
