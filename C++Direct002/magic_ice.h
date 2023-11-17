//=============================================================================
// 
//  �X���@�w�b�_�[ [magic_ice.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _MAGIC_ICE_H_
#define _MAGIC_ICE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �O���錾
//==========================================================================
class CObjectX;

//==========================================================================
// �}�N����`
//==========================================================================
#define MAX_MAGICICE	(320)	// �X���@�̐�

//==========================================================================
// �N���X��`
//==========================================================================
// �X���@�N���X��`
class CMagicIce : public CObject
{
public:
	CMagicIce(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CMagicIce();

	static HRESULT Load(void);
	static CMagicIce *Create(D3DXVECTOR3 pos);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

	bool Set(void);	// �ݒ菈��
	void Collision(void);	// �����蔻��

	D3DXVECTOR3 m_posOrigin;			// ���_
	int m_nNumAll;						// ����
	int m_nIdxBig;						// ����Ȃ�̃C���f�b�N�X
	int m_nLife;						// ����
	static const char *m_pModelFile;	// ���f���t�@�C��
	int m_nWait[MAX_MAGICICE];			// �ҋ@����
	int m_nDirection[MAX_MAGICICE];		// ����
	CObjectX *m_pObjX[MAX_MAGICICE];	// �I�u�W�F�N�gX�̃|�C���^
	static int m_nIdxXFile;				// X�t�@�C���̃C���f�b�N�X�ԍ�
};


#endif