// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_LABELWIDGET_HPP
#define NAZARA_WIDGETS_LABELWIDGET_HPP

#include <Nazara/Widgets/AbstractLabelWidget.hpp>

namespace Nz
{
	class AbstractTextDrawer;

	class NAZARA_WIDGETS_API LabelWidget final : public AbstractLabelWidget
	{
		public:
			using AbstractLabelWidget::AbstractLabelWidget;
			LabelWidget(const LabelWidget&) = delete;
			LabelWidget(LabelWidget&&) = delete;
			~LabelWidget() = default;

			void UpdateText(const AbstractTextDrawer& drawer);

			LabelWidget& operator=(const LabelWidget&) = delete;
			LabelWidget& operator=(LabelWidget&&) = delete;
	};
}

#include <Nazara/Widgets/LabelWidget.inl>

#endif // NAZARA_WIDGETS_LABELWIDGET_HPP
