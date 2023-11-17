//=============================================================================
// 
//  �������w�b�_�[ [explosion_sphere.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _EXPLOSION_SPHERE_H_
#define _EXPLOSION_SPHERE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �N���N���X��`
class CExplosionSphere : public CObject3DMesh
{
public:
	CExplosionSphere(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CExplosionSphere();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static CExplosionSphere *Create(D3DXVECTOR3 pos, float fSizeDest);

private:
	void Collision(void);	// �����蔻��

	int m_nTexIdx;	// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nLife;	// ����
	float m_fSizeDest;	// �ڕW�̃T�C�Y
};



#endif