#include "PagePanel.h"
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

Osp::Ui::Controls::IconListViewItem*
IconListLoader::CreateItem(int index)
{
	String itemText;
	IconListViewItem* pIconListview = new IconListViewItem();
	DisplayImage *pDispImg = null;

	itemText = L"";
	if(pImages && pImages->GetCount() > index)
	{
		pDispImg = static_cast<DisplayImage*>(pImages->GetAt(index));
		if(pDispImg->pBmpImage)
			pIconListview->Construct(*pDispImg->pBmpImage, &itemText);
		else
		{
			pIconListview->Construct(*pBlankImage, &itemText);
		}
	}
	return pIconListview;
}

bool
IconListLoader::DeleteItem(int index, Osp::Ui::Controls::IconListViewItem* pItem)
{
	delete pItem;
	return true;
}

void
IconListLoader::OnIconListViewItemStateChanged(Osp::Ui::Controls::IconListView& view, int index, Osp::Ui::Controls::IconListViewItemStatus status)
{
	pViewParent->HandleContentType(index, pViewParent->EVENT_ICON);
}

int
IconListLoader::GetItemCount(void)
{
	return pImages->GetCount();
}


PagePanel::PagePanel(void)
:__pList(null), __pIconList1(null), __itemHeight(0), __imageGridCount(0),  __pIconLoader1(null), __selfIndex(-1), __pLblMainImage(null), __pLblMainText(null)
{

}

bool
PagePanel::Initialize(PageLoadData *pPageDetails)
{

	if(!pPageDetails)
	{
		AppLog("pPageDetails NULL");
		return null;
	}

	__pageStyle = pPageDetails->__pageLayout;
	__pageTitle = pPageDetails->__pageTitle;
	__pageType = pPageDetails->__pageType;

	__colorHeader = pPageDetails->__colorHeader;
	__colorPagetitle = pPageDetails->__colorPagetitle;
	__colorBody = pPageDetails->__colorBody;
	__colorFont = pPageDetails->__colorFont;
	__selfIndex = pPageDetails->__selfIndex;


	RelativeLayout relativeLayout;
	relativeLayout.Construct();

	if(Construct(relativeLayout, Rectangle(0, 100, 150, 150)) != E_SUCCESS)
	{
		AppLog("Panel Create failed");
		return false;
	}

	return true;
}

result
PagePanel::OnInitializing(void)
{
	SetName(__pageTitle);
	SetBackgroundColor(__colorBody);

	__pControllerForm = GET_CONTROLLER_FORM;

	CreateControls();
	ConfigureLayout();

	return E_SUCCESS;
}

result
PagePanel::OnTerminating(void)
{
	__listElements.RemoveAll(true);
	__listX.RemoveAll(true);
	__listY.RemoveAll(true);
	__listW.RemoveAll(true);
	__listH.RemoveAll(true);
	__listFontSizes.RemoveAll(true);
	__listGarbage.RemoveAll(true);

	if(__pIconLoader1)
		delete __pIconLoader1;

	return E_SUCCESS;
}

