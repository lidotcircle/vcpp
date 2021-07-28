#include "desktopapp.h"
#include "stdafx.h"

#include<Windows.h>
#include<ShObjIdl.h>

#include"BaseForm.h"

#ifndef UNICODE
#define  UNICODE
#endif // UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif // _UNICODE

#define ErrorMessageSize 500

HINSTANCE hIns;

LRESULT CALLBACK WndProc(
	_In_ HWND,
	_In_ UINT,
	_In_ WPARAM,
	_In_ LPARAM
);

extern const GUID CLSID_SHAPE;

int XWinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE prevHInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
);

int XWinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE prevHInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow) 
{
	static TCHAR windowsClassName[] = _T("Windows Desktop App");
	HRESULT hr;
	hr = ::CoInitialize(NULL);
	if (!SUCCEEDED(hr)) {
		::MessageBox(NULL, L"Initialize COM Apartment fail...", L"COM FAULT", MB_OK);
		return 1;
	}

	IFileOpenDialog* pFileDialog;
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog,
		reinterpret_cast<void**>(&pFileDialog));
	if (SUCCEEDED(hr)) {
		// success
		hr = pFileDialog->Show(NULL);
		if (!SUCCEEDED(hr)) {
			fileDialogFail:
			::MessageBox(NULL, L"Fault in file dialog", L"Message", MB_OK);
			pFileDialog->Release();
			::CoUninitialize();
			return 1;
		}
		IShellItem* s_item;
		hr = pFileDialog->GetResult(&s_item);
		if (!SUCCEEDED(hr)) goto fileDialogFail;
		LPWSTR fileDisplayName;
		hr = s_item->GetDisplayName(SIGDN_FILESYSPATH, &fileDisplayName);
		if (!SUCCEEDED(hr)) { s_item->Release(); goto fileDialogFail; }
		::MessageBox(NULL, fileDisplayName, L"Selected File", MB_OK);
		::CoTaskMemFree(fileDisplayName);
		pFileDialog->Release();
		s_item->Release();
	}
	else {
		::MessageBox(NULL, L"Fail to get FileDialog", L"Message", MB_OK);
		::CoUninitialize();
		return 1;
	}

	// initialize the WNDCLASSEX
	WNDCLASSEX wcx;
	wcx.cbSize        = sizeof(WNDCLASSEX);
	wcx.cbClsExtra    = 0;
	wcx.cbWndExtra    = 0;
	wcx.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME+1);
	wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcx.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
	wcx.hIconSm       = LoadIcon(hInstance, IDI_APPLICATION);
	wcx.hInstance     = hInstance;
	wcx.lpfnWndProc   = (WNDPROC)WndProc;
	wcx.lpszClassName = windowsClassName;
	wcx.lpszMenuName  = NULL;
	wcx.style         = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wcx)) {
		MessageBox(NULL, _T("Register the windows class failed."), _T("Failed"), NULL);
		return 1;
	}

	hIns = hInstance;

	// create a window
	static TCHAR windowsTitle[] = _T("Windows Desktop Test");
	HWND window = CreateWindow(windowsClassName, windowsTitle,
							   WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
							   CW_USEDEFAULT, 1000, 1000, 
							   NULL, NULL, hInstance, NULL);

	if (!window) {
		DWORD errorCode = GetLastError();
		wchar_t errorBuf[ErrorMessageSize];
		wsprintf(errorBuf, _T("%d"), errorCode);
		::lstrcat(errorBuf, _T("windows create failed."));
		MessageBox(NULL, errorBuf, _T("Failed"), NULL);
		return 1;
	}

	// show window
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	MSG __msg;
	while (GetMessage(&__msg, NULL, 0, 0)) {
		TranslateMessage(&__msg);
		DispatchMessage(&__msg);
	}
	return (int)__msg.wParam;
}

LRESULT CALLBACK WndProc(
	_In_ HWND   hwnd,
	_In_ UINT   msg,
	_In_ WPARAM wpara,
	_In_ LPARAM lpara
)
{
	switch (msg) {
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC _hdc = BeginPaint(hwnd, &ps);
		FillRect(_hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
	} return 0;
	case WM_CLOSE:
	{
		if (MessageBox(NULL, L"ÊÇ·ñ¹Ø±Õ´°¿Ú?", L"Message", MB_OKCANCEL) == IDOK) {
			DestroyWindow(hwnd);
			::CoUninitialize();
		}
		return 0;
	}
	}
	return DefWindowProc(hwnd, msg, wpara, lpara);
}