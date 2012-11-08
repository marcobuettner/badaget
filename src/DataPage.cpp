#include "DataPage.h"
#include "ViewMainForm.h"
#include "stdlib.h"
#include <FXml.h>
#include <FIo.h>
#include <FSystem.h>


using namespace Osp::Base::Utility;
using namespace Osp::Base;
using namespace Osp::Base::Collection;
using namespace Osp::Graphics;
using namespace Osp::Ui::Controls;
using namespace Osp::Ui;
using namespace Osp::Net;
using namespace Osp::Media;
using namespace Osp::Xml;
using namespace Osp::App;
using namespace Osp::Net::Http;
using namespace Osp::Io;
using namespace Osp::Base::Runtime;
using namespace Osp::System;


result
PageDetails::Load(void)
{
	LinkedList pageDetails;

	if(__pPagePanel == null)
	{
		if(
			(__pageType.Equals(PAGE_TYPE_TEXT, false) == true) ||
			(__pageType.Equals(PAGE_TYPE_AUDIO, false) == true) ||
			(__pageType.Equals(PAGE_TYPE_VIDEO, false) == true) ||
			(__pageType.Equals(PAGE_TYPE_IMAGE, false) == true)
		  )
		{
			Initialize();

			PageLoadData loadData;
			loadData.__pageLayout = __pageLayout;
			loadData.__pageTitle = __pageTitle;
			loadData.__pageType = __pageType;
			loadData.__colorHeader = __colorHeader;
			loadData.__colorPagetitle = __colorPagetitle;
			loadData.__colorBody = __colorBody;
			loadData.__colorFont = __colorFont;
			loadData.__selfIndex = __selfIndex;

			ViewMainForm *pMainForm = GET_MAIN_FORM;
			if(!(__pPagePanel = pMainForm->CreatePage(&loadData)))
			{
				AppLog("CreatePage Failed!");
				return E_FAILURE;
			}
		}
		else
		{
			AppLog("UNSUPPORTED TYPE!");
			return E_FAILURE;
		}
	}
	else
	{
		ViewMainForm *pMainForm = GET_MAIN_FORM;
		if(pMainForm->SetActivePanel(__pPagePanel) == false)
		{
			AppLog("SetActivePanel Failed!");
			return E_FAILURE;
		}
	}

	return E_SUCCESS;
}

PageDetails::PageDetails(void)
{
	__pageType.Clear();
	__pageSource.Clear();
	__pageLayout.Clear();
	__pageTitle.Clear();
	__pageSource.Clear();

}

PageDetails::~PageDetails(void)
{

}

void
PageDetails::Initialize(void)
{
	__pPagePanel = null;
	__pSession = null;
	__pTransaction = null;
	__pImg = null;
	__pageState = PAGE_STATE_INITIALIZED;
}

void
PageDetails::Destroy(void)
{
	if(Application::GetInstance()->GetAppFrame() && __pPagePanel)
	{
		ViewMainForm *pMainForm = GET_MAIN_FORM;
		pMainForm->RemovePanel(__pPagePanel);
		__pPagePanel = null;
	}

	if(__pSession != null)
	{
		__pSession->CancelTransaction(*__pTransaction);
		__pSession->CloseAllTransactions();
		delete __pSession;
		__pSession = null;
	}

	if(__pImg)
	{
		delete __pImg;
		__pImg = null;
	}

	__listImages.RemoveAll(true);
	__listImgReqId.RemoveAll(true);
	__listHeadlines.RemoveAll(true);
	__listTimes.RemoveAll(true);
	__listLinks.RemoveAll(true);

	__pageState = PAGE_STATE_INITIALIZED;

}

