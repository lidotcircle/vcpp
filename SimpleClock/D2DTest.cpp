#include "stdafx.h"
#include <d2d1.h>
#include <cmath>
#include <dwrite.h>

#include <d2d1_1.h>
#include <d3d11.h>

#include <stdexcept>

#include <wincodec.h>

#include "resource.h"

#include <wrl.h>

#include "D2dTest.h"

#include <mssm/presentation/presentation.hpp>

#ifndef __DEBUG__
#define __DEBUG__
#endif // __DEBUG__

#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "d3d11.lib")

bool D3TEST() {
	D3D_FEATURE_LEVEL feature_levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	D3D_FEATURE_LEVEL x_feature;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> device_context;
	DX::ThrowIfFailed(
		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
			0,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			feature_levels,
			7,
			D3D11_SDK_VERSION,
			&device,
			&x_feature,
			&device_context
			)
	);

	Microsoft::WRL::ComPtr<IDXGIDevice> xdxgi_device;
	DX::ThrowIfFailed(
		device.As(&xdxgi_device)
	);

	Microsoft::WRL::ComPtr<ID2D1Device> d2d_device;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d_device_context;

	Microsoft::WRL::ComPtr<ID2D1Factory1> p_d2d1_factory;
	DX::ThrowIfFailed(
		D2D1CreateFactory(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &p_d2d1_factory)
	);

	DX::ThrowIfFailed(
		p_d2d1_factory->CreateDevice(xdxgi_device.Get(), &d2d_device)
	);
	DX::ThrowIfFailed(
		d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS::D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&d2d_device_context)
	);
	return true;
}

bool MainWindow::initializeResources() 
{

	if (this->m_hwnd == nullptr) {
		return false;
	}
	if (this->mp_brush != nullptr) {
		return true;
	}

	D3TEST();

	RECT cilentRectangle;
	::GetClientRect(this->m_hwnd, &cilentRectangle);

	D2D1_SIZE_U size = D2D1::SizeU(cilentRectangle.right, cilentRectangle.bottom);
	const D2D1_COLOR_F color = D2D1::ColorF(0.0f, 1.0f, 0.0f);

	HRESULT hr;

	hr = ::D2D1CreateFactory(D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&this->mp_factory);
	if (!SUCCEEDED(hr)) goto clean_resources;

	hr = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE::DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), 
		reinterpret_cast<IUnknown**>(&this->mp_write_factory));
	if (!SUCCEEDED(hr)) goto clean_resources;

	hr = this->mp_write_factory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_BLACK, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"zh-CN", &this->mp_text_format);
	this->mp_text_format->SetReadingDirection(DWRITE_READING_DIRECTION::DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);
	this->mp_text_format->SetFlowDirection(DWRITE_FLOW_DIRECTION::DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM);

	if (!SUCCEEDED(hr)) goto clean_resources;
	hr = this->mp_write_factory->CreateTextLayout(L"DrawTextLayout", ::lstrlenW(L"DrawTextLayout"), 
		this->mp_text_format, 1000, 1000, &this->mp_text_layout);
	if (!SUCCEEDED(hr)) goto clean_resources;

	hr = this->mp_factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(this->m_hwnd, size),
		&this->mp_renderTarget
	);
	if (!SUCCEEDED(hr)) goto clean_resources;

	hr = this->mp_renderTarget->CreateSolidColorBrush(color, &this->mp_brush);
	if (SUCCEEDED(hr)) {
		this->CalculateLayout();
	}
	else goto clean_resources;

	return true;

clean_resources:
	this->releaseResources();
	return false;
}

void MainWindow::releaseResources()
{
	SafeRelease(&this->mp_brush);
	SafeRelease(&this->mp_renderTarget);
	SafeRelease(&this->mp_factory);
	SafeRelease(&this->mp_write_factory);
	SafeRelease(&this->mp_text_layout);
	SafeRelease(&this->mp_text_format);
}

