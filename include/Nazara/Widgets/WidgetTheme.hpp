// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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
	class ImageButtonWidget;
	class ImageButtonWidgetStyle;
	class LabelWidget;
	class LabelWidgetStyle;
	class ProgressBarWidget;
	class ProgressBarWidgetStyle;
	class ScrollAreaWidget;
	class ScrollAreaWidgetStyle;
	class ScrollbarWidget;
	class ScrollbarWidgetStyle;
	class ScrollbarButtonWidget;
	class ScrollbarButtonWidgetStyle;

	class NAZARA_WIDGETS_API WidgetTheme
	{
		public:
			struct Config;

			WidgetTheme() = default;
			WidgetTheme(const WidgetTheme&) = delete;
			WidgetTheme(WidgetTheme&&) = default;
			virtual ~WidgetTheme();

			virtual std::unique_ptr<ButtonWidgetStyle> CreateStyle(ButtonWidget* buttonWidget) const = 0;
			virtual std::unique_ptr<CheckboxWidgetStyle> CreateStyle(CheckboxWidget* checkboxWidget) const = 0;
			virtual std::unique_ptr<ImageButtonWidgetStyle> CreateStyle(ImageButtonWidget* imageButtonWidget) const = 0;
			virtual std::unique_ptr<LabelWidgetStyle> CreateStyle(LabelWidget* labelWidget) const = 0;
			virtual std::unique_ptr<ProgressBarWidgetStyle> CreateStyle(ProgressBarWidget* labelWidget) const = 0;
			virtual std::unique_ptr<ScrollAreaWidgetStyle> CreateStyle(ScrollAreaWidget* scrollareaWidget) const = 0;
			virtual std::unique_ptr<ScrollbarWidgetStyle> CreateStyle(ScrollbarWidget* scrollbarWidget) const = 0;
			virtual std::unique_ptr<ScrollbarButtonWidgetStyle> CreateStyle(ScrollbarButtonWidget* scrollbarButtonWidget) const = 0;

			inline const Config& GetConfig() const;

			WidgetTheme& operator=(const WidgetTheme&) = delete;
			WidgetTheme& operator=(WidgetTheme&&) = default;

			struct Config
			{
				std::shared_ptr<MaterialInstance> scrollbarButtonDownMaterial;
				std::shared_ptr<MaterialInstance> scrollbarButtonDownHoveredMaterial;
				std::shared_ptr<MaterialInstance> scrollbarButtonDownPressedMaterial;
				std::shared_ptr<MaterialInstance> scrollbarButtonLeftMaterial;
				std::shared_ptr<MaterialInstance> scrollbarButtonLeftHoveredMaterial;
				std::shared_ptr<MaterialInstance> scrollbarButtonLeftPressedMaterial;
				std::shared_ptr<MaterialInstance> scrollbarButtonRightMaterial;
				std::shared_ptr<MaterialInstance> scrollbarButtonRightHoveredMaterial;
				std::shared_ptr<MaterialInstance> scrollbarButtonRightPressedMaterial;
				std::shared_ptr<MaterialInstance> scrollbarButtonUpMaterial;
				std::shared_ptr<MaterialInstance> scrollbarButtonUpHoveredMaterial;
				std::shared_ptr<MaterialInstance> scrollbarButtonUpPressedMaterial;
				float scrollbarButtonCornerSize;
				float scrollbarButtonCornerTexcoords;
			};

		protected:
			Config m_config;
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
}

#include <Nazara/Widgets/WidgetTheme.inl>

#endif // NAZARA_WIDGETS_WIDGETTHEME_HPP
