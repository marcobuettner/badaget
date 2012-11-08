#include "ViewImageForm.h"
#include "FContent.h"


using namespace Osp::Graphics;
using namespace Osp::App;
using namespace Osp::Base::Runtime;
using namespace Osp::Base::Collection;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;
using namespace Osp::Content;

bool
ViewImageForm::Initialize(GalleryData *pImageParams)
{
	if(pImageParams == null)
	{
		AppLog("pImageParams NULL");
		return false;
	}
	__pImages = pImageParams->__pImages;
	__pTexts = pImageParams->__pTexts;
	__pLinks = pImageParams->__pLinks;

	__startIndex = pImageParams->__startIndex;

	__colorBody = pImageParams->__colorBody;
	__colorFont = pImageParams->__colorFont;
	__colorHeader = pImageParams->__colorHeader;
	__colorPagetitle = pImageParams->__colorPagetitle;

	__pShareMenu = null; __pGallery = null;

	if(Construct(L"GalleryForm") != E_SUCCESS)
	{
		AppLog("Form Create Failed");
		return false;
	}
	return true;
}

result
ViewImageForm::OnInitializing(void)
{
	String imgDesc = *(static_cast<String*>(__pTexts->GetAt(__startIndex)));
	Label *pLblLine = null, *pLblCount = null;
	String countText(25);
	Label *pLblDesc = null;

	SetFormBackEventListener(this);

	__pGallery = static_cast<Gallery*>(GetControl(L"IDC_GALLERY1"));
	if(!__pGallery)
	{
		return E_FAILURE;
	}

	pLblDesc = static_cast<Label*>(GetControl(L"IDC_LBLDESC"));
	if(!pLblDesc)
	{
		return E_FAILURE;
	}

	pLblLine = static_cast<Label*>(GetControl(L"IDC_LBLLINE"));
	if(!pLblLine)
	{
		return E_FAILURE;
	}

	pLblCount = static_cast<Label*>(GetControl(L"IDC_LBLCOUNT"));
	if(!pLblCount)
	{
		return E_FAILURE;
	}

	SetBackgroundColor(__colorBody);

	pLblLine->SetBackgroundColor(__colorHeader);
	pLblCount->SetBackgroundColor(__colorBody);
	__pGallery->SetBackgroundColor(__colorBody);
	pLblDesc->SetBackgroundColor(__colorBody);

	pLblCount->SetTextColor(__colorFont);
	pLblDesc->SetTextColor(__colorFont);
	pLblDesc->SetText(imgDesc);

	countText.Format(25, L"%d of %d", __startIndex + 1, __pImages->GetCount());
	pLblCount->SetText(countText);

	Footer *pFooter = GetFooter();
	pFooter->SetItemColor(FOOTER_ITEM_STATUS_NORMAL, Color(0, 0, 0));
	pFooter->SetButtonColor(BUTTON_ITEM_STATUS_NORMAL, Color(0, 0, 0));
	pFooter->SetColor(FOOTER_COLOR);
	pFooter->AddActionEventListener(*this);


	__pGallery->SetItemProvider(*this);
	__pGallery->AddGalleryEventListener(*this);

	int i = 0;
	for(; i <= __startIndex; i++)
	{
		__pGallery->SetCurrentItemIndex(i);
		__pGallery->Draw();
	}

	return E_SUCCESS;
}

result
ViewImageForm::OnTerminating(void)
{
	if(__pShareMenu)
	{
		__pShareMenu->CancelBTPush();
		delete __pShareMenu;
	}

	return E_SUCCESS;
}

result
ViewImageForm::OnDraw(void)
{
	return E_SUCCESS;
}

void
ViewImageForm::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
 	switch(actionId)
	{
		case ID_SHARE:
		{
			ShowShareMenu();
		}
		break;

		case ID_SAVE:
		{
			SaveImage();
		}
		break;
	}
}

void
ViewImageForm::OnFormBackRequested(Osp::Ui::Controls::Form& source)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	pFrame->RemoveControl(*this);
}

int
ViewImageForm::GetItemCount(void)
{
	return __pImages->GetCount();
}

