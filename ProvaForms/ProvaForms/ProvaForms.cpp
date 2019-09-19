// ProvaForms.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ProvaForms.h"
#pragma comment(lib, "comctl32.lib") // For toolbar
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR fileNotOpen[MAX_LOADSTRING];
HWND toolbar;
HIMAGELIST g_hImageList = NULL;
HIMAGELIST hImageList;
DWORD g_BytesTransferred = 0;
char* readedFromFile;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ComingSoon(HWND, UINT, WPARAM, LPARAM);
void			    writeFile(HWND, int, LPCWSTR, const char text[]);
bool				readFile(HWND hWnd, LPCWSTR name);
void	CALLBACK	FileIOCompletionRoutine(__in DWORD dwErrorCode, __in DWORD dwNumberOfBytesTransfered, __in LPOVERLAPPED lpOverlapped);
HWND				CreateToolbar(HWND);

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
    LoadStringW(hInstance, IDC_PROVAFORMS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROVAFORMS));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROVAFORMS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROVAFORMS);
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
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			case IDM_NEW:
				EnableMenuItem(GetMenu(hWnd), IDM_SAVE, MF_ENABLED);
				EnableMenuItem(GetMenu(hWnd), IDM_SAVE_AS, MF_ENABLED);
				EnableMenuItem(GetMenu(hWnd), IDM_BEEPER, MF_ENABLED);
				EnableMenuItem(GetMenu(hWnd), IDM_INITIAL_POS, MF_ENABLED);
				EnableMenuItem(GetMenu(hWnd), IDM_TRAY, MF_ENABLED);
				EnableMenuItem(GetMenu(hWnd), IDM_WALL, MF_ENABLED);
				EnableMenuItem(GetMenu(hWnd), IDM_DWALL, MF_ENABLED);
				EnableMenuItem(GetMenu(hWnd), IDM_BEXIT, MF_ENABLED);
				SendMessage(toolbar,TB_ENABLEBUTTON,IDM_SAVE,true);
				SendMessage(toolbar, TB_ENABLEBUTTON, IDM_SAVE_AS, true);
				break;
			case IDM_SAVE_AS:
				writeFile(hWnd, CREATE_NEW, L".\\temp.txt", "hello world");
				break;
			case IDM_SAVE:
				writeFile(hWnd, OPEN_EXISTING, L".\\tempppp.txt", "123456789");
                break;
			case IDM_OPEN:
				if (readFile(hWnd, L".\\temp.txt"))
					writeFile(hWnd, CREATE_NEW, L".\\output.txt", readedFromFile);
				break;
			case BTN_BUTTON1:
				DestroyWindow(hWnd);
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_CREATE:
	{ 
		HINSTANCE hIns = ((LPCREATESTRUCT)lParam)->hInstance;
		HICON hBitmap = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
		//HBITMAP hBitmap = (HBITMAP)LoadImage(NULL,"karel.ico",IMAGE_BITMAP,0, 0,LR_LOADFROMFILE);
		//HWND hwndButton = CreateWindowEx(0, L"BUTTON", L"Posició inicial del Beeper", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, 55, 60, 45, 45, hWnd, (HMENU)BTN_BUTTON1, hIns, NULL);
		//SendMessage(hwndButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hBitmap);

		// Creating the tootlbar
		toolbar = CreateToolbar(hWnd);

		return 0;
	}
    case WM_PAINT:
        {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		//TCHAR string[] = _T("No hi ha cap fitxer obert");
		LoadStringW(hInst, IDS_FNO, fileNotOpen, MAX_LOADSTRING);
		TextOut(hdc,
			2, 60,
			fileNotOpen, _tcslen(fileNotOpen));
		EndPaint(hWnd, &ps);
        }
        break;
	case WM_SIZE:
		SendMessage(toolbar, TB_AUTOSIZE, 0, 0);
		break;
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
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			PostQuitMessage(0);
			return (INT_PTR)TRUE;
			break;
		}
        break;
    }
    return (INT_PTR)FALSE;
}
// Message handler for coming soon box.
INT_PTR CALLBACK ComingSoon(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

HWND CreateToolbar(HWND hWndParent)
{
	// Declare and initialize local constants.
	const int ImageListID = 0;
	const int numButtons = 5;
	const int bitmapSize = 16;

	const DWORD buttonStyles = BTNS_AUTOSIZE;

	// Create the toolbar.
	HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE, 0, 0, 0, 0,
		hWndParent, NULL, hInst, NULL);

	if (hWndToolbar == NULL)
		return NULL;

	// Create the image list.
	g_hImageList = ImageList_Create(bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
		ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
		numButtons, 0);

	// Set the image list.
	SendMessage(hWndToolbar, TB_SETIMAGELIST,
		(WPARAM)ImageListID,
		(LPARAM)g_hImageList);

	// Load the button images.
	SendMessage(hWndToolbar, TB_LOADIMAGES,
		(WPARAM)IDB_STD_SMALL_COLOR,
		(LPARAM)HINST_COMMCTRL);

	// Load button text
	int toolbartext = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM)hInst, (LPARAM)IDS_TOOLBAR);

	// Initialize button info.
	// IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.

	TBBUTTON tbButtons[numButtons] =
	{
		{ MAKELONG(STD_DELETE, ImageListID),   IDM_EXIT, TBSTATE_ENABLED, buttonStyles, {0}, 0, toolbartext },
		{ MAKELONG(STD_FILENEW,  ImageListID), IDM_NEW,  TBSTATE_ENABLED, buttonStyles, {0}, 0, toolbartext+1 },
		{ MAKELONG(STD_FILEOPEN, ImageListID), IDM_OPEN, TBSTATE_ENABLED, buttonStyles, {0}, 0, toolbartext + 2 },
		{ MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVE, 0, buttonStyles, {0}, 0, toolbartext + 3 },
		{ MAKELONG(STD_PROPERTIES, ImageListID), IDM_INSERT, 0, buttonStyles, {0}, 0, toolbartext + 4 }
	};

	// Add buttons.
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);

	// Resize the toolbar, and then show it.
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hWndToolbar, TRUE);

	return hWndToolbar;
}

