// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"
#include <vector>
#include <gdiplus.h>

using namespace Gdiplus;



string16 GetAppFileName()
{
	char16 filename[MAX_PATH + 10] = {0};
	GetModuleFileNameW(NULL, filename, MAX_PATH);
	return string16(filename);
}


string16 GetAppPath()
{
	string16 filename = GetAppFileName();
	size_t i = filename.find_last_of(L'\\');
	string16 s = filename.substr(0, i);

	if (*(s.end() - 1) != L'\\')
		s.append(L"\\");

	return s;
}

std::string to_utf8(const string16& string)
{
	int required_size =
		WideCharToMultiByte(CP_UTF8, 0,
		string.c_str(), (int)string.size(),
		NULL, 0, NULL, NULL);

	if (required_size == 0)
		return StringUtil::sEmpty;

	std::vector<char> buf(++required_size);
	WideCharToMultiByte(CP_UTF8, 0,
		string.c_str(), (int)string.size(),
		&buf[0], required_size,
		NULL, NULL);
	return std::string(&buf[0]);
}




namespace cactus
{
	int KMemoryDC::GetEncoderClsid(const wchar_t* format, CLSID* pClsid)
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes
		ImageCodecInfo* pImageCodecInfo = NULL;
		Gdiplus::GetImageEncodersSize(&num, &size);

		if (size == 0)
			return -1;  // Failure

		pImageCodecInfo = (ImageCodecInfo*)(malloc(size));

		if (pImageCodecInfo == NULL)
			return -1;  // Failure

		Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j) {
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;  // Success
			}
		}

		free(pImageCodecInfo);
		return -1;  // Failure
	}

	BOOL KMemoryDC::SaveHDCToFile(const wchar_t* filename)
	{
		BOOL bRet = FALSE;
		CLSID pngClsid;
		GetEncoderClsid(L"image/bmp", &pngClsid);
		Gdiplus::Bitmap *pbmSrc = Gdiplus::Bitmap::FromHBITMAP(m_bmp.m_hBitmap, NULL);

		if (pbmSrc->Save(filename, &pngClsid) == Ok) {
			bRet = TRUE;
		}

		delete pbmSrc;
		return bRet;
	}

	BOOL KMemoryDC::ReSize(RECT& rcPaint, COLORREF clrBk, LPCSTR pszSkinName)
	{
		m_rcPaint = rcPaint;
		ATLASSERT(m_hDC != NULL);
		m_bmp.Attach(KCreateCompatibleBitmap(m_hDCOriginal, m_rcPaint.right - m_rcPaint.left, m_rcPaint.bottom - m_rcPaint.top));
		ATLASSERT(m_bmp.m_hBitmap != NULL);
		m_hBmpOld = SelectBitmap(m_bmp);

		if (pszSkinName) {
			//          CDCHandle dc( m_hDC );
			//          BkSkin::Draw( dc, pszSkinName, CRect(m_rcPaint), 0 );
			Gdiplus::Graphics gs(m_hDC);
			Gdiplus::SolidBrush bgBrush(Color(237, 243, 246));
			gs.FillRectangle(&bgBrush, 0, 0, m_rcPaint.right - m_rcPaint.left, m_rcPaint.bottom - m_rcPaint.top);
		} else if (clrBk != CLR_INVALID) {
			Gdiplus::Graphics gs(m_hDC);
			Gdiplus::SolidBrush bgBrush(Gdiplus::Color(254, GetRValue(clrBk), GetGValue(clrBk), GetBValue(clrBk)));
			gs.FillRectangle(&bgBrush, 0, 0, m_rcPaint.right - m_rcPaint.left, m_rcPaint.bottom - m_rcPaint.top);
		}

		SetViewportOrg(-m_rcPaint.left, -m_rcPaint.top);
		return TRUE;
	}

	BOOL KMemoryDC::Init(HDC hDc, BOOL bAutoPaint)
	{
		m_hDCOriginal = hDc;

		if (IsNull())
			CreateCompatibleDC(hDc);

		return TRUE;
	}

	KMemoryDC::KMemoryDC() : m_bAutoPaint(FALSE)
		, m_hBmpOld(NULL)
	{
	}

	KMemoryDC::KMemoryDC(HDC hDC, RECT& rcPaint, BOOL bAutoPaint /*= TRUE */) : m_hDCOriginal(hDC), m_hBmpOld(NULL)
	{
		m_bAutoPaint = bAutoPaint;
		m_rcPaint = rcPaint;
		CreateCompatibleDC(m_hDCOriginal);
		ATLASSERT(m_hDC != NULL);
		m_bmp.Attach(KCreateCompatibleBitmap(m_hDCOriginal, m_rcPaint.right - m_rcPaint.left, m_rcPaint.bottom - m_rcPaint.top));
		ATLASSERT(m_bmp.m_hBitmap != NULL);
		m_hBmpOld = SelectBitmap(m_bmp);
		SetViewportOrg(-m_rcPaint.left, -m_rcPaint.top);
	}

	KMemoryDC::~KMemoryDC()
	{
		if (m_bAutoPaint && m_hDCOriginal && m_hDC)
			::BitBlt(m_hDCOriginal, m_rcPaint.left, m_rcPaint.top, m_rcPaint.right - m_rcPaint.left, m_rcPaint.bottom - m_rcPaint.top, m_hDC, m_rcPaint.left, m_rcPaint.top, SRCCOPY);

		if (m_hBmpOld)
			SelectBitmap(m_hBmpOld);
	}

	HBITMAP KMemoryDC::KCreateCompatibleBitmap(HDC hdc, int nWidth, int nHeight)
	{
		BYTE*                       pBits = NULL;
		BITMAPINFOHEADER            bmih;
		ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
		bmih.biSize                 = sizeof(BITMAPINFOHEADER);
		bmih.biWidth                = nWidth;
		bmih.biHeight               = nHeight;
		bmih.biPlanes               = 1 ;
		bmih.biBitCount             = 32;        //这里一定要是32
		bmih.biCompression          = BI_RGB ;
		bmih.biSizeImage            = 0 ;
		bmih.biXPelsPerMeter        = 0 ;
		bmih.biYPelsPerMeter        = 0 ;
		bmih.biClrUsed              = 0 ;
		bmih.biClrImportant         = 0 ;
		HBITMAP hBitMap = CreateDIBSection(hdc, (BITMAPINFO *)  &bmih, 0, (VOID**)&pBits, NULL, 0);
		return hBitMap;
	}
}

