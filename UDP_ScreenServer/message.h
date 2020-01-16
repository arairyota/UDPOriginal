#pragma	once

#define	NAMEMAX	(32)
#define MSGTYPE_REGIST			(0)		// クライアント登録
#define MSGTYPE_CLIENTINFO		(1)		// クライアント情報
#define	MSGTYPE_POSITIONINFO	(2)		// ポジション情報
#define	MSGTYPE_ELIMINATE		(3)		// クライアント削除

/*----------------------------------------------

	UDPスクリーンサーバーメッセージ定義ヘッダー

------------------------------------------------*/
// メッセージタイプ0(REGIST)
struct MSG0
{
	int		type;						// メッセージタイプ
	char	name[NAMEMAX];				// 名前
};

// メッセージタイプ1(CLIENTINFO)
struct MSG1
{
	int		type;						// メッセージタイプ
	short	id;							// クライアントID
	short	x;							// 現在座標X
	short	y;							// 現在座標Y
};

// メッセージタイプ2(POSITIONINFO)
struct MSG2
{
	int		type;						// メッセージタイプ
	char	name[NAMEMAX];				// 名前
	short	id;							// クライアントID
	short	x;							// 現在座標X
	short	y;							// 現在座標Y
};

// メッセージタイプ3(ELIMINATE)
struct MSG3
{
	int		type;						// メッセージタイプ
	int		id;							// クライアントID
};