bool
PageDetails::ParseFeed(void)
{
	xmlDocPtr pDoc = null;
	xmlNodePtr pCurNode = null, pItemNode = null, pMediaNode = null;
	xmlAttrPtr pProperties = null;
	String itemHead, itemImgPath, itemTime;
	String itemLink;
	int itemIndex = -1;
	String itemDataType;
	DisplayImage *pDispImg = null;

	__listHeadlines.RemoveAll(true);
	__listTimes.RemoveAll(true);
	__listLinks.RemoveAll(true);
	__listImages.RemoveAll(true);

	pDoc = xmlParseFile(__xmlFileName);

	if (pDoc == NULL)
	{
		AppLog("xmlParseFile error");
		return false;
	}

	pCurNode = xmlDocGetRootElement(pDoc);
	if (pCurNode == NULL)
	{
		xmlFreeDoc(pDoc);
		return false;
	}

	pCurNode = pCurNode->children;

	while(pCurNode != NULL && pCurNode->type != XML_ELEMENT_NODE)
	{
		pCurNode = pCurNode->next;
	}

	pCurNode = pCurNode->children;

	for (;pCurNode != NULL; pCurNode = pCurNode->next)
	{
		if(stricmp((const char*)pCurNode->name, TAG_ITEM) == 0)
		{
			pDispImg = new DisplayImage();

			itemHead.Clear(); itemTime.Clear(); itemLink.Clear();
			itemIndex++;

			for(pItemNode = pCurNode->xmlChildrenNode; pItemNode != NULL; pItemNode = pItemNode->next)
			{
				if(pItemNode->type != XML_ELEMENT_NODE)
					continue;

				if(stricmp((const char*)pItemNode->name, TAG_TITLE) == 0)
				{
					Osp::Base::Utility::StringUtil::Utf8ToString
					((const char*)pItemNode->children->content, itemHead);
					__listHeadlines.Add(*(new String(itemHead)));
				}
				else if(stricmp((const char*)pItemNode->name, TAG_LINK) == 0)
				{
					Osp::Base::Utility::StringUtil::Utf8ToString
					((const char*)pItemNode->children->content, itemLink);
					__listLinks.Add(*(new String(itemLink)));
				}
				else if((stricmp((const char*)pItemNode->name, TAG_PUB_DATE) == 0) ||
						(stricmp((const char*)pItemNode->name, TAG_PUB_DATE2) == 0))
				{
					Osp::Base::Utility::StringUtil::Utf8ToString
					((const char*)pItemNode->children->content, itemTime);
					__listTimes.Add(*(new String(itemTime)));
				}

				else if(stricmp((const char*)pItemNode->name, TAG_ENCLOSURE) == 0)
				{
					for(pProperties = pItemNode->properties;
						pProperties != null; pProperties = pProperties->next)
					{
						if(stricmp((const char*)(pProperties->name), ATTRIBUTE_DATA_TYPE) == 0)
						{
							Osp::Base::Utility::StringUtil::Utf8ToString
								((const char*)pProperties->children->content, itemDataType);
							break;
						}
					}
					if(itemDataType.StartsWith(DATA_TYPE_IMAGE, 0))
					{
						for(pProperties = pItemNode->properties;
								pProperties != null; pProperties = pProperties->next)
						{
							if(stricmp((const char*)pProperties->name, ATTRIBUTE_HEIGHT) == 0)
							{
								pDispImg->height = atoi((char*)pProperties->children->content);
							}
							else if(stricmp((const char*)pProperties->name, ATTRIBUTE_WIDTH) == 0)
							{
								pDispImg->width = atoi((char*)pProperties->children->content);
							}
							else if(stricmp((const char*)pProperties->name, ATTRIBUTE_URL) == 0)
							{
								Osp::Base::Utility::StringUtil::Utf8ToString
								((const char*)pProperties->children->content, pDispImg->url);
							}
						}
					}
					pDispImg->index = itemIndex;
				}
				else if(stricmp((const char*)pItemNode->name, TAG_CONTENT) == 0)
				{
					for(pProperties = pItemNode->properties; pProperties != null;
												pProperties = pProperties->next)
					{
						if(stricmp((const char*)pProperties->name, ATTRIBUTE_DATA_TYPE) == 0)
						{
							Osp::Base::Utility::StringUtil::Utf8ToString
									((const char*)pProperties->children->content, itemDataType);
						}
					}
					if(itemDataType.StartsWith(DATA_TYPE_IMAGE, 0))
					{
						for(pProperties = pItemNode->properties; pProperties != null;
								pProperties = pProperties->next)
						{
							if(stricmp((const char*)pProperties->name, ATTRIBUTE_HEIGHT) == 0)
							{
								pDispImg->height = atoi((char*)pProperties->children->content);
							}
							else if(stricmp((const char*)pProperties->name, ATTRIBUTE_WIDTH) == 0)
							{
								pDispImg->width = atoi((char*)pProperties->children->content);
							}
							else if(stricmp((const char*)pProperties->name, ATTRIBUTE_URL) == 0)
							{
								Osp::Base::Utility::StringUtil::Utf8ToString
										((const char*)pProperties->children->content, pDispImg->url);
							}
						}
					}
					pDispImg->index = itemIndex;
				}
				else if(stricmp((const char*)pItemNode->name, TAG_GROUP) == 0)
				{
					for(pMediaNode = pItemNode->children; pMediaNode != null; pMediaNode = pMediaNode->next)
					{
						if(stricmp((const char*)pMediaNode->name, TAG_THUMBNAIL) == 0)
						{
							for(pProperties = pMediaNode->properties; pProperties != null;
									pProperties = pProperties->next)
							{
								if(stricmp((const char*)pProperties->name, ATTRIBUTE_HEIGHT) == 0)
								{
									pDispImg->height = atoi((char*)pProperties->children->content);
								}
								else if(stricmp((const char*)pProperties->name, ATTRIBUTE_WIDTH) == 0)
								{
									pDispImg->width = atoi((char*)pProperties->children->content);
								}
								else if(stricmp((const char*)pProperties->name, ATTRIBUTE_URL) == 0)
								{
									Osp::Base::Utility::StringUtil::Utf8ToString
											((const char*)pProperties->children->content, pDispImg->url);
								}

							}
							pDispImg->index = itemIndex;
						}
					}
				}
			}
			__listImages.Add(*pDispImg);
		}
	}
	xmlFreeDoc(pDoc);

	LinkedList *pArgs = new LinkedList();
	pArgs->Add(__listHeadlines);
	pArgs->Add(__listLinks);
	pArgs->Add(__listTimes);
	pArgs->Add(*__pPagePanel);

	ViewMainForm *pMainForm = GET_MAIN_FORM;
	pMainForm->SendUserEvent(ViewMainForm::NOTIFY_TEXT_COMPLETE, pArgs);

	HandleUrlImages();

	return true;
}

