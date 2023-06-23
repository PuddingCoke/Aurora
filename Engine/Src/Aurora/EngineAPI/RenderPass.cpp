#include<Aurora/EngineAPI/RenderPass.h>

RenderPass::RenderPass() :
	context(new GraphicsContext())
{

}

RenderPass::~RenderPass()
{
	delete context;
}

ID3D11CommandList* RenderPass::finishRecord()
{
	ID3D11CommandList* list;

	context->FinishCommandList(&list);

	context->ResetStates();

	return list;
}
