#include "ControllerForm.h"
#include "ViewMainForm.h"
#include "stdlib.h"
#include <FXml.h>

using namespace Osp::Xml;
using namespace Osp::App;
using namespace Osp::Graphics;
using namespace Osp::Media;
using namespace Osp::Ui::Controls;
using namespace Osp::Ui;
using namespace Osp::Base;
using namespace Osp::Base::Collection;

ControllerForm::ControllerForm(void)
{
}

ControllerForm::~ControllerForm(void)
{
}

bool
ControllerForm::Initialize(void)
{
	result res = E_SUCCESS;

	__activePageIndex = -1;

	if((res = Form::Construct(FORM_STYLE_NORMAL)) != E_SUCCESS)
	{
		AppLog("ControllerForm Create Failed [%s]", GetErrorMessage(res));
		return false;
	}
	SetName(CONTROLLER_FORM);


	return true;
}


void
ControllerForm::GetColorsFromNode(xmlNodePtr pCurNode, Osp::Graphics::Color& color)
{
	xmlAttrPtr pProperties = null;
	int r = 0, g = 0, b = 0;

	if(!pCurNode)
		return;

	for(pProperties = pCurNode->properties; pProperties != null; pProperties = pProperties->next)
	{
		if(stricmp((const char*)(pProperties->name), ATTRIBUTE_RED) == 0)
		{
			r = atoi((char*)pProperties->children->content);
		}
		else if(stricmp((const char*)(pProperties->name), ATTRIBUTE_GREEN) == 0)
		{
			g = atoi((char*)pProperties->children->content);
		}
		else if(stricmp((const char*)(pProperties->name), ATTRIBUTE_BLUE) == 0)
		{
			b = atoi((char*)pProperties->children->content);
		}
	}

	color.SetColorComponents(r, g, b);
}

bool
ControllerForm::ParseProfile(void)
{
	xmlDocPtr pDoc = null;
	xmlNodePtr pCurNode = null, pPageNode = null, pHeaderNode = null, pSubPageNode = null;
	xmlAttrPtr pMainProperties = null, pSubProperties = null;
	PageDetails *pDetails = null;

	pDoc = xmlParseFile(PROFILE_XML_PATH);

	if (pDoc == null)
	{
		AppLog("xmlParseFile error");
		return false;
	}

	pCurNode = xmlDocGetRootElement(pDoc);
	if (pCurNode == null)
	{
		xmlFreeDoc(pDoc);
		return false;
	}

	for(pCurNode = pCurNode->children; pCurNode != null; pCurNode = pCurNode->next)
	{
		if(stricmp((const char*)(pCurNode->name), TAG_HEADER) == 0)
		{
			for(pMainProperties = pCurNode->properties; pMainProperties != null; pMainProperties = pMainProperties->next)
			{
				if(stricmp((const char*)(pMainProperties->name), ATTRIBUTE_TITLE) == 0)
				{
					Utility::StringUtil::Utf8ToString
									((const char*)pMainProperties->children->content, __title);
				}
				else if(stricmp((const char*)(pMainProperties->name), ATTRIBUTE_ICON_PATH) == 0)
				{
					Utility::StringUtil::Utf8ToString
									((const char*)pMainProperties->children->content, __headerIconPath);
				}
				else
				{
					delete pDetails;
					xmlFreeDoc(pDoc);
					return false;
				}
			}
			for(pHeaderNode = pCurNode->children; pHeaderNode != null; pHeaderNode = pHeaderNode->next)
			{
				if(stricmp((const char*)(pHeaderNode->name), TAG_HEADER_COLOR) == 0)
				{
					GetColorsFromNode(pHeaderNode, __colorHeader);
				}
				else if(stricmp((const char*)(pHeaderNode->name), TAG_HEADER_IMAGE) == 0)
				{
					for(pSubProperties = pHeaderNode->properties; pSubProperties != null; pSubProperties = pSubProperties->next)
					{
						if(stricmp((const char*)(pSubProperties->name), ATTRIBUTE_SOURCE) == 0)
						{
							Utility::StringUtil::Utf8ToString
									((const char*)pSubProperties->children->content, __headerBgImagePath);
						}
					}
				}
			}
		}
		else if(stricmp((const char*)(pCurNode->name), TAG_PAGE) == 0)
		{
			pDetails = new PageDetails();
			pDetails->Initialize();

			pDetails->__colorHeader = __colorHeader;

			for(pSubProperties = pCurNode->properties; pSubProperties != null; pSubProperties = pSubProperties->next)
			{
				if(stricmp((const char*)(pSubProperties->name), ATTRIBUTE_LAYOUT) == 0)
				{
					Utility::StringUtil::Utf8ToString
									((const char*)pSubProperties->children->content, pDetails->__pageLayout);
				}
				else if(stricmp((const char*)(pSubProperties->name), ATTRIBUTE_TYPE) == 0)
				{
					Utility::StringUtil::Utf8ToString
									((const char*)pSubProperties->children->content, pDetails->__pageType);

				}
				else if(stricmp((const char*)(pSubProperties->name), ATTRIBUTE_TITLE) == 0)
				{
					Utility::StringUtil::Utf8ToString
									((const char*)pSubProperties->children->content, pDetails->__pageTitle);
				}
				else
				{
					delete pDetails;
					xmlFreeDoc(pDoc);
					return false;
				}
			}

			for(pPageNode = pCurNode->children; pPageNode != null; pPageNode = pPageNode->next)
			{
				if(stricmp((const char*)(pPageNode->name), TAG_SOURCE) == 0)
				{
					Utility::StringUtil::Utf8ToString
									((const char*)pPageNode->children->content, pDetails->__pageSource);
				}
				else if(stricmp((const char*)(pPageNode->name), TAG_PAGE_TITLE) == 0)
				{
					GetColorsFromNode(pPageNode, pDetails->__colorPagetitle);
				}
				else if(stricmp((const char*)(pPageNode->name), TAG_BODY) == 0)
				{
					GetColorsFromNode(pPageNode, pDetails->__colorBody);

					for(pSubPageNode = pPageNode->children; pSubPageNode != null; pSubPageNode = pSubPageNode->next)
					{
						if(stricmp((const char*)(pSubPageNode->name), TAG_FONT) == 0)
						{
							GetColorsFromNode(pSubPageNode, pDetails->__colorFont);
						}
					}
				}
			}

			pDetails->__selfIndex  = __pages.GetCount();
			__pages.Add(*(pDetails));
		}
	}

	xmlFreeDoc(pDoc);

	return true;
}

