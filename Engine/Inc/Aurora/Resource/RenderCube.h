#pragma once

#ifndef _RENDERCUBE_H_
#define _RENDERCUBE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/Composition/RSView.h>

class RenderCube :public Texture2D, public RSView
{
public:

	RenderCube(const UINT& resolution, const FMT& fmt, const float color[4] = DirectX::Colors::Black);

	~RenderCube();

};

#endif // !_RENDERCUBE_H_
