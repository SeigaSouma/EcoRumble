//=============================================================================
// 
//  �X�R�A�w�b�_�[ [score.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SCORE_H_
#define _SCORE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "resultmanager.h"

//==========================================================================
// �O���錾
//==========================================================================
class CNumber;

//==========================================================================
// �N���X��`
//==========================================================================
// �w�i�N���X��`
class CScore
{
public:

	CScore(int nPriority = 6);
	~CScore();

	static HRESULT Load(void);
	static void Unload(void);
	static CScore *Create(void);
	static CScore *Create(D3DXVECTOR3 pos);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Add(int nValue);
	void Add(CResultManager::ADDTYPE AddType);

private:
	//==============================
	// �}�N����`
	//=============================
#define NUM_SCORE	(6)	// ����

	void SetValue(void);

	int m_nNum;								// �l
	CNumber *m_apNumber[NUM_SCORE];			// �����̃I�u�W�F�N�g
	int m_nTexIdx;					// �e�N�X�`���̃C���f�b�N�X�ԍ�
};



#endif