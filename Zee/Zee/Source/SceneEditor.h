#ifndef SCENE_EDITOR_H
#define SCENE_EDITOR_H

#include "wx/wx.h"
#include "wxWndTreeGenerator.h"
#include "SceneEditorPanel.h"
#include "Gizmo.h"

class SceneEditorCanvas;

class SceneEditorFrame : public wxFrame
{
	enum 
	{
		ID_Quit = 1,
		ID_About,
		ID_TreeGenerator,
		ID_OK,
		ID_EDITOR_NOTEBOOK,
	};

public:
	SceneEditorFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnTreeGenerator(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();

private:
	SceneEditorPanel* mEditorPanel;
	wxPanel* mCanvasPanel;
	SceneEditorCanvas* mCanvas;
	TreeGeneratorFrame* mWndTreeGenerator;
};

class SceneEditorToolBar : public wxToolBar, public IGizmoEventHandler
{
	enum 
	{
		ID_TOOL_TRANS,
		ID_TOOL_ROTATE,
		ID_TOOL_SCALE,
		ID_RADIO_BUTTON_LOCAL,
		ID_RADIO_BUTTON_GLOBAL,
	};

public:
	SceneEditorToolBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize, long style = wxTB_HORIZONTAL);

	virtual void OnCoordTypeChanged(Gizmo* gizmo);

	void OnToolTransClicked(wxCommandEvent& event);
	void OnToolRotateClicked(wxCommandEvent& event);
	void OnToolScaleClicked(wxCommandEvent& event);
	void OnToolRadioButtonGlobal(wxCommandEvent& event);
	void OnToolRadioButtonLocal(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

private:
	void createWxCtrls();

private:
	wxRadioButton* mButtonGlobal;
	wxRadioButton* mButtonLocal;
};

class SceneEditorCanvas : public wxWindow
{
public:
	SceneEditorCanvas(wxWindow* parent, 
		wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxString& name = wxT("Zee"));

	void RenderLoop();

protected:
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);

private:
	DECLARE_EVENT_TABLE()
};

//class ModelDropTarget : public wxDropTarget
//{
//public:
//	ModelDropTarget(SceneEditorCanvas* canvas);
//
//	virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def);
//	virtual void OnLeave();
//	virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
//
//private:
//	SceneEditorCanvas* mCanvas;
//};

#endif