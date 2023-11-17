//=============================================================================
// 
//  �r���{�[�h���� [objectBillboard.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "objectBillboard.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define POLYGON_TOP		(4)		// �|���S���̒��_��

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CObjectBillboard::CObjectBillboard(int nPriority) : CObject(nPriority)
{
	D3DXMatrixIdentity(&m_mtxWorld);				// ���[���h�}�g���b�N�X
	SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));		// �ʒu
	SetOldPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));	// �O��̈ʒu
	SetMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f));			// �ړ���
	SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));		// ����
	SetSize(D3DXVECTOR2(0.0f, 0.0f));				// �T�C�Y
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// �F
	m_nTexIdx = 0;	// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_pVtxBuff = NULL;		// ���_�o�b�t�@

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CObjectBillboard::~CObjectBillboard()
{

}

//==========================================================================
// �e�N�X�`���̊��蓖��
//==========================================================================
void CObjectBillboard::BindTexture(int nIdx)
{
	// ���蓖�Ă�
	m_nTexIdx = nIdx;
}


//==========================================================================
// ��������
//==========================================================================
CObjectBillboard *CObjectBillboard::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CObjectBillboard *pObjBillBoard = NULL;

	if (pObjBillBoard == NULL)
	{// NULL��������

		// �������̊m��
		pObjBillBoard = DEBUG_NEW CObjectBillboard;

		//if (pObjBillBoard->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pObjBillBoard;
		//	return NULL;
		//}

		if (pObjBillBoard != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pObjBillBoard->Init();
		}

		return pObjBillBoard;
	}

	return NULL;
}

//==========================================================================
// ��������
//==========================================================================
CObjectBillboard *CObjectBillboard::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// �����p�̃I�u�W�F�N�g
	CObjectBillboard *pObjBillBoard = NULL;

	if (pObjBillBoard == NULL)
	{// NULL��������

		// �������̊m��
		pObjBillBoard = DEBUG_NEW CObjectBillboard;

		//if (pObjBillBoard->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pObjBillBoard;
		//	return NULL;
		//}

		if (pObjBillBoard != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pObjBillBoard->Init();

			// �ʒu�E����
			pObjBillBoard->SetPosition(pos);
			pObjBillBoard->SetRotation(rot);
		}

		return pObjBillBoard;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CObjectBillboard::Init(void)
{
	HRESULT hr;

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���_�o�b�t�@�̐���
	if (m_pVtxBuff != NULL)
	{// ���ɏ�񂪓����Ă�ꍇ

		return E_FAIL;
	}

	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * POLYGON_TOP,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr))
	{// ���s�����Ƃ�

		return E_FAIL;
	}

	// ���_���ݒ�
	SetVtx();

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CObjectBillboard::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// �I�u�W�F�N�g�̔j��
	Release();

}

//==========================================================================
// �X�V����
//==========================================================================
void CObjectBillboard::Update(void)
{
	// ���_���ݒ�
	SetVtx();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CObjectBillboard::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �v�Z�p�}�g���b�N�X�錾
	D3DXMATRIX mtxRot, mtxTrans;

	// �r���[�}�g���b�N�X�擾�p
	D3DXMATRIX mtxView;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �r���[�}�g���b�N�X���擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// �|���S�����J�����ɑ΂��Đ��ʂɌ�����
	D3DXMatrixInverse(&m_mtxWorld, NULL, &mtxView);	// �t�z������߂�

#if 0

	// X���ɉ����Ă�
	m_mtxWorld._12 = 0.0f;
	m_mtxWorld._13 = 0.0f;
	m_mtxWorld._14 = 0.0f;

#else

	// ��ŏ�����
	/*m_mtxWorld._21 = 0.0f;
	m_mtxWorld._23 = 0.0f;
	m_mtxWorld._24 = 0.0f;*/

#endif

	// �ʏ�
	m_mtxWorld._41 = 0.0f;
	m_mtxWorld._42 = 0.0f;
	m_mtxWorld._43 = 0.0f;

	// �ʒu�𔽉f����
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, CManager::GetTexture()->GetAdress(m_nTexIdx));

	// �r���{�[�h�̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CObjectBillboard::SetVtx(void)
{

	// ���_���ւ̃|�C���^
	VERTEX_3D *pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	D3DXVECTOR2 size = GetSize();
	D3DXCOLOR col = GetColor();

	// �ʒu���X�V
	pVtx[0].pos = D3DXVECTOR3(-size.x, size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(size.x, size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-size.x, -size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(size.x, -size.y, 0.0f);

	// �@���x�N�g���̐ݒ�
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// ���_�J���[�̐ݒ�
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�o�b�t�@���A�����b�N���b�N
	m_pVtxBuff->Unlock();
}

//==========================================================================
// �}�g���b�N�X�ݒ�
//==========================================================================
void CObjectBillboard::SetWorldMtx(const D3DXMATRIX mtx)
{
	m_mtxWorld = mtx;
}

//==========================================================================
// �}�g���b�N�X�擾
//==========================================================================
D3DXMATRIX CObjectBillboard::GetWorldMtx(void) const
{
	return m_mtxWorld;
}

//==========================================================================
// �ʒu�ݒ�
//==========================================================================
void CObjectBillboard::SetPosition(const D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//==========================================================================
// �ʒu�擾
//==========================================================================
D3DXVECTOR3 CObjectBillboard::GetPosition(void) const
{
	return m_pos;
}

//==========================================================================
// �ʒu�ݒ�
//==========================================================================
void CObjectBillboard::SetOldPosition(const D3DXVECTOR3 posOld)
{
	m_posOld = posOld;
}

//==========================================================================
// �ʒu�擾
//==========================================================================
D3DXVECTOR3 CObjectBillboard::GetOldPosition(void) const
{
	return m_posOld;
}

//==========================================================================
// �ړ��ʐݒ�
//==========================================================================
void CObjectBillboard::SetMove(const D3DXVECTOR3 move)
{
	m_move = move;
}

//==========================================================================
// �ړ��ʎ擾
//==========================================================================
D3DXVECTOR3 CObjectBillboard::GetMove(void) const
{
	return m_move;
}

//==========================================================================
// �����ݒ�
//==========================================================================
void CObjectBillboard::SetRotation(const D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//==========================================================================
// �����擾
//==========================================================================
D3DXVECTOR3 CObjectBillboard::GetRotation(void) const
{
	return m_rot;
}

//==========================================================================
// �F�ݒ�
//==========================================================================
void CObjectBillboard::SetColor(const D3DXCOLOR col)
{
	m_col = col;
}

//==========================================================================
// �F�擾
//==========================================================================
D3DXCOLOR CObjectBillboard::GetColor(void) const
{
	return m_col;
}

//==========================================================================
// �T�C�Y�ݒ�
//==========================================================================
void CObjectBillboard::SetSize(const D3DXVECTOR2 size)
{
	m_fSize = size;
}

//==========================================================================
// �T�C�Y�擾
//==========================================================================
D3DXVECTOR2 CObjectBillboard::GetSize(void) const
{
	return m_fSize;
}

//==========================================================================
// ���_�o�b�t�@�̎擾
//==========================================================================
LPDIRECT3DVERTEXBUFFER9 CObjectBillboard::GetVtxBuff(void) const
{
	return m_pVtxBuff;
}

//==========================================================================
// �r���{�[�h�I�u�W�F�N�g�̎擾
//==========================================================================
CObjectBillboard *CObjectBillboard::GetObjectBillBoard(void)
{
	return this;
}
