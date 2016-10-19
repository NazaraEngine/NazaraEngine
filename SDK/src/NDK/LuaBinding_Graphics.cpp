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
	}
}