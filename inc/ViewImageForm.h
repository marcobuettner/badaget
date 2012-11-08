#ifndef _VIEWIMAGE_H_
#define _VIEWIMAGE_H_

#include "ControllerForm.h"
#include "ShareMenu.h"
#include <FBase.h>
#include <FUi.h>
#include <FIo.h>
#include <FNet.h>
#include <FMedia.h>
#include <FGraphics.h>

class ViewImageForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::Controls::IGalleryItemProvider,
	public Osp::Ui::Controls::IGalleryEventListener,
	public Osp::Ui::Controls::IFormBackEventListener,
	public Osp::Ui::IActionEventListener
{
public:
		ViewImageForm(void) {}
		virtual ~ViewImageForm(void) {}
		bool Initialize(GalleryData *pImageParams);

private:
		static const int ID_SHARE = 301;
		static const int ID_SAVE = 302;

		Osp::Ui::Controls::Gallery* __pGallery;
		Osp::Base::Collection::LinkedList *__pImages, *__pTexts, *__pLinks;
		int __startIndex;
		Osp::Graphics::Color __colorHeader, __colorPagetitle, __colorBody, __colorFont;
		ShareMenu *__pShareMenu;


private:
		void ShowShareMenu(void);
		void SaveImage(void);

		virtual result OnInitializing(void);
		virtual result OnTerminating(void);
		virtual result OnDraw(void);
		void OnFormBackRequested(Osp::Ui::Controls::Form& source) ;
		void OnActionPerformed(const Osp::Ui::Control& source, int actionId);

		// IGalleryItemEventListener
		virtual void OnGalleryCurrentItemChanged (Osp::Ui::Controls::Gallery &view, int index);
		virtual void OnGalleryItemClicked(Osp::Ui::Controls::Gallery& listView, int index);
		virtual void OnGallerySlideShowStarted(Osp::Ui::Controls::Gallery& gallery);
		virtual void OnGallerySlideShowStopped(Osp::Ui::Controls::Gallery& gallery);

		//IGalleryItemProvider
		virtual Osp::Ui::Controls::GalleryItem* CreateItem(int index);
		virtual bool DeleteItem(int index, Osp::Ui::Controls::GalleryItem* pItem);
		virtual int GetItemCount(void);
};


#endif


