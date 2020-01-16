#pragma	once

#define	NAMEMAX	(32)
#define MSGTYPE_REGIST			(0)		// �N���C�A���g�o�^
#define MSGTYPE_CLIENTINFO		(1)		// �N���C�A���g���
#define	MSGTYPE_POSITIONINFO	(2)		// �|�W�V�������
#define	MSGTYPE_ELIMINATE		(3)		// �N���C�A���g�폜

/*----------------------------------------------

	UDP�X�N���[���T�[�o�[���b�Z�[�W��`�w�b�_�[

------------------------------------------------*/
// ���b�Z�[�W�^�C�v0(REGIST)
struct MSG0
{
	int		type;						// ���b�Z�[�W�^�C�v
	char	name[NAMEMAX];				// ���O
};

// ���b�Z�[�W�^�C�v1(CLIENTINFO)
struct MSG1
{
	int		type;						// ���b�Z�[�W�^�C�v
	short	id;							// �N���C�A���gID
	short	x;							// ���ݍ��WX
	short	y;							// ���ݍ��WY
};

// ���b�Z�[�W�^�C�v2(POSITIONINFO)
struct MSG2
{
	int		type;						// ���b�Z�[�W�^�C�v
	char	name[NAMEMAX];				// ���O
	short	id;							// �N���C�A���gID
	short	x;							// ���ݍ��WX
	short	y;							// ���ݍ��WY
};

// ���b�Z�[�W�^�C�v3(ELIMINATE)
struct MSG3
{
	int		type;						// ���b�Z�[�W�^�C�v
	int		id;							// �N���C�A���gID
};
