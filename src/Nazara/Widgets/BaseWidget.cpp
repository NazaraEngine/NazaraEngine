// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <algorithm>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup NDK
	* \class Ndk::BaseWidget
	* \brief Abstract class serving as a base class for all widgets
	*/

	/*!
	* \brief Constructs a BaseWidget object using another widget as its parent
	*
	* \param parent Parent widget, must be valid and attached to a canvas
	*
	* Constructs a BaseWidget object using another widget as a base.
	* This will also register the widget to the canvas owning the top-most widget.
	*/
	BaseWidget::BaseWidget(BaseWidget* parent) :
	BaseWidget()
	{
		NazaraAssert(parent, "Invalid parent");
		NazaraAssert(parent->GetCanvas(), "Parent has no canvas");

		m_canvas = parent->GetCanvas();
		m_widgetParent = parent;
		m_registry = &m_canvas->GetRegistry();

		RegisterToCanvas();
	}

	/*!
	 * \brief Frees the widget, unregistering it from its canvas
	 */
	BaseWidget::~BaseWidget()
	{
		UnregisterFromCanvas();
	}

	/*!
	 * \brief Clears keyboard focus if and only if this widget owns it.
	 */
	void BaseWidget::ClearFocus()
	{
		if (IsRegisteredToCanvas())
			m_canvas->ClearKeyboardOwner(m_canvasIndex);
	}

	/*!
	 * \brief Destroy the widget, deleting it in the process.
	 *
	 * Calling this function immediately destroys the widget, freeing its memory.
	 */
	void BaseWidget::Destroy()
	{
		NazaraAssert(this != m_canvas, "Canvas cannot be destroyed by calling Destroy()");

		m_widgetParent->DestroyChild(this); //< This does delete us
	}

	/*!
	 * \brief Enable or disables the widget background.
	 */
	void BaseWidget::EnableBackground(bool enable)
	{
		if (m_backgroundEntity.has_value() == enable)
			return;

		if (enable)
		{
			auto material = std::make_shared<Material>();
			material->AddPass("ForwardPass", std::make_shared<MaterialPass>(BasicMaterial::GetSettings()));

			m_backgroundSprite = std::make_shared<Nz::Sprite>(std::move(material));
			m_backgroundSprite->SetColor(m_backgroundColor);
			//m_backgroundSprite->SetMaterial(Nz::Material::New((m_backgroundColor.IsOpaque()) ? "Basic2D" : "Translucent2D")); //< TODO: Use a shared material instead of creating one everytime

			entt::entity backgroundEntity = CreateEntity();
			m_registry->emplace<GraphicsComponent>(backgroundEntity).AttachRenderable(m_backgroundSprite);
			m_registry->emplace<NodeComponent>(backgroundEntity).SetParent(this);

			m_backgroundEntity = backgroundEntity;

			BaseWidget::Layout(); // Only layout background
		}
		else
		{
			assert(m_backgroundEntity);

			DestroyEntity(*m_backgroundEntity);
			m_backgroundSprite.reset();
		}
	}

	/*!
	 * \brief Checks if this widget has keyboard focus
	 * \return true if widget has keyboard focus, false otherwise
	 */
	bool BaseWidget::HasFocus() const
	{
		if (!IsRegisteredToCanvas())
			return false;

		return m_canvas->IsKeyboardOwner(m_canvasIndex);
	}

	void BaseWidget::Resize(const Nz::Vector2f& size)
	{
		// Adjust new size
		Nz::Vector2f newSize = size;
		newSize.Maximize(m_minimumSize);
		newSize.Minimize(m_maximumSize);

		NotifyParentResized(newSize);
		m_size = newSize;

		Layout();
	}

	void BaseWidget::SetBackgroundColor(const Color& color)
	{
		m_backgroundColor = color;

		if (m_backgroundSprite)
		{
			m_backgroundSprite->SetColor(color);
			//m_backgroundSprite->GetMaterial()->Configure((color.IsOpaque()) ? "Basic2D" : "Translucent2D"); //< Our sprite has its own material (see EnableBackground)
		}
	}

	void BaseWidget::SetCursor(Nz::SystemCursor systemCursor)
	{
		m_cursor = systemCursor;

		if (IsRegisteredToCanvas())
			m_canvas->NotifyWidgetCursorUpdate(m_canvasIndex);
	}

	void BaseWidget::SetFocus()
	{
		if (IsRegisteredToCanvas())
			m_canvas->SetKeyboardOwner(m_canvasIndex);
	}

	void BaseWidget::SetParent(BaseWidget* widget)
	{
		Canvas* oldCanvas = m_canvas;
		Canvas* newCanvas = widget->GetCanvas();

		// Changing a widget canvas is a problem because of the canvas entities
		NazaraAssert(oldCanvas == newCanvas, "Transferring a widget between canvas is not yet supported");

		Node::SetParent(widget);
		m_widgetParent = widget;

		Layout();
	}

	void BaseWidget::SetRenderingRect(const Nz::Rectf& renderingRect)
	{
		m_renderingRect = renderingRect;

		UpdatePositionAndSize();
		for (const auto& widgetPtr : m_children)
			widgetPtr->UpdatePositionAndSize();
	}

	void BaseWidget::Show(bool show)
	{
		if (m_visible != show)
		{
			m_visible = show;

			if (m_visible)
				RegisterToCanvas();
			else
				UnregisterFromCanvas();

			for (WidgetEntity& entity : m_entities)
			{
				if (entity.isEnabled)
				{
					//entity.handle->Enable(show); //< This will override isEnabled, so reset it next line
					entity.isEnabled = true;
				}
			}

			ShowChildren(show);
		}
	}

	entt::entity BaseWidget::CreateEntity()
	{
		entt::entity newEntity = m_registry->create();
		//newEntity->Enable(m_visible);

		m_entities.emplace_back();
		WidgetEntity& newWidgetEntity = m_entities.back();
		newWidgetEntity.handle = newEntity;
		/*newWidgetEntity.onDisabledSlot.Connect(newEntity->OnEntityDisabled, [this](Entity* entity)
		{
			auto it = std::find_if(m_entities.begin(), m_entities.end(), [&](const WidgetEntity& widgetEntity) { return widgetEntity.handle == entity; });
			NazaraAssert(it != m_entities.end(), "Entity does not belong to this widget");

			it->isEnabled = false;
		});

		newWidgetEntity.onEnabledSlot.Connect(newEntity->OnEntityEnabled, [this](Entity* entity)
		{
			auto it = std::find_if(m_entities.begin(), m_entities.end(), [&](const WidgetEntity& widgetEntity) { return widgetEntity.handle == entity; });
			NazaraAssert(it != m_entities.end(), "Entity does not belong to this widget");

			if (!IsVisible())
				entity->Disable(); // Next line will override isEnabled status

			it->isEnabled = true;
		});*/

		return newEntity;
	}

	void BaseWidget::DestroyEntity(entt::entity entity)
	{
		auto it = std::find_if(m_entities.begin(), m_entities.end(), [&](const WidgetEntity& widgetEntity) { return widgetEntity.handle == entity; });
		NazaraAssert(it != m_entities.end(), "Entity does not belong to this widget");

		m_entities.erase(it);
		m_registry->destroy(entity);
	}

	void BaseWidget::Layout()
	{
		if (m_backgroundSprite)
			m_backgroundSprite->SetSize({ m_size.x, m_size.y });

		UpdatePositionAndSize();
	}

	void BaseWidget::InvalidateNode()
	{
		Node::InvalidateNode();

		UpdatePositionAndSize();
	}

	Nz::Rectf BaseWidget::GetScissorRect() const
	{
		Nz::Vector2f widgetPos = Nz::Vector2f(GetPosition(Nz::CoordSys::Global));
		Nz::Vector2f widgetSize = GetSize();

		Nz::Rectf widgetRect(widgetPos.x, widgetPos.y, widgetSize.x, widgetSize.y);
		Nz::Rectf widgetRenderingRect(widgetPos.x + m_renderingRect.x, widgetPos.y + m_renderingRect.y, m_renderingRect.width, m_renderingRect.height);

		widgetRect.Intersect(widgetRenderingRect, &widgetRect);

		return widgetRect;
	}

	bool BaseWidget::IsFocusable() const
	{
		return false;
	}

	void BaseWidget::OnFocusLost()
	{
	}

	void BaseWidget::OnFocusReceived()
	{
	}

	bool BaseWidget::OnKeyPressed(const Nz::WindowEvent::KeyEvent& key)
	{
		return false;
	}

	void BaseWidget::OnKeyReleased(const Nz::WindowEvent::KeyEvent& /*key*/)
	{
	}

	void BaseWidget::OnMouseEnter()
	{
	}

	void BaseWidget::OnMouseMoved(int /*x*/, int /*y*/, int /*deltaX*/, int /*deltaY*/)
	{
	}

	void BaseWidget::OnMouseButtonPress(int /*x*/, int /*y*/, Nz::Mouse::Button /*button*/)
	{
	}

	void BaseWidget::OnMouseButtonRelease(int /*x*/, int /*y*/, Nz::Mouse::Button /*button*/)
	{
	}

	void BaseWidget::OnMouseWheelMoved(int /*x*/, int /*y*/, float /*delta*/)
	{
	}

	void BaseWidget::OnMouseExit()
	{
	}

	void BaseWidget::OnParentResized(const Nz::Vector2f& /*newSize*/)
	{
	}

	void BaseWidget::OnTextEntered(char32_t /*character*/, bool /*repeated*/)
	{
	}

	void BaseWidget::OnTextEdited(const std::array<char, 32>& /*characters*/, int /*length*/)
	{
	}

	inline entt::registry& BaseWidget::GetRegistry()
	{
		assert(m_registry);
		return *m_registry;
	}

	inline const entt::registry& BaseWidget::GetRegistry() const
	{
		assert(m_registry);
		return *m_registry;
	}

	void BaseWidget::ShowChildren(bool show)
	{
		for (const auto& widgetPtr : m_children)
			widgetPtr->Show(show);
	}

	void BaseWidget::DestroyChild(BaseWidget* widget)
	{
		auto it = std::find_if(m_children.begin(), m_children.end(), [widget] (const std::unique_ptr<BaseWidget>& widgetPtr) -> bool
		{
			return widgetPtr.get() == widget;
		});

		NazaraAssert(it != m_children.end(), "Child widget not found in parent");

		m_children.erase(it);
	}

	void BaseWidget::DestroyChildren()
	{
		m_children.clear();
	}

	void BaseWidget::RegisterToCanvas()
	{
		NazaraAssert(!IsRegisteredToCanvas(), "Widget is already registered to canvas");

		m_canvasIndex = m_canvas->RegisterWidget(this);
	}

	void BaseWidget::UnregisterFromCanvas()
	{
		if (IsRegisteredToCanvas())
		{
			m_canvas->UnregisterWidget(m_canvasIndex);
			m_canvasIndex = InvalidCanvasIndex;
		}
	}

	void BaseWidget::UpdatePositionAndSize()
	{
		if (IsRegisteredToCanvas())
			m_canvas->NotifyWidgetBoxUpdate(m_canvasIndex);

		Nz::Rectf scissorRect = GetScissorRect();

		if (m_widgetParent)
		{
			Nz::Rectf parentScissorRect = m_widgetParent->GetScissorRect();

			if (!scissorRect.Intersect(parentScissorRect, &scissorRect))
				scissorRect = parentScissorRect;
		}

		/*Nz::Recti fullBounds(scissorRect);
		for (WidgetEntity& widgetEntity : m_entities)
		{
			const Ndk::EntityHandle& entity = widgetEntity.handle;
			if (entity->HasComponent<GraphicsComponent>())
				entity->GetComponent<GraphicsComponent>().SetScissorRect(fullBounds);
		}*/
	}
}
