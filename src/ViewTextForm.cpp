#include "ViewTextForm.h"
#include "ImageViewer.h"

using namespace Osp::Graphics;
using namespace Osp::App;
using namespace Osp::Io;
using namespace Osp::Base::Collection;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::Web::Controls;
using namespace Osp::Media;


bool
ViewTextForm::Initialize(ArticleData *pArticleArgs)
{
	__pShareMenu = null; __pWeb = null; __pTextMenu = null;
	__pBmpZoomIn = null; __pBmpZoomOut = null; __pBmpZoomInPress = null; __pBmpZoomOutPress = null;
	__zoomEnabled = false;
	__longPressed = false;

	__headFontSize = MIN_HEAD_FONT_SIZE;
	__bodyFontSize = MIN_BODY_FONT_SIZE;

	if(pArticleArgs == null)
	{
		AppLog("pArticleArgs NULL");
		return false;
	}

	__headline = pArticleArgs->__headline;
	__fullText = pArticleArgs->__description;
	__date = pArticleArgs->__time;
	__link = pArticleArgs->__link;
	__imgPath = pArticleArgs->__imagePath;
	__auth = pArticleArgs->__author;
	__colorBody = pArticleArgs->__colorBody;
	__colorFont = pArticleArgs->__colorFont;
	__colorHeader = pArticleArgs->__colorHeader;
	__colorPagetitle = pArticleArgs->__colorPagetitle;

	result res = E_SUCCESS;

	if((res = Construct(L"ArticleForm")) != E_SUCCESS)
	{
		AppLog("Form Create Failed: %s", GetErrorMessage(res));
		return false;
	}
	return true;
}

result
ViewTextForm::OnInitializing(void)
{
	SetFormBackEventListener(this);
	SetBackgroundColor(__colorBody);

	String headFontSize("HeadFontSize");
	result r = E_SUCCESS;
	AppRegistry *pAppRegistry = Application::GetInstance()->GetAppRegistry();

	r = pAppRegistry->Get(headFontSize, __headFontSize);
	if ( r == E_KEY_NOT_FOUND)
	{
		pAppRegistry->Add(headFontSize, __headFontSize);
	}
	__bodyFontSize = __headFontSize - 2;

	__pWeb = static_cast<Web*>(GetControl(L"IDC_WEB1"));
	if(!__pWeb)
	{
		AppLog("No Web Control!");
		return E_FAILURE;
	}
	__pWeb->SetLoadingListener(this);

	Touch touch;
	touch.SetMultipointEnabled(*__pWeb, true);
	__pWeb->AddTouchEventListener(*this);

	__pSearchPanel = static_cast<Panel*>(GetControl(L"IDC_PANEL1"));
	if(__pSearchPanel)
	{
		__pSearchPanel->SetBackgroundColor(__colorHeader);
		__pSearchPanel->SetShowState(false);
		__pSearchPanel->Draw();

		RelativeLayout* pRelativeLayout = dynamic_cast<RelativeLayout*>(this->GetLayoutN());
		pRelativeLayout->SetRelation(*__pWeb, *this, RECT_EDGE_RELATION_TOP_TO_TOP);
		delete pRelativeLayout;

		__pButtonPrevious = static_cast<Button*>(__pSearchPanel->GetControl(L"IDC_BTNPREV"));
		if(__pButtonPrevious)
		{
			__pButtonPrevious->SetActionId(ID_PREVIOUS);
			__pButtonPrevious->AddActionEventListener(*this);
		}

		__pButtonNext = static_cast<Button*>(__pSearchPanel->GetControl(L"IDC_BTNNEXT"));
		if(__pButtonNext)
		{
			__pButtonNext->SetActionId(ID_NEXT);
			__pButtonNext->AddActionEventListener(*this);
		}

		Button *pBtnClose = static_cast<Button*>(__pSearchPanel->GetControl(L"IDC_BTNCLOSE"));
		if(pBtnClose)
		{
			pBtnClose->SetActionId(ID_CLOSESEARCH);
			pBtnClose->AddActionEventListener(*this);
		}

		__pEditSearch = static_cast<EditField*>(__pSearchPanel->GetControl(L"IDC_EDITSEARCH"));
		if(__pEditSearch)
		{
			__pEditSearch->AddTextEventListener(*this);
			__pEditSearch->AddKeypadEventListener(*this);
			__pEditSearch->SetOverlayKeypadCommandButtonVisible(false);
		}

		Label *pLblSearch = static_cast<Label*>(__pSearchPanel->GetControl(L"IDC_LBLSEARCH"));
		if(pLblSearch)
		{
			pLblSearch->SetBackgroundColor(__colorHeader);
		}
	}

	Footer *pFooter = GetFooter();
	pFooter->AddActionEventListener(*this);
	LoadFooter();

	GetNormalString(__fullText);
	Cleanup(__fullText);

	LoadPage();


	return E_SUCCESS;
}

