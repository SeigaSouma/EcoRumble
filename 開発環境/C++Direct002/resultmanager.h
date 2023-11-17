//=============================================================================
// 
//  ���U���g�}�l�[�W���w�b�_�[ [resultmanager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _RESULTMANAGER_H_
#define _RESULTMANAGER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "constans.h"

// �O���錾
class CEnemy;
class CBoss;

//==========================================================================
// �N���X��`
//==========================================================================
// ���U���g�}�l�[�W���N���X��`
class CResultManager
{
public:

	enum ADDTYPE
	{
		ADDTYPE_NONE = 0,
		ADDTYPE_NORMAL,
		ADDTYPE_BLUE,
		ADDTYPE_GOLD,
		ADDTYPE_MAX
	};

	CResultManager();
	~CResultManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Reset(void);	// ���Z�b�g

	int GetScoreTypeNum(ADDTYPE type);	// �X�R�A�̎�ޖ��̐��擾
	void AddScore(int nValue);			// �X�R�A���Z
	void AddScoreNum(ADDTYPE type);			// �X�R�A�̎�މ��Z
	int GetBaseScore(void);			// ���̃X�R�A�̎擾����
	int GetToatalScore(void);			// �ŏI�X�R�A�̎擾����
	static CResultManager *Create(void);	// ��������
	
private:
	int m_nScoreNum[ADDTYPE_MAX];	// �X�R�A�̎��
	int m_nBaseScore;	// ���̃X�R�A
	int m_nToatalScore;	// �ŏI�X�R�A
};



#endif