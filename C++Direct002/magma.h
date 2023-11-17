//=============================================================================
// 
//  �}�O�}�w�b�_�[ [magma.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _MAGMA_H_
#define _MAGMA_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �}�O�}�N���X��`
class CMagma : public CObject3DMesh
{
public:

	CMagma(int nPriority = 2);
	~CMagma();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	bool IsHit(D3DXVECTOR3& pos);	// ����������

	static CMagma *Create(D3DXVECTOR3 pos, float fWidthLen, int nBlock);
	CMagma *GetMagma(void);
protected:

private:

	int m_nTexIdx;	// �e�N�X�`���̃C���f�b�N�X�ԍ�
};



#endif