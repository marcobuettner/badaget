#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "DataPage.h"
#include <FApp.h>
#include <FBase.h>
#include <FUi.h>
#include "FXml.h"


class ControllerForm :
	public Osp::Ui::Controls::Form
{
private:
		Osp::Base::Collection::LinkedList __pages;
		int	__activePageIndex;
		Osp::Base::String __title, __headerIconPath, __headerBgImagePath;
		Osp::Graphics::Color __colorHeader;

public:
		ControllerForm(void);
		virtual ~ControllerForm(void);
		bool Initialize(void);
		bool GetPageTitles(Osp::Base::Collection::LinkedList *pTitles);
		bool GetArticleData(int selfIndex, const int index, ArticleData *pArticleArgs);
		bool GetAudioData(int selfIndex, const int index, Osp::Base::Collection::LinkedList *pArticleArgs);
		bool GetVideoData(int selfIndex, const int index, Osp::Base::Collection::LinkedList *pArticleArgs);
		PageState GetPageState(int index);
		void ShowPopup(const Osp::Base::String& message);
		void HandleLowMemory(void);
		int GetActivePageIndex(void);
		bool GetDescription(int selfIndex, int index, Osp::Base::String *pDescription);
		bool LoadPage(int index);
		bool FetchData(int index);
		Osp::Base::String GetTitle(void);
		Osp::Base::String GetIconPath(void);
		Osp::Graphics::Color GetHeaderColor(void);
		Osp::Base::String GetHeaderBgPath(void);
		Osp::Graphics::Color GetPageTitleColor(int index);

		static const RequestId REQUEST_LOAD_PAGE = 100;
		static const RequestId REQUEST_DATA = 101;
		static const RequestId UPDATE_QUEUE = 102;

		static const int MAX_PANEL_COUNT = 5;

private:
		bool ParseProfile(void);
		bool KillPage(int index);
		int GetActivePageCount(void);
		void GetColorsFromNode(Osp::Xml::xmlNodePtr pCurNode, Osp::Graphics::Color& color);

private:
		virtual result OnInitializing(void);
		virtual result OnTerminating(void);

		void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);


};
#endif
