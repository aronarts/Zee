#include"Object.h"

void Object::AddChild(Object* child)
{
	_Assert(NULL != child);

	if(std::find(mChildren.begin(), mChildren.end(), child) != mChildren.end())
	{
		// children中已存在此对象指针
		return;
	}
	else
	{
		mChildren.push_back(child);

		child->mParent = this;
		child->updateRelativePostion();
		child->updateRelativeOrientation();
	}
}

void Object::SetParent(Object* parent)
{
	if(NULL == parent)
	{
		Detach();
	}
	else if(parent != mParent)
	{
		Detach();
		parent->AddChild(this);
	}
}

void Object::Detach()
{
	if(NULL == mParent)
	{
		return;
	}
	else
	{
		std::list<Object*>::iterator iter = std::find(mParent->mChildren.begin(), mParent->mChildren.end(), this);
		_Assert(iter != mParent->mChildren.end());

		iter = mParent->mChildren.erase(iter);
		mParent = NULL;		// TODO:和SceneManager一起工作,detach之后从属于root?

		mRelativePos = mWorldPos;
		mRelativeOrient = mWorldOrient;
	}
}

void Object::RemoveChild(Object* child)
{
	_Assert(NULL != child);
	_Assert(std::find(mChildren.begin(), mChildren.end(), child) != mChildren.end());

	if(std::find(mChildren.begin(), mChildren.end(), child) == mChildren.end())
	{
		return;
	}
	else 
	{			
		child->Detach();
	}
}

// 由worldOrientation更新relativeOrientation
void Object::updateRelativeOrientation()
{ 
	if(NULL == mParent)
	{
		mRelativeOrient = mWorldOrient;
	}
	else
	{
		mRelativeOrient = mParent->WorldRotationToLocal(mParent->mWorldOrient.Difference(mWorldOrient));
	}
}

void Object::LookAt(const Vector3& lookAtPos)
{
	// 使用DirectX函数得到矩阵，然后再转换为四元数
	D3DXMATRIX matrix;
	D3DXMatrixLookAtLH(&matrix, &(D3DXVECTOR3(mWorldPos.x, mWorldPos.y, mWorldPos.z)), 
		&(D3DXVECTOR3(lookAtPos.x, lookAtPos.y, lookAtPos.z)), &(D3DXVECTOR3(0, 1, 0)));	// Up向量为(0, 1, 0)

	D3DXMatrixTranspose(&matrix, &matrix);
	mWorldOrient = Quaternion(matrix);

	updateRelativeOrientation();
	updateLocalAxis();

	OnTransformChanged();
}

D3DXMATRIX Object::LocalToWorldMatrix()
{
	D3DXMATRIX matScale, matRot, matTrans;
	D3DXMatrixScaling(&matScale, mScale.x, mScale.y, mScale.z);
	D3DXMatrixTranslation(&matTrans, mWorldPos.x, mWorldPos.y, mWorldPos.z);
	matRot = mWorldOrient.Matrix();

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
	mWorldOrient = worldRotation * mWorldOrient;	// 四元数左乘

	updateRelativeOrientation();
	updateLocalAxis();

	OnTransformChanged();
}

void Object::rotateLocal(Quaternion localRotation)
{
	Quaternion worldRotation = LocalRotationToWorld(localRotation);
	rotate(worldRotation);
}

Quaternion Object::LocalRotationToWorld(const Quaternion& localRotation)
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

Quaternion Object::WorldRotationToLocal(const Quaternion& worldRotation)
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
Vector3 Object::LocalVectorToWorld(const Vector3& localVec)
{
	return localVec * mWorldOrient;
}

Vector3 Object::WorldVectorToLocal(const Vector3& worldVec)
{
	return worldVec * mWorldOrient.Conjugate();
}

void Object::Translate(const Vector3& moveVector)
{
	mWorldPos += moveVector;

	updateRelativePostion();

	OnTransformChanged();
}

void Object::Translate(float x, float y, float z)
{
	Translate(Vector3(x, y, z));
}

void Object::TranslateLocal(const Vector3& moveVector)
{
	Vector3 worldMoveVector = LocalVectorToWorld(moveVector);
	Translate(worldMoveVector);
}

void Object::TranslateLocal(float x, float y, float z)
{
	TranslateLocal(Vector3(x, y, z));
}

// 由localPosition更新worldPosition
void Object::updateWorldPosition()
{
	if(NULL == mParent)
	{
		mWorldPos = mRelativePos;
	}
	else
	{
		mWorldPos = mParent->mWorldPos + mParent->LocalVectorToWorld(mRelativePos);
	}

	OnTransformChanged();
}

// 由localOrientation更新worldOrientation
void Object::updateWorldOrientation()
{
	if(NULL == mParent)
	{
		mWorldOrient = mRelativeOrient;
	}
	else
	{
		mWorldOrient = mParent->LocalRotationToWorld(mRelativeOrient) * mParent->mWorldOrient;
	}

	OnTransformChanged();
}