void
PagePanel::CreateControls(void)
{
	mchar styleDigit;
	RelativeLayout* pRelativeLayout = dynamic_cast<RelativeLayout*>(this->GetLayoutN());

	__pageStyle.GetCharAt(5, styleDigit);

	if(styleDigit == L'1' || styleDigit == L'2' || styleDigit == L'3' || styleDigit == L'4')
	{
		__pList = new ListView();
		__pList->Construct(Rectangle(0, 0, 100, 100), true);
		__pList->SetTextOfEmptyList(L" ");
		__pList->SetBackgroundColor(__colorBody);
		__pList->AddListViewItemEventListener(*this);
		AddControl(*__pList);

		pRelativeLayout->SetRelation(*__pList, *this, RECT_EDGE_RELATION_TOP_TO_TOP);
		pRelativeLayout->SetRelation(*__pList, *this, RECT_EDGE_RELATION_BOTTOM_TO_BOTTOM);
		pRelativeLayout->SetHorizontalFitPolicy(*__pList, FIT_POLICY_PARENT);

	}
	else if(styleDigit == L'5')
	{
		__pLblMainImage = new Label();
		__pLblMainImage->Construct(Rectangle(0, 0, 10, 250), L"");
		__pLblMainImage->SetName(L"IDC_IMGLABEL1");
		__pLblMainImage->AddTouchEventListener(*this);
		AddControl(*__pLblMainImage);

		pRelativeLayout->SetRelation(*__pLblMainImage, *this, RECT_EDGE_RELATION_TOP_TO_TOP);
		pRelativeLayout->SetHorizontalFitPolicy(*__pLblMainImage, FIT_POLICY_PARENT);
		pRelativeLayout->SetCenterAligned(*__pLblMainImage, CENTER_ALIGN_HORIZONTAL);

		Label *pHoldingLabel = new Label();
		pHoldingLabel->Construct(Rectangle(0, 0, GetWidth(), 250), L"");
		AddControl(*pHoldingLabel);

		__pLblMainText = new Label();
		__pLblMainText->Construct(Rectangle(0, 100, 100, 60), L"");
		__pLblMainText->SetName(L"IDC_TXTLABEL1");
		__pLblMainText->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
		__pLblMainText->SetTextVerticalAlignment(ALIGNMENT_MIDDLE);
		__pLblMainText->SetBackgroundColor(Color(0, 0, 0, 50));
		__pLblMainText->SetTextConfig(30, LABEL_TEXT_STYLE_NORMAL);
		__pLblMainText->AddTouchEventListener(*this);
		AddControl(*__pLblMainText);

		pRelativeLayout->SetRelation(*__pLblMainText, *pHoldingLabel, RECT_EDGE_RELATION_BOTTOM_TO_BOTTOM);
		pRelativeLayout->SetHorizontalFitPolicy(*__pLblMainText, FIT_POLICY_PARENT);

		__pList = new ListView();
		__pList->Construct(Rectangle(0, 0, 100, 100), true);
		__pList->SetTextOfEmptyList(L" ");
		__pList->SetBackgroundColor(__colorBody);
		__pList->AddListViewItemEventListener(*this);
		AddControl(*__pList);

		pRelativeLayout->SetRelation(*__pList, *pHoldingLabel, RECT_EDGE_RELATION_TOP_TO_BOTTOM);
		pRelativeLayout->SetRelation(*__pList, *this, RECT_EDGE_RELATION_BOTTOM_TO_BOTTOM);
		pRelativeLayout->SetHorizontalFitPolicy(*__pList, FIT_POLICY_PARENT);

	}
	else if(styleDigit == L'6')
	{
		__pIconList1 = new IconListView();
		__pIconList1->Construct(Rectangle(0, 0, 100, 100), Dimension(150, 120), ICON_LIST_VIEW_STYLE_NORMAL, ICON_LIST_VIEW_SCROLL_DIRECTION_VERTICAL);
		__pIconList1->SetName(L"IDC_ICONLISTVIEW1");
		__pIconList1->SetItemBorderStyle(ICON_LIST_VIEW_ITEM_BORDER_STYLE_NONE);
		__pIconList1->SetItemLayoutHorizontalAlignment(ALIGNMENT_CENTER);
		__pIconList1->SetTextHorizontalAlignment(ALIGNMENT_CENTER);
		__pIconList1->SetTextVerticalAlignment(ICON_LIST_VIEW_ITEM_TEXT_VERTICAL_ALIGNMENT_INSIDE_BOTTOM);
		__pIconList1->SetItemSpacing(5, 0);
		__pIconList1->SetItemTextSize(6);
		__pIconList1->SetTouchAnimationEnabled(false);
		__pIconList1->SetTextOfEmptyList(L" ");
		__pIconList1->SetBackgroundColor(__colorBody);
		AddControl(*__pIconList1);

		pRelativeLayout->SetRelation(*__pIconList1, *this, RECT_EDGE_RELATION_TOP_TO_TOP);
		pRelativeLayout->SetRelation(*__pIconList1, *this, RECT_EDGE_RELATION_BOTTOM_TO_BOTTOM);
		pRelativeLayout->SetHorizontalFitPolicy(*__pIconList1, FIT_POLICY_PARENT);
	}
	else if(styleDigit == L'7')
	{
		__pIconList1 = new IconListView();
		__pIconList1->Construct(Rectangle(0, 0, 100, 250), Dimension(150, 120), ICON_LIST_VIEW_STYLE_NORMAL, ICON_LIST_VIEW_SCROLL_DIRECTION_HORIZONTAL);
		__pIconList1->SetName(L"IDC_ICONLISTVIEW1");
		__pIconList1->SetItemBorderStyle(ICON_LIST_VIEW_ITEM_BORDER_STYLE_NONE);
		__pIconList1->SetItemLayoutHorizontalAlignment(ALIGNMENT_CENTER);
		__pIconList1->SetTextHorizontalAlignment(ALIGNMENT_CENTER);
		__pIconList1->SetTextVerticalAlignment(ICON_LIST_VIEW_ITEM_TEXT_VERTICAL_ALIGNMENT_INSIDE_BOTTOM);
		__pIconList1->SetItemSpacing(5, 0);
		__pIconList1->SetItemTextSize(6);
		__pIconList1->SetTouchAnimationEnabled(false);
		__pIconList1->SetTextOfEmptyList(L" ");
		__pIconList1->SetBackgroundColor(__colorBody);
		AddControl(*__pIconList1);

		pRelativeLayout->SetRelation(*__pIconList1, *this, RECT_EDGE_RELATION_TOP_TO_TOP);
		pRelativeLayout->SetHorizontalFitPolicy(*__pIconList1, FIT_POLICY_PARENT);

		__pList = new ListView();
		__pList->Construct(Rectangle(0, 0, 100, 100), true);
		__pList->SetTextOfEmptyList(L" ");
		__pList->SetBackgroundColor(__colorBody);
		__pList->AddListViewItemEventListener(*this);
		AddControl(*__pList);

		pRelativeLayout->SetRelation(*__pList, *__pIconList1, RECT_EDGE_RELATION_TOP_TO_BOTTOM);
		pRelativeLayout->SetRelation(*__pList, *this, RECT_EDGE_RELATION_BOTTOM_TO_BOTTOM);
		pRelativeLayout->SetHorizontalFitPolicy(*__pList, FIT_POLICY_PARENT);

	}
	else
	{
		AppLog("Invalid style");
	}

	delete pRelativeLayout;
}

