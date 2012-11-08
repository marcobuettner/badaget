#ifndef PAGEPANEL_H_
#define PAGEPANEL_H_

#include "ControllerForm.h"
#include <FBase.h>
#include <FUi.h>
#include <FWeb.h>
#include <FIo.h>
#include <FNet.h>
#include <FMedia.h>
#include <FGraphics.h>

class PagePanel;

class IconListLoader :
	public Osp::Ui::Controls::IIconListViewItemProvider,
	public Osp::Ui::Controls::IIconListViewItemEventListener
{
public:
		IconListLoader(void)
		{
			pTexts = null;
			pImages = null;
			pViewParent = null;
			pBlankImage = null;
		}
		virtual ~IconListLoader(void)
		{
		}

public:
		Osp::Base::Collection::LinkedList *pImages, *pTexts;
		Osp::Graphics::Bitmap *pBlankImage;
		PagePanel *pViewParent;

private:
		// IIconListViewItemEventListener
		virtual void OnIconListViewItemStateChanged (Osp::Ui::Controls::IconListView &view, int index, Osp::Ui::Controls::IconListViewItemStatus status);

		//IIconListViewItemProvider
		virtual Osp::Ui::Controls::IconListViewItem* CreateItem(int index);
		virtual bool DeleteItem(int index, Osp::Ui::Controls::IconListViewItem* pItem);
		virtual int GetItemCount(void);
};


class PagePanel :
	public Osp::Ui::Controls::Panel,
	public Osp::Ui::Controls::IListViewItemEventListener,
	public Osp::Ui::Controls::IListViewItemProvider,
	public Osp::Ui::ITouchEventListener
{
public:
		PagePanel(void);
		virtual ~PagePanel(void) {}
		bool Initialize(PageLoadData *pPageDetails);
		void Notify(RequestId reqId, Osp::Base::Collection::IList* pArgs);
		void ClearData(void);

private:
		static const int ID_FORMAT_STRING = 500;
		static const int ID_FORMAT_BITMAP = 501;
		static const int ID_FORMAT_TIME = 502;

		static const RequestId NOTIFY_TEXT_COMPLETE = 100;
		static const RequestId NOTIFY_IMAGE_COMPLETE = 101;

		static const int EVENT_TOUCH = 201;
		static const int EVENT_LIST = 202;
		static const int EVENT_ICON = 203;

		Osp::Ui::Controls::ListView* __pList;
		Osp::Ui::Controls::IconListView *__pIconList1;
		Osp::Base::Collection::LinkedList __listHeadlines, __listImages, __listLinks, __listTimes;
		Osp::Base::Collection::LinkedList __listWkgHeadlines, __listWkgTimes;
		Osp::Base::Collection::LinkedList __listElements, __listX, __listY, __listW, __listH, __listFontSizes;
		Osp::Base::Collection::LinkedList __listGarbage;
		int __itemHeight;
		Osp::Base::String __fitType;
		Osp::Base::String __pageTitle, __pageType, __pageStyle;
		int __imageGridCount;
		IconListLoader *__pIconLoader1;
		Osp::Graphics::Color __colorHeader, __colorPagetitle, __colorBody, __colorFont;
		int __selfIndex;
		ControllerForm *__pControllerForm;
		Osp::Ui::Controls::Label *__pLblMainImage, *__pLblMainText;

		Osp::Base::Collection::LinkedList __loadingImages;

private:
		bool ConfigureLayout(void);
		bool LoadText(void);
		bool LoadImages(void);
		int ToInt(char* chString);
		void CreateControls(void);
		void HandleContentType(int index, int eventSource);

private:

		result OnInitializing(void);
		result OnTerminating(void);

		// IListViewItemEventListener
		void OnListViewContextItemStateChanged(Osp::Ui::Controls::ListView &listView, int index, int elementId, Osp::Ui::Controls::ListContextItemStatus state);
		void OnListViewItemStateChanged(Osp::Ui::Controls::ListView &listView, int index, int elementId, Osp::Ui::Controls::ListItemStatus status);
		void OnListViewItemSwept(Osp::Ui::Controls::ListView &listView, int index, Osp::Ui::Controls::SweepDirection direction);

		//IListViewItemProvider
		Osp::Ui::Controls::ListItemBase* CreateItem (int index, int itemWidth);
		bool  DeleteItem (int index, Osp::Ui::Controls::ListItemBase *pItem, int itemWidth);
		int GetItemCount(void);

		void OnTouchPressed(const Osp::Ui::Control& source, const Osp::Graphics::Point& currentPosition, const Osp::Ui::TouchEventInfo & touchInfo);
		void OnTouchLongPressed(const Osp::Ui::Control& source, const Osp::Graphics::Point& currentPosition, const Osp::Ui::TouchEventInfo& touchInfo) {}
		void OnTouchReleased(const Osp::Ui::Control& source, const Osp::Graphics::Point& currentPosition, const Osp::Ui::TouchEventInfo& touchInfo) {}
		void OnTouchMoved(const Osp::Ui::Control& source, const Osp::Graphics::Point& currentPosition, const Osp::Ui::TouchEventInfo& touchInfo) {}
		void OnTouchDoublePressed(const Osp::Ui::Control& source, const Osp::Graphics::Point& currentPosition, const Osp::Ui::TouchEventInfo& touchInfo)  {}
		void OnTouchFocusIn(const Osp::Ui::Control& source,	const Osp::Graphics::Point& currentPosition, const Osp::Ui::TouchEventInfo& touchInfo) {}
		void OnTouchFocusOut(const Osp::Ui::Control& source, const Osp::Graphics::Point& currentPosition, const Osp::Ui::TouchEventInfo& touchInfo) {}

		friend class IconListLoader;
};


#endif /* PAGEPANEL_H_ */
