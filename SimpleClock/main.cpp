#define __DEBUG__

#include "desktopapp.h"
#include "stdafx.h"

#include<Windows.h>
#include<ShObjIdl.h>

#include"BaseForm.h"
#include"BaseWindow.h"

#include "BaseWindow.h"

#include "D2dTest.h"

#ifndef UNICODE
#define  UNICODE
#endif // UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif // _UNICODE

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE prevHInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
) 
{
//	XWinMain(hInstance, prevHInstance, lpCmdLine, nCmdShow);
	/*
	BaseForm* form = new BaseForm(hInstance, L"BaseForm", L"Test Window");
	form->ShowWindow();
	delete form;
	*/

	::CoInitialize(NULL);

	MainWindow mWindow;
	if (!mWindow.Create(L"Test Window"))
		return 1;

	HWND hwnd = mWindow.window();
	::ShowWindow(hwnd, nCmdShow);
	::UpdateWindow(hwnd);

	// message queue handle
	MSG msg;
	while (GetMessage(&msg, hwnd, 0, 0)) {
		TranslateMessage(&msg);
		if (msg.message == WM_NULL)
			break;
		DispatchMessage(&msg);
	}

	return 0;
}