bool
PageDetails::GetDescription(int index, Osp::Base::String *pDescription)
{
	xmlDocPtr pDoc = null;
	xmlNodePtr pCurNode = null, pItemNode = null;
	int i=0;
	String itemTime, itemHead, itemImgPath, itemDesc, itemAuth, itemLink;
	String itemDataType;

	if(pDescription == null)
	{
		AppLog("pFullData null");
		return false;
	}

	pDoc = xmlParseFile(__xmlFileName);

	if (pDoc == NULL)
	{
		AppLog("xmlParseFile error");
		return false;
	}

	pCurNode = xmlDocGetRootElement(pDoc);
	if (pCurNode == NULL)
	{
		AppLog("xmlDocGetRootElement error");
		xmlFreeDoc(pDoc);
		return false;
	}

	pCurNode = pCurNode->children;

	while(pCurNode != NULL && pCurNode->type != XML_ELEMENT_NODE)
	{
		pCurNode = pCurNode->next;
	}

	pCurNode = pCurNode->children;

	for (;pCurNode != NULL; pCurNode = pCurNode->next)
	{
		if(stricmp((const char*)pCurNode->name, TAG_ITEM) == 0)
		{
			if(i<index)
			{
				i++;
				continue;
			}
			for(pItemNode = pCurNode->xmlChildrenNode; pItemNode != NULL; pItemNode = pItemNode->next)
			{
				if(pItemNode->type != XML_ELEMENT_NODE)
					continue;

				if(stricmp((const char*)pItemNode->name, TAG_DESCRIPTION) == 0)
				{
					if(pItemNode->children)
					{
						Osp::Base::Utility::StringUtil::Utf8ToString
						((const char*)pItemNode->children->content, itemDesc);
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_ENCODED) == 0)
				{
					if(pItemNode->children)
					{
						Osp::Base::Utility::StringUtil::Utf8ToString
						((const char*)pItemNode->children->content, itemDesc);
					}
				}
			}

			Cleanup(itemDesc);
			pDescription->Clear();
			pDescription->SetCapacity(MAX_DESC_LENGTH);
			pDescription->Append(itemDesc);

			xmlFreeDoc(pDoc);
			return true;
		}
	}

	xmlFreeDoc(pDoc);
	return false;
}

