/*
 * ImageViewer.h
 *
 *  Created on: Oct 4, 2011
 *      Author: test
 */

#ifndef IMAGEVIEWER_H_
#define IMAGEVIEWER_H_


#include "ControllerForm.h"
#include "ViewTextForm.h"
#include <FBase.h>
#include <FUi.h>
#include <FIo.h>
#include <FNet.h>
#include <FMedia.h>
#include <FGraphics.h>

class ImageViewer :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::Controls::IGalleryItemProvider,
	public Osp::Ui::Controls::IFormBackEventListener,
	public Osp::Ui::IActionEventListener
{
public:
		ImageViewer(void) {}
		virtual ~ImageViewer(void) {}
		bool Initialize(Osp::Graphics::Bitmap *pImage, Osp::Graphics::Color bgColor);

private:
		static const int ID_SHARE = 301;
		static const int ID_SAVE = 302;

		Osp::Ui::Controls::Gallery* __pGallery;
		ShareMenu *__pShareMenu;
		Osp::Graphics::Bitmap* __pDisplayImage;
		Osp::Base::String savedPath;
		Osp::Graphics::Color __bgColor;


private:
		void ShowShareMenu();
		void SaveImage(void);

		virtual result OnInitializing();
		virtual result OnTerminating();
		virtual result OnDraw();
		void OnFormBackRequested(Osp::Ui::Controls::Form& source) ;
		void OnActionPerformed(const Osp::Ui::Control& source, int actionId);

		//IGalleryItemProvider
		virtual Osp::Ui::Controls::GalleryItem* CreateItem(int index);
		virtual bool DeleteItem(int index, Osp::Ui::Controls::GalleryItem* pItem);
		virtual int GetItemCount(void);
};


#endif /* IMAGEVIEWER_H_ */
