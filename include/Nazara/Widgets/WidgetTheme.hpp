// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_WIDGETTHEME_HPP
#define NAZARA_WIDGETS_WIDGETTHEME_HPP

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/Enums.hpp>

namespace Nz
{
	class AbstractTextDrawer;
	class ButtonWidget;
	class ButtonWidgetStyle;
	class CheckboxWidget;
	class CheckboxWidgetStyle;
	class LabelWidget;
	class LabelWidgetStyle;

	class NAZARA_WIDGETS_API WidgetTheme
	{
		public:
			WidgetTheme() = default;
			WidgetTheme(const WidgetTheme&) = delete;
			WidgetTheme(WidgetTheme&&) = default;
			virtual ~WidgetTheme();

			virtual std::unique_ptr<ButtonWidgetStyle> CreateStyle(ButtonWidget* buttonWidget) const = 0;
			virtual std::unique_ptr<CheckboxWidgetStyle> CreateStyle(CheckboxWidget* buttonWidget) const = 0;
			virtual std::unique_ptr<LabelWidgetStyle> CreateStyle(LabelWidget* buttonWidget) const = 0;

			WidgetTheme& operator=(const WidgetTheme&) = delete;
			WidgetTheme& operator=(WidgetTheme&&) = default;

		private:
	};

	class NAZARA_WIDGETS_API BaseWidgetStyle
	{
		public:
			inline BaseWidgetStyle(BaseWidget* widget, int renderLayerCount);
			BaseWidgetStyle(const BaseWidgetStyle&) = delete;
			BaseWidgetStyle(BaseWidgetStyle&&) = default;
			virtual ~BaseWidgetStyle();

			inline entt::entity CreateEntity();
			entt::entity CreateGraphicsEntity();
			inline void DestroyEntity(entt::entity entity);

			inline entt::registry& GetRegistry();
			inline const entt::registry& GetRegistry() const;
			UInt32 GetRenderMask() const;
			inline int GetRenderLayerCount() const;

			virtual void UpdateRenderLayer(int baseRenderLayer) = 0;

			BaseWidgetStyle& operator=(const BaseWidgetStyle&) = delete;
			BaseWidgetStyle& operator=(BaseWidgetStyle&&) = default;

		private:
			BaseWidget* m_widgetOwner;
			int m_renderLayerCount;
	};

	class NAZARA_WIDGETS_API ButtonWidgetStyle : public BaseWidgetStyle
	{
		public:
			using BaseWidgetStyle::BaseWidgetStyle;
			ButtonWidgetStyle(const ButtonWidgetStyle&) = delete;
			ButtonWidgetStyle(ButtonWidgetStyle&&) = default;
			~ButtonWidgetStyle() = default;

			virtual void Layout(const Vector2f& size) = 0;

			virtual void OnHoverBegin();
			virtual void OnHoverEnd();
			virtual void OnPress();
			virtual void OnRelease();

			virtual void UpdateText(const AbstractTextDrawer& drawer) = 0;

			ButtonWidgetStyle& operator=(const ButtonWidgetStyle&) = delete;
			ButtonWidgetStyle& operator=(ButtonWidgetStyle&&) = default;
	};

	class NAZARA_WIDGETS_API CheckboxWidgetStyle : public BaseWidgetStyle
	{
		public:
			using BaseWidgetStyle::BaseWidgetStyle;
			CheckboxWidgetStyle(const CheckboxWidgetStyle&) = delete;
			CheckboxWidgetStyle(CheckboxWidgetStyle&&) = default;
			~CheckboxWidgetStyle() = default;

			virtual void Layout(const Vector2f& size) = 0;

			virtual void OnHoverBegin();
			virtual void OnHoverEnd();
			virtual void OnNewState(CheckboxState newState);
			virtual void OnPress();
			virtual void OnRelease();

			CheckboxWidgetStyle& operator=(const CheckboxWidgetStyle&) = delete;
			CheckboxWidgetStyle& operator=(CheckboxWidgetStyle&&) = default;
	};

	class NAZARA_WIDGETS_API LabelWidgetStyle : public BaseWidgetStyle
	{
		public:
			using BaseWidgetStyle::BaseWidgetStyle;
			LabelWidgetStyle(const LabelWidgetStyle&) = delete;
			LabelWidgetStyle(LabelWidgetStyle&&) = default;
			~LabelWidgetStyle() = default;

			virtual void Layout(const Vector2f& size) = 0;

			virtual void OnHoverBegin();
			virtual void OnHoverEnd();

			virtual void UpdateText(const AbstractTextDrawer& drawer) = 0;

			LabelWidgetStyle& operator=(const LabelWidgetStyle&) = delete;
			LabelWidgetStyle& operator=(LabelWidgetStyle&&) = default;
	};
}

#include <Nazara/Widgets/WidgetTheme.inl>

#endif // NAZARA_WIDGETS_WIDGETTHEME_HPP