result
ViewTextForm::OnTerminating(void)
{

	if(__pShareMenu)
	{
		__pShareMenu->CancelBTPush();
		delete __pShareMenu;
	}

	AppRegistry *pAppRegistry = Application::GetInstance()->GetAppRegistry();
	String headFontSize("HeadFontSize");
	pAppRegistry->Set(headFontSize, __headFontSize);

	__listGarbage.RemoveAll(true);

	return E_SUCCESS;
}

void
ViewTextForm::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	switch(actionId)
	{
		case ID_SHARE:
		{
			ShowShareMenu();
		}
		break;

		case ID_SEARCH:
		{
			HandleSearch();
		}
		break;

		case ID_ZOOM:
		{
			HandleZoom();
		}
		break;


		case ID_ZOOM_IN:
		{
			if(__headFontSize - 2 >= MIN_HEAD_FONT_SIZE)
			{
				__headFontSize -= 2;
				__bodyFontSize -= 2;
				LoadPage();
				Draw();
				Show();
			}
		}
		break;

		case ID_ZOOM_OUT:
		{
			if(__headFontSize + 2 < MAX_HEAD_FONT_SIZE)
			{
				__headFontSize += 2;
				__bodyFontSize += 2;
				LoadPage();
				Draw();
				Show();
			}
		}
		break;

		case ID_COPY:
		{
			CopyToClipboard();
		}
		break;

		case ID_TRANSLATE:
		{
			SearchInternet();
		}
		break;

		case ID_CLOSESEARCH:
		{
			CloseSearchBar();
		}
		break;

		case ID_PREVIOUS:
		{
			__pWeb->SearchText(__pEditSearch->GetText(), false);
		}
		break;

		case ID_NEXT:
		{
			__pWeb->SearchText(__pEditSearch->GetText());
		}
		break;

		default:
			break;
	}
}

void
ViewTextForm::OnFormBackRequested(Osp::Ui::Controls::Form& source)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();

	if(__zoomEnabled == true)
	{
		__zoomEnabled = false;
		LoadFooter();
	}
	else
	{
		pFrame->RemoveControl(*this);
	}
}

