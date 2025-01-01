// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_WIDGETTHEME_HPP
#define NAZARA_WIDGETS_WIDGETTHEME_HPP

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/Enums.hpp>
#include <span>

namespace Nz
{
	class AbstractLabelWidget;
	class AbstractTextAreaWidget;
	class AbstractTextDrawer;
	class ButtonWidget;
	class ButtonWidgetStyle;
	class CheckboxWidget;
	class CheckboxWidgetStyle;
	class ImageButtonWidget;
	class ImageButtonWidgetStyle;
	class LabelWidgetStyle;
	class ProgressBarWidget;
	class ProgressBarWidgetStyle;
	class ScrollAreaWidget;
	class ScrollAreaWidgetStyle;
	class ScrollbarWidget;
	class ScrollbarWidgetStyle;
	class ScrollbarButtonWidget;
	class ScrollbarButtonWidgetStyle;
	class TextAreaWidgetStyle;

	class NAZARA_WIDGETS_API WidgetTheme
	{
		public:
			WidgetTheme() = default;
			WidgetTheme(const WidgetTheme&) = delete;
			WidgetTheme(WidgetTheme&&) = default;
			virtual ~WidgetTheme();

			virtual std::unique_ptr<ButtonWidgetStyle> CreateStyle(ButtonWidget* buttonWidget) const = 0;
			virtual std::unique_ptr<CheckboxWidgetStyle> CreateStyle(CheckboxWidget* checkboxWidget) const = 0;
			virtual std::unique_ptr<ImageButtonWidgetStyle> CreateStyle(ImageButtonWidget* imageButtonWidget) const = 0;
			virtual std::unique_ptr<LabelWidgetStyle> CreateStyle(AbstractLabelWidget* labelWidget) const = 0;
			virtual std::unique_ptr<ProgressBarWidgetStyle> CreateStyle(ProgressBarWidget* labelWidget) const = 0;
			virtual std::unique_ptr<ScrollAreaWidgetStyle> CreateStyle(ScrollAreaWidget* scrollareaWidget) const = 0;
			virtual std::unique_ptr<ScrollbarWidgetStyle> CreateStyle(ScrollbarWidget* scrollbarWidget) const = 0;
			virtual std::unique_ptr<ScrollbarButtonWidgetStyle> CreateStyle(ScrollbarButtonWidget* scrollbarButtonWidget) const = 0;
			virtual std::unique_ptr<TextAreaWidgetStyle> CreateStyle(AbstractTextAreaWidget* textAreaWidget) const = 0;

			WidgetTheme& operator=(const WidgetTheme&) = delete;
			WidgetTheme& operator=(WidgetTheme&&) = default;
	};

	class NAZARA_WIDGETS_API BaseWidgetStyle
	{
		public:
			inline BaseWidgetStyle(BaseWidget* widget, int renderLayerCount);
			BaseWidgetStyle(const BaseWidgetStyle&) = delete;
			BaseWidgetStyle(BaseWidgetStyle&&) = default;
			virtual ~BaseWidgetStyle();

			inline entt::entity CreateEntity();
			inline entt::entity CreateGraphicsEntity(Node* parent = nullptr);
			inline void DestroyEntity(entt::entity entity);

			template<typename T> T* GetOwnerWidget() const;
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

			virtual void OnDisabled();
			virtual void OnEnabled();
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

	class NAZARA_WIDGETS_API ImageButtonWidgetStyle : public BaseWidgetStyle
	{
		public:
			using BaseWidgetStyle::BaseWidgetStyle;
			ImageButtonWidgetStyle(const ImageButtonWidgetStyle&) = delete;
			ImageButtonWidgetStyle(ImageButtonWidgetStyle&&) = default;
			~ImageButtonWidgetStyle() = default;

			virtual void Layout(const Vector2f& size) = 0;

			virtual void OnHoverBegin();
			virtual void OnHoverEnd();
			virtual void OnPress();
			virtual void OnRelease();
			virtual void OnUpdate() = 0;

			ImageButtonWidgetStyle& operator=(const ImageButtonWidgetStyle&) = delete;
			ImageButtonWidgetStyle& operator=(ImageButtonWidgetStyle&&) = default;
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

	class NAZARA_WIDGETS_API ProgressBarWidgetStyle : public BaseWidgetStyle
	{
		public:
			using BaseWidgetStyle::BaseWidgetStyle;
			ProgressBarWidgetStyle(const ProgressBarWidgetStyle&) = delete;
			ProgressBarWidgetStyle(ProgressBarWidgetStyle&&) = default;
			~ProgressBarWidgetStyle() = default;

