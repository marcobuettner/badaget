#include "Main.h"
#include "ControllerForm.h"
#include "ViewMainForm.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::System;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Base::Collection;

AppBuilder::AppBuilder()
{
}

AppBuilder::~AppBuilder()
{
}

Application*
AppBuilder::CreateInstance(void)
{
	// Create the instance through the constructor.
	return new AppBuilder();
}

bool
AppBuilder::OnAppInitializing(AppRegistry& appRegistry)
{
	Frame *pFrame = GetAppFrame()->GetFrame();
	ControllerForm *pControllerForm = new ControllerForm();
	pControllerForm->Initialize();
	pFrame->AddControl(*pControllerForm);

	//load first page
	pControllerForm->LoadPage(0);

	return true;
}

bool
AppBuilder::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{

	return true;
}

void
AppBuilder::OnForeground(void)
{
}

void
AppBuilder::OnBackground(void)
{
}

void
AppBuilder::OnLowMemory(void)
{
	// TODO:
	/* Waiting for design team's advice: whether to stop application or close some forms */

	ControllerForm *pControllerForm = null;

	if(GetAppFrame() && GetAppFrame()->GetFrame())
	{
		pControllerForm = static_cast<ControllerForm*>(GetAppFrame()->GetFrame()->GetControl(L"ControllerForm"));
	}

	if(pControllerForm)
	{
		// Display Low Memory message box
		String errorMsg;
		Application::GetInstance()->GetAppResource()->GetString(L"IDS_MEMORY_LOW", errorMsg);
		pControllerForm->ShowPopup(errorMsg);

		pControllerForm->HandleLowMemory();
	}

}

void
AppBuilder::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
	// TODO:
	/* Waiting for design team's advice: whether to stop application or close some forms */
}

void
AppBuilder::OnScreenOn (void)
{

}

void
AppBuilder::OnScreenOff (void)
{

}