//Open or create and write to file
void writeFile(HWND hWnd, int mode, LPCWSTR name, const char text[]) {
	HANDLE hFile;
	//char text[] = "hello world"; - test text for write
	DWORD dwBytesToWrite = (DWORD)strlen(text);
	DWORD dwBytesWritten = 0;
	BOOL bErrorFlag = FALSE;

	hFile = CreateFile(name,   // file to open
		GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		mode,					// create or open existing
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(hWnd,
			(mode == CREATE_NEW)
			? TEXT("No s'ha pogut crear el fitxer.\nError: INVALID_HANDLE_VALUE\nPot ser que el fitxer ja existeixi o que no es tinguin els permisos necessaris")
			: TEXT("No s'ha pogut obrir el fitxer.\nError: INVALID_HANDLE_VALUE\nPot ser que el fitxer no existeixi o que no es tinguin els permisos necessaris"),
			TEXT("Error"), MB_ICONERROR);
		return;
	}

	bErrorFlag = WriteFile(
		hFile,           // open file handle
		text,      // start of data to write
		dwBytesToWrite,  // number of bytes to write
		&dwBytesWritten, // number of bytes that were written
		NULL);            // no overlapped structure
	
	if (FALSE == bErrorFlag) {
		MessageBox(hWnd, TEXT("No s'ha pogut escriure el fitxer."), TEXT("Error"), MB_ICONEXCLAMATION);
	} else {
		if (dwBytesWritten != dwBytesToWrite)
		{
			// This is an error because a synchronous write that results in
			// success (WriteFile returns TRUE) should write all data as
			// requested. This would not necessarily be the case for
			// asynchronous writes.
			MessageBox(hWnd, TEXT("No s'ha pogut escriure el fitxer.\nError: Els bytes totals són diferents dels escrits"), TEXT("Error"), MB_ICONERROR);
		}
		else {
			MessageBox(hWnd, TEXT("El fitxer s\'ha desat correctament"), TEXT("Desat!"), MB_ICONINFORMATION);
		}
	}
	CloseHandle(hFile);
}

// Read a file
// ===============

//		- Callback for errors
void CALLBACK FileIOCompletionRoutine(__in DWORD dwErrorCode, __in DWORD dwNumberOfBytesTransfered, __in LPOVERLAPPED lpOverlapped){
	_tprintf(TEXT("Error code:\t%x\n"), dwErrorCode);
	_tprintf(TEXT("Number of bytes:\t%x\n"), dwNumberOfBytesTransfered);
	g_BytesTransferred = dwNumberOfBytesTransfered;
}

//		- Main function
bool readFile(HWND hWnd, LPCWSTR name) {
	HANDLE hFile;
	DWORD  dwBytesRead = 0;
	char   ReadBuffer[MAX_LOADSTRING] = { 0 };
	OVERLAPPED ol = { 0 };

	hFile = CreateFile(name,   // file to open
		GENERIC_READ,          // open for reading
		0,					   // do not share
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
		NULL);                 // no attr. template

	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(hWnd,
			TEXT("No s'ha pogut obrir el fitxer.\nError: INVALID_HANDLE_VALUE\nPot ser que el fitxer no existeixi o que no es tinguin els permisos necessaris"),
			TEXT("Error"), MB_ICONERROR);
		return false;
	}

	if (FALSE == ReadFileEx(hFile, ReadBuffer, MAX_LOADSTRING - 1, &ol, FileIOCompletionRoutine)){
		MessageBox(hWnd,
			TEXT("No s'ha pogut llegir del fitxer..."),
			TEXT("Error"), MB_ICONERROR);
		CloseHandle(hFile);
		return false;
	}

	SleepEx(5000, TRUE);
	dwBytesRead = g_BytesTransferred;
	bool toReturn = false;

	if (dwBytesRead > 0 && dwBytesRead <= MAX_LOADSTRING - 1){
		ReadBuffer[dwBytesRead] = '\0'; // NULL character
		MessageBox(hWnd,
			TEXT("El fitxer s'ha llegit correctament"),
			TEXT("Llegit!"), MB_ICONINFORMATION);
		toReturn = true;
		readedFromFile = ReadBuffer;
	}else if (dwBytesRead == 0){
		MessageBox(hWnd,
			TEXT("El fitxer està buit"),
			TEXT("Error"), MB_ICONEXCLAMATION);
	}else{
		MessageBox(hWnd,
			TEXT("No s'ha pogut llegir del fitxer.\nError: Valor no vàlid a la variable dels bytes llegits (dwBytesRead)"),
			TEXT("Error"), MB_ICONERROR);
	}
	return toReturn;
	CloseHandle(hFile);
}