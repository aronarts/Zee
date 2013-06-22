#include"Object.h"
#include"stdio.h"

// 由worldOrientation更新relativeOrientation
void Object::updateRelativeOrientation()
{ 
	if(NULL == parent)
	{
		relativeOrientation = worldOrientation;
	}
	else
	{
		relativeOrientation = parent->WorldRotationToLocal(parent->worldOrientation.Difference(worldOrientation));
	}
}

void Object::LookAt(Vector3 lookAtPos)
{
	// 使用DirectX函数得到矩阵，然后再转换为四元数
	D3DXMATRIX matrix;
	D3DXMatrixLookAtLH(&matrix, &(D3DXVECTOR3(worldPosition.x, worldPosition.y, worldPosition.z)), 
		&(D3DXVECTOR3(lookAtPos.x, lookAtPos.y, lookAtPos.z)), &(D3DXVECTOR3(0, 1, 0)));	// Up向量为(0, 1, 0)

	D3DXMatrixTranspose(&matrix, &matrix);
	worldOrientation = Quaternion(matrix);

	updateRelativeOrientation();
	updateLocalAxis();

	updateChildrenLocation();
}

D3DXMATRIX Object::LocalToWorldMatrix()
{
	D3DXMATRIX matScale, matRot, matTrans;
	D3DXMatrixScaling(&matScale, worldScale.x, worldScale.y, worldScale.z);
	D3DXMatrixTranslation(&matTrans, worldPosition.x, worldPosition.y, worldPosition.z);
	matRot = worldOrientation.Matrix();

	return matScale * matRot * matTrans;
}

// 在世界坐标系下以欧拉角(eulerX, eulerY, eulerZ)旋转
void Object::Rotate(float eulerX, float eulerY, float eulerZ)
{
	Quaternion worldRotation(eulerX, eulerY, eulerZ);
	rotate(worldRotation);
}

// 在本地坐标系下以欧拉角(eulerX, eulerY, eulerZ)旋转
// 注意这里的local实际指的是object space
void Object::RotateLocal(float eulerX, float eulerY, float eulerZ)
{
	Quaternion localRotation(eulerX, eulerY, eulerZ);
	rotateLocal(localRotation);
}

void Object::rotate(Quaternion worldRotation)
{
	worldOrientation = worldRotation * worldOrientation;		// 四元数是左乘

	updateRelativeOrientation();
	updateLocalAxis();

	updateChildrenLocation();
}

void Object::rotateLocal(Quaternion localRotation)
{
	Quaternion worldRotation = LocalRotationToWorld(localRotation);
	rotate(worldRotation);
}

Quaternion Object::LocalRotationToWorld(Quaternion localRotation)
{
	// 将本地旋转轴变换为世界旋转轴，得到世界下的旋转四元数，再乘以之前的旋转四元数即可
	if(localRotation.w == 1)
		return localRotation;

	float s = sqrt(1 - localRotation.w * localRotation.w);

	Vector3 localRotAxis = (1 / s) * Vector3(localRotation.x, localRotation.y, localRotation.z);
	Vector3 worldRotAxis = LocalVectorToWorld(localRotAxis);

	Quaternion worldRotation = Quaternion(localRotation.w,
		worldRotAxis.x * s,
		worldRotAxis.y * s,
		worldRotAxis.z * s);

	return worldRotation;
}

Quaternion Object::WorldRotationToLocal(Quaternion worldRotation)
{
	if(worldRotation.w == 1)
		return worldRotation;

	float s = sqrt(1 - worldRotation.w * worldRotation.w);

	Vector3 worldRotAxis = (1 / s) * Vector3(worldRotation.x, worldRotation.y, worldRotation.z);
	Vector3 localRotAxis = WorldVectorToLocal(worldRotAxis);

	Quaternion localRotation = Quaternion(worldRotation.w,
		localRotAxis.x * s,
		localRotAxis.y * s,
		localRotAxis.z * s);

	return localRotation;
}

// 此方法指的是object space下的向量(而非local space(parent space)下的向量)转到world space
Vector3 Object::LocalVectorToWorld(Vector3 localVec)
{
	return localVec * worldOrientation;
}

Vector3 Object::WorldVectorToLocal(Vector3 worldVec)
{
	return worldVec * worldOrientation.Conjugate();
}

void Object::Translate(Vector3 moveVector)
{
	worldPosition += moveVector;

	updateRelativePostion();
	updateChildrenLocation();
}

void Object::Translate(float x, float y, float z)
{
	Translate(Vector3(x, y, z));
}

void Object::TranslateLocal(Vector3 moveVector)
{
	moveVector = LocalVectorToWorld(moveVector);
	Translate(moveVector);
}

void Object::TranslateLocal(float x, float y, float z)
{
	TranslateLocal(Vector3(x, y, z));
}

// 由localPosition更新worldPosition
void Object::updateWorldPosition()
{
	if(NULL == parent)
	{
		worldPosition = relativePosition;
	}
	else
	{
		worldPosition = parent->worldPosition + parent->LocalVectorToWorld(relativePosition);
	}
}

// 由localOrientation更新worldOrientation
void Object::updateWorldOrientation()
{
	if(NULL == parent)
	{
		worldOrientation = relativeOrientation;
	}
	else
	{
		worldOrientation = parent->LocalRotationToWorld(relativeOrientation) * parent->worldOrientation;
	}
}

void Object::updateWorldScale()
{
	if(NULL == parent)
	{
		worldScale = relativeScale;
	}
	else
	{
		// TODO:此scale必然有问题, 因为坐标系不同, sclae轴不一样
		worldScale.x = parent->worldScale.x * relativeScale.x; 
		worldScale.y = parent->worldScale.y * relativeScale.y; 
		worldScale.z = parent->worldScale.z * relativeScale.z; 
	}
}

// 由worldRotation更新本地方向轴向量
void Object::updateLocalAxis()
{
	worldRightVector = Vector3(1, 0, 0);
	worldUpVector = Vector3(0, 1, 0);
	worldForwardVector = Vector3(0, 0, 1);

	worldRightVector = worldRightVector * worldOrientation;
	worldUpVector = worldUpVector * worldOrientation;
	worldForwardVector = worldForwardVector * worldOrientation;
}

// 由worldPosition更新relativePosition
void Object::updateRelativePostion()
{
	if(NULL == parent)
	{
		relativePosition = worldPosition;
	}
	else
	{
		relativePosition = parent->WorldVectorToLocal(worldPosition - parent->worldPosition);
	}
}

void Object::updateRelativeScale()
{
	if(NULL == parent)
	{
		relativeScale = worldScale;
	}
	else
	{
		_Assert(parent->worldScale.x != 0 && parent->worldScale.y != 0 && parent->worldScale.z != 0);

		relativeScale.x = worldScale.x / parent->worldScale.x;
		relativeScale.y = worldScale.y / parent->worldScale.y;
		relativeScale.z = worldScale.z / parent->worldScale.z;
	}
}

void Object::updateChildrenLocation()
{
	for(std::list<Object*>::iterator iter = children.begin(); iter != children.end(); ++iter)
	{
		(*iter)->updateWorldPosition();
		(*iter)->updateWorldOrientation();
		(*iter)->updateLocalAxis();
	}
}
