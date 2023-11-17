//=============================================================================
// 
//  �p���[�Q�[�W���� [power_gauge.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "power_gauge.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "particle.h"
#include "calculation.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define WIDTH	(40.0f * 9.0f)	// ����
#define HEIGHT	(5.0f * 9.0f)	// �c��

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
const char *CPowerGauge::m_apTextureFile[VTX_MAX] =					// �e�N�X�`���̃t�@�C��
{
	NULL,
	NULL,
	NULL,
	"data\\TEXTURE\\hypnosis_fram.png",
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CPowerGauge::CPowerGauge(int nPriority) : CObject(nPriority)
{
	// �e�N�X�`���f�[�^�̔z�񕪌J��Ԃ�
	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		m_nTexIdx[nCntGauge] = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
		m_sPowerGauge[nCntGauge] = {};		// �I�u�W�F�N�g2D�̃|�C���^
	}
	m_Level = LEVEL_1;	// �`���[�W�i�K
	m_nCntWaitTime = 0;	// �҂�����
	m_nCntCharge = 0;	// �`���[�W�J�E���g
	m_nMaxCharge = 0;	// �ő�`���[�W�J�E���g
	m_bFever = false;	// FEVER���̔���
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CPowerGauge::~CPowerGauge()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CPowerGauge *CPowerGauge::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CPowerGauge *pGauge = NULL;

	if (pGauge == NULL)
	{// NULL��������

		// �������̊m��
		pGauge = DEBUG_NEW CPowerGauge;

		if (pGauge != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pGauge->Init();
		}

		return pGauge;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CPowerGauge::Init(void)
{

	// �e��ϐ��̏�����
	m_nCntCharge = 0;
	m_nMaxCharge = 180 * 7;

	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		// ��������
		m_sPowerGauge[nCntGauge].pObj2D = CObject2D::Create(7);

		m_sPowerGauge[nCntGauge].pObj2D->SetPosition(D3DXVECTOR3(640.0f, 650.0f, 0.0f));	// �ʒu
		m_sPowerGauge[nCntGauge].pObj2D->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));				// �T�C�Y

		// �e��ϐ��̏�����
		m_sPowerGauge[nCntGauge].fMaxWidth = m_sPowerGauge[nCntGauge].pObj2D->GetSize().x;	// ���̍ő�l
		m_sPowerGauge[nCntGauge].fMaxHeight = m_sPowerGauge[nCntGauge].pObj2D->GetSize().y;	// �����̍ő�l

		if (nCntGauge == VTX_GAUGE || nCntGauge == VTX_RED)
		{// �Q�[�W�����͏���0
			m_sPowerGauge[nCntGauge].pObj2D->SetSize(D3DXVECTOR2(0.0f, HEIGHT));			// �T�C�Y
		}

		// ���_�J���[�̐ݒ�
		switch (nCntGauge)
		{
		case VTX_BLACK:		// ���n

			// �F�ݒ�
			m_sPowerGauge[nCntGauge].pObj2D->SetColor(D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f));
			break;

			case VTX_RED:		// �ϓ�����Q�[�W

			// �F�ݒ�
			m_sPowerGauge[nCntGauge].pObj2D->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
			break;

		case VTX_GAUGE:		// �ϓ�����Q�[�W

			// �F�ݒ�
			m_sPowerGauge[nCntGauge].pObj2D->SetColor(D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
			break;

		case VTX_FRAM:		// �t���[��

			// �F�ݒ�
			m_sPowerGauge[nCntGauge].pObj2D->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			break;
		}

		// �e�N�X�`���̊��蓖��
		m_nTexIdx[nCntGauge] = CManager::GetTexture()->Regist(m_apTextureFile[nCntGauge]);

		// �e�N�X�`���̊��蓖��
		m_sPowerGauge[nCntGauge].pObj2D->BindTexture(m_nTexIdx[nCntGauge]);
	}

	// ��ސݒ�
	SetType(CObject::TYPE_ITEM);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CPowerGauge::Uninit(void)
{
	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		// �I������
		m_sPowerGauge[nCntGauge].pObj2D->Uninit();
		m_sPowerGauge[nCntGauge].pObj2D = NULL;
	}

	// �J������
	Release();

}

//==========================================================================
// �X�V����
//==========================================================================
void CPowerGauge::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		if (nCntGauge == VTX_GAUGE)
		{// �Q�[�W���������@

			// ��������
			GaugeDecrement(nCntGauge);
			ChangeColor(nCntGauge);
		}
	}

	UpdateChargeCount();

	// ���݂̒i�K�����߂�
	if ((float)m_nCntCharge / (float)m_nMaxCharge >= (1.0f / (float)LEVEL_MAX) * LEVEL_1 &&
		(float)m_nCntCharge / (float)m_nMaxCharge < (1.0f / (float)LEVEL_MAX) * LEVEL_2)
	{
		m_Level = LEVEL_1;
	}
	else if (
		(float)m_nCntCharge / (float)m_nMaxCharge >= (1.0f / (float)LEVEL_MAX) * LEVEL_2 &&
		(float)m_nCntCharge / (float)m_nMaxCharge < (1.0f / (float)LEVEL_MAX) * LEVEL_3)
	{
		m_Level = LEVEL_2;
	}
	else if (
		(float)m_nCntCharge / (float)m_nMaxCharge >= (1.0f / (float)LEVEL_MAX) * LEVEL_3 &&
		(float)m_nCntCharge / (float)m_nMaxCharge < (1.0f / (float)LEVEL_MAX) * LEVEL_MAX)
	{
		m_Level = LEVEL_3;
	}
	
	if ((float)m_nCntCharge / (float)m_nMaxCharge == 1.0f || m_bFever == true)
	{
		m_Level = LEVEL_MAX;
		m_bFever = true;
	}

	// ���_���ݒ�
	SetVtx();
}

