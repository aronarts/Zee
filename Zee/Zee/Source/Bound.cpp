#include"Bound.h"
#include "Common.h"

AABBox AABBox::Invalid(Vector3(FLT_MAX, FLT_MAX, FLT_MAX), Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX));

AABBox::AABBox(const Vector3& min /*= Vector3(FLT_MAX, FLT_MAX, FLT_MAX)*/, 
			   const Vector3& max /*= Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX)*/)
{
	mMin = min;
	mMax = max;
}

AABBox::AABBox(const Vector3& center, float width, float height, float length)
{
	Vector3 size(width, height, length);

	mMax = center + 0.5f * size;
	mMin = center - 0.5f * size;
}

Vector3 AABBox::GetVertex(int ix)
{
	Vector3 vert;
	switch(ix){
	case 0:
		vert = mMin;
		break;
	case 1:
		vert = Vector3(mMin.x, mMin.y, mMax.z);
		break;
	case 2:
		vert = Vector3(mMax.x, mMin.y, mMax.z);
		break;
	case 3:
		vert = Vector3(mMax.x, mMin.y, mMin.z);
		break;
	case 4:
		vert = Vector3(mMin.x, mMax.y, mMin.z);
		break;
	case 5:
		vert = Vector3(mMin.x, mMax.y, mMax.z);
		break;
	case 6:
		vert = mMax;
		break;
	case 7:
		vert = Vector3(mMax.x, mMax.y, mMin.z);
		break;
	}

	return vert;
}

bool AABBox::isPointInside(Vector3 vert)
{
	if(vert.x > mMin.x && vert.x < mMax.x &&
		vert.y > mMin.y && vert.y < mMax.y &&
		vert.z > mMin.z && vert.z < mMax.z)
		return true;
	else 
		return false;
}

AABBox AABBox::CombineBBox(const AABBox& box1, const AABBox& box2)
{
	AABBox resultBox = box1;

	if(box2.mMin.x < resultBox.mMin.x)
		resultBox.mMin.x = box2.mMin.x;

	if(box2.mMin.y < resultBox.mMin.y)
		resultBox.mMin.y = box2.mMin.y;

	if(box2.mMin.z < resultBox.mMin.z)
		resultBox.mMin.z = box2.mMin.z;

	if(box2.mMax.x > resultBox.mMax.x)
		resultBox.mMax.x = box2.mMax.x;

	if(box2.mMax.y > resultBox.mMax.y)
		resultBox.mMax.y = box2.mMax.y;

	if(box2.mMax.z > resultBox.mMax.z)
		resultBox.mMax.z = box2.mMax.z;

	return resultBox;
}

bool AABBox::isValid()
{
	if(mMin.x > mMax.x)
		return false;

	if(mMin.y > mMax.y)
		return false;

	if(mMin.z > mMax.z)
		return false;

	return true;
}

Vector3 AABBox::GetCenter()
{
	_Assert(isValid());

	return 0.5f * (mMin + mMax);
}

Ray::Ray(const Vector3& pos /*= Vector3::Zero*/, const Vector3& dir /*= Vector3(0, 0, 1)*/)
{
	mPos = pos;
	mDir = dir;
}

bool IntersectRayAABB(const Vector3& rayPos, const Vector3& rayDir, const AABBox& box, Vector3* hitPos, float* dist)
{
//	_Assert(NULL != hitPos);
//	_Assert(NULL != dist);

	float tMin = 0;
	float tMax = FLT_MAX;

	float rp[3] = { rayPos.x, rayPos.y, rayPos.z };
	float rd[3] = { rayDir.x, rayDir.y, rayDir.z };

	float boxMin[3] = { box.mMin.x, box.mMin.y, box.mMin.z };
	float boxMax[3] = { box.mMax.x, box.mMax.y, box.mMax.z };

	for(int i = 0; i < 3; ++i)
	{
		if(fabs(rd[i]) < FLT_EPSILON)
		{
			if(rp[i] < boxMin[i] || rp[i] > boxMax[i])
				return false;
		}
		else
		{
			float t1 = (boxMin[i] - rp[i]) / rd[i];
			float t2 = (boxMax[i] - rp[i]) / rd[i];

			if(t1 > t2)
				std::swap(t1, t2);

			if(t1 > tMin)
				tMin = t1;

			if(t2 < tMax)
				tMax = t2;

			if(tMin > tMax)
				return false;
		}
	}

	Vector3 hitP = rayPos + tMin * rayDir;
	float d = VectorLength(rayPos - hitP);

	if(hitPos)
		*hitPos = hitP;

	if(dist)
		*dist = d;

	return true;
}
