//------------------------------------
//
//		UDPスクリーンクライアント
//
//------------------------------------
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <process.h>
#include <time.h>
#include "wsock32error.h"
#include "message.h"

//------------------------------------
// マクロ
//------------------------------------
#define		NAME		"win32A"
#define		TITLE		"UDP ScreenClient(アクティブ時のみキー受付)"

#define		PORTNO		(20250)

#define		MAXPLAYER	(100)
#define		RECT_WID	(100)
#define		RECT_HEI	(50)
#define		SCREEN_WID	(640)
#define		SCREEN_HEI	(480)

#define		INTERVAL	(33)	// 30FPS

//------------------------------------
// ライブラリ
//------------------------------------
#pragma	comment(lib,"ws2_32.lib")
#pragma	comment(lib,"winmm.lib")

//------------------------------------
// プロトタイプ
//------------------------------------

// クライアント情報
struct ClientInfo
{
	int				status;					// 0: not use  1: use
	int				x;						// X座標
	int				y;						// Y座標
	sockaddr_in		fromaddr;				// アドレス
	char			name[NAMEMAX];			// 名前
};

LRESULT CALLBACK WndProc(HWND hWnd,			// ウインドウメッセージ関数 
			 UINT message, 
			 WPARAM wParam, 
			 LPARAM lParam);
void	DrawFrame(HDC hDC);					// 描画関数
void	UdpInit();							// UDP初期処理
void	UdpTerm();							// UDP終了処理

//------------------------------------
// グローバル
//------------------------------------
SOCKET		g_mysock;						// ソケット番号
sockaddr_in	g_myaddr;						// 自分のアドレス
sockaddr_in	g_toaddr;						// 送信先アドレス

ClientInfo	g_Client[MAXPLAYER];
char		g_recvbuf[256];					// 受信バッファ
char		g_sendbuf[256];					// 送信データ		

char		g_serverip[256] = "127.0.0.1";	// サーバーアドレス
char		g_myname[NAMEMAX] = "NO NAME";	// 表示名
int			g_myid = -1;					// クライアントID

HDC			g_hMemDC;						// メモリDC(バックバッファ描画用)
HBITMAP		g_hBitmap;						// バックバッファ
HGDIOBJ		g_hBrush;						// 透過ブラシ