// 由worldRotation更新本地方向轴向量
void Object::updateLocalAxis()
{
	mWorldRight = Vector3(1, 0, 0);
	mWorldUp = Vector3(0, 1, 0);
	mWorldForward = Vector3(0, 0, 1);

	mWorldRight = mWorldRight * mWorldOrient;
	mWorldUp = mWorldUp * mWorldOrient;
	mWorldForward = mWorldForward * mWorldOrient;
}

// 由worldPosition更新relativePosition
void Object::updateRelativePostion()
{
	if(NULL == mParent)
	{
		mRelativePos = mWorldPos;
	}
	else
	{
		mRelativePos = mParent->WorldVectorToLocal(mWorldPos - mParent->mWorldPos);
	}
}

void Object::updateChildrenTransform()
 {
	for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		(*iter)->updateWorldPosition();
		(*iter)->updateWorldOrientation();
		(*iter)->updateLocalAxis();
	}
}

void Object::scaleChildren(const Vector3& scale)
{
	for(std::list<Object*>::iterator iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		(*iter)->Scale(scale);
	}
}

void Object::SetRelativePosition(const Vector3& pos)
{
	mRelativePos = pos;
	updateWorldPosition();

	OnTransformChanged();
}

void Object::SetRelativePosition(float x, float y, float z)
{
	SetRelativePosition(Vector3(x, y, z));
}

Vector3 Object::GetRelativePosition()
{
	return mRelativePos;
}

void Object::SetWorldPosition(const Vector3& pos)
{
	mWorldPos = pos;
	updateRelativePostion();

	OnTransformChanged();
}

void Object::SetWorldPosition(float x, float y, float z)
{
	SetWorldPosition(Vector3(x, y, z));
}

Vector3 Object::GetWorldPosition()
{
	return mWorldPos;
}

Quaternion Object::GetWorldOrient()
{
	return mWorldOrient;
}

Vector3 Object::GetWorldPosition(const Vector3& localMoveVector)
{
	Vector3 resultPos = mWorldPos;
	resultPos += localMoveVector.x * mWorldRight.Normalized();
	resultPos += localMoveVector.y * mWorldUp.Normalized();
	resultPos += localMoveVector.z * mWorldForward.Normalized();

	return resultPos;
}

void Object::SetRelativeOrientation(float eulerX, float eulerY, float eulerZ)
{
	mRelativeOrient = Quaternion(eulerX, eulerY, eulerZ);
	updateWorldOrientation();
	updateLocalAxis();

	OnTransformChanged();
}

void Object::SetWorldOrientation(Quaternion orient)
{
	mWorldOrient = orient;
	updateRelativeOrientation();
	updateLocalAxis();

	OnTransformChanged();
}

void Object::SetWorldOrientation(float eulerX, float eulerY, float eulerZ)
{
	SetWorldOrientation(Quaternion(eulerX, eulerY, eulerZ));
}

Vector3 Object::GetWorldRight()
{
	return mWorldRight;
}

Vector3 Object::GetWorldUp()
{
	return mWorldUp;
}

Vector3 Object::GetWorldForward()
{
	return mWorldForward;
}

void Object::Scale(const Vector3& scale)
{
	_Assert(scale.x != 0 && scale.y !=0 && scale.z != 0);

	mScale.x *= scale.x;
	mScale.y *= scale.y;
	mScale.z *= scale.z;

	scaleChildren(scale);
	OnTransformChanged();
}

void Object::Scale(float x, float y, float z)
{
	Scale(Vector3(x, y, z));
}

void Object::SetScale(const Vector3& scale)
{
	//_Assert(scale.x != 0 && scale.y !=0 && scale.z != 0);

	Vector3 oldScale = mScale;
	_Assert(oldScale.x != 0 && oldScale.y != 0 && oldScale.z != 0);

	mScale = scale;
	scaleChildren(Vector3(mScale.x / oldScale.x, mScale.y / oldScale.y, mScale.z / oldScale.z));

	OnTransformChanged();
}

void Object::SetScale(float x, float y, float z)
{
	SetScale(Vector3(x, y, z));
}

Object::ObjectList Object::GetChildren()
{
	return mChildren;
}

// 直接返回mChildern会返回一个list的拷贝, 影响效率, 所以直接用指针
void Object::GetChildrenV2(ObjectList* &children)
{
	children = &mChildren;
}

Vector3 Object::GetScale()
{
	return mScale;
}

void Object::OnTransformChanged()		// TODO:改为一个标记, 在FrameUpdate时有标记则调用, 否则可能被多次调用
{
	updateChildrenTransform();
}

Object* Object::GetParent()
{
	return mParent;
}