bool
ControllerForm::LoadPage(int index)
{
	PageDetails *pDetails = null;

	if(index < 0 || index >= __pages.GetCount())
	{
		AppLog("invalid index: [%d]", index);
		return false;
	}

	if(GetActivePageCount() == MAX_PANEL_COUNT)
	{
		PageDetails *pTempPage = null;
		for(int i = 0; i < __pages.GetCount(); i++)
		{
			pTempPage = static_cast<PageDetails*>(__pages.GetAt(i));
			if((pTempPage->IsAlive() == true) && i != __activePageIndex)
			{
				KillPage(i);
				break;
			}
		}
	}

	__activePageIndex = index;
	pDetails = (static_cast<PageDetails*>(__pages.GetAt(index)));
	return pDetails->Load();
}

bool
ControllerForm::KillPage(int index)
{
	PageDetails *pDetails = null;

	if(index < 0 || index >= __pages.GetCount())
	{
		AppLog("invalid Index");
		return false;
	}

	pDetails = (static_cast<PageDetails*>(__pages.GetAt(index)));
	pDetails->Destroy();

	return true;
}

int
ControllerForm::GetActivePageCount()
{
	int count = 0;
	PageDetails *pDetails = null;

	for(int i = 0; i < __pages.GetCount(); i++)
	{
		pDetails = static_cast<PageDetails*>(__pages.GetAt(i));
		if(pDetails->IsAlive() == true)
		{
			count++;
		}
	}

	return count;
}

int
ControllerForm::GetActivePageIndex(void)
{
	return __activePageIndex;
}

PageState
ControllerForm::GetPageState(int index)
{
	PageDetails *pDetails = null;

	if(index < 0 || index >= __pages.GetCount())
	{
		AppLog("invalid Index");
		return PAGE_STATE_MAX;
	}

	pDetails = (static_cast<PageDetails*>(__pages.GetAt(index)));
	return pDetails->GetState();
}

bool
ControllerForm::FetchData(int index)
{
	PageDetails *pDetails = null;

	if(index < 0 || index >= __pages.GetCount())
	{
		AppLog("invalid index: %d", index);
		return false;
	}

	pDetails = (static_cast<PageDetails*>(__pages.GetAt(index)));
	return pDetails->FetchFeed();
}

result
ControllerForm::OnInitializing(void)
{
	if(ParseProfile() == false)
		return E_FAILURE;
	else
	{
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		ViewMainForm *pForm2 = new ViewMainForm();
		pForm2->Initialize();
		pFrame->AddControl(*pForm2);
		pFrame->SetCurrentForm(*pForm2);
		pForm2->Draw();
		pForm2->Show();

		return E_SUCCESS;
	}

}

result
ControllerForm::OnTerminating(void)
{

	for(int i = 0; i < __pages.GetCount(); i++)
	{
		KillPage(i);
	}
	__pages.RemoveAll(true);

	return E_SUCCESS;
}

