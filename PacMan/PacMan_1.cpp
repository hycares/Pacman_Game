// PacMan.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "PacMan.h"
#include "PacManMove.h"

#define MAX_LOADSTRING 100
#define WINLENTH 700 //700
#define WINHIGHT 740 //740
#define MAXVISSPEED 18 //最小精度是18ms
#define MIDVISSPEED 58
#define MINVISSPEED 98
#define CHK_SUCCESS "Check Success"

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
	
//游戏内移动物体:
PacMan *pPacMan;
PacMove *pEne1;
PacMove *pEne2;
PacMove *pEne3;
PacMove *pEne4;

std::fstream check("check.dat", ios::in | ios::out | ios::binary);
//std::fstream file("stage.dat", ios::in | ios::out);//进度资源

// 此代码模块中包含的函数的前向声明:

//内存释放函数:
template <typename T>
void ReleaseM(T pointer)
{
	if (pointer != NULL)
		delete pointer;
}
unsigned int SaveStage(unsigned int i=0);
void ResetPacMove();

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int, HWND&);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	if (check == NULL)
	{
		MessageBox(NULL, TEXT("Error to start"), TEXT("PacTips"), MB_ICONERROR);
		system("@echo off>check.dat");
		check.open("check.dat", ios::in | ios::out);
		check.write(reinterpret_cast<char *>(CHK_SUCCESS), sizeof(CHK_SUCCESS));
	}
	else
	{
		char buffer[20];
		check.read(buffer, 20);
		if (strcmp(CHK_SUCCESS, buffer))
		{}
	}
	unsigned int stage = 1;
	stage = SaveStage(12);

	unsigned int NUMS = 14;
	pPacMan = new PacMan(PACSHOW_R, PACSHOW_C);
	pEne1   = new RedEne(ENESHOW_R, ENESHOW_C);
	pEne2   = new RedEne(ENESHOW_R, ENESHOW_C);
	pEne3   = new BlueEne(ENESHOW_R, ENESHOW_C);
	pEne4   = new GreenEne(ENESHOW_R, ENESHOW_C);

	//计时修改
	double TimeStart = 0;
	double TimeEnd = 0;
	//
	Stage* Map = new Stage(1);
	Map->InitialMap();
	PacMove::pMapStage = Map;

	Enemy::pacman = pPacMan;

	MSG msg;
	HACCEL hAccelTable;
	HWND hwnd;
	//static HDC stagehdc;
	static DWORD SysTime = 0;
	static DWORD OldTime = 0;
	unsigned int VisSpeed = MIDVISSPEED;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PACMAN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow, hwnd))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PACMAN));

	// 主消息循环:
	while ((pPacMan->GetToward() != CATCH) && (stage < NUMS))
	{
		
		if (pPacMan->Win())
		{
			HDC hdc;
			stage++;	
			hdc = GetDC(hwnd);
			ResetPacMove();
			
			if (stage < NUMS)
			{
				MessageBox(hwnd, TEXT("You Win this stage"), TEXT("PacTips"), MB_OK);
				//PacMove::pMapStage = Map[stage];
				Map->StageWin();
				Map->InitialMap();
				
				RECT rect;
				rect.bottom = WINHIGHT;
				rect.top = 0;
				rect.left = 0;
				rect.right = WINLENTH;
				FillRect(hdc, &rect, CreateSolidBrush(RGB(255,255,255)));
				PacMove::pMapStage->DrawMap(hdc);
			}
			ReleaseDC(hwnd, hdc);
			continue;
		}
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if ((GetAsyncKeyState(VK_UP) & 0x8000) || ((GetAsyncKeyState('W') & 0x8000)))
		{
			pPacMan->SetTwCmd(UP);
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000 || (GetAsyncKeyState('S') & 0x8000))
		{
			pPacMan->SetTwCmd(DOWN);
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x8000 || (GetAsyncKeyState('A') & 0x8000))
		{
			pPacMan->SetTwCmd(LEFT);
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || (GetAsyncKeyState('D') & 0x8000))
		{
			pPacMan->SetTwCmd(RIGHT);
		}
		if ((GetAsyncKeyState(VK_SPACE) & 0x8000) || (GetAsyncKeyState('P') & 0x800f) ||
			(GetAsyncKeyState(VK_PAUSE) & 0X8000))
		{
			pPacMan->SysPause();
		}
		if (GetAsyncKeyState(VK_PRIOR) & 0x8000) //加速控制键
		{
			if (VisSpeed > MAXVISSPEED)
				VisSpeed -= 4;
		}
		if (GetAsyncKeyState(VK_NEXT) & 0x8000) //减速控制键
		{
			if (VisSpeed < MINVISSPEED)
				VisSpeed += 4;
		}
		if (GetAsyncKeyState(VK_HOME) & 0x8000) //回复默认速度
		{
			VisSpeed = MIDVISSPEED;
		}
		else 
		{
			//int rtime = rand()%10000+1;
			//OldTime = GetTickCount();
			//if (((OldTime- SysTime) > VisSpeed))//58 控制动画速度
			TimeStart = clock();
			if (TimeStart - TimeEnd > VisSpeed)
			{
				HDC hdc = GetDC(hwnd);
				
				pEne1->action();
				pEne2->action();
				pEne3->action();
				pEne4->action();
				pPacMan->action();
				                  
				PacMove::pMapStage->DrawPea(hdc);
				pEne1->DrawBlank(hdc);
				pEne2->DrawBlank(hdc);
				pEne3->DrawBlank(hdc);
				pEne4->DrawBlank(hdc);
				pPacMan->DrawBlank(hdc);

				pEne1->Draw(hdc);
				pEne2->Draw(hdc);
				pEne3->Draw(hdc);
				pEne4->Draw(hdc);
				pPacMan->Draw(hdc);

				if ((pPacMan->GetToward()) == CATCH)
					break;

				ReleaseDC(hwnd, hdc);

				TimeEnd = clock();
				//SysTime = GetTickCount();
			}
		}
		//DeleteDC(hdc);
	}

	if (pPacMan->GetToward() == CATCH)
	{
	//	DeleteDC(hdc);
		MessageBox(hwnd, TEXT("You Lose,~~o(>_<)o ~~"), TEXT("PacTips"), MB_OK);
	}
	else
	{
		MessageBox(hwnd, TEXT("You Win, ╮(╯▽╰)╭"), TEXT("PacTips"), MB_OK);
	}

	//释放内存块:
	ReleaseM(pPacMan);
	ReleaseM(pEne1);
	ReleaseM(pEne2);
	ReleaseM(pEne3);
	ReleaseM(pEne4);
	ReleaseM(Map);

	check.close();