using namespace cactus;


void draw01(cairo_t* cr)
{
    cairo_set_source_rgb(cr, 0.1, 0, 0);

    /* 不旋转 */
    cairo_rectangle(cr, 500, 50, 200, 200);
    cairo_set_line_width(cr, 14);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL); 
    cairo_stroke(cr);

    cairo_set_font_size(cr, 30.0);
    cairo_move_to(cr, 510, 100);
    cairo_show_text(cr, "NO Rotate");


    /* 将坐标軕旋转60度 */
    cairo_rotate(cr, M_PI / 3);
    cairo_rectangle(cr, 500, 50, 200, 200);
    cairo_set_line_width(cr, 14);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND); 
    cairo_stroke(cr);    

    cairo_set_font_size(cr, 30.0);
    cairo_move_to(cr, 510, 100);
    cairo_show_text(cr, "Rotate 60");

    /* 将坐标軕旋转-60度, 回到正常情况 */
    cairo_rotate(cr, -(M_PI / 3));
    cairo_rectangle(cr, 500, 400, 200, 200);
    cairo_set_line_width(cr, 14);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND); 
    cairo_stroke(cr);    

    cairo_set_font_size(cr, 30.0);
    cairo_move_to(cr, 510, 450);
    cairo_show_text(cr, "Rotate -60");
}




CMainDlg::CMainDlg()
{
    InitDrawPara();
	m_filename = GetAppPath() + L"png-0070.png";
}

CMainDlg::~CMainDlg()
{

}


LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // center the dialog on the screen
    CenterWindow();

    // set icons
    HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
    SetIcon(hIconSmall, FALSE);

	

    return TRUE;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
    dlg.DoModal();
    return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add validation code
    //EndDialog(wID);
    return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    EndDialog(wID);
    return 0;
}

void CMainDlg::InitDrawPara()
{
    m_PrePt = CPoint(-1,-1);
    m_DownPt = CPoint(-1,-1);
}

LRESULT CMainDlg::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CPaintDC dc(m_hWnd);
    CRect rcClient;
    GetClientRect(&rcClient);
    CRect rcDraw(rcClient.left, rcClient.top, rcClient.Width(), 3*(rcClient.Height()/4));

    KMemoryDC memDC(dc, rcDraw, FALSE);
    memDC.SetBkMode(TRANSPARENT);
    Gdiplus::Graphics g(memDC);
    //DoPaint(g, rcDraw);

    //g.SetSmoothingMode(SmoothingModeAntiAlias);
    //g.Clear(Color::MakeARGB(255, 0, 255, 255));
    //g.ReleaseHDC(memDC.m_hDC);
	
	cairo_surface_t *surface = cairo_win32_surface_create(memDC.m_hDC);
	cairo_t *cr = cairo_create(surface);
    cairo_surface_destroy(surface);

    //draw01(cr);

	// Draw on the cairo context.
	gradientExample(cr, rcDraw);


	LinearGradientBrush linGrBrush(Point(100,0),Point(100,100),Color(255,255,0,0),Color(255,0,255,0));
	Color colors[] =
	{
		Color(255, 255, 0, 0),   // red
		Color(255, 255, 255, 0), //yellow
		Color(255, 0, 0, 255),   // blue
		Color(255, 0, 255, 0)
	};  // green
	REAL positions[] =
	{
		0.0f,
		0.33f,
		0.66f,
		1.0f
	};
	linGrBrush.SetInterpolationColors(colors, positions,4);
	// 填充指定区域矩形
	g.FillRectangle(&linGrBrush,600,0,100,100);

    //cairo_surface_t *image;  
    //image = cairo_image_surface_create_from_png("bird.png");
    //cairo_set_source_surface(cr, image, 0, 0);
    //cairo_paint(cr);
    //cairo_surface_destroy(image);
    // Cleanup.
    

	
	

	//cairo_surface_write_to_png(surface, "test-chinese.png");
	//cairo_surface_destroy(surface);

	
	g.ReleaseHDC(memDC.m_hDC);
	cairo_destroy(cr);

    dc.BitBlt(0, 0, rcDraw.Width(), rcDraw.Height(), memDC, 0, 0, SRCCOPY);

	 
    
	

    return 0;
}

LRESULT CMainDlg::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return 1;
}

void CMainDlg::DoPaint(Gdiplus::Graphics& g, CRect& rcDraw)
{
    g.SetSmoothingMode(SmoothingModeAntiAlias);
	//g.Clear(Color::MakeARGB(255, 32, 32, 32));	
    Rect rc(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());
	SolidBrush br(Color::Blue);
    g.FillRectangle(&br, rc);

	

	DrawBackgroud(g, rc);


    LinearGradientBrush linGrBrush(Point(100,0),Point(100,100),Color(255,255,0,0),Color(255,0,255,0));
    Color colors[] =
    {
        Color(255, 255, 0, 0),   // red
        Color(255, 255, 255, 0), //yellow
        Color(255, 0, 0, 255),   // blue
        Color(255, 0, 255, 0)
    };  // green
    REAL positions[] =
    {
        0.0f,
        0.33f,
        0.66f,
        1.0f
    };
    linGrBrush.SetInterpolationColors(colors, positions,4);
    // 填充指定区域矩形
    g.FillRectangle(&linGrBrush,100,0,100,100);

	FontFamily fontFamily(L"Lucida Console");   
	Gdiplus::Font grFont(&fontFamily, 9, FontStyleRegular, UnitPoint);
	

	SolidBrush br2(Color::DarkViolet);
	PointF      pointF(10.0f, 20.0f);	

	string16 message(L"......MINGRISOFT");

	//g.DrawString(message.c_str(), message.length(), &grFont3, PointF(0.0f, 0.0f), &brush3);

	for (int i = 0; i <= 360; i += 10)
	{	
		//平移Graphics对象到窗体中心
		g.TranslateTransform(rcDraw.Width() / 2, rcDraw.Height() / 2);
		//设置Graphics对象的输出角度
		g.RotateTransform(i);
		//设置文字填充颜色


		
		//旋转显示文字
		g.DrawString(message.c_str(), message.length(), &grFont, pointF, &br2);
		//恢复全局变换矩阵
		g.ResetTransform();
	}

	
	//g.Clear(SystemColors::getActiveBorder());

	
}

