//------------------------------------
//
//		UDP�X�N���[���T�[�o�[
//
//		���ӎ����j�X���b�h���g�p����T���v���Ȃ̂Ńv���W�F�N�g�̐ݒ��
//				�@�R�[�h�������}���`�X���b�h�ɂ��Ă����K�v������܂��B
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
// �}�N��
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
// ���C�u����
//------------------------------------
#pragma	comment(lib,"ws2_32.lib")
#pragma	comment(lib,"winmm.lib")

//------------------------------------
// �v���g�^�C�v
//------------------------------------

// �N���C�A���g���
struct ClientInfo
{
	int				status;					// 0: not use  1: use
	int				x;						// X���W
	int				y;						// Y���W
	sockaddr_in		fromaddr;				// �A�h���X
	char			name[128];				// ���O
};

LRESULT CALLBACK WndProc(HWND hWnd,			// �E�C���h�E���b�Z�[�W�֐� 
			 UINT message, 
			 WPARAM wParam, 
			 LPARAM lParam);
void	DrawFrame(HDC hDC);					// �`��֐�
void	UdpInit();							// UDP��������
void	UdpTerm();							// UDP�I������

//------------------------------------
// �O���[�o���ϐ�
//------------------------------------
SOCKET		g_mysock;						// �\�P�b�g�ԍ�
sockaddr_in	g_myaddr;						// �����̃A�h���X

ClientInfo	g_Client[MAXPLAYER];
char		g_recvbuf[256];					// ��M�o�b�t�@
char		g_sendbuf[256];					// ���M�f�[�^		

HDC			g_hMemDC;						// ������DC(�o�b�N�o�b�t�@�`��p)
HBITMAP		g_hBitmap;						// �o�b�N�o�b�t�@
HGDIOBJ		g_hBrush;						// ���߃u���V

//------------------------------------
// WinMain�֐�
//------------------------------------
int APIENTRY WinMain(HINSTANCE 	hInstance, 		// �A�v���P�[�V�����C���X�^���X�l
					 HINSTANCE 	hPrevInstance,	// �Ӗ��Ȃ�
					 LPSTR 		lpszArgs, 		// �N�����̈���������
					 int 		nWinMode)		// �E�C���h�E�\�����[�h
{
	HWND			hWnd;						// �E�C���h�E�n���h��
	MSG				msg;						// ���b�Z�[�W�\����
	WNDCLASSEX		wcex;						// �E�C���h�E�N���X�\����

	DWORD			nowtime;					// ���ݎ���
	static DWORD	oldtime;					// �O��`�掞��

	// �E�C���h�E�N���X���̃Z�b�g
	wcex.hInstance		= hInstance;			// �C���X�^���X�l�̃Z�b�g
	wcex.lpszClassName	= NAME;					// �N���X��
	wcex.lpfnWndProc	= (WNDPROC)WndProc;		// �E�C���h�E���b�Z�[�W�֐�
	wcex.style			= 0;					// �E�C���h�E�X�^�C��
	wcex.cbSize 		= sizeof(WNDCLASSEX);	// �\���̂̃T�C�Y
	wcex.hIcon			= LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);	// ���[�W�A�C�R��
	wcex.hIconSm		= LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);		// �X���[���A�C�R��
	wcex.hCursor		= LoadCursor((HINSTANCE)NULL, IDC_ARROW);		// �J�[�\���X�^�C��
	wcex.lpszMenuName	= 0; 					// ���j���[�Ȃ�
	wcex.cbClsExtra		= 0;					// �G�L�X�g���Ȃ�
	wcex.cbWndExtra		= 0;					
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);		// �w�i�F��

	if (!RegisterClassEx(&wcex)) return FALSE;	// �E�C���h�E�N���X�̓o�^

	hWnd = CreateWindow(NAME, 											// �E�C���h�E�N���X�̖��O
				TITLE, 													// �^�C�g��
				WS_CAPTION | WS_SYSMENU,								// �E�C���h�E�X�^�C��
				CW_USEDEFAULT, CW_USEDEFAULT,							// X���W,Y���W
				SCREEN_WID + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,		// ��
				SCREEN_HEI + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +	// ����
					GetSystemMetrics(SM_CYCAPTION),
				HWND_DESKTOP, 											// �e�E�C���h�E�Ȃ�
				(HMENU)NULL, 											// ���j���[�Ȃ�
				hInstance, 												// �C���X�^���X�n���h��
				(LPVOID)NULL);											// �ǉ������Ȃ�

	if (!hWnd) return FALSE;

	// �E�C���h�E��\������
	ShowWindow(hWnd, nWinMode);
	UpdateWindow(hWnd);

	// UDP��������
	UdpInit();
	
	// ���b�Z�[�W����[�v
	while(1)
	{
		// ���b�Z�[�W�����邩�ǂ������`�F�b�N
		if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
       	{
			// ���b�Z�[�W���擾
			if(!GetMessage(&msg, NULL, 0, 0))
			{ 
				break; 
			}
			else
			{
				TranslateMessage(&msg); 		// �L�[�{�[�h�g�p���\�ɂ���
				DispatchMessage(&msg); 			// �R���g���[����Windows�ɖ߂�
			}
		}
		else
		{
			nowtime = timeGetTime();			// ���ݎ����l��
			if(nowtime - oldtime > INTERVAL)
			{
				oldtime = nowtime;
				DrawFrame(g_hMemDC);
				InvalidateRect(hWnd, NULL, false);
			}
		}
	}

	// UDP�I������
	UdpTerm();
	
	return msg.wParam;
}