void
PagePanel::Notify(RequestId reqId, Osp::Base::Collection::IList* pArgs)
{
	switch(reqId)
	{
		case NOTIFY_TEXT_COMPLETE:
		{
			__listHeadlines.AddItems(*(static_cast<LinkedList*>(pArgs->GetAt(0))));
			__listLinks.AddItems(*(static_cast<LinkedList*>(pArgs->GetAt(1))));
			__listTimes.AddItems(*(static_cast<LinkedList*>(pArgs->GetAt(2))));

			__listWkgHeadlines.AddItems(__listHeadlines);
			__listWkgTimes.AddItems(__listTimes);

			if(__imageGridCount != IMAGE_GRID_COUNT_MAX)
			{
				__listWkgHeadlines.RemoveItems(0, __imageGridCount, false);
				__listWkgTimes.RemoveItems(0, __imageGridCount, false);
			}

			LoadText();
		}
		break;

		case NOTIFY_IMAGE_COMPLETE:
		{
			__listImages.AddItems(*(static_cast<LinkedList*>(pArgs->GetAt(0))));
			__loadingImages.AddItems(__listImages);

			LoadImages();

		}
		break;
	}
}

void
PagePanel::ClearData(void)
{
	__listHeadlines.RemoveAll(false);
	__listLinks.RemoveAll(false);
	__listWkgHeadlines.RemoveAll(false);

	__listTimes.RemoveAll(false);
	__listWkgTimes.RemoveAll(false);

	__listImages.RemoveAll(false);
	__loadingImages.RemoveAll(false);
}

