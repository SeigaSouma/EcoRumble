//=============================================================================
// 
//  ���U���g�v���C���[�w�b�_�[ [resultplayer.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _RESULTPLAYER_H_
#define _RESULTPLAYER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "player.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ���U���g�v���C���[�N���X��`
class CResultPlayer : public CPlayer
{
public:

	CResultPlayer(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CResultPlayer();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Update(void) override;

	void UninitByMode(void);
private:

	// �I�[�o�[���C�h���ꂽ�֐�
	void Controll(void) override;	// ����

	// �����o�֐�
	void UpdateByStep(void);	// �X�e�b�v���Ƃ̍X�V����
	
};



#endif