//------------------------------------
// �N���C�A���g�̏���`�悷��
//------------------------------------
void DrawClientInfo(HDC hDC, int cx, int cy, int w, int h, ClientInfo ci)
{
	char	str[256];
	
	Rectangle(hDC, (cx - w / 2), (cy - h / 2), (cx + w / 2), (cy + h / 2));
	sprintf(str, "%s/%d %s\0", inet_ntoa(ci.fromaddr.sin_addr), ntohs(ci.fromaddr.sin_port), ci.name);
	TextOut(hDC, (cx - w / 3), cy, str, strlen(str));
}

//------------------------------------
// �N���C�A���g�̏�񌟍�
//------------------------------------
void DrawClient(HDC hDC, ClientInfo ci)
{
	if(ci.status == 1)
	{
		DrawClientInfo(hDC, ci.x, ci.y, RECT_WID, RECT_HEI, ci);
	}
}

//------------------------------------
// �t���[���`��
//------------------------------------
void DrawFrame(HDC hDC)
{
	PatBlt(hDC, 0, 0, SCREEN_WID, SCREEN_HEI, WHITENESS);		// ��ʂ𔒂œh��Ԃ�

	// �N���C�A���g�̏�񐔕����[�v
	for(int i = 0; i < MAXPLAYER; i++)
	{		
		if(g_Client[i].status == 1)
		{
			// �N���C�A���g�̏���`�悷��
			DrawClient(hDC, g_Client[i]);				
		}
	}
}