bool
PagePanel::ConfigureLayout(void)
{
	xmlDocPtr pDoc = null;
	xmlNodePtr pCurNode = null, pSubNode = null, pMainNode = null;
	xmlAttrPtr pProperties = null;
	char designSource[10];
	ByteBuffer *pBuf = Utility::StringUtil::StringToUtf8N(__pageStyle);
	int x = 0, y = 0, w = 0, h = 0, size = 0;

	strcpy(designSource, "/Home/");
	strcat(designSource, (char*)pBuf->GetPointer());
	strcat(designSource, ".xml");

	pDoc = xmlParseFile(designSource);

	if (pDoc == NULL)
	{
		AppLog("xmlParseFile error");
		return false;
	}

	pMainNode = xmlDocGetRootElement(pDoc);
	if (pMainNode == NULL)
	{
		AppLog("xmlDocGetRootElement error");
		xmlFreeDoc(pDoc);
		return false;
	}

	for(pMainNode = pMainNode->children; pMainNode != null; pMainNode = pMainNode->next)
	{
		if(stricmp((const char*)pMainNode->name, TAG_DESIGN_LIST) == 0)
		{
			pProperties = pMainNode->properties;
			__itemHeight = ToInt((char*)pProperties->children->content);

			for(pCurNode = pMainNode->children; pCurNode != null; pCurNode = pCurNode->next)
			{
				if(stricmp((const char*)pCurNode->name, TAG_DESIGN_TEXT) == 0)
				{
					for(pSubNode = pCurNode->children; pSubNode != null; pSubNode = pSubNode->next)
					{
						if(stricmp((const char*)pSubNode->name, TAG_DESIGN_X) == 0)
						{
							x = ToInt((char*)pSubNode->children->content);
						}
						else if(stricmp((const char*)pSubNode->name, TAG_DESIGN_Y) == 0)
						{
							y = ToInt((char*)pSubNode->children->content);
						}
						else if(stricmp((const char*)pSubNode->name, TAG_DESIGN_W) == 0)
						{
							w = ToInt((char*)pSubNode->children->content);
						}
						else if(stricmp((const char*)pSubNode->name, TAG_DESIGN_H) == 0)
						{
							h = ToInt((char*)pSubNode->children->content);
						}
						else if(stricmp((const char*) pSubNode->name, TAG_DESIGN_FONTSIZE) == 0)
						{
							size = ToInt((char*)pSubNode->children->content);
						}
					}
					__listElements.Add(*(new String(TAG_DESIGN_TEXT)));
					__listX.Add(*(new Integer(x)));
					__listY.Add(*(new Integer(y)));
					__listW.Add(*(new Integer(w)));
					__listH.Add(*(new Integer(h)));
					__listFontSizes.Add(*(new Integer(size)));
				}
				else if(stricmp((const char*)pCurNode->name, TAG_DESIGN_IMAGE) == 0)
				{
					for(pSubNode = pCurNode->children; pSubNode != null; pSubNode = pSubNode->next)
					{
						if(stricmp((const char*)pSubNode->name, TAG_DESIGN_X) == 0)
						{
							x = ToInt((char*)pSubNode->children->content);
						}
						else if(stricmp((const char*)pSubNode->name, TAG_DESIGN_Y) == 0)
						{
							y = ToInt((char*)pSubNode->children->content);
						}
						else if(stricmp((const char*)pSubNode->name, TAG_DESIGN_W) == 0)
						{
							w = ToInt((char*)pSubNode->children->content);
						}
						else if(stricmp((const char*)pSubNode->name, TAG_DESIGN_H) == 0)
						{
							h = ToInt((char*)pSubNode->children->content);
						}
						else if(stricmp((const char*) pSubNode->name, TAG_DESIGN_TYPE) == 0)
						{
							Osp::Base::Utility::StringUtil::Utf8ToString
								((const char*)pSubNode->children->content, __fitType);
						}
					}
					__listElements.Add(*(new String(TAG_DESIGN_IMAGE)));
					__listX.Add(*(new Integer(x)));
					__listY.Add(*(new Integer(y)));
					__listW.Add(*(new Integer(w)));
					__listH.Add(*(new Integer(h)));
					__listFontSizes.Add(*(new Integer(0)));
				}
			}
		}
		else if(stricmp((const char*)pMainNode->name, TAG_DESIGN_ICONLIST) == 0)
		{
			__imageGridCount = ToInt((char*)pMainNode->children->content);
		}
	}

	return true;
}