//------------------------------------
// WinMain関数
//------------------------------------
int APIENTRY WinMain(HINSTANCE 	hInstance, 		// アプリケーションインスタンス値
					 HINSTANCE 	hPrevInstance,	// 意味なし
					 LPSTR 		lpszArgs, 		// 起動時の引数文字列
					 int 		nWinMode)		// ウインドウ表示モード
{
	HWND			hWnd;						// ウインドウハンドル
	MSG				msg;						// メッセージ構造体
	WNDCLASSEX		wcex;						// ウインドウクラス構造体

	DWORD			nowtime;					// 現在時刻
	static DWORD	oldtime;					// 前回描画時刻

	// ウインドウクラス情報のセット
	wcex.hInstance		= hInstance;			// インスタンス値のセット
	wcex.lpszClassName	= NAME;					// クラス名
	wcex.lpfnWndProc	= (WNDPROC)WndProc;		// ウインドウメッセージ関数
	wcex.style			= 0;					// ウインドウスタイル
	wcex.cbSize 		= sizeof(WNDCLASSEX);	// 構造体のサイズ
	wcex.hIcon			= LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);	// ラージアイコン
	wcex.hIconSm		= LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);		// スモールアイコン
	wcex.hCursor		= LoadCursor((HINSTANCE)NULL, IDC_ARROW);		// カーソルスタイル
	wcex.lpszMenuName	= 0; 					// メニューなし
	wcex.cbClsExtra		= 0;					// エキストラなし
	wcex.cbWndExtra		= 0;					
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);		// 背景色白

	if (!RegisterClassEx(&wcex)) return FALSE;	// ウインドウクラスの登録

	hWnd = CreateWindow(NAME, 											// ウインドウクラスの名前
				TITLE, 													// タイトル
				WS_CAPTION | WS_SYSMENU,								// ウインドウスタイル
				CW_USEDEFAULT, CW_USEDEFAULT,							// X座標,Y座標
				SCREEN_WID + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,		// 幅
				SCREEN_HEI + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +	// 高さ
					GetSystemMetrics(SM_CYCAPTION),
				HWND_DESKTOP, 											// 親ウインドウなし
				(HMENU)NULL, 											// メニューなし
				hInstance, 												// インスタンスハンドル
				(LPVOID)NULL);											// 追加引数なし

	if (!hWnd) return FALSE;

	// ウインドウを表示する
	ShowWindow(hWnd, nWinMode);
	UpdateWindow(hWnd);

	// UDP初期化
	UdpInit();
	
	// メッセージ･ループ
	while(1)
	{
		// メッセージがあるかどうかをチェック
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
       	{
			// メッセージを取得
			if(!GetMessage(&msg, NULL, 0, 0)) 
			{
				break;
			}
			else
			{
				TranslateMessage(&msg); 		// キーボード使用を可能にする
				DispatchMessage(&msg); 			// コントロールをWindowsに戻す
			}
		}
		else
		{
			nowtime = timeGetTime();			// 現在時刻獲得
			if(nowtime - oldtime > INTERVAL)
			{
				if(g_myid >= 0)
				{
					if(GetActiveWindow() == hWnd)	// アクティブ時のみキー受付
					{
						// キー入力検知
						if(GetAsyncKeyState(VK_UP) & 0x8000)
						{
							g_Client[g_myid].y -= 4;
						}
						if(GetAsyncKeyState(VK_DOWN) & 0x8000)
						{
							g_Client[g_myid].y += 4;
						}
						if(GetAsyncKeyState(VK_RIGHT) & 0x8000)
						{
							g_Client[g_myid].x += 4;
						}
						if(GetAsyncKeyState(VK_LEFT) & 0x8000)
						{
							g_Client[g_myid].x -= 4;
						}
					}

					int sts = SOCKET_ERROR;

					//##################################################
					// 位置情報を構造体にセットして送信
					// (sendto)
					//##################################################
					MSG2* pMsg2 = (MSG2*)g_sendbuf;

					pMsg2->id = g_myid;
					pMsg2->type = MSGTYPE_POSITIONINFO;
					pMsg2->x = g_Client[g_myid].x;
					pMsg2->y = g_Client[g_myid].y;
					strcpy(pMsg2->name, g_myname);
					
					sts = sendto(g_mysock, g_sendbuf, sizeof(MSG2), 0, (sockaddr*)&g_toaddr, sizeof(g_toaddr));


					if(sts == SOCKET_ERROR)
					{
						errcom(WSAGetLastError());
					}
				}

				oldtime = nowtime;
				DrawFrame(g_hMemDC);
				InvalidateRect(hWnd, NULL, false);
			}
		}
	}

	// UDP終了処理
	UdpTerm();
	
	return msg.wParam;
}

//------------------------------------
// クライアントの情報を描画する
//------------------------------------
void DrawClientInfo(HDC hDC, int cx, int cy, int w, int h, ClientInfo ci)
{
	char	str[256];
	
	Rectangle(hDC, (cx - w / 2), (cy - h / 2), (cx + w / 2), (cy + h / 2));

	sprintf(str, "%s\0", ci.name);
	TextOut(hDC, (cx - w / 3), cy, str, strlen(str));
}

//------------------------------------
// クライアントの情報検索
//------------------------------------
void DrawClient(HDC hDC, ClientInfo ci)
{
	if(ci.status == 1)
	{
		DrawClientInfo(hDC, ci.x, ci.y, RECT_WID, RECT_HEI, ci);
	}
}

