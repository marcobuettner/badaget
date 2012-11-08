#include "ViewMainForm.h"
#include "ViewTextForm.h"
#include "ViewImageForm.h"
#include <FApp.h>
#include <FXml.h>
#include <FIo.h>

using namespace Osp::Base::Collection;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::App;
using namespace Osp::Xml;
using namespace Osp::Io;

ViewMainForm::ViewMainForm(void)
{
}

ViewMainForm::~ViewMainForm(void)
{
}

bool
ViewMainForm::Initialize()
{
	__pActivePanel = null;

	if(Construct(MAIN_FORM) != E_SUCCESS)
	{
		AppLog("Form Create Failed");
		return false;
	}

	return true;
}

result
ViewMainForm::OnInitializing(void)
{
	result r = E_SUCCESS;
	LinkedList *pTitles = null;

	int i = 0;

	__pControllerForm = GET_CONTROLLER_FORM;
	if(!__pControllerForm)
	{
		AppLog("ControllerForm does not exist!");
		return E_FAILURE;
	}

	String title = __pControllerForm->GetTitle();
	String iconPath = __pControllerForm->GetIconPath();
	Color colorHeader = __pControllerForm->GetHeaderColor();
	String headerBgPath = __pControllerForm->GetHeaderBgPath();

	AppResource *pAppResource  = Application::GetInstance()->GetAppResource();
	Bitmap *pBmpRefresh = pAppResource->GetBitmapN(PATH_ICON_REFRESH_NORMAL);
	Bitmap *pBmpRefreshPressed = pAppResource->GetBitmapN(PATH_ICON_REFRESH_PRESSED);
	Bitmap *pBmpLogo = pAppResource->GetBitmapN(iconPath);

	__listGarbage.Add(*pBmpRefreshPressed);
	__listGarbage.Add(*pBmpRefresh);
	__listGarbage.Add(*pBmpLogo);

	ButtonItem  buttonItem;
	buttonItem.Construct(BUTTON_ITEM_STYLE_ICON, ID_REFRESH);
	buttonItem.SetIcon(BUTTON_ITEM_STATUS_NORMAL, pBmpRefresh);
	buttonItem.SetIcon(BUTTON_ITEM_STATUS_PRESSED, pBmpRefreshPressed);

	Header *pHeader = GetHeader();

	if(headerBgPath.GetLength() != 0)
	{
		pHeader->SetStyle(HEADER_STYLE_TITLE);
		pHeader->SetButton(BUTTON_POSITION_RIGHT, buttonItem);
		pHeader->SetColor(colorHeader);
		pHeader->SetButtonColor(BUTTON_ITEM_STATUS_NORMAL, colorHeader);

		Bitmap *pBmpBgImage = pAppResource->GetBitmapN(headerBgPath);
		__listGarbage.Add(*pBmpBgImage);
		pHeader->SetBackgroundBitmap(pBmpBgImage);
	}
	else
	{
		pHeader->SetStyle(HEADER_STYLE_TITLE);
		pHeader->SetButton(BUTTON_POSITION_RIGHT, buttonItem);
		pHeader->SetColor(colorHeader);
		pHeader->SetButtonColor(BUTTON_ITEM_STATUS_NORMAL, colorHeader);
		pHeader->SetTitleIcon(pBmpLogo);
		pHeader->SetTitleText(title);
	}
	pHeader->AddActionEventListener(*this);

	__pTabBar = new TabBar();
	__pTabBar->Construct(0, 0, GetWidth());
	AddControl(*__pTabBar);
	if(__pTabBar)
	{
		pTitles = new LinkedList();
		if(__pControllerForm->GetPageTitles(pTitles) == false)
		{
			AppLog("GetPageTitles FAILED");
			pTitles->RemoveAll(true);
			delete pTitles;
			return E_FAILURE;
		}

		for(; i<pTitles->GetCount(); i++)
		{
			TabBarItem tabBarItem;
			title = *(static_cast<String*>(pTitles->GetAt(i)));
			tabBarItem.Construct(title, ID_TABBAR_START + i);
			__pTabBar->AddItem(tabBarItem);
		}

		__pTabBar->AddActionEventListener(*this);
		__pTabBar->SetItemSelected(0);

		pTitles->RemoveAll(false);
		delete pTitles;

	}

	return r;

}

result
ViewMainForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	__listGarbage.RemoveAll(true);

	return r;
}

PagePanel*
ViewMainForm::CreatePage(PageLoadData *pPageDetails)
{
	if(!pPageDetails)
	{
		AppLog("pPageDetails NULL");
		return null;
	}

	__colorPagetitle = pPageDetails->__colorPagetitle;
	__selfIndex = pPageDetails->__selfIndex;

	SetBgColors();

	__pControllerForm->FetchData(__selfIndex);

	PagePanel *pPagePanel = CreatePanel(pPageDetails);
	AddControl(*pPagePanel);
	SetActivePanel(pPagePanel);

	return pPagePanel;
}