bool
PagePanel::LoadText(void)
{
	if(!__pList)
	{
		AppLog("list control does not exist");
		return false;
	}
	else
	{
		Color dividerColor(235,235,235);

		if(__colorBody.GetRed() < 170 && __colorBody.GetRed() > 80)
			dividerColor.SetRed(125);
		else if(__colorBody.GetRed() < 80)
				dividerColor.SetRed(15);

		if(__colorBody.GetGreen() < 170 && __colorBody.GetGreen() > 80)
			dividerColor.SetGreen(125);
		else if(__colorBody.GetGreen() < 80)
				dividerColor.SetGreen(15);

		if(__colorBody.GetBlue() < 170 && __colorBody.GetBlue() > 80)
			dividerColor.SetBlue(125);
		else if(__colorBody.GetBlue() < 80)
				dividerColor.SetBlue(15);

		__pList->SetItemDividerColor(dividerColor);
		__pList->SetItemProvider(*this);
		__pList->Draw();
		__pList->Show();
		Draw(); Show();
	}

	return true;
}

bool
PagePanel::LoadImages(void)
{
	DisplayImage *pDispImg = null;

	if(__pLblMainImage)
	{
		pDispImg = static_cast<DisplayImage*>(__listImages.GetAt(0));
		if(pDispImg && pDispImg->pBmpImage)
		{
			__pLblMainImage->SetBackgroundBitmap(*pDispImg->pBmpImage);
			__pLblMainImage->Draw(); __pLblMainImage->Show();
			if(__pLblMainText)
			{
				String mainHeadline = *(static_cast<String*>(__listHeadlines.GetAt(0)));
				__pLblMainText->SetText(mainHeadline);
				__pLblMainText->Draw(); __pLblMainText->Show();
			}
		}
	}

	Draw(); Show();
	if(__imageGridCount != IMAGE_GRID_COUNT_MAX)
	{
		__loadingImages.RemoveItems(0, __imageGridCount, false);
	}

	if(__pIconList1)
	{
		__pIconLoader1 = new IconListLoader();
		__pIconLoader1->pImages = &__loadingImages;
		__pIconLoader1->pTexts = &__listWkgHeadlines;
		__pIconLoader1->pViewParent = this;

		Canvas canvas;
		canvas.Construct(Rectangle(0, 0, 150, 120));
		canvas.FillRectangle(__colorBody, Rectangle(0, 0, 150, 120));

		Bitmap *pBlankImage = new Bitmap();
		pBlankImage->Construct(canvas, canvas.GetBounds());
		__pIconLoader1->pBlankImage = pBlankImage;
		__listGarbage.Add(*pBlankImage);
		__pIconList1->SetItemProvider(*__pIconLoader1);
		__pIconList1->AddIconListViewItemEventListener(*__pIconLoader1);
		__pIconList1->UpdateList();
		__pIconList1->RequestRedraw();
		Draw(); Show();
	}
	else
	{
		AppLog("icon list does not exist");
	}

	if(__pList)
	{
		__pList->UpdateList();
		__pList->RequestRedraw();
		Draw(); Show();
	}

	return true;
}

void
PagePanel::OnListViewItemStateChanged(Osp::Ui::Controls::ListView &listView, int index, int elementId, Osp::Ui::Controls::ListItemStatus status)
{
	HandleContentType(index, EVENT_LIST);
	return;
}

void
PagePanel::OnListViewItemSwept(Osp::Ui::Controls::ListView &listView, int index, Osp::Ui::Controls::SweepDirection direction)
{

}
void
PagePanel::OnListViewContextItemStateChanged(Osp::Ui::Controls::ListView &listView, int index, int elementId, Osp::Ui::Controls::ListContextItemStatus state)
{

}

