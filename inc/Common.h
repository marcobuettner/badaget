#ifndef COMMON_H_
#define COMMON_H_

#define LOGO_PATH						L"logo.png"
#define	PATH_ICON_REFRESH_NORMAL		L"00_Header_icon_refresh.png"
#define	PATH_ICON_REFRESH_PRESSED		L"00_Header_icon_refresh_press.png"
#define	PATH_ICON_ZOOMIN_NORMAL			L"00_footer_icon_minus.png"
#define	PATH_ICON_ZOOMIN_PRESSED		L"00_footer_icon_minus_press.png"
#define	PATH_ICON_ZOOMOUT_NORMAL		L"00_footer_icon_plus.png"
#define	PATH_ICON_ZOOMOUT_PRESSED		L"00_footer_icon_plus_press.png"

#define FOOTER_COLOR					Color(90, 90, 90)

#define CONTROLLER_FORM					L"ControllerForm"
#define MAIN_FORM						L"MainForm"

#define GET_MAIN_FORM					static_cast<ViewMainForm*>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl(L"MainForm"))
#define GET_CONTROLLER_FORM				static_cast<ControllerForm*>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl(L"ControllerForm"))

#define MAX_BODY_FONT_SIZE				30
#define MAX_HEAD_FONT_SIZE				32
#define MIN_BODY_FONT_SIZE				12
#define MIN_HEAD_FONT_SIZE				17

#define PROFILE_XML_PATH				"/Home/Profile.xml"
#define ARTICLE_FILE_PATH				L"/Home/article.html"

/* Profile.xml Tags */
#define TAG_PAGE						"page"
#define TAG_SOURCE						"source"
#define TAG_HEADER						"header"
#define TAG_HEADER_COLOR				"BGColor"
#define TAG_HEADER_IMAGE				"BGImagePath"
#define TAG_PAGE_TITLE					"pagetitle"
#define TAG_BODY						"body"
#define TAG_BODYFG						"bodyFG"
#define TAG_FOOTER						"footer"
#define TAG_FONT						"font"

#define ATTRIBUTE_LAYOUT				"layout"
#define ATTRIBUTE_TYPE					"type"
#define ATTRIBUTE_TITLE					"title"
#define ATTRIBUTE_ICON_PATH				"titleIconPath"
#define ATTRIBUTE_RED					"r"
#define ATTRIBUTE_GREEN					"g"
#define ATTRIBUTE_BLUE					"b"
#define ATTRIBUTE_SIZE					"size"
#define ATTRIBUTE_WIDTH					"width"
#define ATTRIBUTE_SOURCE				"src"

/* RSS Tags */
#define TAG_ITEM						"item"
#define TAG_TITLE						"title"
#define TAG_LINK						"link"
#define TAG_PUB_DATE					"pubDate"
#define TAG_PUB_DATE2					"date"
#define TAG_ENCLOSURE					"enclosure"
#define TAG_CONTENT						"content"
#define TAG_GROUP						"group"
#define TAG_THUMBNAIL					"thumbnail"
#define TAG_DESCRIPTION					"description"
#define TAG_ENCODED						"encoded"
#define TAG_CREATOR						"creator"
#define TAG_IMAGE						"image"


#define ATTRIBUTE_DATA_TYPE				"type"
#define ATTRIBUTE_HEIGHT				"height"
#define ATTRIBUTE_WIDTH					"width"
#define ATTRIBUTE_URL					"url"


#define DATA_TYPE_IMAGE					L"image"
#define DATA_TYPE_AUDIO					L"audio"
#define DATA_TYPE_VIDEO					L"video"


#define PAGE_TYPE_TEXT					L"text"
#define PAGE_TYPE_AUDIO					L"audio"
#define PAGE_TYPE_VIDEO					L"video"
#define PAGE_TYPE_IMAGE					L"image"

/* Design Source Tags */
#define TAG_DESIGN_LIST					"list"
#define TAG_DESIGN_TEXT					"TXT"
#define TAG_DESIGN_X					"x"
#define TAG_DESIGN_Y					"y"
#define TAG_DESIGN_W					"w"
#define TAG_DESIGN_H					"h"
#define TAG_DESIGN_TYPE					"type"
#define TAG_DESIGN_FONTSIZE				"fontsize"
#define TAG_DESIGN_IMAGE				"image"
#define TAG_DESIGN_ICONLIST				"iconlist"

#define FIT_TYPE_CLIP					L"clip"
#define FIT_TYPE_SCALE					L"scale"
#define FIT_TYPE_FIT					L"fit"

#define IMAGE_GRID_COUNT_MAX			9999

#define TIMEOUT_POPUP					2000

#define DEFAULT_LIST_IMAGE_WIDTH		200
#define DEFAULT_LIST_IMAGE_HEIGHT		150


#define PAGE_XML_PATH_PREFIX			"/Home/List_"

#define FONT_SIZE_TIME					17
#define GAP_HEAD_TIME					8
#define TIME_AREA_HEIGHT				20
#define MAX_DESC_LENGTH					150

#define IMAGE_DECODE_QCOUNT_MAX			3

#endif /* COMMON_H_ */