void
ViewTextForm::LoadPage(void)
{
	String htmlPage;
	String bgColorCSS(30), fgColorCSS(30);

	htmlPage.EnsureCapacity(__fullText.GetLength() + __imgPath.GetLength() + 400);

	bgColorCSS.Format(30, L"rgb(%d, %d, %d);", __colorBody.GetRed(), __colorBody.GetGreen(), __colorBody.GetBlue());
	fgColorCSS.Format(30, L"rgb(%d, %d, %d);", __colorFont.GetRed(), __colorFont.GetGreen(), __colorFont.GetBlue());
	htmlPage = L"<HTML>";
	htmlPage += L"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />";
	htmlPage += L"<META name=\"viewport\" content=\"width=device-width, initial-scale=1.0 \" />";

	htmlPage += "<style> head {padding-top:5px; padding-right:10px; padding-left:10px; font-size:18px;}</style>";

	htmlPage += "<BODY>";
	htmlPage += "<style> .postBody img { max-width: 280px; height: auto; display: block; margin-left: 12px; margin-right: 12px;}";

	htmlPage += "p.head {padding-top:5px; padding-right:12px; padding-left:12px; font-size:";
	htmlPage.Append(__headFontSize);
	htmlPage += "px;}";

	htmlPage += "p.authdate {padding-top:1px; padding-right:12px; padding-left:12px; font-size:";
	htmlPage.Append(__bodyFontSize);
	htmlPage += "px;}";

	htmlPage += "p {text-align:left-align; padding-top:1px; padding-bottom:1px;  padding-right:10px; padding-left:10px; font-size:";
	htmlPage.Append(__bodyFontSize);
	htmlPage += "px;}";

	htmlPage += "ul.main {list-style-type:disc; list-style-position: outside; margin: 10 10 20 20px}";
	htmlPage += "blockquote {text-align:justify; width: 220px; font-size:";
	htmlPage.Append(__bodyFontSize);
	htmlPage += "px;}";

	htmlPage += "body {background-color: ";
	htmlPage += bgColorCSS;
	htmlPage += " color: ";
	htmlPage += fgColorCSS;
	htmlPage += "}";

	htmlPage += "</style>";

	htmlPage += "<p class = \"head\"><b>";
	htmlPage += __headline;
	htmlPage += "</b></p>";

	htmlPage += "<p class = \"authdate\">";
	htmlPage += __auth;
	htmlPage += "<br>";
	htmlPage += __date;
	htmlPage += "</p>";

	htmlPage += "<div class = \"postBody\"><p>";
	if(__imgPath.GetLength() > 1)
	{

		htmlPage += "<center><img src = \"";
		htmlPage += __imgPath;
		htmlPage += "\"></center>";
	}

	htmlPage += __fullText;

	htmlPage += "</p></div></BODY></HTML>";

	File htmlSaveFile;
	htmlSaveFile.Construct(ARTICLE_FILE_PATH, L"w+");
	htmlSaveFile.Write(htmlPage);

	__pWeb->LoadUrl(ARTICLE_FILE_PATH);

}

void
ViewTextForm::LoadFooter(void)
{
	Footer *pFooter = GetFooter();

	if(!pFooter)
	{
		AppLog("No Footer available!");
		return;
	}

	if(__zoomEnabled == false)
	{
		FooterItem itemShare, itemSearch, itemZoom;
		String share, search, zoom;

		pFooter->SetStyle(FOOTER_STYLE_BUTTON_TEXT);

		Application::GetInstance()->GetAppResource()->GetString(L"IDS_SHARE", share);
		Application::GetInstance()->GetAppResource()->GetString(L"IDS_SEARCH", search);
		Application::GetInstance()->GetAppResource()->GetString(L"IDS_ZOOM", zoom);

		itemShare.Construct(ID_SHARE);
		itemSearch.Construct(ID_SEARCH);
		itemZoom.Construct(ID_ZOOM);

		itemShare.SetText(share);
		itemSearch.SetText(search);
		itemZoom.SetText(zoom);

		pFooter->AddItem(itemShare);
		pFooter->AddItem(itemSearch);
		pFooter->AddItem(itemZoom);

	}
	else
	{
		FooterItem itemZoomIn, itemZoomOut;

		itemZoomIn.Construct(ID_ZOOM_IN);
		itemZoomOut.Construct(ID_ZOOM_OUT);

		pFooter->SetStyle(FOOTER_STYLE_SEGMENTED_ICON);

		if(!__pBmpZoomIn)
		{
			AppResource *pAppResource  = Application::GetInstance()->GetAppResource();

			__pBmpZoomIn = pAppResource->GetBitmapN(PATH_ICON_ZOOMIN_NORMAL);
			__pBmpZoomInPress = pAppResource->GetBitmapN(PATH_ICON_ZOOMIN_PRESSED);
			__pBmpZoomOut = pAppResource->GetBitmapN(PATH_ICON_ZOOMOUT_NORMAL);
			__pBmpZoomOutPress = pAppResource->GetBitmapN(PATH_ICON_ZOOMOUT_PRESSED);

			__listGarbage.Add(*__pBmpZoomIn);
			__listGarbage.Add(*__pBmpZoomInPress);
			__listGarbage.Add(*__pBmpZoomOut);
			__listGarbage.Add(*__pBmpZoomOutPress);
		}

		itemZoomIn.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pBmpZoomIn);
		itemZoomIn.SetIcon(FOOTER_ITEM_STATUS_PRESSED, __pBmpZoomInPress);
		itemZoomOut.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pBmpZoomOut);
		itemZoomOut.SetIcon(FOOTER_ITEM_STATUS_PRESSED, __pBmpZoomOutPress);

		pFooter->AddItem(itemZoomOut);
		pFooter->AddItem(itemZoomIn);
	}

	pFooter->SetBackButton();
	pFooter->SetItemColor(FOOTER_ITEM_STATUS_NORMAL, Color::COLOR_BLACK);
	pFooter->SetItemColor(FOOTER_ITEM_STATUS_SELECTED, Color::COLOR_BLACK);
	pFooter->SetButtonColor(BUTTON_ITEM_STATUS_NORMAL, Color::COLOR_BLACK);
	pFooter->SetColor(FOOTER_COLOR);

	Draw(); Show();
}

