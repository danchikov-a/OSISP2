#include <Windows.h>

RECT window = {0, 0, 400, 400};

int windowHeight;
int windowWidth;

int columnWidth;
int rowHeight;

const int amountOfColumns = 4;
const int amountOfRows = 3;

const CHAR *str = "Hello WORLDLWL QLEL WQEL WQL!";

void drawLine(HDC memDc, int startX, int startY, int finishX, int finishY);

void drawColumnLines(HDC memDc);

int fillCell(HDC memDc, int startX, int startY);

void fillTable(HDC memDc);

void drawTable(HWND hwnd) {
    GetClientRect(hwnd, &window);
    windowHeight = window.bottom;
    windowWidth = window.right;

    rowHeight = windowHeight / amountOfRows;
    columnWidth = windowWidth / amountOfColumns;

    PAINTSTRUCT ps;
    HDC hdc;
    HDC memDc;
    HBITMAP memBmp;
    HBRUSH backgroundBrush;

    COLORREF black = RGB(0, 0, 0);
    COLORREF white = RGB(255, 255, 255);

    GetClientRect(hwnd, &window);
    windowHeight = window.bottom;
    windowWidth = window.right;

    hdc = BeginPaint(hwnd, &ps);
    memDc = CreateCompatibleDC(hdc);
    memBmp = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);

    auto oldBmp = (HBITMAP) SelectObject(memDc, memBmp);
    backgroundBrush = CreateSolidBrush(white);
    FillRect(memDc, &window, backgroundBrush);
    DeleteObject(backgroundBrush);

    SetTextColor(memDc, black);
    SetBkMode(memDc, TRANSPARENT);

    drawColumnLines(memDc);
    fillTable(memDc);

    BitBlt(hdc, 0, 0, windowWidth, windowHeight, memDc, 0, 0, SRCCOPY);
    SelectObject(hdc, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDc);

    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }

        case WM_SIZE: {
            InvalidateRect(hwnd, nullptr, TRUE);
            break;
        }

        case WM_PAINT: {
            drawTable(hwnd);
            break;
        }

        default: {
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int nCmdShow) {
    WNDCLASSEX windowClass;
    HWND winHandle;
    MSG message;

    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_DBLCLKS;
    windowClass.lpfnWndProc = WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = "MainWindow";
    windowClass.hIconSm = windowClass.hIcon;

    RegisterClassEx(&windowClass);

    winHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                               "MainWindow",
                               "Main Window",
                               WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
                               100,
                               100,
                               window.right,
                               window.bottom,
                               NULL,
                               NULL,
                               hInstance,
                               0
    );

    ShowWindow(winHandle, nCmdShow);
    UpdateWindow(winHandle);

    while (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return (int) message.wParam;
}

void drawLine(HDC memDc, int startX, int startY, int finishX, int finishY) {
    MoveToEx(memDc, startX, startY, NULL);
    LineTo(memDc, finishX, finishY);
}

void drawColumnLines(HDC memDc) {
    for (int j = 0, x = 0; j <= amountOfColumns; j++, x += (int) columnWidth) {
        drawLine(memDc, x, 0, x, (int) windowHeight);
    }
}

int fillCell(HDC memDc, int startX, int startY) {
    RECT textRect;
    //TODO
    SetRect(&textRect, startX, startY + rowHeight / 2.5, startX + columnWidth, startY + rowHeight);
    //TODO
    int textHeight = DrawText(memDc, (LPCSTR) str, strlen(str), &textRect,
                              DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK | DT_NOCLIP);
    return textHeight;
}

void fillTable(HDC memDc) {
    for (int i = 0, columnCounter = 0, startX = 0, startY = 0;
         i < amountOfColumns * amountOfRows;
         i++, columnCounter++, startX += columnWidth) {

        if (columnCounter == amountOfColumns) {
            startY += rowHeight;
            drawLine(memDc, 0, startY, windowWidth, startY);
            columnCounter = 0;
            startX = 0;
        }

        int textHeight = fillCell(memDc, startX, startY);

        if (textHeight > rowHeight) {
            rowHeight = textHeight;
        }
    }
}