bool
PageDetails::GetTextData(const int index, ArticleData *pFullData)
{
	xmlDocPtr pDoc = null;
	xmlNodePtr pCurNode = null, pItemNode = null;
	xmlAttrPtr pProperties = null;
	int i=0;
	String itemTime, itemHead, itemImgPath, itemDesc, itemAuth, itemLink;
	int h=0, w=0;
	String itemDataType;

	if(pFullData == null)
	{
		AppLog("pFullData null");
		return false;
	}

	pDoc = xmlParseFile(__xmlFileName);

	if (pDoc == NULL)
	{
		AppLog("xmlParseFile error");
		return false;
	}

	pCurNode = xmlDocGetRootElement(pDoc);
	if (pCurNode == NULL)
	{
		AppLog("xmlDocGetRootElement error");
		xmlFreeDoc(pDoc);
		return false;
	}

	pCurNode = pCurNode->children;

	while(pCurNode != NULL && pCurNode->type != XML_ELEMENT_NODE)
	{
		pCurNode = pCurNode->next;
	}

	pCurNode = pCurNode->children;

	for (;pCurNode != NULL; pCurNode = pCurNode->next)
	{
		if(stricmp((const char*)pCurNode->name, TAG_ITEM) == 0)
		{
			if(i<index)
			{
				i++;
				continue;
			}
			for(pItemNode = pCurNode->xmlChildrenNode; pItemNode != NULL; pItemNode = pItemNode->next)
			{
				if(pItemNode->type != XML_ELEMENT_NODE)
					continue;

				if(stricmp((const char*)pItemNode->name, TAG_TITLE) == 0)
				{
					if(pItemNode->children)
					{
						Osp::Base::Utility::StringUtil::Utf8ToString
						((const char*)pItemNode->children->content, itemHead);
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_DESCRIPTION) == 0)
				{
					if(pItemNode->children)
					{
						Osp::Base::Utility::StringUtil::Utf8ToString
						((const char*)pItemNode->children->content, itemDesc);
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_ENCODED) == 0)
				{
					if(pItemNode->children)
					{
						Osp::Base::Utility::StringUtil::Utf8ToString
						((const char*)pItemNode->children->content, itemDesc);
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_LINK) == 0)
				{
					if(pItemNode->children)
					{
						Osp::Base::Utility::StringUtil::Utf8ToString
						((const char*)pItemNode->children->content, itemLink);
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_PUB_DATE) == 0)
				{
					if(pItemNode->children)
					{
						Osp::Base::Utility::StringUtil::Utf8ToString
						((const char*)pItemNode->children->content, itemTime);
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_CREATOR) == 0)
				{
					if(pItemNode->children)
					{
						Osp::Base::Utility::StringUtil::Utf8ToString
						((const char*)pItemNode->children->content, itemAuth);
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_IMAGE) == 0)
				{
					if(pItemNode->children)
					{
						Osp::Base::Utility::StringUtil::Utf8ToString
						((const char*)pItemNode->children->content, itemImgPath);

						/* default sizes */
						w = DEFAULT_LIST_IMAGE_WIDTH;
						h = DEFAULT_LIST_IMAGE_HEIGHT;
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_ENCLOSURE) == 0)
				{
					for(pProperties = pItemNode->properties;
						pProperties != null; pProperties = pProperties->next)
					{
						if(stricmp((const char*)(pProperties->name), ATTRIBUTE_DATA_TYPE) == 0)
						{
							Osp::Base::Utility::StringUtil::Utf8ToString
								((const char*)pProperties->children->content, itemDataType);
							break;
						}
					}
					if(itemDataType.StartsWith(DATA_TYPE_IMAGE, 0))
					{
						for(pProperties = pItemNode->properties;
								pProperties != null; pProperties = pProperties->next)
						{
							if(stricmp((const char*)(pProperties->name), ATTRIBUTE_URL) == 0)
							{
								Osp::Base::Utility::StringUtil::Utf8ToString
								((const char*)pProperties->children->content, itemImgPath);
								break;
							}
						}
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_CONTENT) == 0)
				{
					for(pProperties = pItemNode->properties; pProperties != null;
												pProperties = pProperties->next)
					{
						if(stricmp((const char*)pProperties->name, ATTRIBUTE_DATA_TYPE) == 0)
						{
							Osp::Base::Utility::StringUtil::Utf8ToString
									((const char*)pProperties->children->content, itemDataType);
						}
					}
					if(itemDataType.StartsWith(DATA_TYPE_IMAGE, 0))
					{
						for(pProperties = pItemNode->properties; pProperties != null;
								pProperties = pProperties->next)
						{
							if(stricmp((const char*)pProperties->name, ATTRIBUTE_HEIGHT) == 0)
							{
								h = atoi((char*)pProperties->children->content);
							}
							else if(stricmp((const char*)pProperties->name, ATTRIBUTE_WIDTH) == 0)
							{
								w = atoi((char*)pProperties->children->content);
							}
							else if(stricmp((const char*)pProperties->name, ATTRIBUTE_URL) == 0)
							{
								Osp::Base::Utility::StringUtil::Utf8ToString
								((const char*)pProperties->children->content, itemImgPath);
							}
						}
					}
				}
			}

			pFullData->__headline.SetCapacity(itemHead.GetCapacity());
			pFullData->__description.SetCapacity(itemDesc.GetCapacity());
			pFullData->__time.SetCapacity(itemTime.GetCapacity());
			pFullData->__link.SetCapacity(itemLink.GetCapacity());
			pFullData->__imagePath.SetCapacity(itemImgPath.GetCapacity());
			pFullData->__author.SetCapacity(itemAuth.GetCapacity());
			pFullData->__headline = itemHead;
			pFullData->__description = itemDesc;
			pFullData->__time = itemTime;
			pFullData->__link = itemLink;
			pFullData->__imagePath = itemImgPath;
			pFullData->__author = itemAuth;

			pFullData->__colorBody = __colorBody;
			pFullData->__colorFont = __colorFont;
			pFullData->__colorHeader = __colorHeader;
			pFullData->__colorPagetitle= __colorPagetitle;

			xmlFreeDoc(pDoc);
			return true;
		}
	}

	xmlFreeDoc(pDoc);
	return false;

}

