//=============================================================================
//
// �u���b�N���� [block.h]
// Author : ���n�Ή�
//
//=============================================================================

#ifndef _BLOCK_H_		//���̃}�N����`������Ă��Ȃ�������
#define _BLOCK_H_		//��d�C���N���[�h�h�~�̃}�N�����`����

#include "main.h"
#include "object2D.h"

//==========================================================================
// �O���錾
//==========================================================================

//==========================================================================
// �N���X��`
//==========================================================================
// �u���b�N�N���X��`
class CBlock : public CObject2D
{
public:

	CBlock(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CBlock();

	static HRESULT Load(void);
	static void Unload(void);
	static CBlock *Create(void);
	static CBlock *Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 size, const int nWidth = 1, const int nHeight = 1);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR2 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

private:
	int m_nWidth;	// ��������
	int m_nHeight;	// �c������
	int m_nTexIdx;	// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nAlphaCnt;	// �����x�̃J�E���g
};

#endif