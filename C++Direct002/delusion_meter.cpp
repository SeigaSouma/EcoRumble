//=============================================================================
// 
//  �ϑz�̃��[�^�[���� [delusion_meter.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "delusion_meter.h"
#include "input.h"
#include "manager.h"
#include "game.h"
#include "texture.h"
#include "renderer.h"
#include "calculation.h"
#include "particle.h"
#include "player.h"
#include "delusion_point.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define VERTEX	(3)
#define LENGTH	(50.0f)		// ����
#define TEXTURE	"data\\TEXTURE\\block000.jpg"
#define TEXTURE_METER	"data\\TEXTURE\\bullet_01.png"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
int CDelusionMeter::m_nNumAll = 0;		// �ϑz�̃��[�^�[�̑���

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CDelusionMeter::CDelusionMeter(int nPriority) : CObject2D(nPriority)
{
	memset(&m_fDestLength[0], 0, sizeof(m_fDestLength));	// �ڕW�̒���
	memset(&m_fLength[0], 0, sizeof(m_fLength));			// ����
	memset(&m_nPoint[0], 0, sizeof(m_nPoint));				// �v�f�|�C���g
	m_nRemainPoint = 0;										// �c��|�C���g
	m_nChangeIdx = 0;										// �ύX����C���f�b�N�X�ԍ�
	m_pDelusionPoint = NULL;								// �ϑz�|�C���g
	m_pMeterMain = NULL;									// �{���̃��[�^�[
	m_pMeter = NULL;			// �{���̃��[�^�[

	// �������Z
	m_nNumAll++;
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CDelusionMeter::~CDelusionMeter()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CDelusionMeter *CDelusionMeter::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CDelusionMeter *pItem = NULL;

	if (pItem == NULL)
	{// NULL��������

		// �������̊m��
		pItem = DEBUG_NEW CDelusionMeter;

		if (pItem != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pItem->Init();
		}

		return pItem;
	}

	return NULL;
}
//
////==========================================================================
//// ����������
////==========================================================================
//HRESULT CDelusionMeter::Init(void)
//{
//	HRESULT hr;
//
//	//*********************************
//	// ���[�^�[���n
//	//*********************************
//	m_pMeterMain = CObject2D::Create();
//
//	if (m_pMeterMain == NULL)
//	{// ���s���Ă�����
//		return E_FAIL;
//	}
//
//	// ��ނ̐ݒ�
//	//m_pMeterMain->SetType(CObject::TYPE_DELUSION_METER_MAIN);
//
//	// �e�N�X�`���̓ǂݍ���
//	int nMainTex = CManager::GetTexture()->Regist(TEXTURE_METER);
//
//	// �e�N�X�`���̊��蓖��
//	m_pMeterMain->BindTexture(nMainTex);
//
//	m_pMeterMain->SetSize(D3DXVECTOR2(sinf(D3DXToRadian(120)) * LENGTH * 4, sinf(D3DXToRadian(-120)) * (-LENGTH * 4)));	// �T�C�Y
//	m_pMeterMain->SetPosition(D3DXVECTOR3(640.0f, 250.0f, 0.0f));	// �ʒu
//	m_pMeterMain->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));
//
//
//	//*********************************
//	// ���[�^�[����
//	//*********************************
//	// �e��ϐ��̏�����
// 	SetSize(D3DXVECTOR2(0.0f, 0.0f));				// �T�C�Y
//	SetPosition(D3DXVECTOR3(640.0f, 250.0f, 0.0f));	// �ʒu
//	SetColor(D3DXCOLOR(1.0f, 0.6f, 0.6f, 0.8f));
//	m_nRemainPoint = 3;								// �c��|�C���g
//	m_nChangeIdx = TYPE_POWER;						// �ύX����v�f�̃C���f�b�N�X�ԍ�
//
//	for (int nCntVtx = 0; nCntVtx < VERTEX; nCntVtx++)
//	{
//		// �ڕW�̒���
//		m_fDestLength[nCntVtx] = LENGTH;
//		m_nPoint[nCntVtx] = 1;	// �v�f�|�C���g
//	}
//
//	// ��ނ̐ݒ�
//	SetType(CObject::TYPE_DELUSION_METER);
//
//	// ����������
//	hr = CObject2D::Init(3);
//
//	if (FAILED(hr))
//	{// ���s�����Ƃ�
//
//		return E_FAIL;
//	}
//
//	// �e�N�X�`���̓ǂݍ���
//	m_nTexIdx = CManager::GetTexture()->Regist(NULL);
//
//	// �e�N�X�`���̊��蓖��
//	BindTexture(m_nTexIdx);
//
//	return S_OK;
//}


