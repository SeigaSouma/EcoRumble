//=============================================================================
// 
//  �A�C�e������ [bullet.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "item.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "explosion.h"
#include "sound.h"
#include "particle.h"
#include "score.h"
#include "collision.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define WIDTH	(30.0f)		// ����
#define HEIGHT	(30.0f)		// �c��

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
const char *CItem::m_apTextureFile[] =					// �e�N�X�`���̃t�@�C��
{
	NULL,
	"data\\TEXTURE\\bullet_01.png",
};
int CItem::m_nNumAll = 0;		// �A�C�e���̑���

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CItem::CItem(int nPriority) : CObject2D(nPriority)
{
	// �e�N�X�`���f�[�^�̔z�񕪌J��Ԃ�
	for (int nCntTex = 0; nCntTex < sizeof(m_apTextureFile) / sizeof(*m_apTextureFile); nCntTex++)
	{
		m_nTexIdx[nCntTex] = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
	}

	// �l�̃N���A
	m_type = TYPE_NONE;

	// �������Z
	m_nNumAll++;
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CItem::CItem(TYPE type, int nPriority) : CObject2D(nPriority)
{

	// �l�̃N���A
	m_type = type;

	// �������Z
	m_nNumAll++;
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CItem::~CItem()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CItem *CItem::Create(TYPE type)
{
	// �����p�̃I�u�W�F�N�g
	CItem *pItem = NULL;

	if (pItem == NULL)
	{// NULL��������

		// �������̊m��
		pItem = DEBUG_NEW CItem;

		//if (pItem->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pItem;
		//	return NULL;
		//}

		if (pItem != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ��ސݒ�
			pItem->m_type = type;

			// ����������
			pItem->Init();
		}

		return pItem;
	}

	return NULL;
}

//==========================================================================
// ��������(�I�[�o�[���[�h)
//==========================================================================
CItem *CItem::Create(TYPE type, const D3DXVECTOR3 pos)
{
	// �����p�̃I�u�W�F�N�g
	CItem *pItem = NULL;

	if (pItem == NULL)
	{// NULL��������

		// �������̊m��
		pItem = DEBUG_NEW CItem;

		//if (pItem->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pItem;
		//	return NULL;
		//}

		if (pItem != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ��ސݒ�
			pItem->m_type = type;

			// �ʒu���蓖��
			pItem->SetPosition(pos);

			// ����������
			pItem->Init();
		}

		return pItem;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CItem::Init(void)
{
	HRESULT hr;

	// �e��ϐ��̏�����
 	SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// �T�C�Y

	// ��ނ̐ݒ�
	SetType(CObject::TYPE_ITEM);

	// ����������
	hr = CObject2D::Init();

	// �e�N�X�`���f�[�^�̔z�񕪌J��Ԃ�
	for (int nCntTex = 0; nCntTex < sizeof(m_apTextureFile) / sizeof(*m_apTextureFile); nCntTex++)
	{
		// �e�N�X�`���̊��蓖��
		m_nTexIdx[nCntTex] = CManager::GetTexture()->Regist(m_apTextureFile[nCntTex]);

		// �e�N�X�`���̊��蓖��
		BindTexture(m_nTexIdx[nCntTex]);
	}

	if (FAILED(hr))
	{// ���s�����Ƃ�

		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CItem::Uninit(void)
{
	// �I������
	CObject2D::Uninit();

	// �������Z
	m_nNumAll--;
}

//==========================================================================
// �X�V����
//==========================================================================
void CItem::Update(void)
{
	// ���_���ݒ�
	SetVtx();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CItem::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	CObject2D::Draw();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CItem::SetVtx(void)
{
	// ���_�ݒ�
	CObject2D::SetVtx();
}

//==========================================================================
// �����擾
//==========================================================================
int CItem::GetNumAll(void)
{
	return m_nNumAll;
}
