//=============================================================================
// 
//  �`���[�g���A���w�b�_�[ [tutorial.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "manager.h"

//==========================================================================
// �O���錾
//==========================================================================
class CTutorialStep;
class CTutorialText;

//==========================================================================
// �N���X��`
//==========================================================================
// �`���[�g���A���N���X��`
class CTutorial : public CScene
{
public:

	CTutorial();
	~CTutorial();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	CPlayer *GetPlayer(void) override;
	CPlayer **GetPlayer(int n) override;

	static bool IsMovingPlayer(void);		// �v���C���[�������Ă锻��擾
	static void SetEnableMovingPlayer(void);		// �v���C���[�������Ă锻��
	static void SetDesableMovingPlayer(void);		// �v���C���[�������Ă锻��OFF
	static CTutorialStep *GetStep(void);	// �X�e�b�v�擾
private:

	CPlayer *m_pPlayer;		// �v���C���[�̃I�u�W�F�N�g
	static bool m_bMovingPlayer;	// �v���C���[�������Ă锻��
	static CTutorialStep *m_pStep;	// �X�e�b�v
};



#endif