void MainWindow::CalculateLayout()
{
	if (this->mp_brush != nullptr) {
		RECT clientRect;
		::GetClientRect(this->m_hwnd, &clientRect);
		const auto x = clientRect.right / 2;
		const auto y = clientRect.bottom / 2;
		const auto radius = min(x, y);
		this->m_ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
	}
}

void MainWindow::drawLine(D2D1_POINT_2F endPoint, ID2D1Brush* brush, float strokeWidth)
{
	this->mp_renderTarget->DrawLine(this->m_ellipse.point, endPoint, brush, strokeWidth);
	return;
}

bool drawXYZ(ID2D1Factory* factory, ID2D1RenderTarget* target)
{
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> l_brush_sd;
	DX::ThrowIfFailed(target->CreateSolidColorBrush(D2D1::ColorF(0.1, 0.1, 0.8, 0.5), &l_brush_sd));

	D2D1_POINT_2F points[] = {
		{0.0f, 0.0f}, {30, 30}, {30, 0}, {0, 30}
	};

	Microsoft::WRL::ComPtr<ID2D1PathGeometry> l_path;
	Microsoft::WRL::ComPtr<ID2D1GeometrySink> l_sink;
	DX::ThrowIfFailed(factory->CreatePathGeometry(&l_path));
	DX::ThrowIfFailed(l_path->Open(&l_sink));
	l_sink->BeginFigure(D2D1::Point2F(5, 5), D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
	l_sink->AddLines(points, ARRAYSIZE(points));
	l_sink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED);
	l_sink->Close();

	target->FillGeometry(l_path.Get(), l_brush_sd.Get());
	return true;
}

void sm_test(ID2D1Factory* factory, ID2D1RenderTarget* target)
{
	SMNode<double> n1 = SMNode<double>(20, 300.0);

	SMNode<double> n2 = SMNode<double>(200, 100.0);
	SMBeam<double> b1 = SMBeam<double>(n1, n2);
	SMSupport<double> s1 = SMSupport<double>::CreateDirSupport(n1);
	SMSupport<double> s2 = SMSupport<double>::CreateRollSupport(n2);

	s1.Angle() = -180;

	b1.draw_with(factory, target);
	s1.draw_with(factory, target);
	s2.draw_with(factory, target);
}

