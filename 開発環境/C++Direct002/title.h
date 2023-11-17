//=============================================================================
// 
//  �^�C�g���w�b�_�[ [title.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TITLE_H_
#define _TITLE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "manager.h"

//==========================================================================
// �O���錾
//==========================================================================

//==========================================================================
// �N���X��`
//==========================================================================
// �^�C�g���N���X��`
class CTitle : public CScene
{
public:

	CTitle();
	~CTitle();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	int GetSwitchWave(void);	// �E�F�[�u�J�ڎ擾
	void SetStageSwitch(void);	// �X�e�[�W�̐؂�ւ��ݒ�

private:
	int m_nCntSwitch;		// �؂�ւ��̃J�E���^�[
	int m_nSwitchWave;		// �E�F�[�u�̑J��
	bool m_bStageSwitch;	// �X�e�[�W�؂�ւ��̃t���O

};



#endif