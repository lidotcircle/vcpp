#ifndef _D2DTEST_H
#define _D2DTEST_H

#include "BaseForm.h"
#include "BaseWindow.h"

#include<d2d1.h>
#include<dwrite.h>

template<typename T>
inline void SafeRelease(T** ptr) {
	if (*ptr) {
		(*ptr)->Release();
		*ptr = NULL;
	}
}

class MainWindow : public BaseWND<MainWindow> {
private:
	D2D1_ELLIPSE m_ellipse;
	ID2D1HwndRenderTarget* mp_renderTarget;
	ID2D1SolidColorBrush* mp_brush;
	ID2D1Factory* mp_factory;
	IDWriteFactory* mp_write_factory;
	IDWriteTextFormat* mp_text_format;
	IDWriteTextLayout* mp_text_layout;

	bool initializeResources();
	void releaseResources();
	void CalculateLayout();
	void OnResize();
	void OnPaint();

	void drawLine(D2D1_POINT_2F endPoint, ID2D1Brush* brush, float strokeBold);

public:
#define IDT_TIMER1 1

	MainWindow():
		BaseWND(), mp_renderTarget(nullptr), 
		mp_brush(nullptr), mp_factory(nullptr),
		mp_write_factory(nullptr), mp_text_format(nullptr), 
		mp_text_layout(nullptr){
		this->_logger = new logger(L"MainWindowsLog");
	}
	~MainWindow() {
		BaseWND::~BaseWND();
		::KillTimer(this->m_hwnd, IDT_TIMER1);

		if (this->_logger != nullptr)
			delete this->_logger;
	}

	LPCWSTR className() {
		return L"MainWindow";
	}

	bool Create(
		LPCWSTR WindowName,
		DWORD dwStyle = WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT,
		HWND hParent = NULL,
		HMENU hMenu = NULL
	) {
		if (!BaseWND::Create(WindowName, dwStyle, x, y, width, height, hParent, hMenu))
			return false;
		SetTimer(this->m_hwnd, IDT_TIMER1, 500, NULL);
		return true;
	}

	LRESULT handleProc(UINT msg, WPARAM wp, LPARAM lp);
};

#endif // _D2DTEST_H