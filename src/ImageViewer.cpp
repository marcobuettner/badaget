#include "ImageViewer.h"

using namespace Osp::Graphics;
using namespace Osp::App;
using namespace Osp::Base::Runtime;
using namespace Osp::Base::Collection;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Media;

bool
ImageViewer::Initialize(Bitmap *pImage, Color bgColor)
{
	savedPath.Clear();

	if(pImage == null)
	{
		AppLog("pDisplayImage NULL");
		return false;
	}

	__pDisplayImage = new Bitmap();
	__pDisplayImage->Construct(*pImage, Rectangle(0,0, pImage->GetWidth(), pImage->GetHeight()));

	__bgColor = bgColor;

	if(Construct(L"ImageViewer") != E_SUCCESS)
	{
		AppLog("Form Create Failed");
		return false;
	}
	return true;
}

result
ImageViewer::OnInitializing()
{
	SetFormBackEventListener(this);

	__pGallery = static_cast<Gallery*>(GetControl(L"IDC_GALLERY1"));
	if(!__pGallery)
	{
		AppLog("Gallery Control does not exist");
		return E_FAILURE;
	}

	__pGallery->SetBackgroundColor(__bgColor);
	__pGallery->SetItemProvider(*this);
	Touch touch;
	touch.SetMultipointEnabled(*__pGallery, true);


	GetFooter()->AddActionEventListener(*this);
	return E_SUCCESS;
}

result
ImageViewer::OnTerminating()
{
	delete __pDisplayImage;

	if(__pShareMenu)
	{
		__pShareMenu->CancelBTPush();
		delete __pShareMenu;
	}

	return E_SUCCESS;
}

result
ImageViewer::OnDraw()
{
	return E_SUCCESS;
}

void
ImageViewer::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
 	switch(actionId)
	{
		case ID_SHARE:
		{
			SaveImage();
			ShowShareMenu();
		}
		break;

		case ID_SAVE:
		{
			SaveImage();

			ControllerForm *pControllerForm = GET_CONTROLLER_FORM;
			String popupMsg;
			Application::GetInstance()->GetAppResource()->GetString(L"IDS_SAVED", popupMsg);
			pControllerForm->ShowPopup(popupMsg);
		}
		break;

		default:
			break;
	}
}

void
ImageViewer::OnFormBackRequested(Osp::Ui::Controls::Form& source)
{
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	pFrame->RemoveControl(*this);
}

int
ImageViewer::GetItemCount(void)
{
	return 1;
}

Osp::Ui::Controls::GalleryItem*
ImageViewer::CreateItem(int index)
{
	GalleryItem* pGallery =  null;

	pGallery = new GalleryItem();
	pGallery->Construct(*__pDisplayImage);

	return pGallery;
}

bool
ImageViewer::DeleteItem(int index, Osp::Ui::Controls::GalleryItem* pItem)
{
	delete pItem;
	return true;
}
void
ImageViewer::ShowShareMenu()
{
	if(!__pShareMenu)
	{
		Point loadPoint(50,0);
		loadPoint.y = GetFooter()->GetY() + 40;

		__pShareMenu = new ShareMenu();
		__pShareMenu->Initialize(loadPoint, savedPath, false);
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
ImageViewer::SaveImage(void)
{
	if(savedPath.GetLength() != 0)
		return;						/*Already saved*/

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

	savedPath = L"/Media/Images/";
	savedPath += Application::GetInstance()->GetAppName();
	savedPath.Append(lastFileName);
	savedPath += ".jpg";

	img.Construct();
	res = img.EncodeToFile(*__pDisplayImage, IMG_FORMAT_JPG, savedPath, true);

	pAppRegistry->Set(fileNameKey, lastFileName + 1);

}
