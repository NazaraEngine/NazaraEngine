// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseWidget.hpp>
#include <NDK/Canvas.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/World.hpp>
#include <algorithm>

namespace Ndk
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
		m_world = m_canvas->GetWorld();

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
		if (m_backgroundEntity.IsValid() == enable)
			return;

		if (enable)
		{
			m_backgroundSprite = Nz::Sprite::New();
			m_backgroundSprite->SetColor(m_backgroundColor);
			m_backgroundSprite->SetMaterial(Nz::Material::New((m_backgroundColor.IsOpaque()) ? "Basic2D" : "Translucent2D"));

			m_backgroundEntity = CreateEntity();
			m_backgroundEntity->AddComponent<GraphicsComponent>().Attach(m_backgroundSprite, -1);
			m_backgroundEntity->AddComponent<NodeComponent>().SetParent(this);

			BaseWidget::Layout(); // Only layout background
		}
		else
		{
			m_backgroundEntity->Kill();
			m_backgroundSprite.Reset();
		}
	}

	void BaseWidget::GrabKeyboard()
	{
		m_canvas->SetKeyboardOwner(this);
	}

	void BaseWidget::SetBackgroundColor(const Nz::Color& color)
	{
		m_backgroundColor = color;

		if (m_backgroundSprite)
		{
			m_backgroundSprite->SetColor(color);
			m_backgroundSprite->GetMaterial()->Configure((color.IsOpaque()) ? "Basic2D" : "Translucent2D"); //< Our sprite has its own material (see EnableBackground)
		}
	}

	void BaseWidget::SetCursor(Nz::SystemCursor systemCursor)
	{
		m_cursor = systemCursor;

		if (IsRegisteredToCanvas())
			m_canvas->NotifyWidgetCursorUpdate(m_canvasIndex);
	}

	void BaseWidget::SetSize(const Nz::Vector2f& size)
	{
		SetContentSize({std::max(size.x - m_padding.left - m_padding.right, 0.f), std::max(size.y - m_padding.top - m_padding.bottom, 0.f)});
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

			for (const EntityHandle& entity : m_entities)
				entity->Enable(show);

			for (const auto& widgetPtr : m_children)
				widgetPtr->Show(show);
		}
	}

	EntityHandle BaseWidget::CreateEntity()
	{
		EntityHandle newEntity = m_world->CreateEntity();
		newEntity->Enable(m_visible);

		m_entities.emplace_back(newEntity);
		return newEntity;
	}

	void BaseWidget::DestroyEntity(Entity* entity)
	{
		auto it = std::find(m_entities.begin(), m_entities.end(), entity);
		NazaraAssert(it != m_entities.end(), "Entity does not belong to this widget");

		m_entities.erase(it);
	}

	void BaseWidget::Layout()
	{
		if (IsRegisteredToCanvas())
			m_canvas->NotifyWidgetBoxUpdate(m_canvasIndex);

		if (m_backgroundEntity)
			m_backgroundSprite->SetSize(m_contentSize.x + m_padding.left + m_padding.right, m_contentSize.y + m_padding.top + m_padding.bottom);
	}

	void BaseWidget::InvalidateNode()
	{
		Node::InvalidateNode();

		if (IsRegisteredToCanvas())
			m_canvas->NotifyWidgetBoxUpdate(m_canvasIndex);
	}

	void BaseWidget::OnKeyPressed(const Nz::WindowEvent::KeyEvent& /*key*/)
	{
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

	void BaseWidget::OnMouseExit()
	{
	}

	void BaseWidget::OnParentResized(const Nz::Vector2f& /*newSize*/)
	{
	}

	void BaseWidget::OnTextEntered(char32_t /*character*/, bool /*repeated*/)
	{
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
}
