//=============================================================================
// 
//  �_���[�W�Q�[�g�w�b�_�[ [damage_gate.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _DAMAGE_GATE_H_
#define _DAMAGE_GATE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �O���錾
//==========================================================================
class CObjectX;

//==========================================================================
// �}�N����`
//==========================================================================
#define MAX_DAMAGEGATE	(6)	// �X���@�̐�

//==========================================================================
// �N���X��`
//==========================================================================
// �X���@�N���X��`
class CDamageGate : public CObject
{
public:
	CDamageGate(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CDamageGate();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static HRESULT Load(void);
	static CDamageGate *Create(void);
private:

	void Collision(int nCntGate);	// �����蔻��

	D3DXVECTOR3 m_posOrigin;			// ���_
	D3DXVECTOR3 m_rotOrigin;			// ����
	float m_fLength;					// ���_����̋���
	CObjectX *m_pObjX[MAX_DAMAGEGATE];	// �I�u�W�F�N�gX�̃|�C���^
	static int m_nIdxXFile;				// X�t�@�C���̃C���f�b�N�X�ԍ�
};


#endif