void
ViewTextForm::GetNormalString(String& htmlPage)
{
	if(htmlPage.GetCapacity() == 0)
		return;
	htmlPage.Replace(L"&nbsp;", L" ");
	htmlPage.Replace(L"&quot;", L"\"");
	htmlPage.Replace(L"&apos;", L"'");
	htmlPage.Replace(L"&amp;", L"&");
	htmlPage.Replace(L"&lt;", L"<");
	htmlPage.Replace(L"&gt;", L">");
}

void
ViewTextForm::Cleanup(String& htmlPage)
{
	int index = 0;
	String strIFrame("<iframe"), strIFrameEnd("</iframe>");
	String strA("<a"), strAEnd("</a>");
	int indexOf = 0, indexOf2 = 0;
	result res = E_SUCCESS;

	while(1)
	{
		ClearDiv(0, htmlPage);
		htmlPage.IndexOf(L"</div", 0, index);
		if(index == -1)
			break;
	}

	while(1)
	{
		if((res = htmlPage.IndexOf(strIFrame, 0, indexOf)) != E_SUCCESS)
			break;

		htmlPage.IndexOf(L">", indexOf, indexOf2);
		htmlPage.Insert('\n', indexOf);
		htmlPage.Remove(indexOf+1, indexOf2 - indexOf +  1);

		htmlPage.IndexOf(strIFrameEnd, 0, indexOf);
		htmlPage.Remove(indexOf, strIFrameEnd.GetLength());
	}

	while(1)
	{
		if((res = htmlPage.IndexOf(strA, 0, indexOf)) != E_SUCCESS)
			break;

		htmlPage.IndexOf(L">", indexOf, indexOf2);
		htmlPage.Remove(indexOf, indexOf2 - indexOf +  1);

		htmlPage.IndexOf(strAEnd, 0, indexOf);
		htmlPage.Remove(indexOf, strAEnd.GetLength());
	}
}

void
ViewTextForm::ClearDiv(const int index, String& desc)
{
	String strDiv("<div"), strDivEnd("</div>");
	int indexOfStart = 0, indexOfEnd = 0;
	result res = E_SUCCESS;

	if((res = desc.IndexOf(strDiv, index, indexOfStart)) == E_SUCCESS)
	{
		ClearDiv(indexOfStart + strDiv.GetLength(), desc);
		if((res = desc.IndexOf(strDivEnd, indexOfStart, indexOfEnd)) == E_SUCCESS)
		{
			desc.Remove(indexOfStart, indexOfEnd + strDivEnd.GetLength() - indexOfStart);
		}
	}
	else
	{
		return;
	}

}

bool
ViewTextForm::OnHttpAuthenticationRequestedN(const Osp::Base::String& host, const Osp::Base::String& realm, const Osp::Web::Controls::AuthenticationChallenge& authentication)
{

	return false;
}

void
ViewTextForm::OnHttpAuthenticationCanceled()
{

}

void
ViewTextForm::OnLoadingStarted(void)
{

}

void
ViewTextForm::OnLoadingCanceled(void)
{

}

void
ViewTextForm::OnLoadingErrorOccurred(LoadingErrorType error, const String& reason)
{
	String errorMsg;
	Application::GetInstance()->GetAppResource()->GetString(L"IDS_NETWORK_ERROR", errorMsg);
	ControllerForm *pControllerForm = GET_CONTROLLER_FORM;
	pControllerForm->ShowPopup(errorMsg);

}

void
ViewTextForm::OnLoadingCompleted(void)
{

}

