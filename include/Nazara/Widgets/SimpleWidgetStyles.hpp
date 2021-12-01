// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_SIMPLEWIDGETSTYLES_HPP
#define NAZARA_WIDGETS_SIMPLEWIDGETSTYLES_HPP

#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>
#include <entt/entt.hpp>

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

			void OnHoverBegin() override;
			void OnHoverEnd() override;
			void OnPress() override;
			void OnRelease() override;

			void UpdateText(const AbstractTextDrawer& drawer) override;

			SimpleButtonWidgetStyle& operator=(const SimpleButtonWidgetStyle&) = delete;
			SimpleButtonWidgetStyle& operator=(SimpleButtonWidgetStyle&&) = default;

			struct StyleConfig
			{
				std::shared_ptr<Material> hoveredMaterial;
				std::shared_ptr<Material> material;
				std::shared_ptr<Material> pressedMaterial;
				std::shared_ptr<Material> pressedHoveredMaterial;
				float cornerSize;
				float cornerTexCoords;
			};

		protected:
			virtual void UpdateMaterial(bool hovered, bool pressed);

		private:
			std::shared_ptr<Material> m_hoveredMaterial;
			std::shared_ptr<Material> m_material;
			std::shared_ptr<Material> m_pressedMaterial;
			std::shared_ptr<Material> m_pressedHoveredMaterial;
			std::shared_ptr<SlicedSprite> m_sprite;
			std::shared_ptr<TextSprite> m_textSprite;
			entt::entity m_spriteEntity;
			entt::entity m_textEntity;
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

			SimpleCheckboxWidgetStyle& operator=(const SimpleCheckboxWidgetStyle&) = delete;
			SimpleCheckboxWidgetStyle& operator=(SimpleCheckboxWidgetStyle&&) = default;

			struct StyleConfig
			{
				std::shared_ptr<Material> backgroundMaterial;
				std::shared_ptr<Material> backgroundHoveredMaterial;
				std::shared_ptr<Material> checkMaterial;
				std::shared_ptr<Material> tristateMaterial;
				float backgroundCornerSize;
				float backgroundCornerTexCoords;
			};

		protected:
			virtual void UpdateMaterial(bool hovered);

		private:
			std::shared_ptr<Material> m_checkMaterial;
			std::shared_ptr<Material> m_hoveredMaterial;
			std::shared_ptr<Material> m_material;
			std::shared_ptr<Material> m_tristateMaterial;
			std::shared_ptr<Sprite> m_checkSprite;
			std::shared_ptr<SlicedSprite> m_backgroundSprite;
			entt::entity m_backgroundEntity;
			entt::entity m_checkEntity;
			bool m_isHovered;
	};

	class NAZARA_WIDGETS_API SimpleLabelWidgetStyle : public LabelWidgetStyle
	{
		public:
			SimpleLabelWidgetStyle(LabelWidget* labelWidget, std::shared_ptr<Material> material, std::shared_ptr<Material> hoveredMaterial = {});
			SimpleLabelWidgetStyle(const SimpleLabelWidgetStyle&) = delete;
			SimpleLabelWidgetStyle(SimpleLabelWidgetStyle&&) = default;
			~SimpleLabelWidgetStyle() = default;

			void Layout(const Vector2f& size) override;

			void OnHoverBegin() override;
			void OnHoverEnd() override;

			void UpdateText(const AbstractTextDrawer& drawer) override;

			SimpleLabelWidgetStyle& operator=(const SimpleLabelWidgetStyle&) = delete;
			SimpleLabelWidgetStyle& operator=(SimpleLabelWidgetStyle&&) = default;

		protected:
			virtual void UpdateMaterial(bool hovered);

		private:
			std::shared_ptr<Material> m_hoveredMaterial;
			std::shared_ptr<Material> m_material;
			std::shared_ptr<TextSprite> m_textSprite;
			entt::entity m_entity;
	};
}

#include <Nazara/Widgets/DefaultWidgetTheme.inl>

#endif // NAZARA_WIDGETS_SIMPLEWIDGETSTYLES_HPP
