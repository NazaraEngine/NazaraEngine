// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_BASEWIDGET_HPP
#define NAZARA_WIDGETS_BASEWIDGET_HPP

#include <Nazara/Core/Node.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Platform/Mouse.hpp>
#include <Nazara/Platform/WindowEvent.hpp>
#include <Nazara/Widgets/Export.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <concepts>
#include <limits>
#include <optional>

namespace Nz
{
	class BaseWidgetStyle;
	class Canvas;
	class WidgetTheme;

	class NAZARA_WIDGETS_API BaseWidget : public Node
	{
		friend BaseWidgetStyle;
		friend Canvas;

		public:
			BaseWidget(BaseWidget* parent);
			BaseWidget(const BaseWidget&) = delete;
			BaseWidget(BaseWidget&&) = delete;
			virtual ~BaseWidget();

			template<std::derived_from<BaseWidget> T, typename... Args> T* Add(Args&&... args);
			template<std::derived_from<BaseWidget> T> T* AddChild(std::unique_ptr<T>&& widget);

			inline void Center();
			inline void CenterHorizontal();
			inline void CenterVertical();

			void ClearFocus();
			inline void ClearRenderingRect();

			void Destroy();

			virtual void EnableBackground(bool enable);
			inline void EnableMouseInput(bool enable);

			template<typename F> void ForEachWidgetChild(F&& iterator, bool onlyVisible = true);
			template<typename F> void ForEachWidgetChild(F&& iterator, bool onlyVisible = true) const;

			inline const Color& GetBackgroundColor() const;
			inline Canvas* GetCanvas();
			inline const Canvas* GetCanvas() const;
			inline SystemCursor GetCursor() const;
			inline float GetHeight() const;

			inline float GetMaximumHeight() const;
			inline Vector2f GetMaximumSize() const;
			inline float GetMaximumWidth() const;

			inline float GetMinimumHeight() const;
			inline Vector2f GetMinimumSize() const;
			inline float GetMinimumWidth() const;

			inline float GetPreferredHeight() const;
			inline Vector2f GetPreferredSize() const;
			inline float GetPreferredWidth() const;

			inline const Rectf& GetRenderingRect() const;

			inline Vector2f GetSize() const;
			inline const std::shared_ptr<WidgetTheme>& GetTheme() const;
			inline std::size_t GetVisibleWidgetChildCount() const;
			inline float GetWidth() const;
			inline std::size_t GetWidgetChildCount() const;

			bool HasFocus() const;

			inline void Hide();

			inline bool IsMouseInputEnabled() const;
			inline bool IsVisible() const;

			std::unique_ptr<BaseWidget> ReleaseFromParent();
			void Resize(const Vector2f& size);
			inline void ResizeToPreferredSize();

			virtual void SetBackgroundColor(const Color& color);
			void SetCursor(SystemCursor systemCursor);

			inline void SetFixedHeight(float fixedHeight);
			inline void SetFixedSize(const Vector2f& fixedSize);
			inline void SetFixedWidth(float fixedWidth);

			void SetFocus();

			inline void SetMaximumHeight(float maximumHeight);
			inline void SetMaximumSize(const Vector2f& maximumSize);
			inline void SetMaximumWidth(float maximumWidth);

			inline void SetMinimumHeight(float minimumHeight);
			inline void SetMinimumSize(const Vector2f& minimumSize);
			inline void SetMinimumWidth(float minimumWidth);

			virtual void SetRenderingRect(const Rectf& renderingRect);
			inline void SetRenderLayerOffset(int renderLayerOffset);

			void Show(bool show = true);

			BaseWidget& operator=(const BaseWidget&) = delete;
			BaseWidget& operator=(BaseWidget&&) = delete;

			NazaraSignal(OnWidgetResized, const BaseWidget* /*widget*/, const Vector2f& /*size*/);
			NazaraSignal(OnWidgetVisibilityUpdated, const BaseWidget* /*widget*/, bool /*isVisible*/);

		protected:
			void ClearMouseFocus();

			entt::entity CreateEntity();
			entt::entity CreateGraphicsEntity(Node* parent = nullptr);
			void DestroyEntity(entt::entity entity);

