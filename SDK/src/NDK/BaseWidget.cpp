// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/BaseWidget.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	BaseWidget::~BaseWidget()
	{
		for (BaseWidget* child : m_children)
			delete child;
	}

	inline void BaseWidget::EnableBackground(bool enable)
	{
		if (m_backgroundEntity.IsValid() == enable)
			return;

		if (enable)
		{
			m_backgroundSprite = Nz::Sprite::New();
			m_backgroundSprite->SetColor(m_backgroundColor);
			m_backgroundSprite->SetMaterial(Nz::Material::New((m_backgroundColor.IsOpaque()) ? "Basic2D" : "Translucent2D"));

			m_backgroundEntity = m_world->CreateEntity();
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

	void BaseWidget::SetSize(const Nz::Vector2f& size)
	{
		SetContentSize({std::max(size.x - m_padding.left - m_padding.right, 0.f), std::max(size.y - m_padding.top - m_padding.bottom, 0.f)});
	}

	EntityHandle BaseWidget::CreateEntity()
	{
		m_entities.emplace_back(m_world->CreateEntity());
		return m_entities.back();
	}

	void BaseWidget::DestroyEntity(Entity* entity)
	{
		auto it = std::find(m_entities.begin(), m_entities.end(), entity);
		NazaraAssert(it != m_entities.end(), "Entity does not belong to this widget");

		m_entities.erase(it);
	}

	void BaseWidget::Layout()
	{
		if (m_backgroundEntity)
		{
			NodeComponent& node = m_backgroundEntity->GetComponent<NodeComponent>();
			node.SetPosition(-m_padding.left, -m_padding.top);

			m_backgroundSprite->SetSize(m_contentSize.x + m_padding.left + m_padding.right, m_contentSize.y + m_padding.top + m_padding.bottom);
		}
	}
}