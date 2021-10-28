// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_LABELWIDGET_HPP
#define NAZARA_WIDGETS_LABELWIDGET_HPP

#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>

namespace Nz
{
	class AbstractTextDrawer;
	class TextSprite;

	class NAZARA_WIDGETS_API LabelWidget : public BaseWidget
	{
		public:
			LabelWidget(BaseWidget* parent);
			LabelWidget(const LabelWidget&) = delete;
			LabelWidget(LabelWidget&&) = default;
			~LabelWidget() = default;

			inline void UpdateText(const AbstractTextDrawer& drawer, float scale = 1.f);

			LabelWidget& operator=(const LabelWidget&) = delete;
			LabelWidget& operator=(LabelWidget&&) = default;

		private:
			entt::entity m_textEntity;
			std::shared_ptr<TextSprite> m_textSprite;
	};
}

#include <Nazara/Widgets/LabelWidget.inl>

#endif // NAZARA_WIDGETS_LABELWIDGET_HPP
