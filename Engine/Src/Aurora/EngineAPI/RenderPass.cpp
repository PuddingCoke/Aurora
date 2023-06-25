#include<Aurora/EngineAPI/RenderPass.h>

RenderPass::RenderPass() :
	context(new GraphicsContext())
{

}

RenderPass::~RenderPass()
{
	delete context;
}

std::future<ID3D11CommandList*> RenderPass::GetPassResult()
{
	return std::async(std::launch::async, [&]
		{
			recordCommand();

			return finishRecord();
		});
}

ID3D11CommandList* RenderPass::finishRecord()
{
	ID3D11CommandList* list;

	context->FinishCommandList(&list);

	context->ResetStates();

	return list;
}