//------------------------------------
// �E�C���h�E�v���V�[�W��
//------------------------------------
LRESULT WINAPI WndProc(HWND hWnd, 				// �E�C���h�E�n���h���l
						UINT message,			// ���b�Z�[�W���ʎq
						WPARAM wParam,			// �t�я��P
						LPARAM lParam)			// �t�я��Q
{
	HDC hDC;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		// �o�b�N�o�b�t�@����
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
		// �o�b�N�o�b�t�@�]��
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
		// �o�b�N�o�b�t�@���
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
// UDP����M�֐�
//		p1 : �X���b�h�ɓn�����f�[�^�A�h���X(����͉����n���Ȃ�)
//------------------------------------
void RecvThreadSub(void *p1)
{
	int	len;

	sockaddr_in		fromaddr;			// ���M���A�h���X
	int				sts = SOCKET_ERROR;
	int				errcode;
	int				i;

	MSG0* pMsg0;
	MSG1* pMsg1;
	MSG2* pMsg2;
	MSG3* pMsg3;

	srand(timeGetTime());

	// �i�v���[�v
	while(1)
	{
		len = sizeof(sockaddr);	// ���M���A�h���X��

		//##################################################
		// �f�[�^��M
		// (recvfrom)
		//##################################################
		sts = recvfrom(g_mysock, g_recvbuf, sizeof(g_recvbuf), 0, (sockaddr*)&fromaddr, &len);
			   
		if (sts != SOCKET_ERROR)
		{
			// �擪�Ɋi�[�������b�Z�[�W�^�C�v���擾
			int type;
			memcpy(&type, g_recvbuf, sizeof(int));

			switch(type)
			{
			// �o�^��񂪑��M����Ă�����
			case MSGTYPE_REGIST:
				pMsg0 = (MSG0*)g_recvbuf;
				pMsg1 = (MSG1*)g_sendbuf;

				for(i = 0; i < MAXPLAYER; i++)
				{
					// �N���C�A���g��Ԃ��������H
					if(g_Client[i].status == 0)
					{
						// �z��̋󂫈ʒu�ɃN���C�A���g����o�^
						// (�z��C���f�b�N�X���N���C�A���gID�ƂȂ�)
						g_Client[i].fromaddr = fromaddr;
						strcpy(g_Client[i].name, pMsg0->name);
						g_Client[i].status = 1;

						//##################################################
						// �����ʒu���T�[�o�[���Ń����_���Ɍ���
						//##################################################
						pMsg1->x = g_Client[i].x = rand() % SCREEN_WID;
						pMsg1->y = g_Client[i].y = rand() % SCREEN_HEI;
						pMsg1->id = i;
						pMsg1->type = MSGTYPE_CLIENTINFO;

						//##################################################
						// ���M���ɃN���C�A���g����ԐM
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

			// �ʒu��񂪑��M����Ă�����
			case MSGTYPE_POSITIONINFO:
				pMsg2 = (MSG2*)g_recvbuf;

				// �N���C�A���g��Ԃ��L�����H
				if(g_Client[pMsg2->id].status == 1)
				{
					//##################################################
					// ���M����Ă������W�ōX�V����
					// (�z��C���f�b�N�X���N���C�A���gID�ƂȂ�)
					//##################################################
					g_Client[pMsg2->id].x = pMsg2->x;
					g_Client[pMsg2->id].y = pMsg2->y;
					
					// �ʒu����S�N���C�A���g�ɑ��M
					for(int i = 0; i < MAXPLAYER; i++)
					{
						if(g_Client[i].status)
						{
							//##################################################
							// �L���ȑS�N���C�A���g�ɑ��M
							// (sendto)
							//##################################################
							sts = sendto(g_mysock, g_recvbuf, sizeof(g_recvbuf), 0, (sockaddr*)&g_Client[i].fromaddr, sizeof(g_Client[i].fromaddr));
													   
							// ������(���M�����ɏ��������邱��)
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


			// �폜��񂪑��M����Ă�����
			case MSGTYPE_ELIMINATE:
				pMsg3 = (MSG3*)g_recvbuf;

				//##################################################
				// �Y���N���C�A���g������
				// (�z��C���f�b�N�X���N���C�A���gID�ƂȂ�)
				//##################################################
				g_Client[pMsg3->id].status = 0;


				// �폜����S�N���C�A���g�ɑ��M
				for(int i = 0; i < MAXPLAYER; i++)
				{
					if(g_Client[i].status)
					{
						//##################################################
						// �L���ȑS�N���C�A���g�ɑ��M
						// (sendto)
						//##################################################
						sts = sendto(g_mysock, g_recvbuf, sizeof(g_recvbuf), 0, (sockaddr*)&g_Client[i].fromaddr, sizeof(g_Client[i].fromaddr));


						// ������(���M�����ɏ��������邱��)
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
			if(errcode != WSAEINTR)	// recvfrom�ɂ��u���b�L���O���L�����Z�����ꂽ�ꍇ������
			{
				errcom(errcode);
			}
			break;
		}		
	}
}

//------------------------------------
// UDP��������
//------------------------------------
void UdpInit()
{
	WSADATA		wd;							// WSAStartup�p
	int			sts;						// �߂�l
	int			errcode;					// �\�P�b�g�̃G���[�R�[�h
	WORD		requiredversion;			// 

	// ���̃v���O�������v������o�[�W����
	requiredversion = MAKEWORD(2, 0);

	// WinSock������
	sts = WSAStartup(MAKEWORD(2, 0), &wd);
	if(sts != 0)
	{
		errcode = WSAGetLastError();
		errcom(errcode);
		return;
	}
	// �o�[�W�����`�F�b�N
	if(wd.wVersion != requiredversion)
	{
		MessageBox(NULL,"VERSION ERROR!", "", MB_OK);
		return;
	}

	// �\�P�b�g�쐬(UDP�v���g�R���p�̃\�P�b�g�쐬)
	g_mysock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(g_mysock == INVALID_SOCKET)
	{
		errcode = WSAGetLastError();
		errcom(errcode);
		return;
	}

	// �����̃\�P�b�g��IP�A�h���X�A�|�[�g�ԍ������蓖�Ă�
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
	// �f�[�^����M���邽�߂̃X���b�h���J�n������
	// (_beginthread)
	//##################################################
	_beginthread(RecvThreadSub, 0, NULL);


}

//------------------------------------
// UDP�I������
//------------------------------------
void UdpTerm(){

	int		sts;
	int		errcode;					// �\�P�b�g�̃G���[�R�[�h

	// �\�P�b�g���N���[�Y
	sts = closesocket(g_mysock);
	if(sts != 0)
	{
		errcode = WSAGetLastError();
		errcom(errcode);
	}

	// WinSock�̌㏈��
	WSACleanup();
}
