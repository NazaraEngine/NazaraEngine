#include <ShaderNode/DataModels/FragmentOutput.hpp>

inline FragmentOutput::FragmentOutput(ShaderGraph& graph) :
ShaderNode(graph)
{
	SetPreviewSize({ 128, 128 });
	DisableCustomVariableName();
	EnablePreview(true);
}
