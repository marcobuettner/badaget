#ifndef _DATAPAGE_H_
#define _DATAPAGE_H_

#include "Common.h"
#include <FApp.h>
#include <FBase.h>
#include <FUi.h>
#include <FNet.h>
#include <FMedia.h>

class PagePanel;

enum PageState
{
	PAGE_STATE_INITIALIZED = 0,
	PAGE_STATE_FETCH_ONGOING,
	PAGE_STATE_FETCH_COMPLETE,
	PAGE_STATE_FETCH_FAIL,
	PAGE_STATE_MAX
};

struct PageLoadData
{
	Osp::Base::String __pageType, __pageTitle, __pageLayout;
	Osp::Graphics::Color __colorHeader, __colorPagetitle, __colorBody, __colorFont;
	int __selfIndex;
};

struct ArticleData
{
	Osp::Base::String __headline, __description, __time, __link, __imagePath, __author;
	Osp::Graphics::Color __colorBody, __colorFont, __colorHeader, __colorPagetitle;
};

struct GalleryData
{
	Osp::Base::Collection::LinkedList *__pImages, *__pTexts, *__pLinks;
	Osp::Graphics::Color __colorHeader, __colorPagetitle, __colorBody, __colorFont;
	int __startIndex;
};

class PageDetails:
	public Osp::Media::IImageDecodeUrlEventListener,
	public Osp::Net::Http::IHttpTransactionEventListener
{

private:
		PagePanel *__pPagePanel;
		Osp::Base::String __pageType, __pageSource, __pageLayout, __pageTitle;
		char __xmlFileName[300];
		int __decodeQCount, __currentQIndex;
		Osp::Media::Image *__pImg;
		int __selfIndex;
		Osp::Net::Http::HttpSession*	__pSession;
		Osp::Net::Http::HttpTransaction* __pTransaction;
		PageState __pageState;

		Osp::Base::Collection::LinkedList __listHeadlines, __listTimes, __listLinks;
		Osp::Base::Collection::LinkedList __listImages, __listImgReqId;
		Osp::Graphics::Color __colorHeader, __colorPagetitle, __colorBody, __colorFont;

		static const RequestId UPDATE_QUEUE = 500;

public:
		PageDetails(void);
		virtual ~PageDetails(void);
		bool IsAlive(void) const;
		PageState GetState(void) const;
		void UpdateQCount();

private:
		void Initialize(void);
		result Load(void);
		void Destroy(void);

		bool FetchFeed(void);
		bool ParseFeed(void);
		bool GetTextData(int index, ArticleData *pFullData);
		bool GetAudioData(int index, Osp::Base::Collection::LinkedList *pFullData);
		bool GetVideoData(int index, Osp::Base::Collection::LinkedList *pFullData);
		bool GetDescription(int index, Osp::Base::String *pDescription);

		void HandleUrlImages(void);
		RequestId RequestDecode(void);

		void Cleanup(Osp::Base::String& htmlPage);
		void ClearDiv(const int index, Osp::Base::String& desc);

private:
		virtual result OnInitializing(void) { return E_SUCCESS; }
		virtual result OnTerminating(void) { return E_SUCCESS; }

		void OnTransactionReadyToRead(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int availableBodyLen);
		void OnTransactionAborted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, result r);
		void OnTransactionReadyToWrite(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int recommendedChunkSize);
		void OnTransactionHeaderCompleted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int headerLen, bool rs);
		void OnTransactionCompleted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction);
		void OnTransactionCertVerificationRequiredN(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, Osp::Base::String* pCert);

		void  OnImageDecodeUrlReceived(RequestId reqId, Osp::Graphics::Bitmap *pBitmap, result r, const Osp::Base::String errorCode, const Osp::Base::String errorMessage);

		friend class ControllerForm;
};

class DisplayImage :
		public Osp::Base::Object
{
public:
	DisplayImage(void);
	virtual ~DisplayImage(void);

public:
	Osp::Base::String url;
	int height, width;
	int index;
	Osp::Graphics::Bitmap *pBmpImage;
};


#endif
