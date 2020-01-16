//------------------------------------
//
//		UDPスクリーンサーバー
//
//		注意事項）スレッドを使用するサンプルなのでプロジェクトの設定で
//				　コード生成をマルチスレッドにしておく必要があります。
//
//------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#include <process.h>
#include <time.h>
#include "message.h"
#include "wsock32error.h"

//------------------------------------
// マクロ
//------------------------------------
#define		NAME		"win32A"
#define		TITLE		"UDP ScreenServer"

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
	char			name[128];				// 名前
};

LRESULT CALLBACK WndProc(HWND hWnd,			// ウインドウメッセージ関数 
			 UINT message, 
			 WPARAM wParam, 
			 LPARAM lParam);
void	DrawFrame(HDC hDC);					// 描画関数
void	UdpInit();							// UDP初期処理
void	UdpTerm();							// UDP終了処理

//------------------------------------
// グローバル変数
//------------------------------------
SOCKET		g_mysock;						// ソケット番号
sockaddr_in	g_myaddr;						// 自分のアドレス

ClientInfo	g_Client[MAXPLAYER];
char		g_recvbuf[256];					// 受信バッファ
char		g_sendbuf[256];					// 送信データ		

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

	// UDP初期処理
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
	sprintf(str, "%s/%d %s\0", inet_ntoa(ci.fromaddr.sin_addr), ntohs(ci.fromaddr.sin_port), ci.name);
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
	int				i;

	MSG0* pMsg0;
	MSG1* pMsg1;
	MSG2* pMsg2;
	MSG3* pMsg3;

	srand(timeGetTime());

	// 永久ループ
	while(1)
	{
		len = sizeof(sockaddr);	// 送信元アドレス長

		//##################################################
		// データ受信
		// (recvfrom)
		//##################################################
		sts = recvfrom(g_mysock, g_recvbuf, sizeof(g_recvbuf), 0, (sockaddr*)&fromaddr, &len);
			   
		if (sts != SOCKET_ERROR)
		{
			// 先頭に格納したメッセージタイプを取得
			int type;
			memcpy(&type, g_recvbuf, sizeof(int));

			switch(type)
			{
			// 登録情報が送信されてきた時
			case MSGTYPE_REGIST:
				pMsg0 = (MSG0*)g_recvbuf;
				pMsg1 = (MSG1*)g_sendbuf;

				for(i = 0; i < MAXPLAYER; i++)
				{
					// クライアント状態が無効か？
					if(g_Client[i].status == 0)
					{
						// 配列の空き位置にクライアント情報を登録
						// (配列インデックスがクライアントIDとなる)
						g_Client[i].fromaddr = fromaddr;
						strcpy(g_Client[i].name, pMsg0->name);
						g_Client[i].status = 1;

						//##################################################
						// 初期位置をサーバー側でランダムに決定
						//##################################################
						pMsg1->x = g_Client[i].x = rand() % SCREEN_WID;
						pMsg1->y = g_Client[i].y = rand() % SCREEN_HEI;
						pMsg1->id = i;
						pMsg1->type = MSGTYPE_CLIENTINFO;

						//##################################################
						// 送信元にクライアント情報を返信
						// (sendto)
						//##################################################
						sts = sendto(g_mysock, (char*)pMsg1, sizeof(MSG1), 0, (sockaddr*)&g_Client[i].fromaddr, sizeof(g_Client[i].fromaddr));

						if(sts == SOCKET_ERROR)
						{
							errcode = WSAGetLastError();
							errcom(errcode);
							return;
						}
						break;
					}
				}
				break;

			// 位置情報が送信されてきた時
			case MSGTYPE_POSITIONINFO:
				pMsg2 = (MSG2*)g_recvbuf;

				// クライアント状態が有効か？
				if(g_Client[pMsg2->id].status == 1)
				{
					//##################################################
					// 送信されてきた座標で更新する
					// (配列インデックスがクライアントIDとなる)
					//##################################################
					g_Client[pMsg2->id].x = pMsg2->x;
					g_Client[pMsg2->id].y = pMsg2->y;
					
					// 位置情報を全クライアントに送信
					for(int i = 0; i < MAXPLAYER; i++)
					{
						if(g_Client[i].status)
						{
							//##################################################
							// 有効な全クライアントに送信
							// (sendto)
							//##################################################
							sts = sendto(g_mysock, g_recvbuf, sizeof(g_recvbuf), 0, (sockaddr*)&g_Client[i].fromaddr, sizeof(g_Client[i].fromaddr));
													   
							// 仮処理(送信処理に書き換えること)
							//sts = SOCKET_ERROR;

							if(sts == SOCKET_ERROR)
							{
								errcode = WSAGetLastError();
								errcom(errcode);
								return;
							}
						}
					}
				}
				break;


			// 削除情報が送信されてきた時
			case MSGTYPE_ELIMINATE:
				pMsg3 = (MSG3*)g_recvbuf;

				//##################################################
				// 該当クライアント無効化
				// (配列インデックスがクライアントIDとなる)
				//##################################################
				g_Client[pMsg3->id].status = 0;


				// 削除情報を全クライアントに送信
				for(int i = 0; i < MAXPLAYER; i++)
				{
					if(g_Client[i].status)
					{
						//##################################################
						// 有効な全クライアントに送信
						// (sendto)
						//##################################################
						sts = sendto(g_mysock, g_recvbuf, sizeof(g_recvbuf), 0, (sockaddr*)&g_Client[i].fromaddr, sizeof(g_Client[i].fromaddr));


						// 仮処理(送信処理に書き換えること)
						//sts = SOCKET_ERROR;

						if(sts == SOCKET_ERROR)
						{
							errcode = WSAGetLastError();
							errcom(errcode);
							return;
						}
					}
				}
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
	WSADATA		wd;							// WSAStartup用
	int			sts;						// 戻り値
	int			errcode;					// ソケットのエラーコード
	WORD		requiredversion;			// 

	// このプログラムが要求するバージョン
	requiredversion = MAKEWORD(2, 0);

	// WinSock初期化
	sts = WSAStartup(MAKEWORD(2, 0), &wd);
	if(sts != 0)
	{
		errcode = WSAGetLastError();
		errcom(errcode);
		return;
	}
	// バージョンチェック
	if(wd.wVersion != requiredversion)
	{
		MessageBox(NULL,"VERSION ERROR!", "", MB_OK);
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

	// 自分のソケットにIPアドレス、ポート番号を割り当てる
	g_myaddr.sin_port = htons(PORTNO);
	g_myaddr.sin_family = AF_INET;
	g_myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sts = bind(g_mysock, (sockaddr*)&g_myaddr, sizeof(sockaddr));
	if(sts == SOCKET_ERROR)
	{
		errcode = WSAGetLastError();
		errcom(errcode);
		return;
	}

	//##################################################
	// データを受信するためのスレッドを開始させる
	// (_beginthread)
	//##################################################
	_beginthread(RecvThreadSub, 0, NULL);


}

//------------------------------------
// UDP終了処理
//------------------------------------
void UdpTerm(){

	int		sts;
	int		errcode;					// ソケットのエラーコード

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
