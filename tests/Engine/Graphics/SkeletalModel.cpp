#include <Nazara/Graphics/SkeletalModel.hpp>
#include <Catch/catch.hpp>

SCENARIO("SkeletalModel", "[GRAPHICS][SKELETALMODEL]")
{
	GIVEN("A default skeletal model")
	{
		Nz::SkeletalModel skeletalModel;
		Nz::AnimationRef animation = Nz::Animation::New();

		WHEN("We can load the bob lamp")
		{
			REQUIRE(skeletalModel.LoadFromFile("resources/Engine/Graphics/Bob lamp/bob_lamp_update.md5mesh"));
			REQUIRE(animation->LoadFromFile("resources/Engine/Graphics/Bob lamp/bob_lamp_update.md5anim"));
			skeletalModel.SetAnimation(animation);

			THEN("We can enable its animation")
			{
				REQUIRE(skeletalModel.HasAnimation());
				skeletalModel.EnableAnimation(true);
				skeletalModel.AdvanceAnimation(0.10f);
				REQUIRE(skeletalModel.IsAnimationEnabled());
			}
		}
	}
}