//==================================================================================
// ��������
//==================================================================================
void CPowerGauge::GaugeDecrement(int nCntGauge)
{
	// �T�C�Y�擾
	D3DXVECTOR2 size = m_sPowerGauge[nCntGauge].pObj2D->GetSize();

	// �ڕW�̃T�C�Y�ɂ���
	size.x += (m_sPowerGauge[nCntGauge].fWidthDest - size.x) * 0.25f;

	// �T�C�Y�ݒ�
	m_sPowerGauge[nCntGauge].pObj2D->SetSize(size);

}

//==================================================================================
// �`���[�W�J�E���g�X�V����
//==================================================================================
void CPowerGauge::UpdateChargeCount(void)
{
	if (m_sPowerGauge[VTX_GAUGE].pObj2D == NULL)
	{// NULL��������
		return;
	}

	m_nCntWaitTime--;
	if (m_nCntWaitTime <= 0 || m_bFever == true)
	{
		// ���݂̗̑͐ݒ�
		m_nCntCharge--;

		if (m_bFever == true && m_nCntCharge <= 0)
		{
			m_bFever = false;
		}
	}

	// �l�̐��K������
	ValueNormalize(m_nCntCharge, m_nMaxCharge, 0);

	// �ڕW�̕��ݒ�
	m_sPowerGauge[VTX_GAUGE].fWidthDest =
		m_sPowerGauge[VTX_GAUGE].fMaxWidth * ((float)m_nCntCharge / (float)m_nMaxCharge);
}

//==================================================================================
// �`���[�W�J�E���g�ݒ菈��
//==================================================================================
void CPowerGauge::SetChargeCount(int nValue)
{
	if (m_sPowerGauge[VTX_GAUGE].pObj2D == NULL)
	{// NULL��������
		return;
	}

	m_nCntWaitTime = 100;

	if (m_bFever == false)
	{
		// ���݂̗̑͐ݒ�
		m_nCntCharge += nValue;
	}
	else
	{
		m_nCntCharge += 0;
	}

	// �l�̐��K������
	ValueNormalize(m_nCntCharge, m_nMaxCharge, 0);

	//�ڕW�̕��ݒ�
	m_sPowerGauge[VTX_GAUGE].fWidthDest =
		m_sPowerGauge[VTX_GAUGE].fMaxWidth * ((float)m_nCntCharge / (float)m_nMaxCharge);
}


//==================================================================================
// �F�X�V
//==================================================================================
void CPowerGauge::ChangeColor(int nCntGauge)
{
	// �T�C�Y�擾
	D3DXVECTOR2 size = m_sPowerGauge[nCntGauge].pObj2D->GetSize();

	// �F�擾
	D3DXCOLOR col = m_sPowerGauge[nCntGauge].pObj2D->GetColor();

	switch (m_Level)
	{
	case CPowerGauge::LEVEL_1:
		col = D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f);
		break;

	case CPowerGauge::LEVEL_2:
		col = D3DXCOLOR(0.3f, 1.0f, 0.3f, 1.0f);
		break;

	case CPowerGauge::LEVEL_3:
		col = D3DXCOLOR(0.6f, 0.0f, 0.6f, 1.0f);
		break;

	case CPowerGauge::LEVEL_MAX:
		static int nn = 0, nCnt= 0;
		nn = (nn + 1) % 4;

		if (nn == 0)
		{
			nCnt = (nCnt + 1) % 2;
		}
		
		if(nCnt == 0)
		{
			col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			col = D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f);
		}
		break;
	}

	// �F�ݒ�
	m_sPowerGauge[nCntGauge].pObj2D->SetColor(col);

	// �T�C�Y�擾
	m_sPowerGauge[nCntGauge].pObj2D->SetSize(size);

}

//==========================================================================
// �`�揈��
//==========================================================================
void CPowerGauge::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		// �`�揈��
		m_sPowerGauge[nCntGauge].pObj2D->Draw();
	}

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CPowerGauge::SetVtx(void)
{
	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		// ���_�ݒ�
		m_sPowerGauge[nCntGauge].pObj2D->SetVtx();

		// �ʒu�擾
		D3DXVECTOR3 pos = m_sPowerGauge[nCntGauge].pObj2D->GetPosition();

		// ���_���ւ̃|�C���^
		VERTEX_2D *pVtx;

		// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
		m_sPowerGauge[nCntGauge].pObj2D->GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

		// �T�C�Y�擾
		D3DXVECTOR2 size = m_sPowerGauge[nCntGauge].pObj2D->GetSize();

		//���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(pos.x + -size.x - (m_sPowerGauge[nCntGauge].fMaxWidth - size.x),	pos.y + -size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + size.x - (m_sPowerGauge[nCntGauge].fMaxWidth - size.x),	pos.y + -size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x + -size.x - (m_sPowerGauge[nCntGauge].fMaxWidth - size.x),	pos.y + size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + size.x - (m_sPowerGauge[nCntGauge].fMaxWidth - size.x),	pos.y + size.y, 0.0f);

		// ���_�o�b�t�@���A�����b�N���b�N
		m_sPowerGauge[nCntGauge].pObj2D->GetVtxBuff()->Unlock();
	}
}

//==========================================================================
// �`���[�W�i�K�擾
//==========================================================================
int CPowerGauge::GetLevel(void)
{
	return m_Level;
}
