//=============================================================================
// 
//  �������b�V���w�b�_�[ [speed_rush.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SPEEDRUSH_H_
#define _SPEEDRUSH_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �O���錾
//==========================================================================
class CObjectX;

//==========================================================================
// �}�N����`
//==========================================================================
#define MAX_SPEEDRUSH	(64)	// �������b�V���̐�

//==========================================================================
// �N���X��`
//==========================================================================
// �������b�V���N���X��`
class CSpeedRush : public CObject
{
public:
	CSpeedRush(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CSpeedRush();

	static HRESULT Load(void);
	static CSpeedRush *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nNum);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

	bool Set(void);	// �ݒ菈��
	void Collision(void);	// �����蔻��

	D3DXVECTOR3 m_posOrigin;			// ���_
	D3DXVECTOR3 m_rotOrigin;			// ������
	int m_nNumAll;						// ����
	int m_nLife;						// ����
	static const char *m_pModelFile;	// ���f���t�@�C��
	CObjectX *m_pObjX[MAX_SPEEDRUSH];	// �I�u�W�F�N�gX�̃|�C���^
	int m_nObjLife[MAX_SPEEDRUSH];		// ����
	static int m_nIdxXFile;				// X�t�@�C���̃C���f�b�N�X�ԍ�
};


#endif