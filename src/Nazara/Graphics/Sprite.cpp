// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Scene.hpp>
#include <cstring>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

NzSprite::NzSprite() :
m_color(NzColor::White),
m_textureCoords(0.f, 0.f, 1.f, 1.f),
m_size(64.f, 64.f),
m_verticesUpdated(false)
{
	SetDefaultMaterial();
}

NzSprite::NzSprite(NzTexture* texture) :
m_color(NzColor::White),
m_textureCoords(0.f, 0.f, 1.f, 1.f),
m_size(64.f, 64.f),
m_verticesUpdated(false)
{
	SetTexture(texture, true);
}

NzSprite::NzSprite(const NzSprite& sprite) :
NzSceneNode(sprite),
m_color(sprite.m_color),
m_material(sprite.m_material),
m_textureCoords(sprite.m_textureCoords),
m_size(sprite.m_size),
m_vertices(sprite.m_vertices),
m_verticesUpdated(sprite.m_verticesUpdated)
{
	SetParent(sprite.GetParent());
}

void NzSprite::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
	if (!m_verticesUpdated)
		UpdateVertices();

	renderQueue->AddSprites(m_material, m_vertices, 1);
}

NzSprite* NzSprite::Clone() const
{
	return new NzSprite(*this);
}

NzSprite* NzSprite::Create() const
{
	return new NzSprite;
}

const NzColor& NzSprite::GetColor() const
{
	return m_color;
}

NzMaterial* NzSprite::GetMaterial() const
{
	return m_material;
}

nzSceneNodeType NzSprite::GetSceneNodeType() const
{
	return nzSceneNodeType_Sprite;
}

const NzVector2f& NzSprite::GetSize() const
{
	return m_size;
}

const NzRectf& NzSprite::GetTextureCoords() const
{
	return m_textureCoords;
}

bool NzSprite::IsDrawable() const
{
	return m_material != nullptr;
}

void NzSprite::SetColor(const NzColor& color)
{
	m_color = color;
	m_verticesUpdated = false;
}

void NzSprite::SetDefaultMaterial()
{
	NzMaterialRef material = NzMaterial::New();
	material->Enable(nzRendererParameter_FaceCulling, false);
	material->EnableLighting(false);

	SetMaterial(material);
}

void NzSprite::SetMaterial(NzMaterial* material, bool resizeSprite)
{
	m_material = material;
	if (m_material && resizeSprite)
	{
		NzTexture* diffuseMap = m_material->GetDiffuseMap();
		if (diffuseMap && diffuseMap->IsValid())
			SetSize(NzVector2f(NzVector2ui(diffuseMap->GetSize())));
	}
}

void NzSprite::SetSize(const NzVector2f& size)
{
	m_size = size;

	// On invalide la bounding box
	m_boundingVolume.MakeNull();
	m_boundingVolumeUpdated = false;
	m_verticesUpdated = false;
}

void NzSprite::SetSize(float sizeX, float sizeY)
{
	SetSize(NzVector2f(sizeX, sizeY));
}

void NzSprite::SetTexture(NzTexture* texture, bool resizeSprite)
{
	if (!m_material)
		SetDefaultMaterial();
	else if (m_material->GetReferenceCount() > 1)
		m_material = NzMaterial::New(*m_material); // Copie

	m_material->SetDiffuseMap(texture);
	if (resizeSprite && texture && texture->IsValid())
		SetSize(NzVector2f(NzVector2ui(texture->GetSize())));
}

void NzSprite::SetTextureCoords(const NzRectf& coords)
{
	m_textureCoords = coords;
	m_verticesUpdated = false;
}

void NzSprite::SetTextureRect(const NzRectui& rect)
{
	#if NAZARA_GRAPHICS_SAFE
	if (!m_material)
	{
		NazaraError("Sprite has no material");
		return;
	}

	if (!m_material->HasDiffuseMap())
	{
		NazaraError("Sprite material has no diffuse map");
		return;
	}
	#endif

	NzTexture* diffuseMap = m_material->GetDiffuseMap();

	float invWidth = 1.f/diffuseMap->GetWidth();
	float invHeight = 1.f/diffuseMap->GetHeight();

	SetTextureCoords(NzRectf(invWidth*rect.x, invHeight*rect.y, invWidth*rect.width, invHeight*rect.height));
}

NzSprite& NzSprite::operator=(const NzSprite& sprite)
{
	NzSceneNode::operator=(sprite);

	m_color = sprite.m_color;
	m_material = sprite.m_material;
	m_textureCoords = sprite.m_textureCoords;
	m_size = sprite.m_size;

	// On ne copie pas les sommets finaux car il est très probable que nos paramètres soient modifiés et qu'ils doivent être régénérés de toute façon
	m_verticesUpdated = false;

	return *this;
}

void NzSprite::InvalidateNode()
{
	NzSceneNode::InvalidateNode();

	m_verticesUpdated = false;
}

void NzSprite::Register()
{
	// Le changement de scène peut affecter les sommets
	m_verticesUpdated = false;
}

void NzSprite::Unregister()
{
}

void NzSprite::MakeBoundingVolume() const
{
	NzVector3f down = (m_scene) ? m_scene->GetDown() : NzVector3f::Down();
	NzVector3f right = (m_scene) ? m_scene->GetRight() : NzVector3f::Right();

	m_boundingVolume.Set(NzVector3f(0.f), m_size.x*right + m_size.y*down);
}

void NzSprite::UpdateVertices() const
{
	if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

	NzVector3f down = (m_scene) ? m_scene->GetDown() : NzVector3f::Down();
	NzVector3f right = (m_scene) ? m_scene->GetRight() : NzVector3f::Right();

	m_vertices[0].color = m_color;
	m_vertices[0].position = m_transformMatrix.Transform(NzVector3f(0.f));
	m_vertices[0].uv.Set(m_textureCoords.GetCorner(nzRectCorner_LeftTop));

	m_vertices[1].color = m_color;
	m_vertices[1].position = m_transformMatrix.Transform(m_size.x*right);
	m_vertices[1].uv.Set(m_textureCoords.GetCorner(nzRectCorner_RightTop));

	m_vertices[2].color = m_color;
	m_vertices[2].position = m_transformMatrix.Transform(m_size.y*down);
	m_vertices[2].uv.Set(m_textureCoords.GetCorner(nzRectCorner_LeftBottom));

	m_vertices[3].color = m_color;
	m_vertices[3].position = m_transformMatrix.Transform(m_size.x*right + m_size.y*down);
	m_vertices[3].uv.Set(m_textureCoords.GetCorner(nzRectCorner_RightBottom));

	m_verticesUpdated = true;
}
