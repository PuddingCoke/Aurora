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
	context->FinishCommandList(&commandList);
	context->getStates()->resetStates();
}

ID3D11CommandList* RenderPass::getCommandList() const
{
	return commandList.Get();
}