bool
PageDetails::GetAudioData(const int index, LinkedList *pFullData)
{
	xmlDocPtr pDoc = null;
	xmlNodePtr pCurNode = null, pItemNode = null, pMediaNode = null;
	xmlAttrPtr pProperties = null;
	int i=0;
	String itemAudioPath, itemDataType;

	if(pFullData == null)
	{
		AppLog("pFullData null");
		return false;
	}

	pDoc = xmlParseFile(__xmlFileName);

	if (pDoc == NULL)
	{
		AppLog("xmlParseFile error");
		return false;
	}

	pCurNode = xmlDocGetRootElement(pDoc);
	if (pCurNode == NULL)
	{
		AppLog("xmlDocGetRootElement error");
		xmlFreeDoc(pDoc);
		return false;
	}

	pCurNode = pCurNode->children;

	while(pCurNode != NULL && pCurNode->type != XML_ELEMENT_NODE)
	{
		pCurNode = pCurNode->next;
	}

	pCurNode = pCurNode->children;

	for (;pCurNode != NULL; pCurNode = pCurNode->next)
	{
		if(stricmp((const char*)pCurNode->name, TAG_ITEM) == 0)
		{
			if(i<index)
			{
				i++;
				continue;
			}
			for(pItemNode = pCurNode->xmlChildrenNode; pItemNode != NULL; pItemNode = pItemNode->next)
			{
				if(pItemNode->type != XML_ELEMENT_NODE)
					continue;

				if(stricmp((const char*)pItemNode->name, TAG_ENCLOSURE) == 0)
				{
					for(pProperties = pItemNode->properties;
						pProperties != null; pProperties = pProperties->next)
					{
						if(stricmp((const char*)(pProperties->name), ATTRIBUTE_DATA_TYPE) == 0)
						{
							Osp::Base::Utility::StringUtil::Utf8ToString
								((const char*)pProperties->children->content, itemDataType);
							break;
						}
					}

					if(itemDataType.StartsWith(DATA_TYPE_AUDIO, 0))
					{
						for(pProperties = pItemNode->properties;
									pProperties != null; pProperties = pProperties->next)
						{
							if(stricmp((const char*)(pProperties->name), ATTRIBUTE_URL) == 0)
							{
								Osp::Base::Utility::StringUtil::Utf8ToString
									((const char*)pProperties->children->content, itemAudioPath);
								break;
							}
						}
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_CONTENT) == 0)
				{
					for(pProperties = pItemNode->properties; pProperties != null;
												pProperties = pProperties->next)
					{
						if(stricmp((const char*)pProperties->name, ATTRIBUTE_DATA_TYPE) == 0)
						{
							Osp::Base::Utility::StringUtil::Utf8ToString
									((const char*)pProperties->children->content, itemDataType);
							break;
						}
					}

					if(itemDataType.StartsWith(DATA_TYPE_AUDIO, 0))
					{
						for(pProperties = pItemNode->properties; pProperties != null;
								pProperties = pProperties->next)
						{
							if(stricmp((const char*)pProperties->name, ATTRIBUTE_URL) == 0)
							{
								Osp::Base::Utility::StringUtil::Utf8ToString
								((const char*)pProperties->children->content, itemAudioPath);
							}
						}
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_GROUP) == 0)
				{
					for(pMediaNode = pItemNode->children; pMediaNode != null; pMediaNode = pMediaNode->next)
					{
						if(stricmp((const char*)pMediaNode->name, TAG_CONTENT) == 0)
						{
							for(pProperties = pMediaNode->properties; pProperties != null;
														pProperties = pProperties->next)
							{
								if(stricmp((const char*)pProperties->name, ATTRIBUTE_DATA_TYPE) == 0)
								{
									Osp::Base::Utility::StringUtil::Utf8ToString
											((const char*)pProperties->children->content, itemDataType);
									break;
								}
							}
							if(itemDataType.StartsWith(DATA_TYPE_AUDIO, 0))
							{
								for(pProperties = pMediaNode->properties; pProperties != null;
										pProperties = pProperties->next)
								{
									if(stricmp((const char*)pProperties->name, ATTRIBUTE_URL) == 0)
									{
										Osp::Base::Utility::StringUtil::Utf8ToString
										((const char*)pProperties->children->content, itemAudioPath);
									}
								}
							}
						}
					}
				}
			}
			pFullData->Add(*(new String(itemAudioPath)));
			xmlFreeDoc(pDoc);
			return true;
		}
	}

	xmlFreeDoc(pDoc);
	return false;

}

