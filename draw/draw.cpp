// draw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "draw.h"
#include <cmath>
#include <vector>

#define MAX_LOADSTRING 100
#define TMR_1 1

RECT drawArea1 = { 0, 90, 1200, 800 };
RECT drawArea2 = { 300, 0, 800, 89 };

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

INT value;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


double alfa = 0.3;
double beta = -0.1;
double alfa_prev;
double beta_prev;
double l1 = 175;
double l2 = 125;
int W = 50;
int H = 400;
double speed = 0.01;
int m = 20;
int size = 40;
int height = 0;
int b_k;
Point p_k;
bool hold = false;
int b_h;

class block
{
public:
	block() {};
	~block() {};
	void set_p(Point p) { K = p; }
	Point take_p() { return K; }
	void set_m(int m) { mass = m; }
	int take_m() { return mass; }
private:
	Point K;
	int mass;
};

std::vector <block> blocks;
block new_b;

void math(int x, int y)
{
	double l = sqrt(x*x + y * y);
	double cosy = (l1*l1 + l2 * l2 - l * l) / 2 * l1*l2;
	double siny = sqrt(1 - cosy * cosy);
	double alfa1 = asin((l2*siny) / l);
	double alfa2 = asin(y / l);
	alfa = alfa1 + alfa2;
	beta = asin((y - l1 * sin(alfa)) / l2);
}

void gravity(int k, HWND hWnd)
{
	height = 0;

	Point p1 = blocks[k].take_p();

	for (int i = 0; i < blocks.size(); i++)
	{
		if (i == k)
			continue;

		Point p2 = blocks[i].take_p();

		int d = abs(p1.X - p2.X);

		if (d<size && p2.Y>height && p2.Y<p1.Y)
			height = p2.Y;
	}
	height += size;
	b_k = k;
	SetTimer(hWnd, TMR_1, 1, 0);
}

void draw_blocks(HDC hdc, Pen *pen)
{
	Graphics graphics(hdc);

	for (int i = 0; i < blocks.size(); i++)
	{
		Point j = blocks[i].take_p();
		graphics.DrawRectangle(pen, W + j.X, H - j.Y, size, size);

		LPCWSTR mass_l;
		char mass_c[20];
		itoa(blocks[i].take_m(), mass_c, 10);
		wchar_t mass_w[20];
		mbstowcs(mass_w, mass_c, strlen(mass_c) + 1);
		mass_l = mass_w;

		TextOut(hdc, W + j.X + 12, H - j.Y + 12, mass_l, 2);
	}
}

void MyOnPaint(HDC hdc)
{
	Graphics graphics(hdc);
	Pen GreenPen(Color(255, 0, 255, 0), 5);
	Pen BlackPen(Color(255, 0, 0, 0), 5);

	SolidBrush Brush(Color(255, 255, 0, 0));

	Point a(W, H);
	Point b(a.X + cos(alfa)*l1, a.Y - sin(alfa)*l1);
	Point c(b.X + cos(beta)*l2, b.Y - sin(beta)*l2);

	graphics.DrawLine(&BlackPen, 0, H, 1200, H);

	draw_blocks(hdc, &BlackPen);

	graphics.DrawLine(&GreenPen, a.X, a.Y, b.X, b.Y);
	graphics.DrawLine(&GreenPen, b.X, b.Y, c.X, c.Y);

	graphics.FillEllipse(&Brush, a.X - 7, a.Y - 7, 15, 15);
	graphics.FillEllipse(&Brush, b.X - 7, b.Y - 7, 15, 15);
	graphics.FillEllipse(&Brush, c.X - 7, c.Y - 7, 15, 15);
}

void create_block(int x, int y, HWND hWnd)
{
	Point p(x, y);
	blocks.push_back(new_b);
	blocks[blocks.size() - 1].set_m(m);
	blocks[blocks.size() - 1].set_p(p);

	gravity(blocks.size() - 1, hWnd);
}

void paint(HDC &hdc, HWND hWnd, PAINTSTRUCT &ps) {
	InvalidateRect(hWnd, &drawArea1, TRUE);
	hdc = BeginPaint(hWnd, &ps);

	MyOnPaint(hdc);

	EndPaint(hWnd, &ps);
}

void paint_2(HDC &hdc, HWND hWnd, PAINTSTRUCT &ps) {
	InvalidateRect(hWnd, &drawArea2, TRUE);
	hdc = BeginPaint(hWnd, &ps);

	LPCWSTR text = L"mass (+/-) :";
	TextOut(hdc, 500, 20, text, 13);

	char ch[20];
	itoa(m, ch, 10);
	wchar_t wch[20];
	mbstowcs(wch, ch, strlen(ch) + 1);
	text = wch;
	TextOut(hdc, 575, 20, text, 2);

	text = L"speed (PgUp/PgDown):";
	TextOut(hdc, 500, 40, text, 20);

	int sp = speed * 100;
	itoa(sp, ch, 10);
	mbstowcs(wch, ch, strlen(ch) + 1);
	text = wch;
	TextOut(hdc, 650, 40, text, 2);


	EndPaint(hWnd, &ps);
}


