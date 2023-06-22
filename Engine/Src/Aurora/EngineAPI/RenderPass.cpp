#include<Aurora/EngineAPI/RenderPass.h>

RenderPass::RenderPass():
	context(new GraphicsContext())
{

}

RenderPass::~RenderPass()
{
	delete context;
}

void RenderPass::finishRecord()
{
	context->UnbindRTV();
	context->UnbindPSUAV();
	context->UnbindCSUAV();
	context->FinishCommandList(&commandList);
}

ID3D11CommandList* RenderPass::getCommandList() const
{
	return commandList.Get();
}
