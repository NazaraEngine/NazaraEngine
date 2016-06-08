#include <Nazara/Graphics/DeferredRenderTechnique.hpp>
#include <Catch/catch.hpp>

SCENARIO("DeferredRenderTechnique", "[GRAPHICS][DEFERREDRENDERTECHNIQUE]")
{
	GIVEN("A default deferred render technique")
	{
		Nz::DeferredRenderTechnique deferredRenderTechnique;

		WHEN("We can disable a pass")
		{
			REQUIRE(deferredRenderTechnique.IsPassEnabled(Nz::RenderPassType::RenderPassType_AA, 0));
			deferredRenderTechnique.EnablePass(Nz::RenderPassType::RenderPassType_AA, 0, false);

			THEN("It is disabled")
			{
				REQUIRE(!deferredRenderTechnique.IsPassEnabled(Nz::RenderPassType::RenderPassType_AA, 0));
			}

			AND_THEN("We reset it, it is disabled and not the same as the old one")
			{
				Nz::DeferredRenderPass* oldPass = deferredRenderTechnique.GetPass(Nz::RenderPassType::RenderPassType_AA, 0);
				deferredRenderTechnique.ResetPass(Nz::RenderPassType::RenderPassType_AA, 0);
				REQUIRE(!deferredRenderTechnique.IsPassEnabled(Nz::RenderPassType::RenderPassType_AA, 0));
				REQUIRE(deferredRenderTechnique.GetPass(Nz::RenderPassType::RenderPassType_AA, 0) != oldPass);
			}
		}
	}
}
