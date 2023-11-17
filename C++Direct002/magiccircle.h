//=============================================================================
// 
//  ���@�w�w�b�_�[ [magiccircle.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _MAGICCIRCLE_H_
#define _MAGICCIRCLE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object3D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �e�N���X��`
class CMagicCircle : public CObject3D
{
public:

	CMagicCircle(int nPriority = 5);
	~CMagicCircle();

	static CMagicCircle *Create(void);
	static CMagicCircle *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, D3DXVECTOR3 size);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static int GetNumAll(void);

private:
	D3DXVECTOR3 m_sizeDest;		// �ڕW�̃T�C�Y
	static int m_nNumAll;		// ����
	int m_nTexIdx;				// �e�N�X�`���̃C���f�b�N�X�ԍ�
};


#endif