void
ViewTextForm::OnEstimatedProgress(int progress)
{

}

void
ViewTextForm::OnPageTitleReceived(const Osp::Base::String& title)
{

}

bool
ViewTextForm::OnLoadingRequested(const Osp::Base::String& url, WebNavigationType type)
{

	return false;
}

DecisionPolicy
ViewTextForm::OnWebDataReceived(const Osp::Base::String& mime, const Osp::Net::Http::HttpHeader& header)
{

	return WEB_DECISION_CONTINUE;
}
void
ViewTextForm::OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo)
{

}

void
ViewTextForm::OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo)
{

}

void
ViewTextForm::OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo)
{

}

void
ViewTextForm::OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo)
{
	__longPressed = true;
	__pWeb->SetBlockSelectionPosition(currentPosition);
	__pWeb->ConsumeInputEvent();
}

void
ViewTextForm::OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo)
{
	if(__pWeb->GetTextFromBlock().GetLength() > 0)
		__longPressed = true;
}

void
ViewTextForm::OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo)
{
	if(__pSearchPanel && __pSearchPanel->GetShowState() == true)
		CloseSearchBar();

}

void
ViewTextForm::OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo)
{
	HitElementResult *hitElement = __pWeb->GetElementByPointN(currentPosition);
	Bitmap *pTouchedImage = null;

	if(hitElement)
		pTouchedImage = const_cast<Bitmap*>(hitElement->GetImage());


	if(__longPressed == true)
	{
		ShowTextMenu(currentPosition);
	}
	else if(pTouchedImage)
	{
		Frame *pFrame = (Application::GetInstance()->GetAppFrame()->GetFrame());
		ImageViewer *pImageViewer = new ImageViewer();
		pImageViewer->Initialize(pTouchedImage, __colorBody);
		pFrame->AddControl(*pImageViewer);
		pFrame->SetCurrentForm(*pImageViewer);
		pImageViewer->Draw();
		pImageViewer->Show();

	}

	delete hitElement;
	__longPressed = false;
}
void
ViewTextForm::OnTextValueChanged(const Osp::Ui::Control& source)
{
	if(__pEditSearch->GetTextLength() > 0)
	{
		__pButtonPrevious->SetEnabled(true);
		__pButtonNext->SetEnabled(true);
		__pSearchPanel->Draw();
		__pSearchPanel->Show();
	}
	else
	{
		__pButtonPrevious->SetEnabled(false);
		__pButtonNext->SetEnabled(false);
		__pButtonPrevious->Draw(); __pButtonPrevious->Show();
		__pButtonNext->Draw(); __pButtonNext->Show();
	}
	SetFocus();
}

void
ViewTextForm::OnTextValueChangeCanceled(const Osp::Ui::Control& source)
{
	SetFocus();
}

void
ViewTextForm::OnKeypadActionPerformed (Osp::Ui::Control &source, Osp::Ui::KeypadAction keypadAction)
{

	switch(keypadAction)
	{
		case KEYPAD_ACTION_SEARCH:
		{
			__pWeb->SearchText(__pEditSearch->GetText());
			__pEditSearch->HideKeypad();
			GetFooter()->SetShowState(false);
			Draw(); Show();

		}
		break;

		default:
			break;
	}
}

void
ViewTextForm::OnKeypadClosed (Osp::Ui::Control &source)
{

}

void
ViewTextForm::OnKeypadOpened (Osp::Ui::Control &source)
{

}

void
ViewTextForm::OnKeypadWillOpen (Osp::Ui::Control &source)
{

}

void
ViewTextForm::OnWebPageBlockSelected (Osp::Web::Controls::Web &source, Osp::Graphics::Point &startPoint, Osp::Graphics::Point &endPoint)
{

}

void
ViewTextForm::OnWebPageShowRequested (Osp::Web::Controls::Web &source)
{

}

void
ViewTextForm::OnWebWindowCloseRequested (Osp::Web::Controls::Web &source)
{

}

Osp::Web::Controls::Web *
ViewTextForm::OnWebWindowCreateRequested (void)
{

	return __pWeb;
}

