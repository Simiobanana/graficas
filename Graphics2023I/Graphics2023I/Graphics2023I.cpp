// Graphics2023I.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "Graphics2023I.h"
#include "CDXManager.h"
#include <math.h>
#include "Matrix4D.h"

#define MAX_LOADSTRING 100

// Variables globales:
HINSTANCE hInst;                            // instancia actual
WCHAR szTitle[MAX_LOADSTRING];              // Texto de la barra de título
WCHAR szWindowClass[MAX_LOADSTRING];        // nombre de clase de la ventana principal
CDXManager g_DXManager;
int g_mx;
int g_my;
CCanvas* g_pImage;
CCanvas* g_pImage2;
CCanvas* g_pTexture00;
//DX resource manager

// Declaraciones de funciones adelantadas incluidas en este módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                Render(void);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    VECTOR4D A = { 1,2,3,1 }, B = { 4,5,6,1 }, C;
    C = A + B;
    C = B - A;
    C = A * B;

    // TODO: Colocar código aquí.

    CCanvas* pCanvas = CCanvas::Create(640, 480);
    pCanvas->Clear({ 0,0,0,0 });
    pCanvas->Circle(320, 240, 200, { 255,255,255,0 });
    pCanvas->CreateFileFromCanvas("..\\data\\test.bmp");
    CCanvas::Destroy(pCanvas);
    //Carga de una 

   /* g_pImage = CCanvas::CreateCanvasFromFile("..\\data\\hola.bmp");
    if (!g_pImage)
        MessageBoxA(NULL, "No se pudo recargar el recurso ", "Error", MB_ICONERROR);
    g_pImage2 = CCanvas::CreateCanvasFromFile("..\\data\\imagen8bpp.bmp");
    if(!g_pImage2)
       MessageBoxA(NULL, "No se pudo recargar el recurso ", "Error", MB_ICONERROR);*/
    g_pTexture00 = CCanvas::CreateCanvasFromFile("..\\data\\texture00.bmp");
    if (!g_pTexture00)
        MessageBoxA(NULL, "No se pudo recargar el recurso ", "Error", MB_ICONERROR);
    // Inicializar cadenas globales
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GRAPHICS2023I, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Realizar la inicialización de la aplicación:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHICS2023I));

    MSG msg;

    // Bucle principal de mensajes:
    while (1)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            if (msg.message == WM_QUIT)
                break;
        }
        if (g_DXManager.Ready())
            Render();
    }


    return (int)msg.wParam;
}



//
//  FUNCIÓN: MyRegisterClass()
//
//  PROPÓSITO: Registra la clase de ventana.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHICS2023I));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = 0; // (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = 0; //MAKEINTRESOURCEW(IDC_GRAPHICS2023I);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCIÓN: InitInstance(HINSTANCE, int)
//
//   PROPÓSITO: Guarda el identificador de instancia y crea la ventana principal
//
//   COMENTARIOS:
//
//        En esta función, se guarda el identificador de instancia en una variable común y
//        se crea y muestra la ventana principal del programa.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Almacenar identificador de instancia en una variable global

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    if (!g_DXManager.Initialize(hWnd))
    {
        MessageBox(NULL, L"Error al intentar inicializar Direct3D", L"Error fatal", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCIÓN: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PROPÓSITO: Procesa mensajes de la ventana principal.
//
//  WM_COMMAND  - procesar el menú de aplicaciones
//  WM_PAINT    - Pintar la ventana principal
//  WM_DESTROY  - publicar un mensaje de salida y volver
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_MOUSEMOVE:
        g_mx = LOWORD(lParam);
        g_my = HIWORD(lParam);
        break;

    case WM_SIZE:
    {
        RECT rcClient;
        GetClientRect(hWnd, &rcClient);
        g_DXManager.Resize(rcClient.right, rcClient.bottom);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Analizar las selecciones de menú:
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
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Agregar cualquier código de dibujo que use hDC aquí...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        g_DXManager.Uninitialize();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Controlador de mensajes del cuadro Acerca de.
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

CCanvas::PIXEL ConstantShader(int x, int y)
{
    return { 255,0,0,255 };
}

CCanvas::PIXEL Noise(int x, int y)
{
    unsigned char c = (unsigned char)rand();
    return { c,c,c,c };
}

CCanvas::PIXEL Chess(int x, int y)
{
    int i = x / 64;
    int j = y / 64;

    if ((i % 2) ^ (j % 2))
        return { 0,0,0,255 };
    else
        return { 255,255,255, 0 };
}

void  Render(void)
{
    static float t = 0.0f;
    CCanvas* pCanvas = CCanvas::Create(256, 256);
    pCanvas = g_DXManager.GetCanvas();
    pCanvas->Clear({ 0,0,0,0 });

    //Definir un triangulo
    CCanvas::VERTEX Triangule[6] =
    {
        {{0,-1, 0, 1}, {0,0,0,1}},
        {{1, 1, 0, 1}, {3,0,0,1}},
        {{-1,1, 0, 1}, {0,2,0,1}},
        {{-1,1, 0, 1}, {0,2,0,1}},
        {{1, 1, 0, 1}, {3,0,0,1}},
        {{0, 3, 0, 1}, {3,2,0,1}}
    };
    CCanvas::VERTEX Transformed[6];
    MATRIX4D M = RotationZ(0.5 * t) * Scaling(200, 200, 1) * Translation(g_mx, g_my, 0);
    MATRIX4D T = Translation(-0.3 * t, 0, 0);
    for (int i = 0; i < 6; i++)
    {
        Transformed[i].Position = Triangule[i].Position * M;
        Transformed[i].TexCoord = Triangule[i].TexCoord * T;
    }
    g_pTexture00->SetAddressMode(CCanvas::ADDRESS_MODE_WRAP);
    pCanvas->InverseTextureMapping(Transformed, g_pTexture00,
        &CCanvas::BilinearSampler);
    pCanvas->InverseTextureMapping(Transformed + 3, g_pTexture00,
        &CCanvas::BilinearSampler);

    g_DXManager.Show(pCanvas);
    t += 1.0f / 60;
    CCanvas::Destroy(pCanvas);

}