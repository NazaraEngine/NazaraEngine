// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NAZARA_LABELWIDGET_HPP
#define NAZARA_LABELWIDGET_HPP

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

#endif