bool
PageDetails::GetVideoData(const int index, LinkedList *pFullData)
{
	xmlDocPtr pDoc = null;
	xmlNodePtr pCurNode = null, pItemNode = null, pMediaNode = null;
	xmlAttrPtr pProperties = null;
	int i=0;
	String itemVideoPath, itemDataType;

	if(pFullData == null)
	{
		AppLog("pFullData null");
		return false;
	}

	pDoc = xmlParseFile(__xmlFileName);

	if (pDoc == NULL)
	{
		AppLog("xmlParseFile error");
		return false;
	}

	pCurNode = xmlDocGetRootElement(pDoc);
	if (pCurNode == NULL)
	{
		AppLog("xmlDocGetRootElement error");
		xmlFreeDoc(pDoc);
		return false;
	}

	pCurNode = pCurNode->children;

	while(pCurNode != NULL && pCurNode->type != XML_ELEMENT_NODE)
	{
		pCurNode = pCurNode->next;
	}

	pCurNode = pCurNode->children;

	for (;pCurNode != NULL; pCurNode = pCurNode->next)
	{
		if(stricmp((const char*)pCurNode->name, TAG_ITEM) == 0)
		{
			if(i<index)
			{
				i++;
				continue;
			}
			for(pItemNode = pCurNode->xmlChildrenNode; pItemNode != NULL; pItemNode = pItemNode->next)
			{
				if(pItemNode->type != XML_ELEMENT_NODE)
					continue;

				else if(stricmp((const char*)pItemNode->name, TAG_ENCLOSURE) == 0)
				{
					for(pProperties = pItemNode->properties;
						pProperties != null; pProperties = pProperties->next)
					{
						if(stricmp((const char*)(pProperties->name), ATTRIBUTE_DATA_TYPE) == 0)
						{
							Osp::Base::Utility::StringUtil::Utf8ToString
								((const char*)pProperties->children->content, itemDataType);
							break;
						}
					}

					if(itemDataType.StartsWith(DATA_TYPE_VIDEO, 0))
					{
						for(pProperties = pItemNode->properties;
									pProperties != null; pProperties = pProperties->next)
						{
							if(stricmp((const char*)(pProperties->name), ATTRIBUTE_URL) == 0)
							{
								Osp::Base::Utility::StringUtil::Utf8ToString
									((const char*)pProperties->children->content, itemVideoPath);
								break;
							}
						}
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_CONTENT) == 0)
				{
					for(pProperties = pItemNode->properties; pProperties != null;
												pProperties = pProperties->next)
					{
						if(stricmp((const char*)pProperties->name, ATTRIBUTE_DATA_TYPE) == 0)
						{
							Osp::Base::Utility::StringUtil::Utf8ToString
									((const char*)pProperties->children->content, itemDataType);
						}
					}
					if(itemDataType.StartsWith(DATA_TYPE_VIDEO, 0))
					{
						for(pProperties = pItemNode->properties; pProperties != null;
								pProperties = pProperties->next)
						{
							if(stricmp((const char*)pProperties->name, ATTRIBUTE_URL) == 0)
							{
								Osp::Base::Utility::StringUtil::Utf8ToString
								((const char*)pProperties->children->content, itemVideoPath);
							}
						}
					}
				}
				else if(stricmp((const char*)pItemNode->name, TAG_GROUP) == 0)
				{
					for(pMediaNode = pItemNode->children; pMediaNode != null; pMediaNode = pMediaNode->next)
					{
						if(stricmp((const char*)pMediaNode->name, TAG_CONTENT) == 0)
						{
							for(pProperties = pMediaNode->properties; pProperties != null;
														pProperties = pProperties->next)
							{
								if(stricmp((const char*)pProperties->name, ATTRIBUTE_DATA_TYPE) == 0)
								{
									Osp::Base::Utility::StringUtil::Utf8ToString
											((const char*)pProperties->children->content, itemDataType);
								}
							}
							if(itemDataType.StartsWith(DATA_TYPE_VIDEO, 0))
							{
								for(pProperties = pMediaNode->properties; pProperties != null;
										pProperties = pProperties->next)
								{
									if(stricmp((const char*)pProperties->name, ATTRIBUTE_URL) == 0)
									{
										Osp::Base::Utility::StringUtil::Utf8ToString
										((const char*)pProperties->children->content, itemVideoPath);
									}
								}
							}
						}
					}
				}
			}
			pFullData->Add(*(new String(itemVideoPath)));
			xmlFreeDoc(pDoc);
			return true;
		}
	}

	xmlFreeDoc(pDoc);
	return false;

}
bool
PageDetails::FetchFeed(void)
{
	result r = E_SUCCESS;
	HttpRequest* pRequest = null;

	if(__pSession && __pSession->GetActiveTransactionCount() > 0)
	{
		return true;
	}

	String title;
	title.SetCapacity((strlen(PAGE_XML_PATH_PREFIX) + strlen(".xml")) * 2 + __pageTitle.GetCapacity() + 2);
	title.Append(PAGE_XML_PATH_PREFIX);
	title += __pageTitle;
	title.Append(L".xml");

	ByteBuffer *pBuf = Utility::StringUtil::StringToUtf8N(title);
	strcpy(__xmlFileName, (char*)pBuf->GetPointer());
	delete pBuf;

	File xmlFile;
	xmlFile.Construct(__xmlFileName, L"w", true);
	xmlFile.Truncate(0);

	__pageState = PAGE_STATE_FETCH_ONGOING;

	if(__pSession == null)
	{
		Uri hostUri;
		hostUri.SetUri(__pageSource);
		__pSession = new HttpSession();
		r = __pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, hostUri.GetHost(), null);
		if (IsFailed(r))
			goto CATCH;
	}

	__pTransaction = __pSession->OpenTransactionN();
	if (null == __pTransaction)
	{
		r = GetLastResult();
		goto CATCH;
	}

	r = __pTransaction->AddHttpTransactionListener(*this);
	if (IsFailed(r))
		goto CATCH;

	pRequest = const_cast<HttpRequest*>(__pTransaction->GetRequest());
	if(pRequest == null)
	{
		r = GetLastResult();
		goto CATCH;
	}

	r = pRequest->SetUri(__pageSource);
	if(IsFailed(r))
		goto CATCH;

	r = pRequest->SetMethod(NET_HTTP_METHOD_GET);
	if(IsFailed(r))
		goto CATCH;

	r = __pTransaction->Submit();
	if(IsFailed(r))
		goto CATCH;

	return true;