int OnCreate(HDC &hdc, HWND hWnd, PAINTSTRUCT &ps)
{
	LPCWSTR text = L"1";
	TextOut(hdc, 140, 20, text, 1);
	text = L"2";
	TextOut(hdc, 190, 20, text, 1);
	text = L"3";
	TextOut(hdc, 240, 20, text, 1);
	text = L"4";
	TextOut(hdc, 290, 20, text, 1);

	EndPaint(hWnd, &ps);

	return 0;
}

bool check_1()
{
	Point C(cos(alfa)*l1 + cos(beta)*l2, sin(alfa)*l1 + sin(beta)*l2);

	for (int i = 0; i < blocks.size(); i++)
	{
		Point p = blocks[i].take_p();
		int d1 = C.X - p.X;
		int d2 = p.Y - C.Y;
		if (d1 < size && d1>0 && d2 < size && d2>0)
		{
			b_h = i;
			return true;
		}
	}
	return false;
}

bool check_2()
{
	Point p1 = blocks[b_h].take_p();

	for (int i = 0; i < blocks.size(); i++)
	{
		if (i == b_h)
			continue;

		Point p2 = blocks[i].take_p();
		int d1 = abs(p1.X - p2.X);
		int d2 = abs(p1.Y - p2.Y);

		if (d1 < size && d2 < size)
			return false;
	}

	return true;
}

void move()
{
	Point C(cos(alfa)*l1 + cos(beta)*l2, sin(alfa)*l1 + sin(beta)*l2);
	Point C_prev(cos(alfa_prev)*l1 + cos(beta_prev)*l2, sin(alfa_prev)*l1 + sin(beta_prev)*l2);

	int x = C.X - C_prev.X;
	int y = C.Y - C_prev.Y;

	Point p = blocks[b_h].take_p();
	p.X += x;
	p.Y += y;
	blocks[b_h].set_p(p);
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	value = 10;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAW));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAW));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DRAW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	//OnCreate(hWnd);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	//OnTimer(hWnd,wParam,lParam);

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_UP:
			alfa_prev = alfa;
			beta_prev = beta;
			alfa += speed;
			beta += speed;
			if (hold)
				move();
			paint(hdc, hWnd, ps);
			break;
		case VK_DOWN:
			alfa_prev = alfa;
			beta_prev = beta;
			alfa -= speed;
			beta -= speed;
			if (hold)
				move();
			paint(hdc, hWnd, ps);
			break;
		case VK_LEFT:
			alfa_prev = alfa;
			beta_prev = beta;
			beta += speed;
			if (hold)
				move();
			paint(hdc, hWnd, ps);
			break;
		case VK_RIGHT:
			alfa_prev = alfa;
			beta_prev = beta;
			beta -= speed;
			if (hold)
				move();
			paint(hdc, hWnd, ps);
			break;
		case VK_NUMPAD1:
			create_block(75, 308, hWnd);
			paint(hdc, hWnd, ps);
			break;
		case VK_NUMPAD2:
			create_block(125, 308, hWnd);
			paint(hdc, hWnd, ps);
			break;
		case VK_NUMPAD3:
			create_block(175, 308, hWnd);
			paint(hdc, hWnd, ps);
			break;
		case VK_NUMPAD4:
			create_block(225, 308, hWnd);
			paint(hdc, hWnd, ps);
			break;
		case VK_CONTROL:
			if (hold)
			{
				if (check_2())
				{
					hold = false;
					gravity(b_h, hWnd);
				}

			}
			else
			{
				if (check_1())
				{
					hold = true;
					if (b_h == b_k)
						KillTimer(hWnd, TMR_1);
				}

			}
			break;
		case VK_ADD:
			if (m < 99)
			{
				m++;
				paint_2(hdc, hWnd, ps);
			}
			break;
		case VK_SUBTRACT:
			if (m >1)
			{
				m--;
				paint_2(hdc, hWnd, ps);
			}
			break;
		case VK_PRIOR:
			if (speed < 0.99)
			{
				speed += 0.01;
				paint_2(hdc, hWnd, ps);
			}
			break;
		case VK_NEXT:
			if (speed > 0.01)
			{
				speed -= 0.01;
				paint_2(hdc, hWnd, ps);
			}
			break;
		}
	}
	break;

	switch (wmId)
	{
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnCreate(hdc, hWnd, ps);
		paint_2(hdc, hWnd, ps);
		paint(hdc, hWnd, ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case TMR_1:
			p_k = blocks[b_k].take_p();
			p_k.Y -= 3;
			if (p_k.Y < height)
				p_k.Y = height;
			blocks[b_k].set_p(p_k);

			//
			//
			//if (p_k.X*p_k.X + p_k.Y*p_k.Y < (l1 + l2)*(l1 + l2))
			//{
			//	math(p_k.X, p_k.Y);
			//}
			//
			//

			InvalidateRect(hWnd, &drawArea1, TRUE);
			hdc = BeginPaint(hWnd, &ps);
			paint(hdc, hWnd, ps);
			EndPaint(hWnd, &ps);

			if (p_k.Y == height)
				KillTimer(hWnd, TMR_1);
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
