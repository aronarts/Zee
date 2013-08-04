#include "D3DUtility.h"
#include "Input.h"
#include "GUI.h"

#include "Camera.h"

#include "LightManager.h"
#include "GeometryManager.h"
#include "MaterialManager.h"
#include "SceneManager.h"

#include "Model.h"
#include "Primitive.h"
#include "Material.h"

#include "OBJParser.h"
#include "DebugDrawer.h"

#include <Locale.h>

LabelStyle* leftAlignStyle;

void ApplyFPCameraControllor(Camera* pCamera, float deltaTime);

void AppDestroy();

void OnLostDevice();
void OnResetDevice();

void SetupGUIStyle();
void GUIUpdate();

int GetFPS();

int main(int argc, char* argv[])
{
	_wsetlocale(LC_ALL, L"chs");

	// create window
	const int WND_WIDTH = 1024;
	const int WND_HEIGHT = 600;

	HWND hWnd = RegisterAndCreateWindow(L"Zee", L"Zee", WND_WIDTH, WND_HEIGHT, WndProc);
	_Assert(NULL != hWnd);

	// create d3ddevice
	Driver::CreateD3DDevice(hWnd, D3DMULTISAMPLE_4_SAMPLES);

	// init
	Input::Init(GetModuleHandle(0), Driver::hWnd);
	MaterialManager::Init();
	SceneManager::Init();
	Time::Start();

	SetupGUIStyle();

	// camera
	SceneManager::SetMainCamera(Vector3(0, 2.0f, -4.0f), Vector3::Zero,
		PI/3, (float)Driver::viewPort.Width / (float)Driver::viewPort.Height, 0.1f, 100000.0f);

	// lights
	DirectionalLight dirLight1(L"dirLight1", D3DXCOLOR_RED, Vector3(1.0f, -1.0f, 1.0f));
	DirectionalLight dirLight2(L"dirLight2");

	PointLight pointLight1(L"pointLight1", D3DXCOLOR_YELLOW, Vector3(-4.0f, 0, 0), Vector3(1.0f, 0, 0));
	PointLight pointLight2(L"pointLight2");

	LightManager::SetAmbientLight(D3DXCOLOR_WHITE, 0.2f);
	pointLight1.Enable(false);

	LightManager::AddDirectionalLight(&dirLight1);
	LightManager::AddPointLight(&pointLight1);

	// geo
	Cube* cube1 = new Cube(L"cube1");
	GeometryManager::AddGeometry(cube1);

	cube1->CalculateTBN();
	cube1->BuildGeometry(XYZ_UV_TBN);

	Cylinder* cylinder1 = new Cylinder(L"cylinder1", 0, 0.5f, 2.0f);
	GeometryManager::AddGeometry(cylinder1);

	cylinder1->CalculateNormals();
	cylinder1->BuildGeometry(XYZ_N);

	// material
	Material* mtl1 = new Material(L"mtl1");
	MaterialManager::AddMaterial(mtl1);

	mtl1->SetShader(BumpSpecular);
	mtl1->mShader->SetColorTex(L"./Assets/Textures/6133.jpg");
	mtl1->mShader->SetNormalTex(L"./Assets/Textures/6133Normal.jpg");
	mtl1->mShader->SetSpecShiness(0.4f);

	Material* mtl2 = new Material(L"mtl2");
	MaterialManager::AddMaterial(mtl2);

	mtl2->SetShader(Diffuse);
	//mtl2->shader->SetColorTex(L"./Assets/Textures/6133.jpg");

	// model
	Model* cubeModel = new Model(L"cubeModel", SceneManager::root, cylinder1, mtl2);

	// line test
	std::vector<Vector3> points;
	points.push_back(Vector3(0, 0, 0));
	points.push_back(Vector3(4, 0, 0));
	points.push_back(Vector3(4, 4, 0));


	// start loop
	while(run())
	{
		switch(Driver::D3DDevice->TestCooperativeLevel())
		{
		case D3D_OK:
			{
				// update state
				Time::Tick();

				Input::GetDeviceState(Driver::hWnd);

				GUIUpdate();

				LightManager::Update();

				ApplyFPCameraControllor(SceneManager::mainCamera, Time::deltaTime);

				SceneManager::Update();

				// render
				Driver::Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff1e90ff);
				Driver::BeginScene();

				DebugDrawer::DrawLine(points, 0xffff0000, SceneManager::mainCamera);
				DebugDrawer::DrawCircle(Vector3(4, 4, 0), Vector3(1, 1, 1), 1, 0xffff0000, SceneManager::mainCamera);
				DebugDrawer::DrawSquare(Vector3(0, 0, 0), Vector3(0, 0, 1), 1, 0xffff0000, SceneManager::mainCamera);

				//cubeModel->Draw(SceneManager::mainCamera);
				SceneManager::DrawAll();

				gGUISystem.Draw();

				Driver::EndScene();
				Driver::Present();

				break;
			}
		case D3DERR_DEVICELOST:
			break;
		case D3DERR_DEVICENOTRESET:
			{
				OnLostDevice();
				OnResetDevice();
				break;
			}
		default:
			PostQuitMessage(0);
			break;
		}
	}

	AppDestroy();

	return 0;
}

