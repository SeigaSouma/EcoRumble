//=============================================================================
// 
//  ���U���g�X�R�A�w�b�_�[ [resultscore.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _RESULTSCORE_H_
#define _RESULTSCORE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define RESULTSCORE_DIGIT	(6)	// ����

//==========================================================================
// �O���錾
//==========================================================================
class CNumber;

//==========================================================================
// �N���X��`
//==========================================================================
// �w�i�N���X��`
class CResultScore : public CObject
{
public:

	CResultScore(int nPriority = 7);
	~CResultScore();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetAllArrival(void);	// �S�Ă̓����ݒ�
	void SetValue(int nScore);	// �l�̐ݒ菈��
	static CResultScore *Create(void);
	bool IsSetBase(void) { return m_bSetBase; }
	bool IsSetToatal(void) { return m_bSetToatal; }
private:
	
	void CreateBaseScore(void);		// ���̃X�R�A�̐���
	void CreateToatalScore(void);	// �ŏI�X�R�A�̐���
	void SetBaseValue(void);		// ���̃X�R�A�̒l�ݒ菈��
	void SetToatalValue(void);		// �ŏI�X�R�A�̒l�ݒ菈��
	void MoveBaseScore(void);		// ���X�R�A�̈ړ�����
	void MoveToatalScore(void);		// �ŏI�X�R�A�̈ړ�����

	int m_nBaseNum;					// ���̃X�R�A
	int m_nBaseNumDest;				// ���̃X�R�A�̖ڕW�l
	int m_nToatalNum;				// �l
	int m_nToatalNumDest;			// �ڕW�̒l
	int m_nTexIdx;					// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nTexIdxNumber;			// �����e�N�X�`���̃C���f�b�N�X�ԍ�
	float m_fBasePosDest_X;			// ���X�R�A�̖ڕW�ʒu
	float m_fToatalPosDest_X;		// �ŏI�X�R�A�̖ڕW�ʒu
	bool m_bArrivalBase;			// ���̃X�R�A�̓�������
	bool m_bSetBase;				// ���̃X�R�A�̐ݒ蔻��
	bool m_bArrivalToatal;			// �ŏI�X�R�A�̓�������
	bool m_bSetToatal;				// �ŏI�X�R�A�̐ݒ蔻��
	CObject2D *m_pBaseObj2D;		// ���̃X�R�A2D�̃I�u�W�F�N�g
	CNumber *m_pBaseScore[RESULTSCORE_DIGIT];		// ���̃X�R�A
	CObject2D *m_pToatalObj2D;		// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
	CNumber *m_pToatalScore[RESULTSCORE_DIGIT];			// �����̃I�u�W�F�N�g
};



#endif