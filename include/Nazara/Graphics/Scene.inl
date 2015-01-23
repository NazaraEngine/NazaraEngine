// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Déplacer vers SceneNode et exposer

// Pour être sûr que ce code soit à jour
static_assert(nzSceneNodeType_Max == 6, "Please update the code below");

class NzLight;
class NzModel;
class NzParticleEmitter;
class NzSceneRoot;
class NzSprite;
class NzTextSprite;

template<typename T>
constexpr nzSceneNodeType NzImplGetType()
{
	return nzSceneNodeType_User;
}

template<>
inline constexpr nzSceneNodeType NzImplGetType<NzLight>()
{
	return nzSceneNodeType_Light;
}

template<>
inline constexpr nzSceneNodeType NzImplGetType<NzModel>()
{
	return nzSceneNodeType_Model;
}

template<>
inline constexpr nzSceneNodeType NzImplGetType<NzParticleEmitter>()
{
	return nzSceneNodeType_ParticleEmitter;
}

template<>
inline constexpr nzSceneNodeType NzImplGetType<NzSceneRoot>()
{
	return nzSceneNodeType_Root;
}

template<>
inline constexpr nzSceneNodeType NzImplGetType<NzSprite>()
{
	return nzSceneNodeType_Sprite;
}

template<>
inline constexpr nzSceneNodeType NzImplGetType<NzTextSprite>()
{
	return nzSceneNodeType_TextSprite;
}


template<typename T, typename... Args>
T* NzScene::CreateNode(Args&&... args)
{
	std::unique_ptr<T> node(new T(std::forward<Args>(args)...));
	if (!RegisterSceneNode(NzString(), node.get()))
		return nullptr;

	return node.release();
}

template<typename T, typename... Args>
T* NzScene::CreateNode(const NzString& name, Args&&... args)
{
	std::unique_ptr<T> node(new T(std::forward<Args>(args)...));
	if (!RegisterSceneNode(name, node.get()))
		return nullptr;

	return node.release();
}

template<typename T, typename... Args>
T* NzScene::CreateNode(const NzString& name, const NzString& templateNodeName)
{
	auto it = m_nodeMap.find(templateNodeName);
	if (it == m_nodeMap.end())
	{
		NazaraError("Node \"" + templateNodeName + "\" is not registred");
		return nullptr;
	}

	NzSceneNode* sceneNode = it->second;
	if (NzImplGetType<T>() != sceneNode->GetSceneNodeType())
	{
		NazaraError("Scene node type of T (" + NzString::Number(NzImplGetType<T>()) + ") doesn't match template scene node type (" + NzString::Number(sceneNode->GetSceneNodeType()) + ")");
		return nullptr;
	}

	std::unique_ptr<T> node(static_cast<T*>(sceneNode)->Copy());
	if (!RegisterSceneNode(name, node.get()))
		return nullptr;

	return node.release();
}

#include <Nazara/Graphics/DebugOff.hpp>