void CMainDlg::DrawBackgroud(Gdiplus::Graphics& g, Gdiplus::Rect& rc)
{
	//FontFamily fontFamily(L"Lucida Console"); 	
	//Gdiplus::Font grFont(&fontFamily, 60, FontStyleRegular, UnitPixel);
	TextureBrush br(Image::FromFile(m_filename.c_str()));
	g.FillRectangle(&br, rc);	

}

void CMainDlg::gradientExample(cairo_t* cr, CRect& rcDraw)
{	
	cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
	cairo_set_line_width(cr, 2);
	int i = 1;
	for (i = 1; i <= 3; i++)
	{
		cairo_move_to(cr, (rcDraw.Width()*i)/4, 0);
		cairo_line_to(cr, (rcDraw.Width()*i)/4, rcDraw.Height());
		cairo_move_to(cr, 0, (rcDraw.Height()*i)/4);
		cairo_line_to(cr, rcDraw.Width(), (rcDraw.Height()*i)/4);
	}

	//cairo_stroke(cr);
	
	// 绘制渐变背景
	cairo_pattern_t *pat;
	pat = cairo_pattern_create_linear (0.0, 0.0,  0.0, 256.0);
	cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
	cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
	cairo_rectangle (cr, 0, 0, rcDraw.Width(), rcDraw.Height());
	cairo_set_source (cr, pat);
	cairo_fill (cr);
	cairo_pattern_destroy (pat);


	


	pat = cairo_pattern_create_radial (115.2, 102.4, 25.6,
		102.4,  102.4, 128.0);
	cairo_pattern_add_color_stop_rgba (pat, 0, 1, 1, 1, 1);
	cairo_pattern_add_color_stop_rgba (pat, 1, 0, 0, 0, 1);
	cairo_set_source (cr, pat);
	cairo_arc (cr, 128.0, 128.0, 76.8, 0, 2 * M_PI);
	cairo_fill (cr);
	cairo_pattern_destroy (pat);

	

	
	//for ( i = 1; i <= 10; i++) {
	//	cairo_set_source_rgba(cr, 1, 0, 0, i*0.1);
	//	cairo_rectangle(cr, 50*i, 20, 40, 40);
	//	cairo_fill(cr);  
	//}      
	//

	///************************************************************************/
	///* 平移变换                                                                     */
	///************************************************************************/
	cairo_set_source_rgb(cr, 0.2, 0.3, 0.8);
	cairo_rectangle(cr, 10, 10, 30, 30);
	cairo_fill(cr);

	cairo_translate(cr, 20, 20);
	cairo_set_source_rgb(cr, 0.8, 0.3, 0.2);
	cairo_rectangle(cr, 0, 0, 30, 30);
	cairo_fill(cr);

	cairo_translate(cr, 30, 30);
	cairo_set_source_rgb(cr, 0.8, 0.8, 0.2);
	cairo_rectangle(cr, 0, 0, 30, 30);
	cairo_fill(cr);

	cairo_translate(cr, 40, 40);
	cairo_set_source_rgb(cr, 0.3, 0.8, 0.8);
	cairo_rectangle(cr, 0, 0, 30, 30);
	cairo_fill(cr);    


	cairo_set_font_size(cr, 22);

	cairo_set_source_rgb(cr, 0.9 , 0.9 , 0.9);
	cairo_move_to(cr, 100, 0);
	cairo_select_font_face(cr, "SimSun", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 20);	
	string16 msssage = L"测试 Beckov 2012 , (c) Jan Bodnar测试";
	cairo_show_text(cr, to_utf8(msssage).c_str());
	cairo_stroke(cr); 
	

	cairo_select_font_face (cr, "Microsoft YaHei", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 48);
	cairo_set_source_rgb(cr, 0.9 , 0.9 , 0.9);
	cairo_move_to (cr, 200, 40);
	cairo_show_text (cr, to_utf8(L"中文测试").c_str());
	


	/************************************************************************/
	/* 仿射变换                                                                     */
	/************************************************************************/
	cairo_matrix_t matrix;

	cairo_set_source_rgb(cr, 0.6, 0.6, 0.6);
	cairo_rectangle(cr, 20, 30, 80, 50);
	cairo_fill(cr);
	cairo_matrix_init(&matrix,
		1.0, 0.5,
		0.0, 1.0,
		0.0, 0.0);

	cairo_transform(cr, &matrix);
	cairo_rectangle(cr, 130, 30, 80, 50);
	cairo_fill(cr);
	

	/************************************************************************/
	/* 缩放变换                                                                     */
	/************************************************************************/
	
	cairo_set_source_rgb(cr, 0.2, 0.3, 0.8);
	cairo_rectangle(cr, 10, 10, 90, 90);    
	cairo_fill(cr);

	cairo_scale(cr, 0.6, 0.6);
	cairo_set_source_rgb(cr, 0.8, 0.3, 0.2);
	cairo_rectangle(cr, 30, 30, 90, 90);    
	cairo_fill(cr);  

	cairo_scale(cr, 0.8, 0.8);
	cairo_set_source_rgb(cr, 0.8, 0.8, 0.2);
	cairo_rectangle(cr, 50, 50, 90, 90);    
	cairo_fill(cr);   
	

	/************************************************************************/
	/* 位图                                                                     */
	/************************************************************************/
	/*cairo_surface_t *image;  
	image = cairo_image_surface_create_from_png("bird.png");
	cairo_set_source_surface(cr, image, 0, 0);
	cairo_paint(cr);
	cairo_surface_destroy(image);*/

	// 五角星
	{
		cairo_new_path(cr);
		cairo_rectangle(cr, 0.0, 0.0, 500.0, 500.0);

		cairo_set_source_rgba(cr, 0.20, 0.20, 0.20, 0.0);
		cairo_fill(cr);

		cairo_new_path(cr);
		cairo_move_to(cr,  68.0,  83.0);
		cairo_line_to(cr, 265.0, 418.0);
		cairo_line_to(cr, 288.0,  62.0);
		cairo_line_to(cr,  38.0, 352.0);
		cairo_line_to(cr, 392.0, 268.0);

		cairo_close_path(cr);

		cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
		cairo_set_source_rgba(cr, 0.2, 0.3, 1.0, 1.0);
		cairo_fill_preserve(cr);

		cairo_set_source_rgba(cr, 0.5, 1.0, 0.75, 1.0);
		cairo_set_line_width(cr, 10.0);
		cairo_stroke(cr);
	}

    

	{
        for (int i =0; i < 10; ++i) {
            cairo_move_to(cr,  500,  i+50);
		    cairo_surface_t* p_image = cairo_image_surface_create_from_png("bird.png");

		    cairo_set_source_surface(cr, p_image, 0, 0);
		    cairo_paint(cr);

		    cairo_translate(cr, 160.0, 120.0);
		    cairo_rotate(cr, -45.0 * 3.141592 / 180.0);
		    cairo_translate(cr, -160.0, -120.0);

		    cairo_set_source_surface(cr, p_image, 0, 0);
		    cairo_paint_with_alpha(cr, 0.3f);

		    cairo_surface_destroy(p_image);
        }
	}
		
	

	
	
}