void MainWindow::OnPaint() 
{
	if (this->mp_brush == nullptr)
		return;

	PAINTSTRUCT psc;
	D2D1_POINT_2F endpoint = D2D1::Point2(
		1.0 * this->m_ellipse.point.x, 
		(1.0 * this->m_ellipse.point.y - 0.8 * this->m_ellipse.radiusY));

	SYSTEMTIME time;
	::GetLocalTime(&time); // get location time
	float angle_Hour   = time.wHour * 360 / 12; // angle of hour hand
	float angle_Minute = time.wMinute * 360 / 60; // angle of minute hand
	float angle_Second = time.wSecond * 360 / 60; // angle of second hand

	HRESULT hr;
	ID2D1SolidColorBrush* blackBrush;
	this->mp_renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0, 0.0, 0.0), &blackBrush);

	ID2D1SolidColorBrush* brushA;
	this->mp_renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.7, 0.7, 0.2), &brushA);
	ID2D1SolidColorBrush* brushB;
	this->mp_renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.7, 0.2, 0.5), &brushB);

	ID2D1RoundedRectangleGeometry* roundedRectA;
	this->mp_factory->CreateRoundedRectangleGeometry(
		D2D1::RoundedRect(D2D1::Rect(50.0, 50.0, 150.0, 150.0), 20, 20), &roundedRectA
	);

	HDC hdc = ::BeginPaint(this->m_hwnd, &psc);

	this->mp_renderTarget->BeginDraw();
	this->mp_renderTarget->Clear(D2D1::ColorF(1.0f, 1.0f, 0.8f));

	sm_test(this->mp_factory, this->mp_renderTarget);
	drawXYZ(this->mp_factory, this->mp_renderTarget);

	Microsoft::WRL::ComPtr<ID2D1BitmapBrush> l_brush = nullptr;

	if(FAILED(DUTILS::DrawBitmapBrush(this->mp_factory, this->mp_renderTarget, drawXYZ, 30, 30, &l_brush))) throw std::logic_error("Error!");
	this->mp_renderTarget->FillEllipse(this->m_ellipse, l_brush.Get());

	this->mp_renderTarget->DrawGeometry(roundedRectA, l_brush.Get(), 2.0f);
	this->mp_renderTarget->DrawTextW(L"Hello world!", 12, this->mp_text_format, D2D1::RectF(50.0, 50.0, 150.0, 150.0), brushA);
	this->mp_renderTarget->DrawTextLayout(D2D1::Point2F(500.0, 500.0), this->mp_text_layout, brushB);

	Microsoft::WRL::ComPtr<ID2D1Bitmap> background_pic;
	DX::ThrowIfFailed(DUTILS::LoadBitmapFromResource(this->mp_renderTarget, IDR_WATERIMAGE, L"RT_RCDATA", &background_pic));
	this->mp_renderTarget->DrawBitmap(background_pic.Get(), D2D1::RectF(30, 30, 530, 530), 0.5);

	// Hour
	D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Rotation(angle_Hour, this->m_ellipse.point);
	transform = transform * D2D1::Matrix3x2F::Scale(D2D1::SizeF(1.0, 0.6), this->m_ellipse.point);
	this->mp_renderTarget->SetTransform(transform);
	this->drawLine(endpoint, blackBrush, 5.0);
	// Minute
	transform = D2D1::Matrix3x2F::Rotation(angle_Minute, this->m_ellipse.point);
	transform = transform * D2D1::Matrix3x2F::Scale(D2D1::SizeF(1.0, 1.0), this->m_ellipse.point);
	this->mp_renderTarget->SetTransform(transform);
	this->drawLine(endpoint, blackBrush, 2.1);
	// Second
	transform = D2D1::Matrix3x2F::Rotation(angle_Second, this->m_ellipse.point);
	transform = transform * D2D1::Matrix3x2F::Scale(D2D1::SizeF(1.0, 1.0), this->m_ellipse.point);
	this->mp_renderTarget->SetTransform(transform);
	this->drawLine(endpoint, blackBrush, .88);

	this->mp_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	this->mp_renderTarget->EndDraw();

	// Release ...
	blackBrush->Release();
	brushA->Release();
	brushB->Release();
	roundedRectA->Release();
	::EndPaint(this->m_hwnd, &psc);
}

void MainWindow::OnResize()
{
	if (this->mp_brush == nullptr)
		return;
	RECT clientRec;
	GetClientRect(this->m_hwnd, &clientRec);
	D2D1_SIZE_U size = D2D1::SizeU(clientRec.right, clientRec.bottom);
	this->mp_renderTarget->Resize(size);
	this->CalculateLayout();
//	this->OnPaint();
	::InvalidateRect(this->m_hwnd, NULL, false);
	return;
}

#define LOGGER_EVENT(ev) \
	this->_logger->begin_log(); \
	this->_logger->ostream() << ev << std::endl;

LRESULT MainWindow::handleProc(UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_TIMER: {
		::InvalidateRect(this->m_hwnd, NULL, false);
		LOGGER_EVENT("WM_TIMER");
	}
	case WM_CREATE: {
		LOGGER_EVENT("WM_CREATE");
		this->initializeResources();
	} break;
	case WM_CLOSE: {
		LOGGER_EVENT("WM_CLOSE");
		if (MessageBox(NULL, L"Whether close this window ?", L"Message", MB_OKCANCEL) == IDOK)
			::DestroyWindow(this->m_hwnd);
	} return 0;
	case WM_DESTROY: {
		LOGGER_EVENT("WM_DESTROY");
		this->releaseResources();
		PostQuitMessage(0);
	} return 0;
	case WM_PAINT: {
		LOGGER_EVENT("WM_PAINT");
		this->OnPaint();
	} break;
	case WM_SIZE: {
		LOGGER_EVENT("WM_SIZE");
		this->OnResize();
	} break;
	}
	return ::DefWindowProc(this->m_hwnd, msg, wp, lp);
}