int
PagePanel::GetItemCount(void)
{
	return __listWkgHeadlines.GetCount() + 1;
}

Osp::Ui::Controls::ListItemBase*
PagePanel::CreateItem(int index, int itemWidth)
{
	CustomItem* pItem = null;
	String elemType;
	Integer x,y,w,h;
	EnrichedText *pEnText = null, *pEnTime = null;
	TextElement *pTextElement = null, *pTimeElement = null;
	Font textFont, timeFont;
	int i = 0;
	Integer fontSize;

	if(__listWkgHeadlines.GetCount() > index)
	{
		pItem = new CustomItem();

		pItem->Construct(Osp::Graphics::Dimension(itemWidth,__itemHeight), LIST_ANNEX_STYLE_NORMAL);
		for(; i<__listElements.GetCount(); i++)
		{
			elemType = *(static_cast<String*>(__listElements.GetAt(i)));
			x = *(static_cast<Integer*>(__listX.GetAt(i)));
			y = *(static_cast<Integer*>(__listY.GetAt(i)));
			w = *(static_cast<Integer*>(__listW.GetAt(i)));
			h = *(static_cast<Integer*>(__listH.GetAt(i)));

			if(elemType.Equals(L"TXT", false) == true)
			{
				String text, date;
				if(__listWkgHeadlines.GetAt(index))
				{
					String temp = *(static_cast<String*>(__listWkgHeadlines.GetAt(index)));
					text.SetCapacity(temp.GetCapacity());
					text = temp;
				}
				if(__pageStyle.Equals(String("style4"), false) == true)
				{
					__pControllerForm->GetDescription(__selfIndex, index, &text);
				}
				if(__listWkgTimes.GetAt(index))
				{
					String temp = *(static_cast<String*>(__listWkgTimes.GetAt(index)));
					date.SetCapacity((temp.GetCapacity()));
					date = temp;
				}

				fontSize = *(static_cast<Integer*>(__listFontSizes.GetAt(i)));
				textFont.Construct(FONT_STYLE_PLAIN,fontSize.ToInt());

				pEnText = new EnrichedText();
				pEnText->Construct(Dimension(w.ToInt(),h.ToInt()));
				pEnText->SetVerticalAlignment(TEXT_ALIGNMENT_TOP);
				pEnText->SetHorizontalAlignment(TEXT_ALIGNMENT_LEFT);
				pEnText->SetTextWrapStyle(TEXT_WRAP_WORD_WRAP);
				pEnText->SetTextAbbreviationEnabled(true);

				pTextElement = new TextElement();
				pTextElement->Construct(text);
				pTextElement->SetTextColor(__colorFont);
				pTextElement->SetFont(textFont);
				pEnText->Add(*pTextElement);

				timeFont.Construct(FONT_STYLE_PLAIN, FONT_SIZE_TIME);
				pEnTime = new EnrichedText();
				pEnTime->Construct(Dimension(w.ToInt(),TIME_AREA_HEIGHT));
				pEnTime->SetVerticalAlignment(TEXT_ALIGNMENT_TOP);
				pEnTime->SetHorizontalAlignment(TEXT_ALIGNMENT_LEFT);
				pEnTime->SetTextWrapStyle(TEXT_WRAP_WORD_WRAP);
				pEnTime->SetTextAbbreviationEnabled(true);

				pTimeElement = new TextElement();
				pTimeElement->Construct(date);
				pTimeElement->SetTextColor(__colorFont);
				pTimeElement->SetFont(timeFont);
				pEnTime->Add(*pTimeElement);

				pItem->AddElement(Rectangle(x.ToInt(), y.ToInt(), w.ToInt(), h.ToInt()), ID_FORMAT_STRING, *pEnText);
				pItem->AddElement(Rectangle(x.ToInt(), y.ToInt() + h.ToInt() + GAP_HEAD_TIME, w.ToInt(), TIME_AREA_HEIGHT), ID_FORMAT_TIME, *pEnTime);

				//garbage collection
				__listGarbage.Add(*pEnText);
				__listGarbage.Add(*pTextElement);
				__listGarbage.Add(*pEnTime);
				__listGarbage.Add(*pTimeElement);
			}
			else if(elemType.Equals(L"image", false) == true)
			{
				DisplayImage *pDispImg = null;
				Bitmap *pBmpElem = null;
				int imgH = 0, imgW = 0;

				if(__loadingImages.GetCount() <= index)
				{
					continue;
				}

				pDispImg = static_cast<DisplayImage*>(__loadingImages.GetAt(index));

				if(pDispImg->pBmpImage == null)
				{
					AppLog("No image for item %d", index);
					continue;
				}
				imgH = pDispImg->pBmpImage->GetHeight();
				imgW = pDispImg->pBmpImage->GetWidth();

				if(__fitType.CompareTo(FIT_TYPE_CLIP) ==  0) //create power clip
				{
					if(imgW < (w.ToInt()/2) || imgH < h.ToInt())
					{
						pBmpElem = pDispImg->pBmpImage;
					}
					else // do power clip
					{
						//scale the image to element's Width
						int tempH = 0;
						tempH = imgH * w.ToInt() / imgW;

						//clip bottom parts
						pBmpElem = new Bitmap();
						pBmpElem->Construct(*pDispImg->pBmpImage, Rectangle(0,0,w.ToInt(), h.ToInt()));

						__listGarbage.Add(*pBmpElem);
					}
				}
				else if(__fitType.CompareTo(FIT_TYPE_SCALE) ==  0) //scale
				{
					pBmpElem = pDispImg->pBmpImage;
				}
				else if(__fitType.CompareTo(FIT_TYPE_FIT) ==  0)// fit in center
				{
					int tempW = 0;
					tempW = imgW * h.ToInt() / imgH;
					pDispImg->pBmpImage->Scale(Dimension(tempW, h.ToInt()));
					pBmpElem = pDispImg->pBmpImage;
				}
				else
				{
					AppLog("INVALID IMAGE FIT STYLE");
					continue;
				}

				pItem->AddElement(Rectangle(x.ToInt(),y.ToInt(),w.ToInt(),h.ToInt()), ID_FORMAT_BITMAP, *pBmpElem);
			}
		}
	}

	return pItem;
}