//==========================================================================
// ����������
//==========================================================================
HRESULT CDelusionMeter::Init(void)
{
	HRESULT hr;

	// ��ނ̐ݒ�
	SetType(CObject::TYPE_DELUSION_METER);

	//*********************************
	// ���[�^�[����
	//*********************************
	m_pMeterMain = CObject2D::Create(mylib_const::DEF2D_PRIORITY, 3);

	if (m_pMeterMain == NULL)
	{// ���s���Ă�����
		return E_FAIL;
	}

	// �e�N�X�`���̓ǂݍ���
	int nMainTex = CManager::GetTexture()->Regist(NULL);

	// �e�N�X�`���̊��蓖��
	m_pMeterMain->BindTexture(nMainTex);

	// �e��ϐ��̏�����
	m_pMeterMain->SetSize(D3DXVECTOR2(0.0f, 0.0f));				// �T�C�Y
	m_pMeterMain->SetPosition(D3DXVECTOR3(640.0f, 250.0f, 0.0f));	// �ʒu
	m_pMeterMain->SetColor(D3DXCOLOR(1.0f, 0.6f, 0.6f, 0.8f));
	m_nRemainPoint = 3;								// �c��|�C���g
	m_nChangeIdx = TYPE_POWER;						// �ύX����v�f�̃C���f�b�N�X�ԍ�

	for (int nCntVtx = 0; nCntVtx < VERTEX; nCntVtx++)
	{
		// �ڕW�̒���
		m_fDestLength[nCntVtx] = LENGTH;
		m_nPoint[nCntVtx] = 1;	// �v�f�|�C���g
	}


	//*********************************
	// ���[�^�[���n
	//*********************************
	// ����������
	m_pMeter = m_pMeter->Create(mylib_const::DEF2D_PRIORITY);

	// �e��ϐ��̏�����
	m_pMeter->SetSize(D3DXVECTOR2(sinf(D3DXToRadian(120)) * LENGTH * 4, sinf(D3DXToRadian(-120)) * (-LENGTH * 4)));	// �T�C�Y
	m_pMeter->SetPosition(D3DXVECTOR3(640.0f, 250.0f, 0.0f));	// �ʒu
	m_pMeter->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));

	//if (FAILED(hr))
	//{// ���s�����Ƃ�

	//	return E_FAIL;
	//}

	// �e�N�X�`���̓ǂݍ���
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// �e�N�X�`���̊��蓖��
	m_pMeter->BindTexture(m_nTexIdx);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CDelusionMeter::Uninit(void)
{
	// �ϑz�|�C���g
	if (m_pDelusionPoint != NULL)
	{
		m_pDelusionPoint->Uninit();
		m_pDelusionPoint = NULL;
	}

	if (m_pMeterMain != NULL)
	{// ��񂪓����Ă�����

		// �I������
		m_pMeterMain->Uninit();
		m_pMeterMain = NULL;
	}

	if (m_pMeter != NULL)
	{// ��񂪓����Ă�����

		// �I������
		m_pMeter->Uninit();
		m_pMeter = NULL;
	}

	// �I������
	CObject2D::Uninit();

	// �������Z
	m_nNumAll--;
}