			inline int GetBaseRenderLayer() const;
			inline entt::registry& GetRegistry();
			inline const entt::registry& GetRegistry() const;

			Recti GetScissorBox() const;
			Rectf GetScissorRect() const;

			void InternalResize(const Vector2f& size);

			void InvalidateNode(Invalidation invalidation) override;

			virtual bool IsFocusable() const;
			inline bool IsInside(float x, float y) const;

			virtual void Layout();

			virtual void OnChildAdded(const BaseWidget* child);
			virtual void OnChildPreferredSizeUpdated(const BaseWidget* child);
			virtual void OnChildVisibilityUpdated(const BaseWidget* child);
			virtual void OnChildRemoved(const BaseWidget* child);
			virtual void OnFocusLost();
			virtual void OnFocusReceived();
			virtual bool OnKeyPressed(const WindowEvent::KeyEvent& key);
			virtual bool OnKeyReleased(const WindowEvent::KeyEvent& key);
			virtual void OnMouseEnter();
			virtual bool OnMouseMoved(int x, int y, int deltaX, int deltaY);
			virtual bool OnMouseButtonDoublePress(int x, int y, Mouse::Button button);
			virtual bool OnMouseButtonPress(int x, int y, Mouse::Button button);
			virtual bool OnMouseButtonRelease(int x, int y, Mouse::Button button);
			virtual bool OnMouseButtonTriplePress(int x, int y, Mouse::Button button);
			virtual void OnMouseExit();
			virtual bool OnMouseWheelMoved(int x, int y, float delta);
			virtual void OnRenderLayerUpdated(int baseRenderLayer);
			virtual void OnParentResized(const Vector2f& newSize);
			virtual bool OnTextEntered(char32_t character, bool repeated);
			virtual bool OnTextEdited(const std::array<char, 32>& characters, int length);
			virtual void OnVisibilityUpdated(bool newVisibilityState);

			inline void SetBaseRenderLayer(int baseRenderLayer);
			inline void SetPreferredSize(const Vector2f& preferredSize);
			inline void SetRenderLayerCount(int renderLayerCount);

			virtual void ShowChildren(bool show);

		private:
			inline BaseWidget(std::shared_ptr<WidgetTheme> theme);

			void DestroyChild(BaseWidget* widget);
			void DestroyChildren();
			inline bool IsRegisteredToCanvas() const;
			inline void NotifyParentResized(const Vector2f& newSize);
			void RegisterToCanvas();
			void SetParent(BaseWidget* widget);
			inline void UpdateCanvasIndex(std::size_t index);
			void UnregisterFromCanvas();
			void UpdatePositionAndSize();
			void UpdateRenderLayers();

			struct WidgetEntity
			{
				entt::entity handle;
				bool wasVisible;

				NazaraSlot(GraphicsComponent, OnVisibilityUpdate, onVisibilityUpdate);
			};

			static constexpr std::size_t InvalidCanvasIndex = std::numeric_limits<std::size_t>::max();

			std::optional<entt::entity> m_backgroundEntity;
			std::size_t m_canvasIndex;
			std::shared_ptr<Sprite> m_backgroundSprite;
			std::shared_ptr<WidgetTheme> m_theme;
			std::vector<WidgetEntity> m_entities;
			std::vector<std::unique_ptr<BaseWidget>> m_widgetChilds;
			entt::registry* m_registry;
			Canvas* m_canvas;
			Color m_backgroundColor;
			Rectf m_renderingRect;
			SystemCursor m_cursor;
			Vector2f m_maximumSize;
			Vector2f m_minimumSize;
			Vector2f m_preferredSize;
			Vector2f m_size;
			BaseWidget* m_parentWidget;
			bool m_disableVisibilitySignal;
			bool m_followPreferredSize;
			bool m_isMouseInputEnabled;
			bool m_visible;
			int m_baseRenderLayer;
			int m_renderLayerCount;
			int m_renderLayoutOffset;
	};
}

#include <Nazara/Widgets/BaseWidget.inl>

#endif // NAZARA_WIDGETS_BASEWIDGET_HPP
