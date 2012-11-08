#include "ShareMenu.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::Base::Collection;
using namespace Osp::Net::Bluetooth;
using namespace Osp::Io;

bool
ShareMenu::Initialize(Point loadPoint, String& content, bool isLink)
{
	result r = E_SUCCESS;

	__shareContent.SetCapacity(content.GetCapacity());
	__shareContent = content;

	__isLink = isLink;

	__pBTOppClient = null;

	if((r = Construct(Point(loadPoint), CONTEXT_MENU_STYLE_LIST)) != E_SUCCESS)
	{
		AppLog("ShareMenu create Failed %s", GetErrorMessage(r));
		return false;
	}

	String message, email, bluetooth;
	AppResource *pAppResource = Application::GetInstance()->GetAppResource();

	pAppResource->GetString(L"IDS_MESSAGE", message);
	pAppResource->GetString(L"IDS_EMAIL", email);
	pAppResource->GetString(L"IDS_BLUETOOTH", bluetooth);

	AddItem(message, ID_MESSAGE);
	AddItem(email, ID_EMAIL);
	AddItem(bluetooth, ID_BLUETOOTH);
	AddActionEventListener(*this);

	return true;

}

result
ShareMenu::OnInitializing(void)
{
	return E_SUCCESS;
}

result
ShareMenu::OnTerminating(void)
{
	delete __pBTOppClient;
	return E_SUCCESS;
}

void
ShareMenu::OnActionPerformed(const Osp::Ui::Control& source, int actionId)
{
	switch(actionId)
	{
		case ID_MESSAGE:
		{
			LoadMsgWnd();
		}
		break;

		case ID_EMAIL:
		{
			LoadEmailWnd();
		}
		break;

		case ID_BLUETOOTH:
		{
			LoadBluetoothWnd();
		}
		break;

		default:
			break;

	}
}

void
ShareMenu::LoadEmailWnd(void)
{
	ArrayList* pDataList = null;
	String text(__shareContent);

	pDataList = new ArrayList();
	pDataList->Construct();

	String* pData = null;
	pData = new String(L"subject:");

	String emailText;
	Application::GetInstance()->GetAppResource()->GetString(L"IDS_SHARE_SUBJECT", emailText);
	pData->SetCapacity(pData->GetCapacity() + emailText.GetCapacity());
	pData->Append(emailText);
	pDataList->Add(*pData);

	String* pData2 = null;
	pData2 = new String(L"text:");
	pData2->SetCapacity(pData2->GetCapacity() + __shareContent.GetCapacity());
	pData2->Append(__shareContent);
	pDataList->Add(*pData2);

	if(__isLink == false)
	{
	   String* pData3 = null;
	   pData3 = new String(L"attachments:");
	   pData3->SetCapacity(pData3->GetCapacity() + __shareContent.GetCapacity());
	   pData3->Append(__shareContent);
	   pDataList->Add(*pData3);
	}

	AppControl* pAc = AppManager::FindAppControlN(APPCONTROL_PROVIDER_EMAIL, APPCONTROL_OPERATION_EDIT);
	if(pAc)
	{
	  pAc->Start(pDataList, null);
	  delete pAc;
	}
	pDataList->RemoveAll(true);
	delete pDataList;
}

void
ShareMenu::LoadMsgWnd(void)
{
	ArrayList* pDataList = null;
	pDataList = new ArrayList();
	pDataList->Construct();

	String* pData = null;
	if(__isLink == true)
	{
		pData = new String(L"type:SMS");
	}
	else
	{
		pData = new String(L"type:MMS");
	}
	pDataList->Add(*pData);

	String* pData2 = null;
	pData2 = new String(L"text:");

	pData2->SetCapacity(pData2->GetCapacity() + __shareContent.GetLength());
	pData2->Append(__shareContent);
	pDataList->Add(*pData2);

	String* pData3 = null;
	pData3 = new String(L"to:");
	pDataList->Add(*pData3);

	if(__isLink == false)
	{
	   String* pData4 = null;
	   pData4 = new String(L"attachImage:");
	   pData4->SetCapacity(pData4->GetCapacity() + __shareContent.GetCapacity());
	   pData4->Append(__shareContent);
	   pDataList->Add(*pData4);
	}

	AppControl* pAc = AppManager::FindAppControlN(APPCONTROL_PROVIDER_MESSAGE, APPCONTROL_OPERATION_COMPOSE);
	if(pAc)
	{
	  pAc->Start(pDataList, null);
	  delete pAc;
	}
	pDataList->RemoveAll(true);
	delete pDataList;
}

void
ShareMenu::LoadBluetoothWnd(void)
{
	AppControl* pAc = AppManager::FindAppControlN(APPCONTROL_PROVIDER_BLUETOOTH,
			APPCONTROL_OPERATION_PICK);
	if(pAc != null)
	{
		pAc->Start(null, this);
		delete pAc;
	}
}

void
ShareMenu::OnAppControlCompleted(const Osp::Base::String& appControlId, const Osp::Base::String& operationId, const Osp::Base::Collection::IList* pResultList)
{
	Osp::Base::String* pBTResult = null;
	if(appControlId.Equals(APPCONTROL_PROVIDER_BLUETOOTH)
			&& operationId.Equals(APPCONTROL_OPERATION_PICK))
	{
		pBTResult = (Osp::Base::String*)pResultList->GetAt(0);
		if(pBTResult->Equals(String(APPCONTROL_RESULT_SUCCEEDED)))
		{
			BluetoothDevice* pBTDevice = BluetoothDevice::GetInstanceFromAppControlResultN(*pResultList);

			if(pBTDevice)
			{

				if(!__pBTOppClient)
					__pBTOppClient = new BluetoothOppClient();

				result res = E_SUCCESS;
				String filePath(__shareContent);

				if(__isLink == true)
				{

					filePath = L"/Home/";
					filePath.Append(Application::GetInstance()->GetAppName());
					filePath.Append(".txt");

					File tempFile;
					tempFile.Construct(filePath, L"w+", true);
					tempFile.Write(__shareContent);

				}

				__pBTOppClient->Construct(*this);
				res = __pBTOppClient->PushFile(*pBTDevice, filePath);
				if(res != E_SUCCESS)
				{
					AppLog("PushFile FAILED");
				}
			}
			else
			{
				AppLog("No Device from BT AppControl");
			}
		}
		else if(pBTResult->Equals(String(APPCONTROL_RESULT_FAILED)))
		{
			AppLog("Bluetooth list failed.");
		}
	}
}

void
ShareMenu::OnOppPushResponded (result r)
{
	if(r == E_SYSTEM)
	{
		AppLog("transfer failed");
		String errorMsg;
		ControllerForm *pControllerForm = GET_CONTROLLER_FORM;

		Application::GetInstance()->GetAppResource()->GetString(L"IDS_BLUETOOTH_FAILED", errorMsg);
		pControllerForm->ShowPopup(errorMsg);
	}

}

void
ShareMenu::OnOppTransferDone (const Osp::Base::String &filePath, int fileSize, bool isCompleted)
{

}

void
ShareMenu::OnOppTransferInProgress (int percent)
{

}

void
ShareMenu::CancelBTPush(void)
{
	if(__pBTOppClient)
	{
		__pBTOppClient->CancelPush();
		delete __pBTOppClient;
	}

}