void ApplyFPCameraControllor(Camera* pCamera, float deltaTime)
{
	Vector3 moveVector;
	float moveSpeed = 2.0f;
	float rotateSpeed = 2.0f;

	if(Input::GetKey(DIK_LSHIFT))
		moveSpeed *= 4;

	if(Input::GetKey(DIK_W))
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(0, 0, 1);
	if(Input::GetKey(DIK_S))
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(0, 0, -1);

	if(Input::GetKey(DIK_A))
	{
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(-1, 0, 0);
	}
	if(Input::GetKey(DIK_D))
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(1, 0, 0);

	moveVector = pCamera->LocalVectorToWorld(moveVector);

	if(Input::GetKey(DIK_Q))
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(0, -1, 0);
	if(Input::GetKey(DIK_E))
		moveVector = moveVector + moveSpeed * deltaTime * Vector3(0, 1, 0);


	if(moveVector != Vector3::Zero){
		pCamera->SetTransformDirty(true);
		pCamera->Translate(moveVector);
	}

	if(Input::mouseState.rgbButtons[1] & 0x80){
		float rotationY = rotateSpeed * Input::mouseState.lX / 1000.0f;
		float rotationX = rotateSpeed * Input::mouseState.lY / 1000.0f;

		if(rotationY != 0 || rotationX != 0){
			pCamera->SetTransformDirty(true);
			pCamera->Rotate(0, rotationY, 0);
			pCamera->RotateLocal(rotationX, 0, 0);
		}
	}

}

void SetupGUIStyle()
{
    gDefaultLabelStyle.CreateFont();
    leftAlignStyle = new LabelStyle(TEXT("Consolas"), 7, 15, 0, 0xffe3b706, DT_LEFT | DT_VCENTER);
    leftAlignStyle->CreateFont();

    gDefaultButtonStyle.CreateTextures();
    gDefaultSliderStyle.CreateTextures();
    gDefaultToggleStyle.CreateTextures();
    gDefaultListBoxStyle.CreateTextures();
}

void GUIUpdate()
{
    gGUISystem.clear();

    wchar_t text[MAX_STR_LEN];
	wsprintf(text, TEXT("fps:%d, %d, %d"), GetFPS(), Input::cursorPos.x, Input::cursorPos.y);
    gGUISystem.GUILabel(text, Rect(10, 10, 300, 25), leftAlignStyle);

	static bool enableDirLight1 = false;
	enableDirLight1 = gGUISystem.GUIToggle(Rect(10, 50, 20, 20), enableDirLight1);

	DirectionalLight* dirLight1 = NULL;
	LightManager::GetDirLight(L"dirLight1", &dirLight1);
	dirLight1->Enable(enableDirLight1);
}

int GetFPS()
{
    static int fps = 0;
    static int fpsAccumulator = 0;
    static float elapseTime = 0;

    if(elapseTime > 1.0f)
    {
        fps = fpsAccumulator;

        fpsAccumulator = 0;
        elapseTime = 0;
    }

    elapseTime += Time::deltaTime;
    fpsAccumulator++;

    return fps;
}

void AppDestroy()
{
	SAFE_DELETE(leftAlignStyle);

	Input::Destroy();
	SceneManager::Destory();
	GeometryManager::DeleteAll();
	MaterialManager::DeleteAll();
	Driver::Destory();
}

void OnLostDevice()
{
	gDefaultLabelStyle.OnLostDevice();
	leftAlignStyle->OnLostDevice();

	gGUISystem.OnLostDevice();

	DiffuseShader::OnLostDevice();
	SpecularShader::OnLostDevice();
	BumpSpecularShader::OnLostDevice();

	GeometryManager::OnLostDevice();
}

void OnResetDevice()
{
	if(!Driver::Reset())
		return;

	gDefaultLabelStyle.OnResetDevice();
	leftAlignStyle->OnResetDevice();

	gGUISystem.OnResetDevice();

	DiffuseShader::OnResetDevice();
	SpecularShader::OnResetDevice();
	BumpSpecularShader::OnResetDevice();

	GeometryManager::OnResetDevice();
}