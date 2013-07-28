#include "DirectionalLight.h"
#include "PointLight.h"
#include "AmbientLight.h"
#include <list>

class LightManager
{
public:
	static void AddDirectionalLight(DirectionalLight* light);
	static void AddPointLight(PointLight* light);
	static void Update();

	static void SetAmbientLight(D3DXCOLOR _color, float _intensity);
	static D3DXCOLOR GetFinalAmbientColor();

public:
	static int numActiveDirectionalLights;
	static int numActivePointLights;

	static bool isDirectionalLightsDirty;
	static bool isPointLightsDirty;

	static DirectionalLightData directionalLightsData[MAX_NUM_DIRECTIONAL_LIGHTS];
	static PointLightData pointLightsData[MAX_NUM_POINT_LIGHTS];

private:
	static AmbientLight mAmbientLight;

	static std::list<DirectionalLight*> mDirectionalLights;
	static std::list<PointLight*> mPointLights;
};