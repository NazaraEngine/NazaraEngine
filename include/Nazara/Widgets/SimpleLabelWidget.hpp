// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_SIMPLELABELWIDGET_HPP
#define NAZARA_WIDGETS_SIMPLELABELWIDGET_HPP

#include <Nazara/TextRenderer/SimpleTextDrawer.hpp>
#include <Nazara/Widgets/AbstractLabelWidget.hpp>
#include <NazaraUtils/FunctionRef.hpp>

namespace Nz
{
	class NAZARA_WIDGETS_API SimpleLabelWidget final : public AbstractLabelWidget
	{
		public:
			using AbstractLabelWidget::AbstractLabelWidget;
			SimpleLabelWidget(const SimpleLabelWidget&) = delete;
			SimpleLabelWidget(SimpleLabelWidget&&) = delete;
			~SimpleLabelWidget() = default;

			inline void AppendText(std::string_view str);

			inline void Clear();

			inline unsigned int GetCharacterSize() const;
			inline const SimpleTextDrawer& GetDrawer() const;
			inline const std::string& GetText() const;
			inline const Color& GetTextColor() const;

			inline void SetCharacterSize(unsigned int characterSize);
			inline void SetText(std::string text);
			inline void SetTextColor(const Color& color);

			template<typename F> void UpdateDrawer(F&& callback);

			SimpleLabelWidget& operator=(const SimpleLabelWidget&) = delete;
			SimpleLabelWidget& operator=(SimpleLabelWidget&&) = delete;

		private:
			void UpdateText();
			void UpdateTextAndSize();

			SimpleTextDrawer m_drawer;
	};
}

#include <Nazara/Widgets/SimpleLabelWidget.inl>

#endif // NAZARA_WIDGETS_SIMPLELABELWIDGET_HPP
