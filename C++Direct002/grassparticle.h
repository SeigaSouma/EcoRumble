//=============================================================================
// 
//  ���̃p�[�e�B�N���w�b�_�[ [grassparticle.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _GRASSPARTICLE_H_
#define _GRASSPARTICLE_H_		// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �O���錾
//==========================================================================
class CObject3DMesh;

//==========================================================================
// �}�N����`
//==========================================================================
#define MAX_GRASSPARTICLE	(64)	// ���I�̐�

//==========================================================================
// �N���X��`
//==========================================================================
// ���̃p�[�e�B�N���N���X��`
class CGrassParticle : public CObject
{
public:

	CGrassParticle(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CGrassParticle();

	static CGrassParticle *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 move, int nNum);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Release(int nIdx);
private:

	D3DXVECTOR3 m_posOrigin;		// ���_
	D3DXVECTOR2 m_moveOrigin;		// ���̈ړ���
	D3DXVECTOR3 m_rotOrigin;		// ���̌���
	D3DXVECTOR3 m_move[MAX_GRASSPARTICLE];		// �ړ���
	D3DXVECTOR3 m_rotMove[MAX_GRASSPARTICLE];		// ��]�ړ���
	TYPE m_type;					// ���
	int m_nNumAll;					// ����
	int m_nLife[MAX_GRASSPARTICLE];					// ����
	int m_nMaxLife[MAX_GRASSPARTICLE];					// �ő����
	CObject3DMesh *m_pObjMesh[MAX_GRASSPARTICLE];	// 3D���b�V���̃|�C���^
	static int m_nTexIdx;						// �e�N�X�`���̃C���f�b�N�X�ԍ�
};


#endif