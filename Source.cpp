#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <Windows.h>
#include <CommCtrl.h>
#include <windowsx.h>
#include <tchar.h>

#include "Header.h"

#define MAX_THREADS 3

HANDLE hThreadArray[MAX_THREADS];
DWORD dwThreadIdArray[MAX_THREADS];
BOOL isThread[MAX_THREADS];

HINSTANCE hInstance;
DWORD counter = 0;

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI ThreadOneProc1(LPVOID);
DWORD WINAPI ThreadOneProc2(LPVOID);
DWORD WINAPI ThreadOneProc3(LPVOID);



CONTEXT ct;

int CALLBACK  wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
/*CALLBACK - #define для stdcall (соглашение для вызовов, вызываемый объект будет сам за собой очищать стек
 wWinMain - идентификатор
 HINSTANCE  hInstance - указатель на начало исполняемого модуля, мз сигнатуры. адрес памяти.
 HINSTANCE - не используется был акт. для 16 битных версий виндовс.
 PWSTR szCmdLine - указатель на строку UNICOD символов оканчивающихся нулём. По сути просто аргументы.
 int nCmdShow - параметр отвечающий за то как будет показываться окно (свёрнуто, развёрнуто, на весь экран и т.д.).
*/
{
    hInstance = hInstance;
    int i;
    MSG msg{};                             // Структура, которая содержит в себе информацию о соообщениях (между Windows и окном или между окнами).
    HWND hwnd{};                          // Дескриптор окна ( HANDLE указ. на объект ядра в котором храниться информация о нашем окне).
    WNDCLASSEX wc{ sizeof(WNDCLASSEX) }; // Эта структура отвечает за некие х-ки окна (в фигурных скобках размеры).Исп. агрегатная инициализация.
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WinProc;
    wc.lpszClassName = L"MyAppClass";
    wc.lpszMenuName = nullptr;
    wc.style = CS_VREDRAW | CS_HREDRAW;
    /*
    wc.cbClsExtra = 0; Отвечают за дополнительное выделение памяти в
    wc.cbWndExtra = 0; классе нашего окна (для записи какой либо информации в окно).

    hbrBackground - Это поле принимает дескриптор кисти которая окрашивает окно.
    GetStockObject - Возвращает GDI объект который мы можем привести к типу HBRUSH.

    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);     - HANDLE курсора.
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);   - HANDLE иконки.
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION); - HANDLE иконки (отображается сверху слева в заголовке окна).

    wc.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT
    - Эта процедура отвечает за обработку сообщений (MSG msg{};)
      Принимает 4 обязательных параметра и возвращает LRESULT (результат).
      HWND hWnd     - дескриптор окна к которому приходит сообщение.
      UINT uMsg     - код сообщения.
      WPARAM wParam - указатель в нём храниться необходимая для сообщения информация.
      LPARAM lParam - указатель в нём храниться необходимая для сообщения информация.

      wc.lpszClassName = L"MyAppClass";   - имя класса (любое).
      wc.lpszMenuName = nullptr;          - указатель на имя меню.
      wc.style = CS_VREDRAW | CS_HREDRAW; - стиль окна (2 флага по умолчанию)



    */

    if (!RegisterClassEx(&wc))
        return EXIT_FAILURE;

    if (hwnd = CreateWindow(wc.lpszClassName, L"Лабораторная работа 1(Потоки)", WS_OVERLAPPEDWINDOW, 50, 50, 800, 600, nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
        return EXIT_FAILURE;
    /*
     if (!RegisterClassEx(&wc)) - регистрация в системе класса нашего окна.
      return EXIT_FAILURE;

     if (hwnd = CreateWindow(wc.lpszClassName, L"Заголовок!", WS_OVERLAPPEDWINDOW, 0, 0, 600, 600, nullptr, nullptr, wc.hInstance, nullptr ); hwnd == INVALID_HANDLE_VALUE)
      return EXIT_FAILURE;
      - Создание нашего окна.
      wc.lpszClassName    - имя класса.
      L"Заголовок!"       - заголовок окна.
      WS_OVERLAPPEDWINDOW - стиль окна (стили посмотреть в msdn).
      0, 0,               - X и Y координаты окна (отсчитываются от левой верхней точки).
      600, 600,           - ширина, высота.


     */
    ShowWindow(hwnd, nCmdShow); // показ окна
    UpdateWindow(hwnd);         // перерисовка окна (передаётся HANDLE)

    while (GetMessage(&msg, nullptr, 0, 0)) // Цикл обработки сообщений
    {
        TranslateMessage(&msg); // функция расшифровывает системное сообщение
        DispatchMessage(&msg);  // функция  передаёт сообщение в оконную процедуру на обработку
    }

    return static_cast<int> (msg.wParam); // возвращаемое значение точки входа



    // hWnd - идентификатор окна.
    // nullptr - нулевой указатель.
}



LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    
    switch (uMsg)
    {
        case WM_CREATE:
        {
        #pragma region ProgressBar & Label
            //======================================================================================================================================================
            hLabelTread1 = CreateWindowEx(0, L"STATIC", (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 20, 10, 250, 16, hWnd, (HMENU)0, hInstance, NULL);
            SetWindowText(hLabelTread1, L"Поток 1. Состояние: Не создан");
            hProgress1 = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 20, 30, 400, 30, hWnd, (HMENU)10, hInstance, NULL);
            SendMessage(hProgress1, PBM_SETRANGE, 0, MAKELPARAM(0, 100));

            hLabelTread2 = CreateWindowEx(0, L"STATIC", (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 20, 70, 250, 16, hWnd, (HMENU)1, hInstance, NULL);
            SetWindowText(hLabelTread2, L"Поток 2. Состояние: Не создан");
            hProgress2 = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 20, 90, 400, 30, hWnd, (HMENU)11, hInstance, NULL);
            SendMessage(hProgress2, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
            SendMessage(hProgress2, PBM_SETSTEP, (WPARAM)10, 0);

            hLabelTread3 = CreateWindowEx(0, L"STATIC", (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 20, 130, 250, 16, hWnd, (HMENU)2, hInstance, NULL);
            SetWindowText(hLabelTread3, L"Поток 3. Состояние: Не создан");
            hProgress3 = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 20, 150, 400, 30, hWnd, (HMENU)12, hInstance, NULL);
            SendMessage(hProgress1, PBM_SETPOS, 99, 0);
            //======================================================================================================================================================
        #pragma endregion
            hRadio1 = CreateWindow(L"BUTTON", L"Поток 1",WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, 190, 80, 30, hWnd, (HMENU)30, hInstance, NULL);
            hRadio2 = CreateWindow(L"BUTTON", L"Поток 2", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, 230, 80, 30, hWnd, (HMENU)31, hInstance, NULL);
            hRadio3 = CreateWindow(L"BUTTON", L"Поток 3", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 20, 270, 80, 30, hWnd, (HMENU)32, hInstance, NULL);

            hLabelComboBox = CreateWindowEx(0, L"STATIC", (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 130, 190, 130, 16, hWnd, (HMENU)3, hInstance, NULL);
            SetWindowText(hLabelComboBox, L"Приоритет потока");
            hComboBox = CreateWindow(L"COMBOBOX", L"combobox", WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST, 130, 210, 250, 500, hWnd, (HMENU)40, hInstance, 0);
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Низкий");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Нормальный");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Высокий");

            hButtonPriority = CreateWindow(L"BUTTON", L"Изменить приоритет", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 130, 237, 251, 30, hWnd, (HMENU)50, hInstance, NULL);
            hButtonSuspend = CreateWindow(L"BUTTON", L"Приостановить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 130, 270, 125, 30, hWnd, (HMENU)51, hInstance, NULL);
            hButtonResume = CreateWindow(L"BUTTON", L"Продолжить", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 256, 270, 125, 30, hWnd, (HMENU)52, hInstance, NULL);

            hLine = CreateWindowEx(0, L"STATIC", (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 20, 310, 400, 16, hWnd, (HMENU)4, hInstance, NULL);
            SetWindowText(hLine, L"==================================================");
            //==========================================================================================================================================================
            hLabelWorkThread = CreateWindowEx(0, L"STATIC", (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 165, 340, 110, 16, hWnd, (HMENU)5, hInstance, NULL);
            SetWindowText(hLabelWorkThread, L"Работа потоков");

            hLabelWorkThread1 = CreateWindowEx(0, L"STATIC", (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 20, 370, 110, 16, hWnd, (HMENU)6, hInstance, NULL);
            SetWindowText(hLabelWorkThread1, L"ID Потока 1");
            hIDThread1 = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"erewqr", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 20, 400, 110, 20, hWnd, (HMENU)60, hInstance, NULL);
            
            hLabelWorkThread2 = CreateWindowEx(0, L"STATIC", (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 165, 370, 110, 16, hWnd, (HMENU)7, hInstance, NULL);
            SetWindowText(hLabelWorkThread2, L"ID Потока 2");
            hIDThread2 = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"ewrweqr", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 165, 400, 110, 20, hWnd, (HMENU)61, hInstance, NULL);
            
            hLabelWorkThread3 = CreateWindowEx(0, L"STATIC", (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 310, 370, 110, 16, hWnd, (HMENU)8, hInstance, NULL);
            SetWindowText(hLabelWorkThread3, L"ID Потока 3");
            hIDThread3 = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"yuoiuuyuty", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 310, 400, 110, 20, hWnd, (HMENU)62, hInstance, NULL);

            //===============================================================================================================================================================================================================================
            hLine1 = CreateWindowEx(0, L"STATIC", (LPTSTR)NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, 20, 440, 400, 16, hWnd, (HMENU)9, hInstance, NULL);
            SetWindowText(hLine1, L"==================================================");
            hButtonCreate = CreateWindow(L"BUTTON", L"Создать поток", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 470, 130, 30, hWnd, (HMENU)53, hInstance, NULL);
            hButtonStart = CreateWindow(L"BUTTON", L"Запустить поток", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 156, 470, 130, 30, hWnd, (HMENU)54, hInstance, NULL);
            hButtonTerminate = CreateWindow(L"BUTTON", L"Уничтожить поток", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 291, 470, 130, 30, hWnd, (HMENU)55, hInstance, NULL);
            hButtonContext = CreateWindow(L"BUTTON", L"Получить контекст", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 450, 470, 300, 30, hWnd, (HMENU)56, hInstance, NULL);

            hTextEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 450, 10, 300, 450, hWnd, (HMENU)63, hInstance, NULL);
            SendMessage(hTextEdit, EM_SETREADONLY, TRUE, 0);
            break;
        }
        
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            /*case 56:
            {
                GetThreadContext(hThreadArray[0], &ct);
                break;
            }
            break;*/
            case 53:
            {
                if (SendMessage(hRadio1, BM_GETCHECK, 0, 0) == BST_CHECKED)
                {
                    if (isThread[0]) {
                        MessageBox(NULL, L"Поток уже создан!", L"Предупреждение", MB_ICONWARNING | MB_OK);
                        break;
                    }
                    hThreadArray[0] = CreateThread(NULL, 0, ThreadOneProc1, NULL, 0, &dwThreadIdArray[0]);
                    WCHAR idThread[10];
                    swprintf_s(idThread, 10, L"%d", dwThreadIdArray[0]); // Конвертируем DWORD в WCHAR
                    SendMessage(hIDThread1, WM_SETTEXT, 0, (LPARAM)(LPCWSTR)idThread);
                    SendMessage(hLabelTread1, WM_SETTEXT, 0, (LPARAM)L"Поток 1. Состояние: Создан");

                    isThread[0] = TRUE;
                }
                else if (SendMessage(hRadio2, BM_GETCHECK, 0, 0) == BST_CHECKED)
                {
                    hThreadArray[1] = CreateThread(NULL, 0, ThreadOneProc2, NULL, 0, &dwThreadIdArray[1]);
                    WCHAR idThread[10];
                    swprintf_s(idThread, 10, L"%d", dwThreadIdArray[1]); // Конвертируем DWORD в WCHAR
                    SendMessage(hIDThread2, WM_SETTEXT, 0, (LPARAM)(LPCWSTR)idThread);
                    SendMessage(hLabelTread2, WM_SETTEXT, 0, (LPARAM)L"Поток 2. Состояние: Создан");
                }
                else if (SendMessage(hRadio3, BM_GETCHECK, 0, 0) == BST_CHECKED)
                {
                    hThreadArray[0] = CreateThread(NULL, 0, ThreadOneProc3, NULL, CREATE_SUSPENDED, &dwThreadIdArray[0]);
                    WCHAR idThread[10];
                    swprintf_s(idThread, 10, L"%d", dwThreadIdArray[0]); // Конвертируем DWORD в WCHAR
                    SendMessage(hIDThread1, WM_SETTEXT, 0, (LPARAM)(LPCWSTR)idThread);
                    SendMessage(hLabelTread1, WM_SETTEXT, 0, (LPARAM)L"Поток 3. Состояние: Создан");
                } 
                break;
            }
            break;
           
        }
        case WM_TIMER:
        {
            counter++;
            SendMessage(hProgress2, PBM_STEPIT, 0, 0);
            if (counter == 10) {
                KillTimer(hWnd, 1);
                counter = 0;
            }
            
        }
        break;
        case WM_DESTROY:
        {
            PostQuitMessage(EXIT_SUCCESS);
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam); // вызывается в случае если сообщение не обрабатывается
}

//Потоковые функции//
DWORD WINAPI ThreadOneProc1(LPVOID lpParam)
{

    SendMessage(hLabelTread1, WM_SETTEXT, 0, (LPARAM)L"Поток 1. Состояние: Работает");
    static int count = 0;
    for (;;) {
        for (int i = 0; i < 1; i++) {

        }
        count++;
        if (count == 100) {
            count = 0;
        }
        SendMessage(hProgress1, PBM_SETPOS, count, NULL);
    }
    return 0;
}

DWORD WINAPI ThreadOneProc2(LPVOID lpParam)
{
    for (;;) {
        counter++;
        for (int i = 0; i < 1000000000; i++) {

        }
        SendMessage(hProgress2, PBM_STEPIT, 0, 0);
        if (counter == 10) {
            SendMessage(hLabelTread2, WM_SETTEXT, 0, (LPARAM)L"Поток 2. Состояние: Отработал ");
            break;
        }
    }
    return 0;
}

DWORD WINAPI ThreadOneProc3(LPVOID lpParam)
{
    return 0;
}