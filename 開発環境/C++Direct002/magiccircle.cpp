//=============================================================================
// 
//  �e���� [magiccircle.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "magiccircle.h"
#include "calculation.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\mss02-3.png"
#define WIDTH			(200.0f)						// ����
#define HEIGHT			(200.0f)						// �c��

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
int CMagicCircle::m_nNumAll = 0;						// �e�̑���

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CMagicCircle::CMagicCircle(int nPriority) : CObject3D(nPriority)
{
	m_nTexIdx = 0;	// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_sizeDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ڕW�̃T�C�Y

	// �������Z
	m_nNumAll++;
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CMagicCircle::~CMagicCircle()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CMagicCircle *CMagicCircle::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CMagicCircle *pBullet = NULL;

	if (pBullet == NULL)
	{// NULL��������

		// �������̊m��
		pBullet = DEBUG_NEW CMagicCircle;

		//if (pBullet->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pBullet;
		//	return NULL;
		//}

		if (pBullet != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pBullet->Init();
		}

		return pBullet;
	}

	return NULL;
}

//==========================================================================
// ��������(�I�[�o�[���[�h)
//==========================================================================
CMagicCircle *CMagicCircle::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, D3DXVECTOR3 size)
{
	// �����p�̃I�u�W�F�N�g
	CMagicCircle *pBullet = NULL;

	if (pBullet == NULL)
	{// NULL��������

		// �������̊m��
		pBullet = DEBUG_NEW CMagicCircle;

		//if (pBullet->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pBullet;
		//	return NULL;
		//}

		if (pBullet != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �ʒu�ݒ�
			pBullet->SetPosition(pos);

			// �����ݒ�
			pBullet->SetRotation(rot);

			// �T�C�Y�ݒ�
			pBullet->m_sizeDest = size;

			// ����������
			pBullet->Init();
		}

		return pBullet;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CMagicCircle::Init(void)
{
	HRESULT hr;

	// ��ނ̐ݒ�
	SetType(TYPE_MAGIC_CIRCLE);

	// ����������
	hr = CObject3D::Init();

	// �e�N�X�`���̊��蓖��
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx);

	if (FAILED(hr))
	{// ���s�����Ƃ�

		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CMagicCircle::Uninit(void)
{
	// �I������
	CObject3D::Uninit();

	// �������Z
	m_nNumAll--;
}

//==========================================================================
// �X�V����
//==========================================================================
void CMagicCircle::Update(void)
{
	// �T�C�Y�擾
	D3DXVECTOR3 size = GetSize();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �����␳
	InertiaCorrection(size.x, m_sizeDest.x, 0.2f);
	InertiaCorrection(size.y, m_sizeDest.y, 0.2f);
	InertiaCorrection(size.z, m_sizeDest.z, 0.2f);

	// ��]
	rot.z += D3DX_PI * 0.1f;

	// �����ݒ�
	SetRotation(rot);

	// �T�C�Y�ݒ�
	SetSize(size);

	// ���_���ݒ�
	CObject3D::SetVtx();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CMagicCircle::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// �w�ʂ̃J�����O�Ȃ�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// �`�揈��
	CObject3D::Draw();

	// �A���t�@�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// �J�����O�f�t�H���g
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// ���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//==========================================================================
// �����擾
//==========================================================================
int CMagicCircle::GetNumAll(void)
{
	return m_nNumAll;
}
