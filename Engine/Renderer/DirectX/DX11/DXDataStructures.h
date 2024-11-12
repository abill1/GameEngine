
#ifndef ENGINE_RENDERER_DIRECTX_DX11_DXDATASTRUCTURES_H
#define ENGINE_RENDERER_DIRECTX_DX11_DXDATASTRUCTURES_H

namespace Engine
{

	struct Vertex
	{
		fVect position;
		fVect color;
		fVect2 texcoord;
	};

	struct TriangleIndex
	{
		unsigned int a;
		unsigned int b;
		unsigned int c;
	};
}

#endif // ENGINE_RENDERER_DIRECTX_DX11_DXDATASTRUCTURES_H


