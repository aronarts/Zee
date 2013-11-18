#ifndef SHADER_H
#define SHADER_H

#include "D3DUtility.h"
#include "IReferenceCounted.h"

enum ShadingMethod 
{ 
	Flat,
	View,
	Diffuse, 
	Specular, 
	BumpSpecular,
	InvalidMethod
};

class Material;
class Camera;
class Object;
class Geometry;

struct UtilityShader
{
	static void OnLostDevice()
	{
		effect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		effect->OnResetDevice();
	}

	static void CreateEffectPool()
	{
		HRESULT hr = D3DXCreateEffectPool(&pool);
		_Assert(SUCCEEDED(hr));
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		_Assert(NULL != pool);
		D3DXCreateEffectFromFile(Driver::D3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, pool, &effect, NULL);

		_Assert(NULL != effect);
	}

	static void SetupSharedParams();

	static LPD3DXEFFECTPOOL pool;		// QUESTION:release?
	static LPD3DXEFFECT effect;
};

interface IShader : public IReferenceCounted
{
public:
	IShader(Material* material)
		:mMaterial(material)
	{

	}

	virtual void SetColorTex(wchar_t* texFileName)
	{

	}

	virtual void SetNormalTex(wchar_t* texFileName)
	{

	}

	virtual void SetAmbientColor(D3DXCOLOR color)
	{

	}

	virtual void SetDiffuseColor(D3DXCOLOR color)
	{

	}
	virtual void SetSpecularColor(D3DXCOLOR color)
	{

	}

	virtual void SetSpecShiness(float shiness)
	{

	}
	virtual void SetSpecGloss(float gloss)
	{

	}

	virtual void SetUVTiles(float tilesU, float tilesV)
	{

	}
	virtual void SetUVOffset(float offsetU, float offsetV)
	{

	}

	virtual void Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera)
	{

	}

protected:
	Material* mMaterial;
};

class FlatShader : public IShader
{
public:
	FlatShader(Material* material)
		:IShader(material)
	{

	}

	static void OnLostDevice()
	{
		mEffect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		mEffect->OnResetDevice();
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		_Assert(NULL != UtilityShader::pool);
		D3DXCreateEffectFromFile(Driver::D3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, UtilityShader::pool, &mEffect, NULL);

		_Assert(NULL != mEffect);
	}

	void SetColorTex(wchar_t* texFileName);

	void SetDiffuseColor(D3DXCOLOR color);

	void SetUVTiles(float tilesU, float tilesV);
	void SetUVOffset(float offsetU, float offsetV);

	void Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera);

public:
	static LPD3DXEFFECT mEffect;
};

class ViewShader : public IShader
{
public:
	ViewShader(Material* _material)
		:IShader(_material)
	{

	}

	static void OnLostDevice()
	{
		mEffect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		mEffect->OnResetDevice();
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		_Assert(NULL != UtilityShader::pool);
		D3DXCreateEffectFromFile(Driver::D3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, UtilityShader::pool, &mEffect, NULL);

		_Assert(NULL != mEffect);
	}

	void SetColorTex(wchar_t* texFileName);

	void SetDiffuseColor(D3DXCOLOR color);

	void SetUVTiles(float tilesU, float tilesV);
	void SetUVOffset(float offsetU, float offsetV);

	void Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera);

public:
	static LPD3DXEFFECT mEffect;
};

class DiffuseShader : public IShader
{
public:
	DiffuseShader(Material* material)
		:IShader(material)
	{

	}

	static void OnLostDevice()
	{
		mEffect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		mEffect->OnResetDevice();
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		_Assert(NULL != UtilityShader::pool);
		D3DXCreateEffectFromFile(Driver::D3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, UtilityShader::pool, &mEffect, NULL);

		_Assert(NULL != mEffect);
	}

	void SetColorTex(wchar_t* texFileName);

	void SetAmbientColor(D3DXCOLOR color); 
	void SetDiffuseColor(D3DXCOLOR color);

	void SetUVTiles(float tilesU, float tilesV);
	void SetUVOffset(float offsetU, float offsetV);

	void Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera);

public:
	static LPD3DXEFFECT mEffect;
};

class SpecularShader : public IShader
{
public:
	SpecularShader(Material* _material)
		:IShader(_material)
	{

	}

	static void OnLostDevice()
	{
		mEffect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		mEffect->OnResetDevice();
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		_Assert(NULL != UtilityShader::pool);
		D3DXCreateEffectFromFile(Driver::D3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, UtilityShader::pool, &mEffect, NULL);

		_Assert(NULL != mEffect);
	}

	void SetColorTex(wchar_t* texFileName);

	void SetAmbientColor(D3DXCOLOR color);
	void SetDiffuseColor(D3DXCOLOR color);
	void SetSpecularColor(D3DXCOLOR color);

	void SetSpecShiness(float shiness);
	void SetSpecGloss(float gloss);

	void SetUVTiles(float tilesU, float tilesV);
	void SetUVOffset(float offsetU, float offsetV);

	void Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera);

public:
	static LPD3DXEFFECT mEffect;
};

class BumpSpecularShader : public IShader
{
public:
	BumpSpecularShader(Material* material)
		:IShader(material)
	{

	}

	static void OnLostDevice()
	{
		mEffect->OnLostDevice();
	}

	static void OnResetDevice()
	{
		mEffect->OnResetDevice();
	}

	static void CreateEffectFromFile(wchar_t* FXFileName)
	{
		_Assert(NULL != UtilityShader::pool);
		D3DXCreateEffectFromFile(Driver::D3DDevice, FXFileName, NULL, NULL, D3DXSHADER_DEBUG, UtilityShader::pool, &mEffect, NULL);

		_Assert(NULL != mEffect);
	}

	void SetColorTex(wchar_t* texFileName);
	void SetNormalTex(wchar_t* texFileName);

	void SetAmbientColor(D3DXCOLOR color);
	void SetDiffuseColor(D3DXCOLOR color);
	void SetSpecularColor(D3DXCOLOR color);

	void SetSpecShiness(float shiness);
	void SetSpecGloss(float gloss);

	void SetUVTiles(float tilesU, float tilesV);
	void SetUVOffset(float offsetU, float offsetV);

	void Render(const D3DXMATRIX& matWorld, Geometry* geo, Camera* camera);

public:
	static LPD3DXEFFECT mEffect;
};

#endif