#include "ZeeApp.h"
#include "D3DUtility.h"
#include "Engine.h"
#include "Input.h"
#include "GUI.h"

#include "Camera.h"
#include "CameraController.h"

#include "SceneManager.h"

#include "ModelNode.h"
#include "Primitive.h"
#include "Material.h"

#include "OBJParser.h"
#include "DebugDrawer.h"

#include "Gizmo.h"
#include "BillboardNode.h"
#include "ResourceMgr.h"

#include "Terrain.h"

#include <Locale.h>

enum 
{
	ID_Quit = 1,
	ID_About,
	ID_TreeGenerator,
	ID_OK,
};

IMPLEMENT_APP_CONSOLE(ZeeApp)

// ------------------------------------------
const int WND_WIDTH = 1280;
const int WND_HEIGHT = 720;

ModelNode* cube = NULL;
Gizmo* gizmo = NULL;

Terrain* terrain = NULL;

LabelStyle* leftAlignStyle;

void AppDestroy();

void OnLostDevice();
void OnResetDevice();

void SetupGUIStyle();
void GUIUpdate();

int GetFPS();

void SetUp();
void RenderLoop();
// -------------------------------------------

bool ZeeApp::OnInit()
{
	wxInitAllImageHandlers();

	ZeeFrame* frame = new ZeeFrame(L"Zee", wxPoint(0, 0), wxSize(WND_WIDTH, WND_HEIGHT));
	frame->Show(true);
	SetTopWindow(frame);

	return true;
}

BEGIN_EVENT_TABLE(ZeeFrame, wxFrame)
	EVT_MENU(ID_Quit, ZeeFrame::OnQuit)
	EVT_MENU(ID_TreeGenerator, ZeeFrame::OnTreeGenerator)
	EVT_MENU(ID_About, ZeeFrame::OnAbout)
	EVT_CLOSE(ZeeFrame::OnClose)
END_EVENT_TABLE()

ZeeFrame::ZeeFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
:wxFrame((wxFrame*)NULL, -1, title, pos, size, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)
,mWndTreeGenerator(NULL)
{
	SetIcon(wxIcon(L"./Assets/Icons/Zee.xpm", wxBITMAP_TYPE_XPM));

	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_Quit, L"&Quit");
	
	wxMenu* menuTerrain = new wxMenu;
	menuTerrain->Append(ID_TreeGenerator, L"&Tree Generator");

	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(ID_About, L"&About\tF1");

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, L"&File");
	menuBar->Append(menuTerrain, L"&Terrain");
	menuBar->Append(menuHelp, L"&Help");

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText(L"Welcome To WanderLand!");

	wxSize clientSize = GetClientSize(); 

	mCanvas = new D3D9Canvas(this, wxID_ANY, wxDefaultPosition, clientSize, wxSUNKEN_BORDER);
	mCanvas->InitDriver();

	gEngine = new Engine();
	gEngine->Init();

	SetUp();

	mWndTreeGenerator = new TreeGeneratorFrame(this, L"Tree Generator", wxDefaultPosition, wxDefaultSize);
	mWndTreeGenerator->Centre();
	mWndTreeGenerator->Show(false);

	mWndTreeGenerator->Setup();
}

void ZeeFrame::OnQuit(wxCommandEvent& event)
{
	Close(true);
}

void ZeeFrame::OnClose(wxCloseEvent& event)
{
	cleanupAndDestory();
}

void ZeeFrame::cleanupAndDestory()
{
	AppDestroy();

	if(mWndTreeGenerator)
		mWndTreeGenerator->CleanupAndDestory();
	
	Destroy();
}

void ZeeFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(L"Siny - SinYocto@gmail.com", L"Author", wxOK | wxICON_INFORMATION, this);
}

void ZeeFrame::OnTreeGenerator(wxCommandEvent& event)
{
	mWndTreeGenerator->Show(true);
	mWndTreeGenerator->Raise();
}

