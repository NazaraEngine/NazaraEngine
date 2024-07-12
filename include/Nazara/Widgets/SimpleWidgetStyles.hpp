// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_SIMPLEWIDGETSTYLES_HPP
#define NAZARA_WIDGETS_SIMPLEWIDGETSTYLES_HPP

#include <Nazara/Graphics/LinearSlicedSprite.hpp>
#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>
#include <entt/entt.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_WIDGETS_API SimpleButtonWidgetStyle : public ButtonWidgetStyle
	{
		public:
			struct StyleConfig;

			SimpleButtonWidgetStyle(ButtonWidget* buttonWidget, StyleConfig config);
			SimpleButtonWidgetStyle(const SimpleButtonWidgetStyle&) = delete;
			SimpleButtonWidgetStyle(SimpleButtonWidgetStyle&&) = default;
			~SimpleButtonWidgetStyle() = default;

			void Layout(const Vector2f& size) override;

			void OnDisabled() override;
			void OnEnabled() override;
			void OnHoverBegin() override;
			void OnHoverEnd() override;
			void OnPress() override;
			void OnRelease() override;

			void UpdateRenderLayer(int baseRenderLayer) override;
			void UpdateText(const AbstractTextDrawer& drawer) override;

			SimpleButtonWidgetStyle& operator=(const SimpleButtonWidgetStyle&) = delete;
			SimpleButtonWidgetStyle& operator=(SimpleButtonWidgetStyle&&) = default;

			struct StyleConfig
			{
				std::shared_ptr<MaterialInstance> disabledMaterial;
				std::shared_ptr<MaterialInstance> hoveredMaterial;
				std::shared_ptr<MaterialInstance> material;
				std::shared_ptr<MaterialInstance> pressedMaterial;
				std::shared_ptr<MaterialInstance> pressedHoveredMaterial;
				float cornerSize;
				float cornerTexCoords;
			};

		protected:
			virtual void UpdateMaterial(bool hovered, bool pressed, bool disabled);

		private:
			std::shared_ptr<MaterialInstance> m_disabledMaterial;
			std::shared_ptr<MaterialInstance> m_hoveredMaterial;
			std::shared_ptr<MaterialInstance> m_material;
			std::shared_ptr<MaterialInstance> m_pressedMaterial;
			std::shared_ptr<MaterialInstance> m_pressedHoveredMaterial;
			std::shared_ptr<SlicedSprite> m_sprite;
			std::shared_ptr<TextSprite> m_textSprite;
			entt::entity m_spriteEntity;
			entt::entity m_textEntity;
			bool m_isDisabled;
			bool m_isHovered;
			bool m_isPressed;
	};

	class NAZARA_WIDGETS_API SimpleCheckboxWidgetStyle : public CheckboxWidgetStyle
	{
		public:
			struct StyleConfig;

			SimpleCheckboxWidgetStyle(CheckboxWidget* checkboxWidget, StyleConfig config);
			SimpleCheckboxWidgetStyle(const SimpleCheckboxWidgetStyle&) = delete;
			SimpleCheckboxWidgetStyle(SimpleCheckboxWidgetStyle&&) = default;
			~SimpleCheckboxWidgetStyle() = default;

			void Layout(const Vector2f& size) override;

			void OnHoverBegin() override;
			void OnHoverEnd() override;
			void OnNewState(CheckboxState newState) override;

			void UpdateRenderLayer(int baseRenderLayer) override;

			SimpleCheckboxWidgetStyle& operator=(const SimpleCheckboxWidgetStyle&) = delete;
			SimpleCheckboxWidgetStyle& operator=(SimpleCheckboxWidgetStyle&&) = default;

			struct StyleConfig
			{
				std::shared_ptr<MaterialInstance> backgroundMaterial;
				std::shared_ptr<MaterialInstance> backgroundHoveredMaterial;
				std::shared_ptr<MaterialInstance> checkMaterial;
				std::shared_ptr<MaterialInstance> tristateMaterial;
				float backgroundCornerSize;
				float backgroundCornerTexCoords;
			};

		protected:
			virtual void UpdateMaterial(bool hovered);

		private:
			std::shared_ptr<MaterialInstance> m_checkMaterial;
			std::shared_ptr<MaterialInstance> m_hoveredMaterial;
			std::shared_ptr<MaterialInstance> m_material;
			std::shared_ptr<MaterialInstance> m_tristateMaterial;
			std::shared_ptr<Sprite> m_checkSprite;
			std::shared_ptr<SlicedSprite> m_backgroundSprite;
			entt::entity m_backgroundEntity;
			entt::entity m_checkEntity;
			bool m_isHovered;
	};

	class NAZARA_WIDGETS_API SimpleImageButtonWidgetStyle : public ImageButtonWidgetStyle
	{
		public:
			struct StyleConfig;

			SimpleImageButtonWidgetStyle(ImageButtonWidget* imageButtonWidget, StyleConfig config);
			SimpleImageButtonWidgetStyle(const SimpleImageButtonWidgetStyle&) = delete;
			SimpleImageButtonWidgetStyle(SimpleImageButtonWidgetStyle&&) = default;
			~SimpleImageButtonWidgetStyle() = default;

			void Layout(const Vector2f& size) override;

			void OnHoverBegin() override;
			void OnHoverEnd() override;
			void OnPress() override;
			void OnRelease() override;
			void OnUpdate() override;

			void UpdateRenderLayer(int baseRenderLayer) override;

			SimpleImageButtonWidgetStyle& operator=(const SimpleImageButtonWidgetStyle&) = delete;
			SimpleImageButtonWidgetStyle& operator=(SimpleImageButtonWidgetStyle&&) = default;

			struct StyleConfig
			{
				std::shared_ptr<MaterialInstance> hoveredMaterial;
				float hoveredCornerSize;
				float hoveredCornerTexCoords;
			};

		protected:
			virtual void Update(bool hovered, bool pressed);

		private:
			std::shared_ptr<SlicedSprite> m_hoveredSprite;
			std::shared_ptr<SlicedSprite> m_sprite;
			entt::entity m_entity;
			bool m_isHovered;
			bool m_isPressed;
	};

	class NAZARA_WIDGETS_API SimpleLabelWidgetStyle : public LabelWidgetStyle
	{
		public:
			SimpleLabelWidgetStyle(AbstractLabelWidget* labelWidget, std::shared_ptr<MaterialInstance> material, std::shared_ptr<MaterialInstance> hoveredMaterial = {});
			SimpleLabelWidgetStyle(const SimpleLabelWidgetStyle&) = delete;
			SimpleLabelWidgetStyle(SimpleLabelWidgetStyle&&) = default;
			~SimpleLabelWidgetStyle() = default;

			void Layout(const Vector2f& size) override;

			void OnHoverBegin() override;
			void OnHoverEnd() override;

			void UpdateRenderLayer(int baseRenderLayer) override;
			void UpdateText(const AbstractTextDrawer& drawer) override;

			SimpleLabelWidgetStyle& operator=(const SimpleLabelWidgetStyle&) = delete;
			SimpleLabelWidgetStyle& operator=(SimpleLabelWidgetStyle&&) = default;

		protected:
			virtual void UpdateMaterial(bool hovered);

		private:
			std::shared_ptr<MaterialInstance> m_hoveredMaterial;
			std::shared_ptr<MaterialInstance> m_material;
			std::shared_ptr<TextSprite> m_textSprite;
			entt::entity m_entity;
	};

	class NAZARA_WIDGETS_API SimpleProgressBarWidgetStyle : public ProgressBarWidgetStyle
	{
		public:
			struct StyleConfig;

			SimpleProgressBarWidgetStyle(ProgressBarWidget* progressBarWidget, StyleConfig styleConfig);
			SimpleProgressBarWidgetStyle(const SimpleProgressBarWidgetStyle&) = delete;
			SimpleProgressBarWidgetStyle(SimpleProgressBarWidgetStyle&&) = default;
			~SimpleProgressBarWidgetStyle() = default;

			void Layout(const Vector2f& size) override;

			void UpdateRenderLayer(int baseRenderLayer) override;

			SimpleProgressBarWidgetStyle& operator=(const SimpleProgressBarWidgetStyle&) = delete;
			SimpleProgressBarWidgetStyle& operator=(SimpleProgressBarWidgetStyle&&) = default;

			struct StyleConfig
			{
				std::shared_ptr<MaterialInstance> backgroundMaterial;
				Color progressBarBeginColor;
				Color progressBarEndColor;
				float backgroundCornerSize;
				float backgroundCornerTexCoords;
				float barOffset;
			};

		private:
			std::shared_ptr<MaterialInstance> m_backgroundMaterial;
			std::shared_ptr<SlicedSprite> m_backgroundSprite;
			std::shared_ptr<Sprite> m_progressBarSprite;
			entt::entity m_backgroundEntity;
			entt::entity m_barEntity;
			Color m_progressBarBeginColor;
			Color m_progressBarEndColor;
			float m_barOffset;
	};

	class NAZARA_WIDGETS_API SimpleScrollAreaWidgetStyle : public ScrollAreaWidgetStyle
	{
		public:
			SimpleScrollAreaWidgetStyle(ScrollAreaWidget* scrollAreaWidget);
			SimpleScrollAreaWidgetStyle(const SimpleScrollAreaWidgetStyle&) = delete;
			SimpleScrollAreaWidgetStyle(SimpleScrollAreaWidgetStyle&&) = default;
			~SimpleScrollAreaWidgetStyle() = default;

			void Layout(const Vector2f& size) override;

			void UpdateRenderLayer(int baseRenderLayer) override;

			SimpleScrollAreaWidgetStyle& operator=(const SimpleScrollAreaWidgetStyle&) = delete;
			SimpleScrollAreaWidgetStyle& operator=(SimpleScrollAreaWidgetStyle&&) = default;

		private:
			std::shared_ptr<MaterialInstance> m_hoveredMaterial;
			std::shared_ptr<MaterialInstance> m_material;
			std::shared_ptr<TextSprite> m_textSprite;
			entt::entity m_entity;
	};

	class NAZARA_WIDGETS_API SimpleScrollbarWidgetStyle : public ScrollbarWidgetStyle
	{
		public:
			struct StyleConfig;

			SimpleScrollbarWidgetStyle(ScrollbarWidget* scrollBarWidget, StyleConfig config);
			SimpleScrollbarWidgetStyle(const SimpleScrollbarWidgetStyle&) = delete;
			SimpleScrollbarWidgetStyle(SimpleScrollbarWidgetStyle&&) = default;
			~SimpleScrollbarWidgetStyle() = default;

			std::unique_ptr<ImageButtonWidget> CreateBackButton(ScrollbarWidget* widget, ScrollbarOrientation orientation) override;
			std::unique_ptr<ImageButtonWidget> CreateForwardButton(ScrollbarWidget* widget, ScrollbarOrientation orientation) override;

			void Layout(const Vector2f& size) override;

			void UpdateRenderLayer(int baseRenderLayer) override;

			SimpleScrollbarWidgetStyle& operator=(const SimpleScrollbarWidgetStyle&) = delete;
			SimpleScrollbarWidgetStyle& operator=(SimpleScrollbarWidgetStyle&&) = default;

			struct StyleConfig
			{
				std::shared_ptr<MaterialInstance> backgroundHorizontalMaterial;
				std::shared_ptr<MaterialInstance> backgroundVerticalMaterial;
				std::shared_ptr<MaterialInstance> buttonDownMaterial;
				std::shared_ptr<MaterialInstance> buttonDownHoveredMaterial;
				std::shared_ptr<MaterialInstance> buttonDownPressedMaterial;
				std::shared_ptr<MaterialInstance> buttonLeftMaterial;
				std::shared_ptr<MaterialInstance> buttonLeftHoveredMaterial;
				std::shared_ptr<MaterialInstance> buttonLeftPressedMaterial;
				std::shared_ptr<MaterialInstance> buttonRightMaterial;
				std::shared_ptr<MaterialInstance> buttonRightHoveredMaterial;
				std::shared_ptr<MaterialInstance> buttonRightPressedMaterial;
				std::shared_ptr<MaterialInstance> buttonUpMaterial;
				std::shared_ptr<MaterialInstance> buttonUpHoveredMaterial;
				std::shared_ptr<MaterialInstance> buttonUpPressedMaterial;
				float buttonCornerSize;
				float buttonCornerTexcoords;
			};

		private:
			StyleConfig m_config;
			std::shared_ptr<Sprite> m_backgroundScrollbarSprite;
			std::shared_ptr<SlicedSprite> m_scrollbarSprite;
			entt::entity m_backgroundScrollbarSpriteEntity;
			entt::entity m_scrollbarSpriteEntity;
	};

	class NAZARA_WIDGETS_API SimpleScrollbarButtonWidgetStyle : public ScrollbarButtonWidgetStyle
	{
		public:
			struct StyleConfig;

			SimpleScrollbarButtonWidgetStyle(ScrollbarButtonWidget* scrollbarButtonWidget, StyleConfig config);
			SimpleScrollbarButtonWidgetStyle(const SimpleScrollbarWidgetStyle&) = delete;
			SimpleScrollbarButtonWidgetStyle(SimpleScrollbarButtonWidgetStyle&&) = default;
			~SimpleScrollbarButtonWidgetStyle() = default;

			void Layout(const Vector2f& size) override;

			void OnHoverBegin() override;
			void OnHoverEnd() override;
			void OnGrab() override;
			void OnRelease() override;

			void UpdateRenderLayer(int baseRenderLayer) override;

			SimpleScrollbarButtonWidgetStyle& operator=(const SimpleScrollbarButtonWidgetStyle&) = delete;
			SimpleScrollbarButtonWidgetStyle& operator=(SimpleScrollbarButtonWidgetStyle&&) = default;

			struct StyleConfig
			{
				std::shared_ptr<MaterialInstance> material;
				std::shared_ptr<MaterialInstance> grabbedMaterial;
				std::shared_ptr<MaterialInstance> grabbedHoveredMaterial;
				std::shared_ptr<MaterialInstance> hoveredMaterial;
				float cornerSize;
				float cornerTexCoords;
			};

		protected:
			virtual void Update(bool hovered, bool pressed);

		private:
			StyleConfig m_config;
			std::shared_ptr<MaterialInstance> m_hoveredMaterial;
			std::shared_ptr<MaterialInstance> m_material;
			std::shared_ptr<MaterialInstance> m_pressedMaterial;
			std::shared_ptr<MaterialInstance> m_pressedHoveredMaterial;
			std::shared_ptr<SlicedSprite> m_sprite;
			entt::entity m_entity;
			bool m_isHovered;
			bool m_isPressed;
	};

	class NAZARA_WIDGETS_API SimpleTextAreaWidgetStyle : public TextAreaWidgetStyle
	{
		public:
			struct StyleConfig;

			SimpleTextAreaWidgetStyle(AbstractTextAreaWidget* textAreaWidget, StyleConfig config);
			SimpleTextAreaWidgetStyle(const SimpleTextAreaWidgetStyle&) = delete;
			SimpleTextAreaWidgetStyle(SimpleTextAreaWidgetStyle&&) = default;
			~SimpleTextAreaWidgetStyle() = default;

			void EnableBackground(bool enable) override;

			void Layout(const Vector2f& size) override;

			void OnDisabled() override;
			void OnEnabled() override;
			void OnFocusLost() override;
			void OnFocusReceived() override;

			void UpdateBackgroundColor(const Color& color) override;
			void UpdateCursors(std::span<const Rectf> cursorRects) override;
			void UpdateRenderLayer(int baseRenderLayer) override;
			void UpdateText(const AbstractTextDrawer& drawer) override;
			void UpdateTextOffset(float offset) override;

			SimpleTextAreaWidgetStyle& operator=(const SimpleTextAreaWidgetStyle&) = delete;
			SimpleTextAreaWidgetStyle& operator=(SimpleTextAreaWidgetStyle&&) = default;

			struct StyleConfig
			{
				std::shared_ptr<MaterialInstance> backgroundMaterial;
				std::shared_ptr<MaterialInstance> backgroundDisabledMaterial;
				Color insertionCursorColor;
				Color selectionCursorColor;
				Color selectionCursorColorNoFocus;
				Vector2f padding;
				float backgroundCornerSize;
				float backgroundCornerTexCoords;
			};

		private:
			void UpdateCursorColor(bool hasFocus);

			struct Cursor
			{
				std::shared_ptr<Sprite> sprite;
				entt::entity entity;
			};

			StyleConfig m_config;
			std::shared_ptr<SlicedSprite> m_backgroundSprite;
			std::shared_ptr<TextSprite> m_textSprite;
			std::vector<Cursor> m_cursors;
			entt::entity m_backgroundEntity;
			entt::entity m_textEntity;
			Color m_backgroundColor;
			bool m_hasFocus;
			bool m_isDisabled;
			int m_baseRenderLayer;
	};
}

#include <Nazara/Widgets/DefaultWidgetTheme.inl>

#endif // NAZARA_WIDGETS_SIMPLEWIDGETSTYLES_HPP
