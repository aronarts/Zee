#ifndef MODEL_PANEL_H
#define MODEL_PANEL_H

#include "wx/wx.h"
#include "wx/treectrl.h"

#include "Model.h"

enum
{
	ID_MODELLIST_TREE,
};

class ModelTreeItemData : public wxTreeItemData
{
public:
	ModelTreeItemData(Model* model);

	Model* GetModel();

private:
	Model* mModel;
};

class ModelInspectorPanel;
class ModelListTree;
class ModelPanel : public wxPanel
{
public:
	ModelPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void LoadDataFromScene();

	void OnClose(wxCloseEvent& event);

	DECLARE_EVENT_TABLE() 

private:
	void createWxCtrls();
	void appendModel(wxTreeItemId parentItemId, Model* model);

private:
	wxPanel* mTreePanel;
	ModelListTree* mTreeCtrl;

	ModelInspectorPanel* mInspectorPanel;

	wxImageList* mIconList;
};

class ModelListTree : public wxTreeCtrl
{
public:
	ModelListTree(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize&size = wxDefaultSize, long style = wxTR_DEFAULT_STYLE);

	void OnItemSelected(wxTreeEvent& event);

	void AttachInspectorPanel(ModelInspectorPanel* inspectorPanel);

	DECLARE_EVENT_TABLE()

private:
	ModelInspectorPanel* mInspectorPanel;
};

class ModelPreviewCanvas;
class ModelInspectorPanel : public wxScrolledWindow
{
public: 
	ModelInspectorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	ModelPreviewCanvas* GetModelPreviewCanvas();

	void AttachModel(Model* model);

private:
	void createWxCtrls();

private:
	Model* mModel;
	ModelPreviewCanvas* mModelPreviewCanvas;
};


class ModelPreviewCanvas : public wxWindow
{
public:
	ModelPreviewCanvas(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

	void SetSwapChainIndex(int index);
	void RenderWindow();

	void SetPreviewModel(Model* model);

	DECLARE_EVENT_TABLE()

protected:
	void OnIdle(wxIdleEvent& event);
	void OnClose(wxCloseEvent& event);

private:
	Camera* mCamera;
	Model* mModel;
	int mSwapChainIndex;
};

#endif