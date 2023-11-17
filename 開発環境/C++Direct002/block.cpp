//=============================================================================
// 
// �u���b�N���� [explosion.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "main.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "block.h"
#include "collision.h"
#include "calculation.h"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CBlock::CBlock(int nPriority) : CObject2D(nPriority)
{
	m_nWidth = 0;		// ��������
	m_nHeight = 0;		// �c������
	m_nTexIdx = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_nAlphaCnt = 0;	// �����x�̃J�E���g
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CBlock::~CBlock()
{

}

//==========================================================================
// ��������
//==========================================================================
CBlock *CBlock::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CBlock *pBlock = NULL;

	if (pBlock == NULL)
	{// NULL��������

		// �������̊m��
		pBlock = DEBUG_NEW CBlock;

		//if (pBlock->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pBlock;
		//	return NULL;
		//}

		if (pBlock != NULL)
		{// �������̊m�ۂ��o���Ă�����

			pBlock->m_nWidth = 1;	// ��������
			pBlock->m_nHeight = 1;	// �c������

			// ����������
			pBlock->Init();
		}

		return pBlock;
	}

	return NULL;
}

//==========================================================================
// ��������(�I�[�o�[���[�h)
//==========================================================================
CBlock *CBlock::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 size, const int nWidth, const int nHeight)
{
	// �����p�̃I�u�W�F�N�g
	CBlock *pBlock = NULL;

	if (pBlock == NULL)
	{// NULL��������

		// �������̊m��
		pBlock = DEBUG_NEW CBlock;

		//if (pBlock->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pBlock;
		//	return NULL;
		//}

		if (pBlock != NULL)
		{// �������̊m�ۂ��o���Ă�����

			pBlock->m_nWidth = nWidth;		// ��������
			pBlock->m_nHeight = nHeight;	// �c������

			// ����������
			pBlock->Init(pos, size);
		}

		return pBlock;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CBlock::Init(void)
{
	HRESULT hr;

	// ��ނ̐ݒ�
	SetType(TYPE_BLOCK);

	// �e�N�X�`���̊��蓖��
	m_nTexIdx = CManager::GetTexture()->Regist("data\\TEXTURE\\block000.jpg");

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx);

	// ����������
	hr = CObject2D::Init();

	if (FAILED(hr))
	{// ���s�����Ƃ�

		return E_FAIL;
	}

	// ���_���ւ̃|�C���^
	VERTEX_2D *pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f + 1.0f * m_nHeight);
	pVtx[3].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 1.0f + 1.0f * m_nHeight);

	// ���_�o�b�t�@���A�����b�N���b�N
	GetVtxBuff()->Unlock();

	return S_OK;
}

//==========================================================================
// ����������(�I�[�o�[���[�h)
//==========================================================================
HRESULT CBlock::Init(const D3DXVECTOR3 pos, const D3DXVECTOR2 size)
{
	HRESULT hr;

	// ��ނ̐ݒ�
	SetType(TYPE_BLOCK);

	// �e�N�X�`���̊��蓖��
	m_nTexIdx = CManager::GetTexture()->Regist("data\\TEXTURE\\block000.jpg");

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx);

	// �ʒu�ݒ�
	SetPosition(pos);

	D3DXVECTOR2 sizeBlock = size;

	// ���������L����
	sizeBlock.x *= m_nWidth;
	sizeBlock.y *= m_nHeight;

	// �T�C�Y�ݒ�
	SetSize(sizeBlock);

	// ����������
	hr = CObject2D::Init();

	if (FAILED(hr))
	{// ���s�����Ƃ�

		return E_FAIL;
	}

	// ���_���ւ̃|�C���^
	VERTEX_2D *pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f + 1.0f * m_nHeight);
	pVtx[3].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 1.0f + 1.0f * m_nHeight);

	// ���_�o�b�t�@���A�����b�N���b�N
	GetVtxBuff()->Unlock();

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CBlock::Uninit(void)
{
	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CBlock::Update(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �T�C�Y�擾
	D3DXVECTOR2 size = GetSize();

	// �ߋ��̈ʒu�ݒ�
	SetOldPosition(pos);

	// �F�̎擾
	D3DXCOLOR col = GetColor();

	// �񎟋Ȑ��␳
	CuadricCurveComp(col.a, 100, 0.0f, 1.0f, m_nAlphaCnt);

	// �F�ݒ�
	SetColor(col);

	// ���_���W�X�V
	SetVtx();

	// �u���b�N�Ƃ̓����蔻��
	//if (collision::Collision(pos.x, pos.x + size.x, pos.y, pos.y + size.y, CObject::TYPE_BULLET) == true)
	//{// �������Ă�����

	//	int nnn = 0;
	//}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CBlock::Draw(void)
{
	CObject2D::Draw();
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CBlock::SetVtx(void)
{
	// ���_�ݒ�
	CObject2D::SetVtx();

	// ���_���ւ̃|�C���^
	VERTEX_2D *pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// �ʒu�擾
	D3DXVECTOR3 posBlock = GetPosition();

	// �T�C�Y�擾
	D3DXVECTOR2 sizeBlock = GetSize();

	//���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(posBlock.x, posBlock.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(posBlock.x + sizeBlock.x, posBlock.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(posBlock.x, posBlock.y + sizeBlock.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(posBlock.x + sizeBlock.x, posBlock.y + sizeBlock.y, 0.0f);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f + 1.0f * m_nHeight);
	pVtx[3].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 1.0f + 1.0f * m_nHeight);

	// ���_�o�b�t�@���A�����b�N���b�N
	GetVtxBuff()->Unlock();
}

