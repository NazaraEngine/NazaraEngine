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
		model.Inherit<Nz::InstancedRenderableRef>(instancedRenderable, [] (Nz::ModelRef* model) -> Nz::InstancedRenderableRef*
		{
			return reinterpret_cast<Nz::InstancedRenderableRef*>(model); //TODO: Make a ObjectRefCast
		});

		model.SetConstructor([] (Nz::LuaInstance& /*lua*/, Nz::ModelRef* model, std::size_t /*argumentCount*/)
		{
			Nz::PlacementNew(model, Nz::Model::New());
			return true;
		});

		//modelClass.SetMethod("GetMaterial", &Nz::Model::GetMaterial);
		model.BindMethod("GetMaterialCount", &Nz::Model::GetMaterialCount);
		//modelClass.SetMethod("GetMesh", &Nz::Model::GetMesh);
		model.BindMethod("GetSkin", &Nz::Model::GetSkin);
		model.BindMethod("GetSkinCount", &Nz::Model::GetSkinCount);

		model.BindMethod("IsAnimated", &Nz::Model::IsAnimated);
		model.BindMethod("LoadFromFile", &Nz::Model::LoadFromFile, Nz::ModelParameters());

		model.BindMethod("Reset", &Nz::Model::Reset);

		//modelClass.SetMethod("SetMaterial", &Nz::Model::SetMaterial);
		//modelClass.SetMethod("SetMesh", &Nz::Model::SetMesh);
		//modelClass.SetMethod("SetSequence", &Nz::Model::SetSequence);
		model.BindMethod("SetSkin", &Nz::Model::SetSkin);
		model.BindMethod("SetSkinCount", &Nz::Model::SetSkinCount);

		/*********************************** Nz::Sprite ***********************************/
		sprite.Inherit<Nz::InstancedRenderableRef>(instancedRenderable, [] (Nz::SpriteRef* sprite) -> Nz::InstancedRenderableRef*
		{
			return reinterpret_cast<Nz::InstancedRenderableRef*>(sprite); //TODO: Make a ObjectRefCast
		});

		sprite.SetConstructor([] (Nz::LuaInstance& /*lua*/, Nz::SpriteRef* sprite, std::size_t /*argumentCount*/)
		{
			Nz::PlacementNew(sprite, Nz::Sprite::New());
			return true;
		});

		sprite.BindMethod("GetColor", &Nz::Sprite::GetColor);
		sprite.BindMethod("GetCornerColor", &Nz::Sprite::GetCornerColor);
		//spriteClass.BindMethod("GetMaterial", &Nz::Sprite::GetMaterial);
		sprite.BindMethod("GetOrigin", &Nz::Sprite::GetOrigin);
		sprite.BindMethod("GetSize", &Nz::Sprite::GetSize);
		sprite.BindMethod("GetTextureCoords", &Nz::Sprite::GetTextureCoords);

		sprite.BindMethod("SetColor", &Nz::Sprite::SetColor);
		sprite.BindMethod("SetCornerColor", &Nz::Sprite::SetCornerColor);
		sprite.BindMethod("SetDefaultMaterial", &Nz::Sprite::SetDefaultMaterial);
		//spriteClass.BindMethod("SetMaterial", &Nz::Sprite::SetMaterial, true);
		sprite.BindMethod("SetOrigin", &Nz::Sprite::SetOrigin);
		sprite.BindMethod("SetSize", (void(Nz::Sprite::*)(const Nz::Vector2f&)) &Nz::Sprite::SetSize);
		sprite.BindMethod("SetTexture", &Nz::Sprite::SetTexture, true);
		sprite.BindMethod("SetTextureCoords", &Nz::Sprite::SetTextureCoords);
		sprite.BindMethod("SetTextureRect", &Nz::Sprite::SetTextureRect);
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Graphics classes
	*/

	void LuaBinding::RegisterGraphics(Nz::LuaInstance& instance)
	{
		instancedRenderable.Register(instance);
		model.Register(instance);
		sprite.Register(instance);
	}
}