// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp


#include <NDK/LuaAPI.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Lua/LuaClass.hpp>

namespace Ndk
{
	void LuaAPI::Register_Graphics(Nz::LuaInstance& instance)
	{
		Nz::LuaClass<Nz::InstancedRenderableRef> instancedRenderable("InstancedRenderable");

		Nz::LuaClass<Nz::ModelRef> modelClass("Model");
		modelClass.Inherit<Nz::InstancedRenderableRef>(instancedRenderable, [] (Nz::ModelRef* model) -> Nz::InstancedRenderableRef*
		{
			return reinterpret_cast<Nz::InstancedRenderableRef*>(model); //TODO: Make a ObjectRefCast
		});

		modelClass.SetConstructor([] (Nz::LuaInstance& lua) -> Nz::ModelRef*
		{
			return new Nz::ModelRef(new Nz::Model);
		});

		//modelClass.SetMethod("GetMaterial", &Nz::Model::GetMaterial);
		modelClass.SetMethod("GetMaterialCount", &Nz::Model::GetMaterialCount);
		//modelClass.SetMethod("GetMesh", &Nz::Model::GetMesh);
		modelClass.SetMethod("GetSkin", &Nz::Model::GetSkin);
		modelClass.SetMethod("GetSkinCount", &Nz::Model::GetSkinCount);

		modelClass.SetMethod("IsAnimated", &Nz::Model::IsAnimated);
		modelClass.SetMethod("LoadFromFile", &Nz::Model::LoadFromFile, Nz::ModelParameters());

		modelClass.SetMethod("Reset", &Nz::Model::Reset);

		//modelClass.SetMethod("SetMaterial", &Nz::Model::SetMaterial);
		//modelClass.SetMethod("SetMesh", &Nz::Model::SetMesh);
		//modelClass.SetMethod("SetSequence", &Nz::Model::SetSequence);
		modelClass.SetMethod("SetSkin", &Nz::Model::SetSkin);
		modelClass.SetMethod("SetSkinCount", &Nz::Model::SetSkinCount);

		instancedRenderable.Register(instance);
		modelClass.Register(instance);
	}
}