PagePanel*
ViewMainForm::CreatePanel(PageLoadData *pPageDetails)
{
	PagePanel *pNewPanel = null;

	pNewPanel = new PagePanel();
	pNewPanel->Initialize(pPageDetails);
	AddControl(*pNewPanel);

	RelativeLayout* pRelativeLayout = dynamic_cast<RelativeLayout*>(this->GetLayoutN());

	pRelativeLayout->SetHorizontalFitPolicy(*pNewPanel, FIT_POLICY_PARENT);
	pRelativeLayout->SetRelation(*pNewPanel, *this, RECT_EDGE_RELATION_BOTTOM_TO_BOTTOM);
	pRelativeLayout->SetRelation(*pNewPanel, *__pTabBar, RECT_EDGE_RELATION_TOP_TO_BOTTOM);

	delete pRelativeLayout;

	return pNewPanel;

}

bool
ViewMainForm::SetActivePanel(PagePanel *pPanel)
{
	if(!pPanel)
	{
		AppLog("pPanel NULL");
		return false;
	}
	else if(pPanel->GetParent() != this)
	{
		AppLog("Invalid Panel pointer");
		return false;
	}

	if(__pActivePanel)
	{
		__pActivePanel->SetShowState(false);
	}

	__pActivePanel = pPanel;
	__pActivePanel->SetShowState(true);
	__pActivePanel->Draw();

	SetBgColors();
	UpdateAnimation();

	Draw();
	Show();

	return true;
}

void
ViewMainForm::RemovePanel(PagePanel *pPanel)
{
	if(pPanel && (pPanel->GetParent() == this))
	{
		RemoveControl(*pPanel);
	}
}

void
ViewMainForm::UpdateAnimation(void)
{
	Header *pHeader = GetHeader();
	int index = __pControllerForm->GetActivePageIndex();
	PageState pageState = __pControllerForm->GetPageState(index);

	switch(pageState)
	{
		case PAGE_STATE_INITIALIZED:
		case PAGE_STATE_FETCH_COMPLETE:
		case PAGE_STATE_FETCH_FAIL:
		{
			pHeader->StopWaitingAnimation(HEADER_ANIMATION_POSITION_BUTTON_RIGHT);
		}
		break;
		case PAGE_STATE_FETCH_ONGOING:
		{
			pHeader->PlayWaitingAnimation(HEADER_ANIMATION_POSITION_BUTTON_RIGHT);
		}
		break;

		default:
		{
			AppLog("Invalid Page State");
		}
		break;
	}
	Draw();
	Show();

}

void
ViewMainForm::SetBgColors(void)
{
	Color titleColor(Color::COLOR_WHITE);

	if(__colorPagetitle.GetRed() >  190 && __colorPagetitle.GetBlue() >  190 && __colorPagetitle.GetGreen() >  190)
	{
		titleColor.SetColorComponents(0, 0, 0);
	}

	__pTabBar->SetColor(__colorPagetitle);
	__pTabBar->SetSelectedItemColor(titleColor);
	__pTabBar->SetItemTextColor(TAB_BAR_ITEM_STATUS_NORMAL, titleColor);
	__pTabBar->SetItemTextColor(TAB_BAR_ITEM_STATUS_SELECTED, __colorPagetitle);

}

void
ViewMainForm::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	if(actionId >= ID_TABBAR_START)
	{
		__colorPagetitle = __pControllerForm->GetPageTitleColor(actionId - ID_TABBAR_START);
		SetBgColors();
		__pControllerForm->LoadPage(actionId - ID_TABBAR_START);
	}
	else if(actionId == ID_REFRESH)
	{
		__pActivePanel->ClearData();
		__pControllerForm->FetchData(__pControllerForm->GetActivePageIndex());
		UpdateAnimation();
	}
}

void
ViewMainForm::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	switch(requestId)
	{
	case NOTIFY_TEXT_COMPLETE:
		{
			if(pArgs == null)
			{
				AppLog("INVALID ARGUMENT");
				return;
			}
			if(pArgs->GetCount() == 0)
			{
				AppLog("INVALID ARGUMENT");
				return;
			}

			PagePanel *pPanel = (static_cast<PagePanel*>(pArgs->GetAt(3)));
			pPanel->Notify(requestId, pArgs);

			delete pArgs;
		}
		break;

	case NOTIFY_IMAGE_COMPLETE:
		{
			if(pArgs == null)
			{
				AppLog("INVALID ARGUMENT");
				return;
			}
			if(pArgs->GetCount() == 0)
			{
				AppLog("INVALID ARGUMENT");
				return;
			}

			PagePanel *pPanel = (static_cast<PagePanel*>(pArgs->GetAt(2)));
			pPanel->Notify(requestId, pArgs);

			UpdateAnimation();
			pArgs->RemoveAt(1, true);

			delete pArgs;

		}
		break;
	case NOTIFY_NETWORK_FAIL:
		{
			String errorMsg;
			Application::GetInstance()->GetAppResource()->GetString(L"IDS_NETWORK_ERROR", errorMsg);
			__pControllerForm->ShowPopup(errorMsg);
			UpdateAnimation();
		}
		break;

	default:
		break;
	}
}

int
ViewMainForm::ToInt(char* pString)
{
	int num = 0;
	unsigned int i = 0;

	if(pString == null)
	{
		AppLog("NULL STRING");
		return -1;
	}

	for(i=0; i<strlen(pString); i++)
	{
		if(pString[i] >= '0' && pString[i] <= '9')
		{
			num = num * 10 + (pString[i] - '0');
		}
		else
		{
			AppLog("Invalid Input String");
			return -1;
		}
	}

	return num;

}
