// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/LuaBinding.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	/*!
	* \brief Binds Graphics module to Lua
	*/

	void LuaBinding::BindGraphics()
	{
		/*********************************** Nz::InstancedRenderable ***********************************/

		/*********************************** Nz::Model ***********************************/
		modelClass.Inherit<Nz::InstancedRenderableRef>(instancedRenderable, [] (Nz::ModelRef* model) -> Nz::InstancedRenderableRef*
		{
			return reinterpret_cast<Nz::InstancedRenderableRef*>(model); //TODO: Make a ObjectRefCast
		});

		modelClass.SetConstructor([] (Nz::LuaInstance& /*lua*/, Nz::ModelRef* model, std::size_t /*argumentCount*/)
		{
			Nz::PlacementNew(model, Nz::Model::New());
			return true;
		});

		//modelClass.SetMethod("GetMaterial", &Nz::Model::GetMaterial);
		modelClass.BindMethod("GetMaterialCount", &Nz::Model::GetMaterialCount);
		//modelClass.SetMethod("GetMesh", &Nz::Model::GetMesh);
		modelClass.BindMethod("GetSkin", &Nz::Model::GetSkin);
		modelClass.BindMethod("GetSkinCount", &Nz::Model::GetSkinCount);

		modelClass.BindMethod("IsAnimated", &Nz::Model::IsAnimated);
		modelClass.BindMethod("LoadFromFile", &Nz::Model::LoadFromFile, Nz::ModelParameters());

		modelClass.BindMethod("Reset", &Nz::Model::Reset);

		//modelClass.SetMethod("SetMaterial", &Nz::Model::SetMaterial);
		//modelClass.SetMethod("SetMesh", &Nz::Model::SetMesh);
		//modelClass.SetMethod("SetSequence", &Nz::Model::SetSequence);
		modelClass.BindMethod("SetSkin", &Nz::Model::SetSkin);
		modelClass.BindMethod("SetSkinCount", &Nz::Model::SetSkinCount);

		/*********************************** Nz::Sprite ***********************************/
		spriteClass.Inherit<Nz::InstancedRenderableRef>(instancedRenderable, [] (Nz::SpriteRef* sprite) -> Nz::InstancedRenderableRef*
		{
			return reinterpret_cast<Nz::InstancedRenderableRef*>(sprite); //TODO: Make a ObjectRefCast
		});

		spriteClass.SetConstructor([] (Nz::LuaInstance& /*lua*/, Nz::SpriteRef* sprite, std::size_t /*argumentCount*/)
		{
			Nz::PlacementNew(sprite, Nz::Sprite::New());
			return true;
		});

		spriteClass.BindMethod("GetColor", &Nz::Sprite::GetColor);
		spriteClass.BindMethod("GetCornerColor", &Nz::Sprite::GetCornerColor);
		//spriteClass.BindMethod("GetMaterial", &Nz::Sprite::GetMaterial);
		spriteClass.BindMethod("GetOrigin", &Nz::Sprite::GetOrigin);
		spriteClass.BindMethod("GetSize", &Nz::Sprite::GetSize);
		spriteClass.BindMethod("GetTextureCoords", &Nz::Sprite::GetTextureCoords);

		spriteClass.BindMethod("SetColor", &Nz::Sprite::SetColor);
		spriteClass.BindMethod("SetCornerColor", &Nz::Sprite::SetCornerColor);
		spriteClass.BindMethod("SetDefaultMaterial", &Nz::Sprite::SetDefaultMaterial);
		//spriteClass.BindMethod("SetMaterial", &Nz::Sprite::SetMaterial, true);
		spriteClass.BindMethod("SetOrigin", &Nz::Sprite::SetOrigin);
		spriteClass.BindMethod("SetSize", (void(Nz::Sprite::*)(const Nz::Vector2f&)) &Nz::Sprite::SetSize);
		spriteClass.BindMethod("SetTexture", &Nz::Sprite::SetTexture, true);
		spriteClass.BindMethod("SetTextureCoords", &Nz::Sprite::SetTextureCoords);
		spriteClass.BindMethod("SetTextureRect", &Nz::Sprite::SetTextureRect);
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Graphics classes
	*/

	void LuaBinding::RegisterGraphics(Nz::LuaInstance& instance)
	{
		instancedRenderable.Register(instance);
		modelClass.Register(instance);
		spriteClass.Register(instance);
	}
}