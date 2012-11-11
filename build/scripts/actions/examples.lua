function examples()
	dofile("../examples/build/common.lua")
end

newaction
{
	trigger     = "examples",
	description = "Generate examples",
	execute     = examples
}
