// FileView.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include <commdlg.h>
#include "FileView.h"
#include <vector>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FILEVIEW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FILEVIEW));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FILEVIEW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FILEVIEW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   

   if (!hWnd)
   {
      return FALSE;
   }
   SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOSIZE | SWP_NOMOVE);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

//std::vector<char> buff;


//global buffer for read file
int const buff_size = 1024*1024*1024;
DWORD file_size = 0;
BYTE buff[buff_size] = {0};
SCROLLINFO si;

void openMyFile(HWND hWnd, LPWSTR fileName) {
    HANDLE handle;
    handle = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (handle == INVALID_HANDLE_VALUE)
    {
        OutputDebugStringW(L"Handle invalide.\n");
    }
    DWORD nRead = 0;
    if (ReadFile(handle, buff, 1024*1024, &nRead, NULL) == false)
    {
        OutputDebugStringW(L"cant read.\n");
    };
    file_size = nRead;
    if (file_size > 1024 * 512)
    {
        file_size = 1024 * 512;
    }
    DWORD style = GetWindowLong(hWnd, GWL_STYLE);

    //SetWindowLong(hWnd, GWL_STYLE, style | WS_VSCROLL);

    SetWindowLong(hWnd, GWL_STYLE, style & ~WS_VSCROLL);
    RECT Rect;
    GetClientRect(hWnd, &Rect);
    InvalidateRect(hWnd, &Rect, TRUE);
    UpdateWindow(hWnd);
    CloseHandle(handle);
}

void paintBitMap(HWND hWnd) 
{
    PAINTSTRUCT ps;
    RECT Rect;
    GetClientRect(hWnd, &Rect);
    InvalidateRect(hWnd, &Rect, false);
    HDC hdc = BeginPaint(hWnd, &ps);

    int height, width;

    height = Rect.bottom - Rect.top;
    width = Rect.right - Rect.left;

    int hh = file_size / width;

    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    GetScrollInfo(hWnd, SB_VERT, &si);

    int y = 0; 
    y = si.nPos;
    HDC hDrawDC = ::CreateCompatibleDC(hdc);

    //Create a compatible Bitmap
    
    HBITMAP hDrawBitmap = CreateCompatibleBitmap(hdc, width, hh);
    ::ReleaseDC(hWnd, hDrawDC);

    ::SelectObject(hDrawDC, hDrawBitmap);

    //paintBitMapPage(&hDrawDC, )

    for (int i = 0; i < hh; i++)
    {
        for (int j = 0; j < width; j++)
        {
                BYTE a = buff[y * width + width * i + j];
                SetPixel(hDrawDC, j, i, RGB(a, a, a));
        }
    }
 
    ::BitBlt(hdc, 0, 0, width, hh, hDrawDC, 0, 0, SRCCOPY);

    //Load bitmap to memory DC

    ::DeleteObject(hDrawBitmap);

    ::DeleteDC(hDrawDC);

    // Fill your cache with the white color
    PatBlt(hDrawDC, 0, 0, width, hh, WHITENESS);
    EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_OPEN:
            {
                OPENFILENAME ofn;       // common dialog box structure
                TCHAR szFile[260] = { 0 };       // if using TCHAR macros

                // Initialize OPENFILENAME
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hWnd;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = _T("All\0*.*\0");
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn) == TRUE)
                {
                    MessageBox(hWnd, ofn.lpstrFile, L"Info", MB_OK);
                    openMyFile(hWnd, ofn.lpstrFile);
                }
                break;
            }
            case IDM_ABOUT:
            {
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            }
            case IDM_EXIT:
            {
                DestroyWindow(hWnd);
                break;
            }
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
    {
        OutputDebugStringW(L"This will go to the output.\n");
        paintBitMap(hWnd);
    }
        break;
    case WM_SIZE:
    {
        RECT Rect;
        GetClientRect(hWnd, &Rect);
        int height, width;

        height = Rect.bottom - Rect.top;
        width = Rect.right - Rect.left;

        int hh = file_size / width;

        DWORD style = GetWindowLong(hWnd, GWL_STYLE);


        if (hh > height)
        {
            SetWindowLong(hWnd, GWL_STYLE, style | WS_VSCROLL);
        }
        else
        {
            SetWindowLong(hWnd, GWL_STYLE, style & ~WS_VSCROLL);
        }

        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        si.nMin = 0;
        si.nMax = hh;
        si.nPage = height;
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
    }
    break;
    case WM_VSCROLL:
    {
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &si);

        int y = si.nPos;
        switch (LOWORD(wParam))
        {
        case SB_LINELEFT:
            OutputDebugStringW(L"SB_LINELEFT\n");
            if ( si.nPos - 1 >= 0 )
            {
                si.nPos = si.nPos - 1;
            }
            break;
        case SB_LINERIGHT:
            OutputDebugStringW(L"SB_LINERIGHT\n");
            if (si.nPos + 1 <= si.nMax)
            {
                si.nPos = si.nPos + 1;
            }
            paintBitMap(hWnd);
            break;
        case SB_PAGELEFT:
            OutputDebugStringW(L"SB_PAGELEFT\n");
            if (si.nPos - si.nPage <= 0)
            {
                si.nPos = si.nMin;
            } 
            else 
            {
                si.nPos = si.nPos - si.nPage;
            }
            break;
        case SB_PAGERIGHT:
            OutputDebugStringW(L"SB_PAGERIGHT\n");
            if (si.nPos + si.nPage >= si.nMax)
            {
                si.nPos = si.nMin;
            }
            else 
            {
                si.nPos = si.nPos + si.nPage;
            }
            break;
        case SB_THUMBTRACK:
            OutputDebugStringW(L"SB_THUMBTRACK\n"); 
            si.nPos = si.nTrackPos;
            break;
        default:
            break;
        }
        si.fMask = SIF_POS;
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
        GetScrollInfo(hWnd, SB_VERT, &si);

        if (si.nPos != y)
        {
            paintBitMap(hWnd);
        }
        return 0;
     }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
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