//	file.close();
	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_VREDRAW | CS_HREDRAW;	
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PACMAN));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PACMAN);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND &hwnd)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   hwnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, //去除最大化和窗口大小重绘
      CW_USEDEFAULT, CW_USEDEFAULT, WINLENTH, WINHIGHT, NULL, NULL, hInstance, NULL);

   if (!hwnd)
   {
      return FALSE;
   }

   ShowWindow(hwnd, SW_SHOWNORMAL);
   UpdateWindow(hwnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			pPacMan->Catch();//test!
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		PacMove::pMapStage->DrawMap(hdc);
		PacMove::pMapStage->DrawPea(hdc);
		//
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		if (IDYES == MessageBox(hWnd, TEXT("Do you want to quit?"), TEXT("PacMessage"), MB_YESNO))
			DestroyWindow(hWnd);
	case WM_DESTROY:
		pPacMan->Catch();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
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

void ResetPacMove()
{
	pPacMan->SetPosition(PACSHOW_R, PACSHOW_C);
	pPacMan->SetTwCmd(LEFT);
	pEne1->SetPosition(ENESHOW_R, ENESHOW_C);
	pEne2->SetPosition(ENESHOW_R, ENESHOW_C);
	pEne3->SetPosition(ENESHOW_R, ENESHOW_C);
	pEne4->SetPosition(ENESHOW_R, ENESHOW_C);
	
}

//读取关卡时应形参得是0
unsigned int SaveStage(unsigned int i)
{
	unsigned int stagesave = 1;

	fstream fsave("save.dat", ios::in | ios::out | ios::binary);
	if (fsave == NULL)
	{
		system("del save.dat");
		system("@echo off>save.dat");
		fsave.open("save.dat", ios::in | ios::out | ios::binary);
		fsave.write(reinterpret_cast<const char *>(&stagesave), sizeof(stagesave));
	}
	fsave.seekg(0);
	 
	if (i == 0)
	{	
		fsave.read(reinterpret_cast<char *>(&stagesave), sizeof(stagesave));
		return stagesave;
	}
	else
		fsave.write(reinterpret_cast<const char *>(&stagesave), sizeof(stagesave));

	return 1;	
}