			virtual void Layout(const Vector2f& size) = 0;

			ProgressBarWidgetStyle& operator=(const ProgressBarWidgetStyle&) = delete;
			ProgressBarWidgetStyle& operator=(ProgressBarWidgetStyle&&) = default;
	};

	class NAZARA_WIDGETS_API ScrollAreaWidgetStyle : public BaseWidgetStyle
	{
		public:
			using BaseWidgetStyle::BaseWidgetStyle;
			ScrollAreaWidgetStyle(const ScrollAreaWidgetStyle&) = delete;
			ScrollAreaWidgetStyle(ScrollAreaWidgetStyle&&) = default;
			~ScrollAreaWidgetStyle() = default;

			virtual void Layout(const Vector2f& size) = 0;

			ScrollAreaWidgetStyle& operator=(const ScrollAreaWidgetStyle&) = delete;
			ScrollAreaWidgetStyle& operator=(ScrollAreaWidgetStyle&&) = default;
	};

	class NAZARA_WIDGETS_API ScrollbarWidgetStyle : public BaseWidgetStyle
	{
		public:
			using BaseWidgetStyle::BaseWidgetStyle;
			ScrollbarWidgetStyle(const ScrollbarWidgetStyle&) = delete;
			ScrollbarWidgetStyle(ScrollbarWidgetStyle&&) = default;
			~ScrollbarWidgetStyle() = default;

			virtual std::unique_ptr<ImageButtonWidget> CreateBackButton(ScrollbarWidget* widget, ScrollbarOrientation orientation) = 0;
			virtual std::unique_ptr<ImageButtonWidget> CreateForwardButton(ScrollbarWidget* widget, ScrollbarOrientation orientation) = 0;

			virtual void Layout(const Vector2f& size) = 0;

			virtual void OnButtonGrab();
			virtual void OnButtonRelease();
			virtual void OnHoverBegin();
			virtual void OnHoverEnd();

			ScrollbarWidgetStyle& operator=(const ScrollbarWidgetStyle&) = delete;
			ScrollbarWidgetStyle& operator=(ScrollbarWidgetStyle&&) = default;
	};

	class NAZARA_WIDGETS_API ScrollbarButtonWidgetStyle : public BaseWidgetStyle
	{
		public:
			using BaseWidgetStyle::BaseWidgetStyle;
			ScrollbarButtonWidgetStyle(const ScrollbarButtonWidgetStyle&) = delete;
			ScrollbarButtonWidgetStyle(ScrollbarButtonWidgetStyle&&) = default;
			~ScrollbarButtonWidgetStyle() = default;

			virtual void Layout(const Vector2f& size) = 0;

			virtual void OnHoverBegin();
			virtual void OnHoverEnd();
			virtual void OnGrab();
			virtual void OnRelease();

			ScrollbarButtonWidgetStyle& operator=(const ScrollbarButtonWidgetStyle&) = delete;
			ScrollbarButtonWidgetStyle& operator=(ScrollbarButtonWidgetStyle&&) = default;
	};

	class NAZARA_WIDGETS_API TextAreaWidgetStyle : public BaseWidgetStyle
	{
		public:
			using BaseWidgetStyle::BaseWidgetStyle;
			TextAreaWidgetStyle(const TextAreaWidgetStyle&) = delete;
			TextAreaWidgetStyle(TextAreaWidgetStyle&&) = default;
			~TextAreaWidgetStyle() = default;

			virtual void EnableBackground(bool enable) = 0;

			virtual void Layout(const Vector2f& size) = 0;

			virtual void OnDisabled();
			virtual void OnEnabled();
			virtual void OnFocusLost();
			virtual void OnFocusReceived();
			virtual void OnHoverBegin();
			virtual void OnHoverEnd();

			virtual void UpdateBackgroundColor(const Color& color) = 0;
			virtual void UpdateCursors(std::span<const Rectf> cursorRects) = 0;
			virtual void UpdateText(const AbstractTextDrawer& drawer) = 0;
			virtual void UpdateTextOffset(float offset) = 0;

			TextAreaWidgetStyle& operator=(const TextAreaWidgetStyle&) = delete;
			TextAreaWidgetStyle& operator=(TextAreaWidgetStyle&&) = default;
	};

}

#include <Nazara/Widgets/WidgetTheme.inl>

#endif // NAZARA_WIDGETS_WIDGETTHEME_HPP