BEGIN_EVENT_TABLE(D3D9Canvas, wxWindow)
	EVT_SIZE(D3D9Canvas::OnSize)
	EVT_IDLE(D3D9Canvas::OnIdle)
END_EVENT_TABLE()

D3D9Canvas::D3D9Canvas(wxWindow* parent, wxWindowID id /* = wxID_ANY */, 
					   const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxDefaultSize */, 
					   long style /* = 0 */, const wxString& name /* = wxT */)
					   :wxWindow(parent, id, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
{
	
}

void D3D9Canvas::OnIdle(wxIdleEvent& event)
{
	if(Driver::D3DDevice)
		RenderLoop();

	event.RequestMore(true);
}

void D3D9Canvas::OnSize(wxSizeEvent& event)
{
	// TODO:暂且使用固定窗口, 以后再增加拉伸窗口功能
	wxSize vpSize = GetClientSize();
	Driver::SetViewPort(0, 0, vpSize.x, vpSize.y);
	SceneManager::mainCamera->SetAspect((float)vpSize.x / (float)vpSize.y);
}

void D3D9Canvas::InitDriver()
{
	Driver::CreateD3DDevice((HWND)GetHWND(), D3DMULTISAMPLE_4_SAMPLES);
}

void SetUp()
{
	_wsetlocale(LC_ALL, L"chs");

	Input::Init(GetModuleHandle(0), Driver::hWnd);
	SceneManager::Init();

	SetupGUIStyle();

	// camera
	SceneManager::CreateMainCamera(Vector3(0, 4.0f, -4.0f), Vector3::Zero,
		PI/3, (float)Driver::primaryViewPort.Width / (float)Driver::primaryViewPort.Height, 0.1f, 1000.0f);

	FPCameraController* fpCameraController = new FPCameraController(6.0f, 2.0f, 4.0f);
	//HoverCameraController* hoverCameraController = new HoverCameraController(5.0f, 20.0f, -4*PI/9, 4*PI/9, 2.0f, 100.0f);
	SceneManager::mainCamera->SetCameraController(fpCameraController);

	// lights
	DirectionalLight* dirLight1 = new DirectionalLight(L"dirLight1", D3DXCOLOR_WHITE, Vector3(1.0f, -1.0f, 1.0f));
	PointLight* pointLight1 = new PointLight(L"pointLight1", D3DXCOLOR_YELLOW, Vector3(0, 0, 0), Vector3(1.0f, 0.05f, 0));

	LightManager* lightMgr = gEngine->GetLightManager();
	lightMgr->SetAmbientLight(D3DXCOLOR_WHITE, 0.2f);

	lightMgr->AddDirectionalLight(dirLight1);
	lightMgr->AddPointLight(pointLight1);
	pointLight1->Enable(false);

	// geo
	GeometryManager* geometryMgr = gEngine->GetGeometryManager();

	Cube* cubeGeo = new Cube(L"cubeGeo");
	geometryMgr->AddGeometry(cubeGeo);

	cubeGeo->CalculateTBN();
	cubeGeo->BuildGeometry(XYZ_UV_TBN);

	Cylinder* coneGeo = new Cylinder(L"coneGeo", 0, 0.5f, 2.0f);
	geometryMgr->AddGeometry(coneGeo);

	coneGeo->CalculateNormals();
	coneGeo->BuildGeometry(XYZ_N);

	Cylinder* cylinderGeo = new Cylinder(L"cylinderGeo", 0.5f, 0.5f, 2.0f);
	geometryMgr->AddGeometry(cylinderGeo);

	cylinderGeo->CalculateNormals();
	cylinderGeo->BuildGeometry(XYZ_N);

	Torus* torusGeo = new Torus(L"torusGeo", 1.0f, 0.025f, 32, 8);
	geometryMgr->AddGeometry(torusGeo);

	torusGeo->CalculateNormals();
	torusGeo->BuildGeometry(XYZ_N);

	// material
	MaterialManager* materialMgr = gEngine->GetMaterialManager();

	Material* mtlBump = new Material(L"mtl1");
	materialMgr->AddMaterial(mtlBump);

	mtlBump->SetShader(BumpSpecular);
	mtlBump->mShader->SetColorTex(L"./Assets/Textures/6133.jpg");
	mtlBump->mShader->SetNormalTex(L"./Assets/Textures/6133Normal.jpg");
	mtlBump->mShader->SetSpecShiness(0.4f);

	Material* mtlDiff = new Material(L"mtlDiff");
	materialMgr->AddMaterial(mtlDiff);

	mtlDiff->SetShader(Diffuse);
	//mtl2->shader->SetColorTex(L"./Assets/Textures/6133.jpg");

	Material* mtlSpec = new Material(L"mtlSpec");
	materialMgr->AddMaterial(mtlSpec);

	mtlSpec->SetShader(Specular);
	mtlSpec->mShader->SetColorTex(L"./Assets/Textures/6133.jpg");
	mtlSpec->mShader->SetSpecShiness(0.4f);

	Material* mtlView = new Material(L"mtlView");
	materialMgr->AddMaterial(mtlView);

	mtlView->SetShader(View);
	mtlView->SetDiffuseColor(D3DXCOLOR_RED);

	Material* mtlFlat = new Material(L"mtlFlat");
	materialMgr->AddMaterial(mtlFlat);

	mtlFlat->SetShader(Flat);
	mtlFlat->SetDiffuseColor(D3DXCOLOR_GREEN);

	// model
	cube = new ModelNode(L"cube", NULL, cubeGeo, mtlBump);
	SceneManager::AddSceneNode(cube);
	cube->Translate(2, 0, 0);

	ModelNode* cylinder = new ModelNode(L"cylinder", NULL, cylinderGeo, mtlDiff);
	SceneManager::AddSceneNode(cylinder);
	cylinder->Translate(-2, 0, 0);

	ModelNode* cone = new ModelNode(L"cone", NULL, coneGeo, mtlView);
	SceneManager::AddSceneNode(cone);
	cone->Translate(0, 0, -2);

	ModelNode* torus = new ModelNode(L"torus", NULL, torusGeo, mtlFlat);
	SceneManager::AddSceneNode(torus);
	torus->Translate(0, 0, 2);

	// billboard
	BillboardNode* billboard = new BillboardNode(L"billboard", 1.0f, 1.0f, D3DXCOLOR_YELLOW);
	SceneManager::AddSceneNode(billboard);
	billboard->GetBillboard()->SetTexture(L"./Assets/Textures/light.jpg");

	// gizmo
	gizmo = new Gizmo;
	gizmo->Init();

	// terrain
	PerformanceTimer::Begin(L"building 257 terrain");
	terrain = new Terrain(257, 200.0f, 40.0f);
	terrain->LoadFromHeightMap(L"./Assets/Textures/heightMap257_bit16.raw", 257);
	terrain->BuildTerrain(4);
	terrain->CalcChunkLODDist(SceneManager::mainCamera, 1.0f);

	terrain->SetColorTexes(L"./Assets/Textures/Cliff.jpg", L"./Assets/Textures/Grass_Hill.jpg", 
		L"./Assets/Textures/DirtGrass.jpg", L"./Assets/Textures/Pebbles.jpg");

	terrain->SetSplatMapTex(L"./Assets/Textures/splat.tga");
	terrain->SetMtlParameters(30.0f, 30.0f, D3DXCOLOR_WHITE, D3DXCOLOR_WHITE);
	PerformanceTimer::End();
}

void D3D9Canvas::RenderLoop()
{
	switch(Driver::D3DDevice->TestCooperativeLevel())
	{
	case D3D_OK:
		{
			// update state
			gEngine->FrameUpdate();

			Input::GetDeviceState(Driver::hWnd);

			if(Input::GetKeyUp(DIK_R))
			{
				terrain->createEffect();
			}

			GUIUpdate();


			if(wxWindow::FindFocus() == this)
				SceneManager::mainCamera->ApplyCameraController();

			SceneManager::FrameUpdate();
			terrain->FrameUpdate(SceneManager::mainCamera);

			// render
			Driver::RenderToSwapChain(PRIMARY_SWAPCHAIN);
			Driver::Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x7f36404a, 1.0f);
			Driver::BeginScene();

			Vector2 screenPos(Input::cursorPos.x, Input::cursorPos.y);

			Vector3 rayPos;
			Vector3 rayDir;
			SceneManager::mainCamera->GetScreenRay(screenPos, &rayPos, &rayDir);

			static SceneNode* hitNode = NULL;
			if(Input::GetLeftButtonDown())
			{
				if(!gizmo->IsSelected())
					hitNode = SceneManager::RayIntersect(rayPos, rayDir, NULL, NULL);
			}

			terrain->Draw(SceneManager::mainCamera, true);

			SceneManager::root->SetDrawBBoxFlag(true);
			SceneManager::DrawAll();

			gGUISystem.Draw();

			gizmo->SetActiveType(Gizmo::GIZMO_TRANS);
			gizmo->Draw(hitNode, SceneManager::mainCamera);

			if(hitNode && hitNode->GetNodeType() == SceneNode::SCENE_NODE_BILLBOARD)
			{
				PointLight* pointLight1 = NULL;
				gEngine->GetLightManager()->GetPointLight(L"pointLight1", &pointLight1);
				pointLight1->SetPosition(hitNode->GetWorldPosition());
			}

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

	Vector2 screenPos(Input::cursorPos.x, Input::cursorPos.y);
	Vector2 screenLocation;
	Driver::GetScreenLocation(screenPos, &screenLocation);

	wchar_t text[MAX_STR_LEN];
	YString::Format(text, TEXT("fps:%d"), GetFPS());
	gGUISystem.GUILabel(text, Rect(10, 10, 300, 25), leftAlignStyle);

	YString::Format(text, L"screenPos:%d, %d(%f, %f)", Input::cursorPos.x, Input::cursorPos.y, 
		screenLocation.x, screenLocation.y);
	gGUISystem.GUILabel(text, Rect(10, 40, 300, 25), leftAlignStyle);


	static bool enableDirLight1 = true;
	// enableDirLight1 = gGUISystem.GUIToggle(Rect(10, 150, 20, 20), enableDirLight1);

	DirectionalLight* dirLight1 = NULL;
	gEngine->GetLightManager()->GetDirLight(L"dirLight1", &dirLight1);
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

	elapseTime += gEngine->GetFrameTimer()->GetDeltaTime();
	fpsAccumulator++;

	return fps;
}

void AppDestroy()
{
	SAFE_DELETE(leftAlignStyle);
	gizmo->Destroy();
	SAFE_DELETE(gizmo);

	SAFE_DELETE(terrain);

	gEngine->Destroy();

	Input::Destroy();
	SceneManager::Destory();
	Driver::Destory();

	SAFE_DELETE(gEngine);
}

void OnLostDevice()
{
	leftAlignStyle->OnLostDevice();
	gizmo->OnLostDevice();
	terrain->OnLostDevice();

	gDefaultLabelStyle.OnLostDevice();
	gGUISystem.OnLostDevice();

	ResourceMgr::OnLostDevice();

	gEngine->OnLostDevice();

	Driver::OnLostDevice(); 
}

void OnResetDevice()
{
	if(!Driver::Reset())
		return;

	leftAlignStyle->OnResetDevice();
	gizmo->OnResetDevice();

	terrain->OnResetDevice();

	gDefaultLabelStyle.OnResetDevice();
	gGUISystem.OnResetDevice();

	ResourceMgr::OnResetDevice();

	gEngine->OnResetDevice();

	Driver::OnResetDevice();
}


