#include <Nazara/Graphics/SkeletalModel.hpp>
#include <Catch/catch.hpp>

SCENARIO("SkeletalModel", "[GRAPHICS][SKELETALMODEL]")
{
	GIVEN("A default skeletal model")
	{
		WHEN("We can load the bob lamp")
		{
			Nz::AnimationRef animation = Nz::Animation::LoadFromFile("resources/Engine/Graphics/Bob lamp/bob_lamp_update.md5anim");
			Nz::SkeletalModelRef skeletalModel(static_cast<Nz::SkeletalModel*>(Nz::SkeletalModel::LoadFromFile("resources/Engine/Graphics/Bob lamp/bob_lamp_update.md5mesh").Get()));
			REQUIRE(skeletalModel);
			REQUIRE(animation);
			skeletalModel->SetAnimation(animation);

			THEN("We can enable its animation")
			{
				REQUIRE(skeletalModel->HasAnimation());
				skeletalModel->EnableAnimation(true);
				skeletalModel->AdvanceAnimation(0.10f);
				REQUIRE(skeletalModel->IsAnimationEnabled());
			}
		}
	}
}