Osp::Ui::Controls::GalleryItem*
ViewImageForm::CreateItem(int index)
{
	GalleryItem* pGallery =  null;
	DisplayImage *pDispImg = null;

	pDispImg = static_cast<DisplayImage*>(__pImages->GetAt(index));
	if(pDispImg->pBmpImage)
	{
		pGallery = new GalleryItem();
		pGallery->Construct(*pDispImg->pBmpImage);
	}

	return pGallery;
}

bool
ViewImageForm::DeleteItem(int index, Osp::Ui::Controls::GalleryItem* pItem)
{
	delete pItem;
	return true;
}

void
ViewImageForm::OnGalleryCurrentItemChanged(Osp::Ui::Controls::Gallery& view, int index)
{
	String countText(25);
	String imgDesc = *(static_cast<String*>(__pTexts->GetAt(index)));
	Label *pLblCount = null, *pLblDesc = null;

	countText.Format(25, L"%d of %d", index + 1, __pImages->GetCount());
	pLblCount = static_cast<Label*>(GetControl(L"IDC_LBLCOUNT"));
	pLblCount->SetText(countText);

	pLblDesc = static_cast<Label*>(GetControl(L"IDC_LBLDESC"));
	pLblDesc->SetText(imgDesc);


	/* Disabling the footer if the currently displayed image is blank */
	Footer *pFooter = GetFooter();
	DisplayImage *pDispImg = null;

	pDispImg = static_cast<DisplayImage*>(__pImages->GetAt(index));
	if(pDispImg->pBmpImage)
	{
		pFooter->SetItemEnabled(0, true);
		pFooter->SetItemEnabled(1, true);
	}
	else
	{
		pFooter->SetItemEnabled(0, false);
		pFooter->SetItemEnabled(1, false);
	}

	Draw(); Show();
}

void
ViewImageForm::OnGalleryItemClicked(Osp::Ui::Controls::Gallery& view, int index)
{

}

void
ViewImageForm::OnGallerySlideShowStarted(Osp::Ui::Controls::Gallery& gallery)
{

}

void
ViewImageForm::OnGallerySlideShowStopped(Osp::Ui::Controls::Gallery& gallery)
{

}

void
ViewImageForm::ShowShareMenu(void)
{
	if(!__pShareMenu)
	{
		String link = *(static_cast<String*>(__pLinks->GetAt(__pGallery->GetCurrentItemIndex())));
		Point loadPoint(50,0);
		loadPoint.y = GetFooter()->GetY() + 40;

		__pShareMenu = new ShareMenu();
		__pShareMenu->Initialize(loadPoint, link, true);
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
ViewImageForm::SaveImage(void)
{
	DisplayImage *pDispImg = null;

	pDispImg = static_cast<DisplayImage*>(__pImages->GetAt(__pGallery->GetCurrentItemIndex()));
	if(pDispImg->pBmpImage)
	{
		result res = E_SUCCESS;
		Image img;
		String fileName;
		int lastFileName = 0;
		String fileNameKey("AppLastSavedFileNum");
		AppRegistry *pAppRegistry = Application::GetInstance()->GetAppRegistry();

		res = pAppRegistry->Get(fileNameKey, lastFileName);
		if ( res == E_KEY_NOT_FOUND)
		{
			lastFileName = 1;
			pAppRegistry->Add(fileNameKey, lastFileName);
		}

		Osp::Base::String inputPath(L"/Media/Images/");

		inputPath += Application::GetInstance()->GetAppName();
		inputPath.Append(lastFileName);
		inputPath += ".jpg";

		img.Construct();
		res = img.EncodeToFile(*pDispImg->pBmpImage, IMG_FORMAT_JPG, inputPath, true);

		pAppRegistry->Set(fileNameKey, lastFileName + 1);

		ControllerForm *pControllerForm = GET_CONTROLLER_FORM;
		String popupMsg;
		Application::GetInstance()->GetAppResource()->GetString(L"IDS_SAVED", popupMsg);
		pControllerForm->ShowPopup(popupMsg);
	}
}

