//=============================================================================
// 
//  �^�C�}�[�w�b�_�[ [timer.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TIMER_H_
#define _TIMER_H_	// ��d�C���N���[�h�h�~

#include "main.h"

//==========================================================================
// �O���錾
//==========================================================================
class CNumber;

//==========================================================================
// �N���X��`
//==========================================================================
// �w�i�N���X��`
class CTimer
{
public:

	CTimer(int nPriority = 6);
	~CTimer();

	static HRESULT Load(void);
	static void Unload(void);
	static CTimer *Create(void);
	static CTimer *Create(D3DXVECTOR3 pos);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Add(int nValue);

private:
	//==============================
	// �}�N����`
	//=============================
#define NUM_TIMER	(4)			// ����
#define MAX_TIME	(60 * 99)	// �^�C�}�[�̍ő吔
#define START_TIME	(60 * 3)	// �^�C�}�[�̏����l

	void SetValue(void);

	int m_nNum;							// �l
	int m_nCntTime;						// ���Ԃ̃J�E���g
	CNumber *m_apNumber[NUM_TIMER];		// �����̃I�u�W�F�N�g
	int m_nTexIdx;						// �e�N�X�`���̃C���f�b�N�X�ԍ�
};



#endif