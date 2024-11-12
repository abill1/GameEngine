
#ifndef ENGINE_RENDERER_DIRECTX_COMPONENTS_TRANSFORM_H
#define ENGINE_RENDERER_DIRECTX_COMPONENTS_TRANSFORM_H

namespace Engine
{
	class CTransform
	{
	public:
		CTransform();
		//CTransform(fVect3 _position, fVect3 _scale, fVect3 _rotation, float _angle);
		CTransform(fVect3 _position, fVect3 _rotation, float _angle, float _scale);
		~CTransform();

		Matrix& Update();
		Matrix& GetTransform();
		void SetPosition(float _x, float _y, float _z);
		void SetScale(float _x, float _y, float _z);
		void SetScale(float _scale);
		void SetRotation(float _x, float _y, float _z, float _angle);

	private:
		Matrix transform;
		fVect position;
		fVect scale;
		fVect rotation;
	};
}

#endif // CTransform_h__

