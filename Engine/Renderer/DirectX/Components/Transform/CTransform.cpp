
#include "CTransform.h"


Engine::CTransform::CTransform()
{

}

Engine::CTransform::CTransform(fVect3 _position, fVect3 _rotation, float _angle, float _scale)
	:transform(), 
	position(_position.x, _position.y, _position.z, 1.0f), 
	scale(_scale, _scale, _scale, 0.0f), 
	rotation(_rotation.x, _rotation.y, _rotation.z, _angle)
{
	transform = DirectX::XMMatrixAffineTransformation(
		DirectX::XMLoadFloat4(&scale),
		DirectX::XMVectorZero(),
		DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, 1.0f), rotation.w),
		DirectX::XMLoadFloat4(&position)
	);
}

Engine::CTransform::~CTransform()
{

}

Matrix& Engine::CTransform::Update()
{
	
	transform = DirectX::XMMatrixAffineTransformation(
		DirectX::XMLoadFloat4(&scale), 
		DirectX::XMVectorZero(), 
		DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, 1.0f), rotation.w), 
		DirectX::XMLoadFloat4(&position)
	);
	return transform;
}

Matrix& Engine::CTransform::GetTransform()
{
	return transform;
}

void Engine::CTransform::SetPosition(float _x, float _y, float _z)
{
	position = { _x, _y, _z, 1.0f };
	
}

void Engine::CTransform::SetScale(float _x, float _y, float _z)
{
	scale = { _x, _y, _z, 0.0f};
}

void Engine::CTransform::SetRotation(float _x, float _y, float _z, float _angle)
{
	rotation = { _x, _y, _z, _angle };
}

void Engine::CTransform::SetScale(float _scale)
{
	scale = { _scale, _scale, _scale, 0.0f };
}