void
ControllerForm::OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs)
{
	if (!pArgs || (pArgs->GetCount() == 0))
	{
		AppLog("INVALID ARGUMENT");
		return;
	}

	switch(requestId)
	{
	case UPDATE_QUEUE:
		{
			PageDetails *pDetails = static_cast<PageDetails*>(pArgs->GetAt(0));
			pDetails->UpdateQCount();
			pDetails->HandleUrlImages();
			pArgs->RemoveAll(false);

		}
		break;
	default:
		AppLog("Invalid Request");
		break;
	}

	delete pArgs;
	return;
}

bool
ControllerForm::GetPageTitles(LinkedList *pTitles)
{
	PageDetails *pDetails = null;

	if(pTitles == null)
	{
		AppLog("lstTitles null");
		return false;
	}

	if(__pages.GetCount() == 0)
	{
		AppLog("PageList empty");
		return false;
	}

	for(int i=0; i<__pages.GetCount(); i++)
	{
		pDetails = static_cast<PageDetails*>(__pages.GetAt(i));
		pTitles->Add(pDetails->__pageTitle);
	}

	return true;
}

bool
ControllerForm::GetArticleData(int selfIndex, int index, ArticleData *pArticleArgs)
{
	PageDetails *pDetails = null;

	if(pArticleArgs == null)
	{
		AppLog("pArticleArgs null");
		return false;
	}

	if(selfIndex >= 0 && selfIndex < __pages.GetCount())
	{
		pDetails = (static_cast<PageDetails*>(__pages.GetAt(selfIndex)));
		return pDetails->GetTextData(index, pArticleArgs);
	}
	else
	{
		AppLog("PAGE NOT FOUND!");
		return false;
	}
}

bool
ControllerForm::GetAudioData(int selfIndex, int index, LinkedList *pArticleArgs)
{
	PageDetails *pDetails = null;

	if(pArticleArgs == null)
	{
		AppLog("pArticleArgs null");
		return false;
	}

	if(selfIndex >= 0 && selfIndex < __pages.GetCount())
	{
		pDetails = (static_cast<PageDetails*>(__pages.GetAt(selfIndex)));
		return pDetails->GetAudioData(index, pArticleArgs);
	}
	else
	{
		AppLog("PAGE NOT FOUND!");
		return false;
	}
}

bool
ControllerForm::GetVideoData(int selfIndex, int index, LinkedList *pArticleArgs)
{
	PageDetails *pDetails = null;

	if(pArticleArgs == null)
	{
		AppLog("lstArticleArgs null");
		return false;
	}

	if(selfIndex >= 0 && selfIndex < __pages.GetCount())
	{
		pDetails = (static_cast<PageDetails*>(__pages.GetAt(selfIndex)));
		return pDetails->GetVideoData(index, pArticleArgs);
	}
	else
	{
		AppLog("PAGE NOT FOUND!");
		return false;
	}
}

bool
ControllerForm::GetDescription(int selfIndex, int index, Osp::Base::String *pDescription)
{
	if(pDescription == null)
	{
		AppLog("pDescription null");
		return false;
	}

	if(selfIndex >= 0 && selfIndex < __pages.GetCount())
	{
		PageDetails *pDetails = (static_cast<PageDetails*>(__pages.GetAt(selfIndex)));
		return pDetails->GetDescription(index, pDescription);
	}
	else
	{
		AppLog("PAGE NOT FOUND!");
		return false;
	}
}

Osp::Base::String
ControllerForm::GetTitle(void)
{
	return __title;
}

Osp::Base::String
ControllerForm::GetIconPath(void)
{
	return __headerIconPath;
}

Osp::Graphics::Color
ControllerForm::GetHeaderColor(void)
{
	return __colorHeader;
}

Osp::Base::String
ControllerForm::GetHeaderBgPath(void)
{
	return __headerBgImagePath;
}

Osp::Graphics::Color
ControllerForm::GetPageTitleColor(int index)
{
	if(index >= 0 && index < __pages.GetCount())
	{
		PageDetails *pDetails = (static_cast<PageDetails*>(__pages.GetAt(index)));
		return pDetails->__colorPagetitle;
	}
	else
	{
		return Color::COLOR_BLACK;
	}
}

void
ControllerForm::ShowPopup(const String& message)
{
	MessageBox  *nwPopup = new MessageBox();
	int modal = 0;
	nwPopup->Construct(L"", message, MSGBOX_STYLE_NONE, TIMEOUT_POPUP);
	nwPopup->ShowAndWait(modal);
	delete nwPopup;
}

void
ControllerForm::HandleLowMemory(void)
{
	PageDetails *pTempPage = null;
	for(int i = 0; i < __pages.GetCount(); i++)
	{
		pTempPage = static_cast<PageDetails*>(__pages.GetAt(i));
		if((pTempPage->IsAlive() == true) && i != __activePageIndex)
		{
			KillPage(i);
		}
	}
}