CATCH:
	AppLog("FAIL: %s", GetErrorMessage(r));
	__pageState = PAGE_STATE_FETCH_FAIL;
	ViewMainForm *pViewMainForm = GET_MAIN_FORM;
	pViewMainForm->SendUserEvent(ViewMainForm::NOTIFY_NETWORK_FAIL, null);
	return false;
}


void
PageDetails::OnTransactionReadyToRead(HttpSession& httpSession, HttpTransaction& httpTransaction, int availableBodyLen)
{
	HttpResponse* pHttpResponse = httpTransaction.GetResponse();

	if (pHttpResponse->GetHttpStatusCode() == HTTP_STATUS_OK)
	{
		HttpHeader* pHttpHeader = pHttpResponse->GetHeader();
		String* pHeaderString = pHttpHeader->GetRawHeaderN();
		ByteBuffer* pBuffer = pHttpResponse->ReadBodyN();

		File xmlFile;
		xmlFile.Construct(__xmlFileName, L"a", true);
		xmlFile.Write(*pBuffer);

		delete pHeaderString;
		delete pBuffer;
	}
}

void
PageDetails::OnTransactionAborted(HttpSession& httpSession, HttpTransaction& httpTransaction, result r)
{
	AppLog("####### OnTransactionAborted! (%s)#######", GetErrorMessage(r));
	ViewMainForm *pViewMainForm = GET_MAIN_FORM;
	pViewMainForm->SendUserEvent(ViewMainForm::NOTIFY_NETWORK_FAIL, null);

	delete &httpTransaction;
}

void
PageDetails::OnTransactionReadyToWrite(HttpSession& httpSession, HttpTransaction& httpTransaction, int recommendedChunkSize)
{

}

void
PageDetails::OnTransactionHeaderCompleted(HttpSession& httpSession, HttpTransaction& httpTransaction, int headerLen, bool rs)
{

}

void
PageDetails::OnTransactionCompleted(HttpSession& httpSession, HttpTransaction& httpTransaction)
{
	if(ParseFeed() == false)
	{
		AppLog("RSS Parser Failed!");
		__pageState = PAGE_STATE_FETCH_FAIL;
		ViewMainForm *pViewMainForm = GET_MAIN_FORM;
		pViewMainForm->SendUserEvent(ViewMainForm::NOTIFY_NETWORK_FAIL, null);
	}

	delete &httpTransaction;
}

void
PageDetails::OnTransactionCertVerificationRequiredN(HttpSession& httpSession, HttpTransaction& httpTransaction, Osp::Base::String* pCert)
{

}


void
PageDetails::OnImageDecodeUrlReceived (RequestId reqId, Osp::Graphics::Bitmap *pBitmap, result r, const Osp::Base::String errorCode, const Osp::Base::String errorMessage)
{
	Long rqIdTmp = 0;
	int i = 0, index = -1;
	DisplayImage *pDispImg = null;
	ControllerForm* pControllerForm = GET_CONTROLLER_FORM;

	if(r != E_SUCCESS)
	{
		AppLog("DecodeUrl FAILED! [%S]", errorMessage.GetPointer());
		if(pControllerForm)
		{
			LinkedList *pArgs = new LinkedList();
			pArgs->Add(*this);
			pControllerForm->SendUserEvent(ControllerForm::UPDATE_QUEUE, pArgs);
		}
		return;
	}

	for(; i < __listImgReqId.GetCount(); i++)
	{
		rqIdTmp = *(static_cast<Long*>(__listImgReqId.GetAt(i)));
		if(rqIdTmp.ToLong() == reqId)
		{
			index = i;
			break;
		}
	}

	if(index == -1)
	{
		AppLog("Invalid RequestID!");
		return;
	}

	pDispImg = (static_cast<DisplayImage*>(__listImages.GetAt(index)));
	pDispImg->pBmpImage = new Bitmap();
	pDispImg->pBmpImage->Construct(*pBitmap, Rectangle(0,0,pBitmap->GetWidth(), pBitmap->GetHeight()));

	if(pControllerForm)
	{
		LinkedList *pArgs = new LinkedList();
		pArgs->Add(*this);
		pControllerForm->SendUserEvent(ControllerForm::UPDATE_QUEUE, pArgs);
	}

}

