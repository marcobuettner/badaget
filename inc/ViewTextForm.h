#ifndef _VIEWTEXT_H_
#define _VIEWTEXT_H_

#include "ControllerForm.h"
#include "ShareMenu.h"
#include <FBase.h>
#include <FUi.h>
#include <FIo.h>
#include <FNet.h>
#include <FMedia.h>
#include <FGraphics.h>
#include <FWeb.h>

class ViewTextForm :
	public Osp::Ui::Controls::Form,
	public Osp::Web::Controls::ILoadingListener,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::Controls::IFormBackEventListener,
	public Osp::Ui::ITouchEventListener,
	public Osp::Ui::ITextEventListener,
	public Osp::Ui::IKeypadEventListener,
	public Osp::Web::Controls::IWebUiEventListener
{
public:
		ViewTextForm(void) {}
		virtual ~ViewTextForm(void) {}
		bool Initialize(ArticleData *pArticleArgs);

private:
		static const int ID_PREVIOUS = 101;
		static const int ID_NEXT = 102;
		static const int ID_CLOSESEARCH = 103;

		static const int ID_ZOOM_IN = 201;
		static const int ID_ZOOM_OUT = 202;

		static const int ID_SHARE = 301;
		static const int ID_SEARCH = 302;
		static const int ID_ZOOM = 303;

		static const int ID_COPY = 401;
		static const int ID_TEXTSEARCH = 402;
		static const int ID_TRANSLATE = 403;

		Osp::Base::String __fullText, __headline, __date, __link, __auth;
		Osp::Base::String __imgPath;
		int __width, __height;
		int __headFontSize, __bodyFontSize;
		bool __zoomEnabled;
		bool __longPressed;
		Osp::Graphics::Color __colorHeader, __colorPagetitle, __colorBody, __colorFont;
		Osp::Web::Controls::Web *__pWeb;
		ShareMenu *__pShareMenu;
		Osp::Ui::Controls::ContextMenu *__pTextMenu;
		Osp::Ui::Controls::EditField *__pEditSearch;
		Osp::Ui::Controls::Button *__pButtonPrevious, *__pButtonNext;
		Osp::Ui::Controls::Panel *__pSearchPanel;
		Osp::Base::Collection::LinkedList __listGarbage;
		Osp::Graphics::Bitmap *__pBmpZoomIn, *__pBmpZoomOut, *__pBmpZoomInPress, *__pBmpZoomOutPress;

private:
		void LoadFooter(void);
		void LoadPage(void);
		void ClearDiv(const int index, Osp::Base::String& desc);
		void Cleanup(Osp::Base::String& htmlPage);
		void GetNormalString(Osp::Base::String& htmlPage);
		void ShowShareMenu(void);
		void ShowTextMenu(Osp::Graphics::Point menuPos);
		void CopyToClipboard(void);
		void SearchInternet(void);
		void HandleSearch(void);
		void HandleZoom(void);
		void CloseSearchBar(void);

		virtual result OnInitializing(void);
		virtual result OnTerminating(void);

		void OnFormBackRequested(Osp::Ui::Controls::Form& source) ;
		void OnActionPerformed(const Osp::Ui::Control& source, int actionId);

		virtual bool OnHttpAuthenticationRequestedN(const Osp::Base::String& host, const Osp::Base::String& realm, const Osp::Web::Controls::AuthenticationChallenge& authentication);
		virtual void OnHttpAuthenticationCanceled();
		virtual void OnLoadingStarted(void);
		virtual void OnLoadingCanceled(void);
		virtual void OnLoadingErrorOccurred(Osp::Web::Controls::LoadingErrorType error, const Osp::Base::String &reason);
		virtual void OnLoadingCompleted(void);
		virtual void OnEstimatedProgress(int progress);
		virtual void OnPageTitleReceived(const Osp::Base::String& title);
		virtual bool OnLoadingRequested(const Osp::Base::String& url, Osp::Web::Controls::WebNavigationType type);
		virtual Osp::Web::Controls::DecisionPolicy OnWebDataReceived(const Osp::Base::String& mime, const Osp::Net::Http::HttpHeader& header);

		virtual void  OnTouchDoublePressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
		virtual void  OnTouchFocusIn(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
		virtual void  OnTouchFocusOut(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
		virtual void  OnTouchLongPressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
		virtual void  OnTouchMoved(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
		virtual void  OnTouchPressed(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
		virtual void  OnTouchReleased(const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);

		virtual void OnTextValueChanged(const Osp::Ui::Control& source);
		virtual void OnTextValueChangeCanceled(const Osp::Ui::Control& source);

		virtual void  OnKeypadActionPerformed(Osp::Ui::Control &source, Osp::Ui::KeypadAction keypadAction);
		virtual void  OnKeypadClosed(Osp::Ui::Control &source);
		virtual void  OnKeypadOpened(Osp::Ui::Control &source);
		virtual void  OnKeypadWillOpen(Osp::Ui::Control &source);

		virtual void  OnWebPageBlockSelected(Osp::Web::Controls::Web &source, Osp::Graphics::Point &startPoint, Osp::Graphics::Point &endPoint);
		virtual void  OnWebPageShowRequested(Osp::Web::Controls::Web &source);
		virtual void  OnWebWindowCloseRequested(Osp::Web::Controls::Web &source);
		virtual Osp::Web::Controls::Web *  OnWebWindowCreateRequested(void);

};
#endif
