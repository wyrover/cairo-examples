// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
typedef wchar_t char16;
typedef std::wstring string16;

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };
	CMainDlg();
	virtual ~CMainDlg();

public:
	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)		
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	void InitDrawPara();  
	void DoPaint(Gdiplus::Graphics& g, CRect& rcDraw);

private:
	void DrawBackgroud(Gdiplus::Graphics& g, Gdiplus::Rect& rc);
	void gradientExample(cairo_t* cr, CRect& rcDraw);

protected:
	CPoint m_PrePt; 
	CPoint m_DownPt;

private:
	string16 m_filename;
	
};


//------------------------------------------------
// kmemorydc.h
namespace cactus
{
	class KMemoryDC : public CDC
	{
	public:
		// Data members
		HDC     m_hDCOriginal;
		RECT    m_rcPaint;
		CBitmap m_bmp;
		HBITMAP m_hBmpOld;
		BOOL    m_bAutoPaint;

		static HBITMAP KCreateCompatibleBitmap(HDC hdc, int nWidth, int nHeight);

		//下面2个 一般是使用在 OnPaint DoPaint DrawItem
		KMemoryDC(HDC hDC, RECT& rcPaint, BOOL bAutoPaint = TRUE);

		~KMemoryDC();


		//下面3个函数是用法2,做一个控件的同生命周期的缓存区
		KMemoryDC();

		BOOL Init(HDC hDc, BOOL bAutoPaint);

		BOOL ReSize(RECT& rcPaint, COLORREF clrBk, LPCSTR pszSkinName);

		BOOL SaveHDCToFile(const wchar_t* filename);

	private:
		int GetEncoderClsid(const wchar_t* format, CLSID* pClsid);
	};
}


class StringUtil
{
public:
	static const std::string sEmpty;
	static const std::wstring sWEmpty;
	static const char* getNotNullString(const char* strValue)
	{
		return (strValue ? strValue : sEmpty.c_str());
	}

	static const char16* getNotNullString(const char16* strValue)
	{
		return (strValue ? strValue : sWEmpty.c_str());
	}


	template<typename StringPtrType>
	static bool NullOrEmpty(StringPtrType strPtr)
	{
		return !(strPtr && strPtr[0]);
	}

	template<typename StringPtrType>
	static bool NotNullOrEmpty(StringPtrType strPtr)
	{
		return (strPtr && strPtr[0]);
	}

	template<typename StringType>
	static void Trim(StringType& toTrim)
	{
		TrimRight(toTrim);
		TrimLeft(toTrim);
	}

	template<typename StringType>
	static void TrimLeft(StringType& toTrim)
	{
		while (::isspace(toTrim[0]))
			toTrim.erase(toTrim.begin(), toTrim.begin() + 1);
	}

	template<typename StringType>
	static void TrimRight(StringType& toTrim)
	{
		while (::isspace(toTrim[toTrim.length() - 1]))
			toTrim.erase(toTrim.length() - 1);
	}

	template<typename StringType>
	static void replace(StringType& strBig, const StringType& strSrc, const StringType& strDst)
	{
		typedef typename StringType::size_type size_type;
		size_type pos = 0;
		size_type srcLen = strSrc.size();
		size_type dstLen = strDst.size();

		while ((pos = strBig.find(strSrc, pos)) != StringType::npos) {
			strBig.replace(pos, srcLen, strDst);
			pos += dstLen;
		}
	}
};

__declspec(selectany) const std::wstring StringUtil::sWEmpty = L"";
__declspec(selectany) const std::string StringUtil::sEmpty = "";