void
ViewTextForm::ShowShareMenu(void)
{
	if(!__pShareMenu)
	{
		Point loadPoint(50,0);
		loadPoint.y = GetFooter()->GetY() + 40;

		__pShareMenu = new ShareMenu();
		__pShareMenu->Initialize(loadPoint, __link, true);
		__pShareMenu->Show();
	}
	else
	{
		__pShareMenu->SetShowState(true);
		__pShareMenu->Show();
		Draw(); Show();
	}

}

void
ViewTextForm::ShowTextMenu(Point menuPos)
{
	if(__pTextMenu)
	{
		delete __pTextMenu;
	}

	String copy, search, translate;
	AppResource *pAppResource = Application::GetInstance()->GetAppResource();

	pAppResource->GetString(L"IDS_COPY", copy);
	pAppResource->GetString(L"IDS_TEXTSEARCH", search);
	pAppResource->GetString(L"IDS_TRANSLATE", translate);

	__pTextMenu = new ContextMenu();
	__pTextMenu->Construct(Point(menuPos.x, menuPos.y + __pWeb->GetY() + 40 /* height of status bar */), CONTEXT_MENU_STYLE_LIST );
	__pTextMenu->AddItem(copy, ID_COPY);
	__pTextMenu->AddItem(search, ID_SEARCH);
	__pTextMenu->AddItem(translate, ID_TRANSLATE);
	__pTextMenu->AddActionEventListener(*this);
	__pTextMenu->SetColor(__colorHeader);
	__pTextMenu->Show();

}

void
ViewTextForm::CopyToClipboard(void)
{
	ClipboardItem item;
	item.Construct(CLIPBOARD_DATA_TYPE_TEXT, __pWeb->GetTextFromBlock());

	Clipboard* pClipboard = Clipboard::GetInstance();
	pClipboard->CopyItem(item);

	__pWeb->ReleaseBlock();
}

void
ViewTextForm::SearchInternet(void)
{
	String searchString("http://www.google.com/search?q=");
	searchString.SetCapacity(searchString.GetCapacity() + __pWeb->GetTextFromBlock().GetCapacity());
	searchString += __pWeb->GetTextFromBlock();
	searchString.Replace(L' ', L'+');

	ArrayList* pDataList = null;
	pDataList = new ArrayList();
	pDataList->Construct();

	String* pData = null;
	pData = new String(L"url:");
	pData->SetCapacity(pData->GetCapacity() + searchString.GetCapacity());
	pData->Insert(searchString, 4);
	pDataList->Add(*pData);

	AppControl* pAc = AppManager::FindAppControlN(APPCONTROL_PROVIDER_BROWSER, APPCONTROL_OPERATION_VIEW);
	if(pAc)
	{
	  pAc->Start(pDataList, null);
	  delete pAc;
	}
	pDataList->RemoveAll(true);
	delete pDataList;

}

void
ViewTextForm::CloseSearchBar(void)
{
	if(__pSearchPanel)
	{
		__pSearchPanel->SetShowState(false);
		__pSearchPanel->Draw();

		RelativeLayout* pRelativeLayout = dynamic_cast<RelativeLayout*>(this->GetLayoutN());
		pRelativeLayout->SetRelation(*__pWeb, *this, RECT_EDGE_RELATION_TOP_TO_TOP);

		__pEditSearch->HideKeypad();

		GetFooter()->SetShowState(true);

		Draw(); Show();

		delete pRelativeLayout;
	}
}

void
ViewTextForm::HandleSearch(void)
{
	if(!__pSearchPanel)
	{
		AppLog("No Search Panel");
		return;
	}

	if(__pEditSearch->GetTextLength() < 1)
	{
		__pButtonPrevious->SetEnabled(false);
		__pButtonNext->SetEnabled(false);
	}

	__pSearchPanel->SetShowState(true);
	__pSearchPanel->Draw();

	RelativeLayout* pRelativeLayout = dynamic_cast<RelativeLayout*>(this->GetLayoutN());
	pRelativeLayout->SetRelation(*__pWeb, *__pSearchPanel, RECT_EDGE_RELATION_TOP_TO_BOTTOM);

	GetFooter()->SetShowState(false);

	Draw();
	Show();

	delete pRelativeLayout;
}

void
ViewTextForm::HandleZoom(void)
{
	__zoomEnabled = true;
	LoadFooter();
}
