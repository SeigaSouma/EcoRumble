//=============================================================================
// 
//  �����G�t�F�N�g���� [confusion_effect.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "confusion_effect.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "calculation.h"
#include "objectBillboard.h"
#include "texture.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define LENGTH		(2500.0f)			// ���_����̋���
#define ROT_MOVE	(D3DX_PI * 0.01f)	// ��]�ړ���
#define TEXTURE		"data\\TEXTURE\\confusion_01.png"
#define WIDTH		(40.0f)
#define HEIGHT		(20.0f)

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
int CConfusionEffect::m_nTexIdx = 0;	// �e�N�X�`���̃C���f�b�N�X�ԍ�

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CConfusionEffect::CConfusionEffect(int nPriority) : CObject(nPriority)
{
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���_
	m_rotOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ����
	m_fDistance = 0.0f;								// ���_����̋���
	m_nLife = 0;									// �̗�
	memset(&m_pObjBillboard[0], NULL, sizeof(m_pObjBillboard));				// �r���{�[�h�̃|�C���^
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CConfusionEffect::~CConfusionEffect()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CConfusionEffect *CConfusionEffect::Create(D3DXVECTOR3 pos, float fDistance, int nLife)
{
	// �����p�̃I�u�W�F�N�g
	CConfusionEffect *pBallast = NULL;

	if (pBallast == NULL)
	{// NULL��������

		// �������̊m��
		pBallast = DEBUG_NEW CConfusionEffect;

		if (pBallast != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �������
			pBallast->m_fDistance = fDistance;	// �Ԋu
			pBallast->m_posOrigin = pos;		// ���_
			pBallast->SetPosition(pos);			// ���_
			pBallast->m_nLife = nLife;			// �̗�

			// ����������
			pBallast->Init();
		}
		else
		{
			return NULL;
		}

		return pBallast;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CConfusionEffect::Init(void)
{

	// �e�N�X�`���̊��蓖��
	if (m_nTexIdx == 0)
	{
		m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);
	}

	// ��ނ̐ݒ�
	SetType(TYPE_CONFUSIONEFFECT);

	float fRot = (D3DX_PI * 2.0f) / (float)MAX_CONFUSION;

	for (int nCntGate = 0; nCntGate < MAX_CONFUSION; nCntGate++)
	{// ���ꂼ��̍X�V

		// ��������
		m_pObjBillboard[nCntGate] = CObjectBillboard::Create(
			D3DXVECTOR3(
				m_posOrigin.x + sinf(fRot * nCntGate) * m_fDistance,
				m_posOrigin.y,
				m_posOrigin.z + cosf(fRot * nCntGate) * m_fDistance),
			D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		if (m_pObjBillboard[nCntGate] == NULL)
		{// ���s�����Ƃ�

			delete m_pObjBillboard[nCntGate];
			m_pObjBillboard[nCntGate] = NULL;
			return false;
		}

		// �e�N�X�`���̊��蓖��
		m_pObjBillboard[nCntGate]->BindTexture(m_nTexIdx);

		// �T�C�Y�ݒ�
		m_pObjBillboard[nCntGate]->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CConfusionEffect::Uninit(void)
{
	for (int nCntGate = 0; nCntGate < MAX_CONFUSION; nCntGate++)
	{
		if (m_pObjBillboard[nCntGate] != NULL)
		{// NULL����Ȃ�������

			// �r���{�[�h�̏I������
			m_pObjBillboard[nCntGate]->Uninit();
			m_pObjBillboard[nCntGate] = NULL;
		}
	}

	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CConfusionEffect::Update(void)
{
	// 1������̊p�x
	float fRot = (D3DX_PI * 2.0f) / (float)MAX_CONFUSION;

	// ��]������
	m_rotOrigin.y += ROT_MOVE;
	RotNormalize(m_rotOrigin.y);

	//// �̗�
	//m_nLife--;

	//if (m_nLife <= 0)
	//{// �����Ȃ��Ȃ�����
	//	Uninit();
	//	return;
	//}

	// ���̈ʒu
	D3DXVECTOR3 posParent = GetPosition();

	for (int nCntGate = 0; nCntGate < MAX_CONFUSION; nCntGate++)
	{// ���ꂼ��̍X�V

		if (m_pObjBillboard[nCntGate] == NULL)
		{// NULL��������
			continue;
		}

		// �ʒu�擾
		D3DXVECTOR3 pos = m_pObjBillboard[nCntGate]->GetPosition();

		// �ʒu�X�V
		pos = D3DXVECTOR3(
			posParent.x + sinf(m_rotOrigin.y + fRot * nCntGate) * m_fDistance,
			posParent.y,
			posParent.z + cosf(m_rotOrigin.y + fRot * nCntGate) * m_fDistance);

		// �ʒu�ݒ�
		m_pObjBillboard[nCntGate]->SetPosition(pos);

		// ���_���W�X�V
		m_pObjBillboard[nCntGate]->SetVtx();
	}

}

//==========================================================================
// �ʒu�X�V
//==========================================================================
void CConfusionEffect::UpdatePosition(D3DXVECTOR3 pos)
{
	// ���̈ʒu
	D3DXVECTOR3 posParent = GetPosition();

	// �ʒu�X�V
	posParent.x = pos.x;
	posParent.z = pos.z;
	posParent.y = m_posOrigin.y + pos.y;

	// �ʒu�ݒ�
	SetPosition(posParent);
}

//==========================================================================
// �`�揈��
//==========================================================================
void CConfusionEffect::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	for (int nCntGate = 0; nCntGate < MAX_CONFUSION; nCntGate++)
	{
		if (m_pObjBillboard[nCntGate] == NULL)
		{// NULL��������
			continue;
		}

		// �`�揈��
		m_pObjBillboard[nCntGate]->Draw();
	}

	// �A���t�@�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

}