bool
PagePanel::DeleteItem(int index, Osp::Ui::Controls::ListItemBase* pItem, int itemWidth)
{
	delete pItem;
	pItem = null;
	return true;
}

void
PagePanel::OnTouchPressed(const Osp::Ui::Control& source, const Osp::Graphics::Point& currentPosition, const Osp::Ui::TouchEventInfo & touchInfo)
{
	HandleContentType(0, EVENT_TOUCH);
}

int
PagePanel::ToInt(char* pString)
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

void
PagePanel::HandleContentType(int index, int eventSource)
{
	Frame *pFrame = (Application::GetInstance()->GetAppFrame()->GetFrame());
	LinkedList params;
	int realIndex = 0;

	PageState pageState = __pControllerForm->GetPageState(__selfIndex);
	if(pageState != PAGE_STATE_FETCH_COMPLETE)
	{
		return;
	}

	switch(eventSource)
	{
		case EVENT_TOUCH:
			realIndex = index;
			break;
		case EVENT_LIST:
			if(__imageGridCount != IMAGE_GRID_COUNT_MAX)
				realIndex = index + __imageGridCount;
			else
				realIndex = index;
			break;
		case EVENT_ICON:
			realIndex = index;
			break;
	}


	if(__pageType.Equals(PAGE_TYPE_TEXT, false) == true)
	{
		ViewTextForm *pViewTextForm = null;
		ArticleData articleParams;
		if(__pControllerForm->GetArticleData(__selfIndex, realIndex, &articleParams) == false)
		{
			AppLog("GetArticleData FAILED");
			return;
		}

		pViewTextForm = new ViewTextForm();
		pViewTextForm->Initialize(&articleParams);
		pFrame->AddControl(*pViewTextForm);
		pFrame->SetCurrentForm(*pViewTextForm);
		pViewTextForm->Draw();
		pViewTextForm->Show();
	}
	else if(__pageType.Equals(PAGE_TYPE_IMAGE, false) == true)
	{
		ViewImageForm *pViewImageForm = null;
		GalleryData galleryParams;

		galleryParams.__pImages = &__listImages;
		galleryParams.__pTexts = &__listHeadlines;
		galleryParams.__pLinks = &__listLinks;

		galleryParams.__startIndex = realIndex;

		galleryParams.__colorBody = __colorBody;
		galleryParams.__colorFont = __colorFont;
		galleryParams.__colorHeader = __colorHeader;
		galleryParams.__colorPagetitle = __colorPagetitle;

		if(__listImages.GetCount() == 0)
		{
			String errorMsg;
			Application::GetInstance()->GetAppResource()->GetString(L"IDS_NO_IMAGE", errorMsg);
			__pControllerForm->ShowPopup(errorMsg);
			return;
		}

		pViewImageForm = new ViewImageForm();
		pViewImageForm->Initialize(&galleryParams);
		pFrame->AddControl(*pViewImageForm);
		pFrame->SetCurrentForm(*pViewImageForm);
		pViewImageForm->Draw();
		pViewImageForm->Show();
	}
	else if(__pageType.Equals(PAGE_TYPE_AUDIO, false) == true)
	{
		ArrayList* pDataList = null;
		String* pData = null;
		String* pData2 = null;
		AppControl* pAc = null;

		if(__pControllerForm->GetAudioData(__selfIndex, realIndex, &params) == false)
		{
			AppLog("GetAudioData FAILED");
			return;
		}

		String audioLink = *(static_cast<String*>(params.GetAt(0)));

		params.RemoveAll(true);

		if(audioLink.GetLength() == 0)
		{
			String errorMsg;
			Application::GetInstance()->GetAppResource()->GetString(L"IDS_NO_AUDIO", errorMsg);
			__pControllerForm->ShowPopup(errorMsg);
			return;
		}

		pDataList = new ArrayList();
		pDataList->Construct();

		pData = new String(L"type:audio");
		pDataList->Add(*pData);

		audioLink.SetCapacity(audioLink.GetCapacity() + 12);
		audioLink.Insert(L"path:", 0);


		pData2 = new String(audioLink);
		pDataList->Add(*pData2);

		pAc = AppManager::FindAppControlN(Osp::App::APPCONTROL_PROVIDER_AUDIO , Osp::App::APPCONTROL_OPERATION_PLAY);
		if(pAc)
		{
		  pAc->Start(pDataList, null);
		  delete pAc;
		}
		pDataList->RemoveAll(true);
		delete pDataList;

	}
	else if(__pageType.Equals(PAGE_TYPE_VIDEO, false) == true)
	{
		ArrayList* pDataList = null;
		String* pData = null;
		String* pData2 = null;
		AppControl* pAc = null;

		if(__pControllerForm->GetVideoData(__selfIndex, realIndex, &params) == false)
		{
			AppLog("GetVideoData FAILED");
			return;
		}

		String videoLink = *(static_cast<String*>(params.GetAt(0)));

		if(videoLink.GetLength() == 0)
		{
			String errorMsg;
			Application::GetInstance()->GetAppResource()->GetString(L"IDS_NO_VIDEO", errorMsg);
			__pControllerForm->ShowPopup(errorMsg);
			return;
		}

		params.RemoveAll(true);

		pDataList = new ArrayList();
		pDataList->Construct();

		pData = new String(L"type:video");
		pDataList->Add(*pData);

		videoLink.SetCapacity(videoLink.GetCapacity() + 12);
		videoLink.Insert(L"path:", 0);

		pData2 = new String(videoLink);
		pDataList->Add(*pData2);

		pAc = AppManager::FindAppControlN(Osp::App::APPCONTROL_PROVIDER_VIDEO , Osp::App::APPCONTROL_OPERATION_PLAY);
		if(pAc)
		{
		  pAc->Start(pDataList, null);
		  delete pAc;
		}
		pDataList->RemoveAll(true);
		delete pDataList;

	}
}
