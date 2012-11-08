#ifndef _VIEWMAIN_H_
#define _VIEWMAIN_H_

#include "ControllerForm.h"
#include "PagePanel.h"
#include <FBase.h>
#include <FUi.h>
#include <FWeb.h>
#include <FIo.h>
#include <FNet.h>
#include <FMedia.h>
#include <FGraphics.h>

class ViewMainForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::IActionEventListener
{

// Construction
public:
	ViewMainForm(void);
	virtual ~ViewMainForm(void);
	bool Initialize();

	PagePanel* CreatePage(PageLoadData *pPageDetails);
	bool SetActivePanel(PagePanel *pPanel);
	void RemovePanel(PagePanel *pPanel);

	static const RequestId NOTIFY_TEXT_COMPLETE = 100;
	static const RequestId NOTIFY_IMAGE_COMPLETE = 101;
	static const RequestId NOTIFY_NETWORK_FAIL = 102;

private:
	ControllerForm *__pControllerForm;
	Osp::Ui::Controls::TabBar* __pTabBar;
	Osp::Graphics::Color __colorPagetitle;
	int __selfIndex;
	PagePanel *__pActivePanel;
	Osp::Base::Collection::LinkedList __listGarbage;

private:
	static const int ID_REFRESH = 301;

	static const int ID_TABBAR_START = 700;

	void SetBgColors(void);
	PagePanel* CreatePanel(PageLoadData *pPageDetails);
	int ToInt(char* pString);
	void UpdateAnimation(void);

private:
	result OnInitializing(void);
	result OnTerminating(void);
	void OnActionPerformed(const Osp::Ui::Control& source, int actionId);
	void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);

};

#endif	//_FORM1_H_