//------------------------------------
// フレーム描画
//------------------------------------
void DrawFrame(HDC hDC)
{
	PatBlt(hDC, 0, 0, SCREEN_WID, SCREEN_HEI, WHITENESS);		// 画面を白で塗りつぶす
		
	// クライアントの情報数分ループ
	for(int i = 0; i < MAXPLAYER; i++)
	{		
		if(g_Client[i].status == 1)
		{
			// クライアントの情報を描画する
			DrawClient(hDC, g_Client[i]);				
		}
	}
}

//------------------------------------
// ウインドウプロシージャ
//------------------------------------
LRESULT WINAPI WndProc(HWND hWnd, 				// ウインドウハンドル値
						UINT message,			// メッセージ識別子
						WPARAM wParam,			// 付帯情報１
						LPARAM lParam)			// 付帯情報２
{
	HDC hDC;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		// バックバッファ生成
		hDC = GetDC(hWnd);
		g_hMemDC = CreateCompatibleDC(hDC);
		g_hBitmap = CreateCompatibleBitmap(hDC, SCREEN_WID, SCREEN_HEI);
		g_hBrush = GetStockObject(NULL_BRUSH);
		SelectObject(g_hMemDC, g_hBitmap);
		SelectObject(g_hMemDC, g_hBrush);
		SetBkMode(g_hMemDC, TRANSPARENT);
		SetTextColor(g_hMemDC, RGB(0, 0, 0));
		ReleaseDC(hWnd, hDC);
		break;

	case WM_PAINT:
		// バックバッファ転送
		hDC = BeginPaint(hWnd, &ps);
		BitBlt(hDC, 0, 0, SCREEN_WID, SCREEN_HEI, g_hMemDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_DESTROY:
		// バックバッファ解放
		DeleteDC(g_hMemDC);
		DeleteObject(g_hBitmap);

		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//------------------------------------
// UDP送受信関数
//		p1 : スレッドに渡されるデータアドレス(今回は何も渡さない)
//------------------------------------
void RecvThreadSub(void *p1)
{
	int	len;

	sockaddr_in		fromaddr;			// 送信元アドレス
	int				sts = SOCKET_ERROR;
	int				errcode;

	MSG0* pMsg0;
	MSG1* pMsg1;
	MSG2* pMsg2;
	MSG3* pMsg3;

	// 永久ループ
	while(1)
	{
		len = sizeof(sockaddr);	// 送信元アドレス長

		//##################################################
		// データ受信
		// (recvfrom)
		//##################################################
		sts = recvfrom(g_mysock, g_recvbuf, sizeof(g_recvbuf), 0, (sockaddr*)&g_myaddr, &len);



		if (sts != SOCKET_ERROR)
		{
			// 先頭に格納したメッセージタイプを取得
			int type;
			memcpy(&type, g_recvbuf, sizeof(int));

			switch(type)
			{
			// 登録成功時クライアント情報
			case MSGTYPE_CLIENTINFO:
				pMsg1 = (MSG1*)g_recvbuf;

				// IDを取得
				g_myid = pMsg1->id;
				// 名前をセット
				strcpy(g_Client[g_myid].name, g_myname);

				//##################################################
				// サーバーから指定された初期位置を取得
				// (配列インデックスがクライアントIDとなる)
				//##################################################
				g_Client[pMsg1->id].x = pMsg1->x;
				g_Client[pMsg1->id].y = pMsg1->y;


				// 表示開始
				g_Client[g_myid].status = 1;
				break;

			// 位置情報が送信されてきた時
			case MSGTYPE_POSITIONINFO:
				pMsg2 = (MSG2*)g_recvbuf;

				// 自分自身の位置は反映しない(巻き戻り防止)
				if(pMsg2->id == g_myid) break;

				//##################################################
				// 送信されてきた座標で更新する
				// (配列インデックスがクライアントIDとなる)
				//##################################################
				pMsg2->type = MSGTYPE_POSITIONINFO;
				g_Client[pMsg2->id].x = pMsg2->x;
				g_Client[pMsg2->id].y = pMsg2->y;
				g_Client[pMsg2->id].status = 1;
				strcpy(g_Client[pMsg2->id].name, pMsg2->name);
				break;

			// 削除情報が送信されてきた時
			case MSGTYPE_ELIMINATE:
				//##################################################
				// 該当クライアント無効化
				// (配列インデックスがクライアントIDとなる)
				//##################################################
				pMsg3 = (MSG3*)g_recvbuf;
				g_Client[pMsg3->id].status = 0;


				break;
			}
		}
		else
		{
			errcode = WSAGetLastError();
			if(errcode != WSAEINTR)	// recvfromによるブロッキングがキャンセルされた場合を除く
			{
				errcom(errcode);
			}
			break;
		}		
	}
}

//------------------------------------
// UDP初期処理
//------------------------------------
void UdpInit()
{
	WSADATA		m_wd;						// WSAStartup用
	int			sts;						// 戻り値
	int			errcode;					// ソケットのエラーコード
	WORD		requiredversion;			// 

	// このプログラムが要求するバージョン
	requiredversion = MAKEWORD(2, 0);

	// Winsock初期化
	sts = WSAStartup(MAKEWORD(2, 0), &m_wd);
	if(sts != 0)
	{
		errcode = WSAGetLastError();
		errcom(errcode);
		return;
	}
	// バージョンチェック
	if(m_wd.wVersion != requiredversion)
	{
		MessageBox(NULL, "VERSION ERROR!", "", MB_OK);
		return;
	}

	// ソケット作成(UDPプロトコル用のソケット作成)
	g_mysock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(g_mysock == INVALID_SOCKET)
	{
		errcode = WSAGetLastError();
		errcom(errcode);
		return;
	}

	// ファイルからサーバーアドレスと表示名を読み込む
	FILE* fp = fopen("config.txt", "rt");
	fscanf(fp, "%s", g_serverip);
	fscanf(fp, "%s", g_myname);
	fclose(fp);

	// あて先の設定
	g_toaddr.sin_addr.S_un.S_addr = inet_addr(g_serverip);
	g_toaddr.sin_family = AF_INET;
	g_toaddr.sin_port = htons(PORTNO);

	//##################################################
	// 登録要求を構造体にセットして送信
	// (sendto)
	//##################################################
	MSG0* pMsg0 = (MSG0*)g_sendbuf;

	strcpy(pMsg0->name, "810");
	pMsg0->type = MSGTYPE_REGIST;

	sts = sendto(g_mysock, g_sendbuf, sizeof(MSG0), 0, (sockaddr*)&g_toaddr, sizeof(g_toaddr));

	//// 仮処理(送信処理に書き換えること)
	//sts = SOCKET_ERROR;

	if(sts == SOCKET_ERROR)
	{
		errcode = WSAGetLastError();
		errcom(errcode);
	}
	else
	{
		//##################################################
		// データを受信するためのスレッドを開始させる
		// (_beginthread)
		//##################################################
		_beginthread(RecvThreadSub, 0, NULL);



	}
}

//------------------------------------
// UDP終了処理
//------------------------------------
void UdpTerm()
{
	int		sts = SOCKET_ERROR;
	int		errcode;					// ソケットのエラーコード

	//##################################################
	// 削除要求を構造体にセットして送信
	// (sendto)
	//##################################################
	MSG3* pMsg3 = (MSG3*)g_sendbuf;
	pMsg3->id = g_myid;
	pMsg3->type = MSGTYPE_ELIMINATE;
	sts = sendto(g_mysock, (char*)pMsg3, sizeof(MSG3), 0, (sockaddr*)&g_toaddr, sizeof(sockaddr));


	if(sts == SOCKET_ERROR)
	{
		errcode = WSAGetLastError();
		errcom(errcode);
	}

	// ソケットをクローズ
	sts = closesocket(g_mysock);
	if(sts != 0)
	{
		errcode = WSAGetLastError();
		errcom(errcode);
	}

	// WinSockの後処理
	WSACleanup();
}
