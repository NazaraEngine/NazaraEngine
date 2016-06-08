#include <Nazara/Graphics/RenderTechniques.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Graphics/AbstractRenderTechnique.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>

SCENARIO("RenderTechniques", "[GRAPHICS][RENDERTECHNIQUES]")
{
	GIVEN("Nothing")
	{
		WHEN("We try to get a technique")
		{
			THEN("We should get it")
			{
				std::unique_ptr<Nz::AbstractRenderTechnique> forwardByEnum(Nz::RenderTechniques::GetByEnum(Nz::RenderTechniqueType_BasicForward));
				REQUIRE(forwardByEnum->GetType() == Nz::RenderTechniqueType_BasicForward);

				std::unique_ptr<Nz::AbstractRenderTechnique> forwardByIndex(Nz::RenderTechniques::GetByIndex(1));
				REQUIRE(forwardByIndex->GetType() == Nz::RenderTechniqueType_BasicForward);

				std::unique_ptr<Nz::AbstractRenderTechnique> forwardByName(Nz::RenderTechniques::GetByName(Nz::RenderTechniques::ToString(Nz::RenderTechniqueType_BasicForward)));
				REQUIRE(forwardByName->GetType() == Nz::RenderTechniqueType_BasicForward);
			}

			THEN("We can register a render technique")
			{
				unsigned int previousCount = Nz::RenderTechniques::GetCount();

				Nz::RenderTechniques::Register("test", 23, []() -> Nz::AbstractRenderTechnique* {
					return new Nz::ForwardRenderTechnique;
				});

				REQUIRE(previousCount < Nz::RenderTechniques::GetCount());

				std::unique_ptr<Nz::AbstractRenderTechnique> forwardByRanking(Nz::RenderTechniques::GetByRanking(23));
				REQUIRE(forwardByRanking->GetType() == Nz::RenderTechniqueType_BasicForward);

				std::unique_ptr<Nz::AbstractRenderTechnique> forwardByName(Nz::RenderTechniques::GetByName("test"));
				REQUIRE(forwardByName->GetType() == Nz::RenderTechniqueType_BasicForward);

				Nz::RenderTechniques::Unregister("test");

				REQUIRE(previousCount == Nz::RenderTechniques::GetCount());
			}
		}
	}
}