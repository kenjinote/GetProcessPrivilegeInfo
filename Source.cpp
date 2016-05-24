#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hList;
	switch (msg)
	{
	case WM_CREATE:
		hList = CreateWindow(TEXT("LISTBOX"), 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		{
			HANDLE hToken;
			TCHAR szPrivilegeName[256];
			TCHAR szDisplayName[256];
			DWORD dwLength;
			DWORD dwLanguageId;
			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
			{
				return -1;
			}
			GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &dwLength);
			PTOKEN_PRIVILEGES pTokenPrivileges = (PTOKEN_PRIVILEGES)LocalAlloc(LPTR, dwLength);
			if (pTokenPrivileges == NULL)
			{
				CloseHandle(hToken);
				return -1;
			}
			GetTokenInformation(hToken, TokenPrivileges, pTokenPrivileges, dwLength, &dwLength);
			for (DWORD i = 0; i < pTokenPrivileges->PrivilegeCount; ++i)
			{
				dwLength = sizeof(szPrivilegeName) / sizeof(szPrivilegeName[0]);
				LookupPrivilegeName(NULL, &pTokenPrivileges->Privileges[i].Luid, szPrivilegeName, &dwLength);
				dwLength = sizeof(szDisplayName) / sizeof(szPrivilegeName[0]);
				LookupPrivilegeDisplayName(NULL, szPrivilegeName, szDisplayName, &dwLength, &dwLanguageId);
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)szPrivilegeName);
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)szDisplayName);
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)(pTokenPrivileges->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED ? TEXT("True") : TEXT("False")));
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)TEXT(""));
			}
			CloseHandle(hToken);
			LocalFree(pTokenPrivileges);
		}
		break;
	case WM_SIZE:
		MoveWindow(hList, 10, 10, LOWORD(lParam) - 20, HIWORD(lParam) - 20, TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("プロセストークンに割り当てられている特権情報を取得"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