RequestId
PageDetails::RequestDecode(void)
{
	result res = E_FAILURE;
	Uri uri;
	RequestId reqId = -1;
	DisplayImage *pDispImg = null;

	if(__listImages.GetCount() == 0)
	{
		AppLog("Image Url Count 0");
		return -1;
	}

	pDispImg = (static_cast<DisplayImage*>(__listImages.GetAt(__currentQIndex)));
	res = uri.SetUri(pDispImg->url);
	if(res != E_SUCCESS)
	{
		__currentQIndex++;
		return -1;
	}

	if(__pImg == null)
	{
		__pImg = new Image();
		__pImg->Construct();
	}

	res = __pImg->DecodeUrl(uri,BITMAP_PIXEL_FORMAT_ARGB8888,pDispImg->width, pDispImg->height, reqId,
			(IImageDecodeUrlEventListener&)*this, Osp::Media::Image::TIMEOUT_INFINITE);

	if(res != E_SUCCESS)
	{
		__currentQIndex++;
		return -1;
	}

	__decodeQCount++;
	__currentQIndex++;

	return reqId;

}

void
PageDetails::HandleUrlImages(void)
{
	while( (__decodeQCount < IMAGE_DECODE_QCOUNT_MAX) && /* slots free in decode queue */
			__currentQIndex < __listImages.GetCount() )
	{
		__listImgReqId.Add(*(new Long(RequestDecode())));
	}
	if(__currentQIndex >= __listImages.GetCount()
			&& __decodeQCount == 0)
	{
		LinkedList *pArgs = new LinkedList();

		pArgs->Add(__listImages);
		pArgs->Add(*(new Integer(__selfIndex)));
		pArgs->Add(*__pPagePanel);

		__pageState = PAGE_STATE_FETCH_COMPLETE;
		ViewMainForm *pMainForm = GET_MAIN_FORM;
		pMainForm->SendUserEvent(ViewMainForm::NOTIFY_IMAGE_COMPLETE, pArgs);
		__listImgReqId.RemoveAll(true);
		__currentQIndex = 0;
	}
}

bool
PageDetails::IsAlive(void) const
{
	if(__pPagePanel)
		return true;
	else
		return false;
}

PageState
PageDetails::GetState(void)  const
{
	return __pageState;
}

void
PageDetails::Cleanup(String& htmlPage)
{
	int index = 0;
	String strIFrame("<iframe"), strIFrameEnd("</iframe>");
	String strA("<a"), strAEnd("</a>");
	String strP("<p"), strPEnd("</p>");
	String strImg("<img"), strImgEnd("</img>");
	int indexOf = 0, indexOf2 = 0;
	result res = E_SUCCESS;

	htmlPage.Replace(L"&nbsp;", L" ");
	htmlPage.Replace(L"&quot;", L"\"");
	htmlPage.Replace(L"&apos;", L"'");
	htmlPage.Replace(L"&amp;", L"&");
	htmlPage.Replace(L"&lt;", L"<");
	htmlPage.Replace(L"&gt;", L">");

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

	// remove <a>, </a> tags
	while(1)
	{
		if((res = htmlPage.IndexOf(strA, 0, indexOf)) != E_SUCCESS)
			break;

		htmlPage.IndexOf(L">", indexOf, indexOf2);
		htmlPage.Remove(indexOf, indexOf2 - indexOf +  1);

		htmlPage.IndexOf(strAEnd, 0, indexOf);
		htmlPage.Remove(indexOf, strAEnd.GetLength());
	}

	// remove <img> tags
	while(1)
	{
		if((res = htmlPage.IndexOf(strImg, 0, indexOf)) != E_SUCCESS)
			break;

		htmlPage.IndexOf(L">", indexOf, indexOf2);
		htmlPage.Remove(indexOf, indexOf2 - indexOf +  1);

		htmlPage.IndexOf(strImgEnd, 0, indexOf);
		htmlPage.Remove(indexOf, strImgEnd.GetLength());
	}

	// remove <p>, </p> tags
	while(1)
	{
		if((res = htmlPage.IndexOf(strP, 0, indexOf)) != E_SUCCESS)
			break;

		htmlPage.IndexOf(L">", indexOf, indexOf2);
		htmlPage.Remove(indexOf, indexOf2 - indexOf +  1);

		htmlPage.Replace(strPEnd, L". ");

	}
}

void
PageDetails::ClearDiv(const int index, String& desc)
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

void
PageDetails::UpdateQCount()
{
	__decodeQCount--;
}


DisplayImage::DisplayImage(void)
{
	height = DEFAULT_LIST_IMAGE_HEIGHT; width = DEFAULT_LIST_IMAGE_WIDTH;
	index = -1;
	url.Clear();
	pBmpImage = null;
}

DisplayImage::~DisplayImage(void)
{
	delete pBmpImage;
}