//==========================================================================
// �X�V����
//==========================================================================
void CDelusionMeter::Update(void)
{
/*
	CObject *pObj = CObject::GetObject();
	CObject *pObjNext = GetNext();
	CObject *pObjMainNext = m_pMeterMain->GetNext();*/

	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �ϑz�|�C���g
	if (m_pDelusionPoint == NULL)
	{
		// �ϑz�|�C���g�̐���
		m_pDelusionPoint = CDelusionPoint::Create();
	}

	// �����␳
	for (int nCntVtx = 0; nCntVtx < VERTEX; nCntVtx++)
	{
		// �ڕW�̒���
		m_fDestLength[nCntVtx] = LENGTH * m_nPoint[nCntVtx];

		// �����␳
		InertiaCorrection(m_fLength[nCntVtx], m_fDestLength[nCntVtx], 0.1f);
	}

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	if (pInputKeyboard->GetTrigger(DIK_SPACE) == true && m_nRemainPoint == 0)
	{// SPACE�������ꂽ��, �폜

		// ����`�F���W
		CManager::GetScene()->GetPlayer()->ChangeWeapon(&m_nPoint[0]);

		// �I������
		Uninit();
		return;
	}

	if (pInputKeyboard->GetTrigger(DIK_UP) == true && m_nRemainPoint > 0)
	{// UP

		// �|�C���g�ǉ�
		m_nPoint[m_nChangeIdx]++;

		// �c��|�C���g���炷
		m_nRemainPoint--;
	}

	if (pInputKeyboard->GetTrigger(DIK_DOWN) == true && m_nPoint[m_nChangeIdx] > 1)
	{// DOWN && �����邾���|�C���g�c���Ă� && �|�C���g��

		// �|�C���g���炷
		m_nPoint[m_nChangeIdx]--;

		// �c��|�C���g���₷
		m_nRemainPoint++;
	}

	if (pInputKeyboard->GetTrigger(DIK_RIGHT) == true)
	{// �ύX
		m_nChangeIdx = (m_nChangeIdx + 1) % VERTEX;
	}
	if (pInputKeyboard->GetTrigger(DIK_LEFT) == true)
	{// �ύX
		m_nChangeIdx = (m_nChangeIdx + (VERTEX - 1)) % VERTEX;
	}

	if (m_pDelusionPoint != NULL)
	{// ��������Ă�����

		// �l�̐ݒ�
		m_pDelusionPoint->SetValue(m_nRemainPoint);
	}

	// ���_���W�擾
	D3DXVECTOR3 *pVtxPos = m_pMeterMain->GetVtxPos();

	pVtxPos[0] = D3DXVECTOR3(sinf(D3DXToRadian(-120)) * m_fLength[0], cosf(D3DXToRadian(-120)) * -m_fLength[0], 0.0f);
	pVtxPos[1] = D3DXVECTOR3(sinf(D3DXToRadian(0)) * m_fLength[1], cosf(D3DXToRadian(0)) * -m_fLength[1], 0.0f);
	pVtxPos[2] = D3DXVECTOR3(sinf(D3DXToRadian(120)) * m_fLength[2], cosf(D3DXToRadian(120)) * -m_fLength[2], 0.0f);

	my_particle::Create(m_pMeterMain->GetPosition() + pVtxPos[m_nChangeIdx], my_particle::TYPE_CHECK2D);
	my_particle::Create(m_pMeterMain->GetPosition(), my_particle::TYPE_CHECK2D);

	// ���_���W�ݒ�
	m_pMeterMain->SetVtxPos(&pVtxPos[0]);

	// ���_���ݒ�
	SetVtx();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CDelusionMeter::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	/*pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);*/

	//CObject2D::Draw();
	m_pMeter->Draw();
	m_pMeterMain->Draw(VERTEX);

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CDelusionMeter::SetVtx(void)
{
	// ���_�ݒ�
	m_pMeter->SetVtx();
	m_pMeterMain->SetVtx(VERTEX);
}

//==========================================================================
// �����擾
//==========================================================================
int CDelusionMeter::GetNumAll(void)
{
	return m_nNumAll;
}
