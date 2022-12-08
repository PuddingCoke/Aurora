#pragma once

#ifndef _STRUCTUREDBUFFER_H_
#define _STRUCTUREDBUFFER_H_

#include"DX/Resource/Buffer.h"
#include"DX/View/ShaderResourceView.h"

class StructuredBuffer :public Buffer, public ShaderResourceView
{

};

#endif // !_STRUCTUREDBUFFER_H_
