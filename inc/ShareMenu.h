/*
 * ShareMenu.h
 *
 *  Created on: Oct 12, 2011
 *      Author: test
 */

#ifndef SHAREMENU_H_
#define SHAREMENU_H_

#include "ControllerForm.h"
#include <FBase.h>
#include <FUi.h>
#include <FIo.h>
#include <FNet.h>
#include <FMedia.h>
#include <FGraphics.h>
#include <FWeb.h>

class ShareMenu :
	public Osp::Ui::Controls::ContextMenu,
	public Osp::Ui::IActionEventListener,
	public Osp::App::IAppControlEventListener,
	public Osp::Net::Bluetooth::IBluetoothOppClientEventListener
{
public:
		ShareMenu(void) {}
		virtual ~ShareMenu(void) {}
		bool Initialize(Osp::Graphics::Point loadPoint, Osp::Base::String& content, bool isLink);
		void CancelBTPush(void);

private:
		static const int ID_MESSAGE = 501;
		static const int ID_EMAIL = 502;
		static const int ID_BLUETOOTH = 503;

		Osp::Base::String __shareContent;
		bool __isLink;
		Osp::Net::Bluetooth::BluetoothOppClient *__pBTOppClient;

		void LoadEmailWnd(void);
		void LoadMsgWnd(void);
		void LoadBluetoothWnd(void);

		virtual result OnInitializing(void);
		virtual result OnTerminating(void);
		void OnActionPerformed(const Osp::Ui::Control& source, int actionId);
		virtual void  OnAppControlCompleted (const Osp::Base::String &providerId, const Osp::Base::String &operationId, const Osp::Base::Collection::IList *pResultList);

		virtual void  OnOppPushResponded (result r);
		virtual void  OnOppTransferDone (const Osp::Base::String &filePath, int fileSize, bool isCompleted);
		virtual void  OnOppTransferInProgress (int percent);

};
#endif /* SHAREMENU_H_ */
