//=============================================================================
// 
//  �w�i���� [delusion_point.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "delusion_point.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "object2D.h"
#include "number.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\number_05.png"	// �e�N�X�`���̃t�@�C��
#define TEXT_TEXTURE	"data\\TEXTURE\\remainpoint_01.png"	// �e�N�X�`���̃t�@�C��
#define MAX_VALUE		(999999)						// �l�̍ő�l
#define TEX_U			(0.1f)							// U�̕���
#define WIDTH			(40.0f)							// ����
#define HEIGHT			(50.0f)							// �c��
#define DIS_X			(50.0f)							// �Ԋu
#define SCROLL_SPEED	(-0.005f)						// �X�N���[�����x

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CDelusionPoint::CDelusionPoint(int nPriority)
{
	// �l�̃N���A
	m_apNumber = NULL;	// �����̃I�u�W�F�N�g
	m_nNum = 0;			// �l
	m_nTexIdx = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CDelusionPoint::~CDelusionPoint()
{

}

//==========================================================================
// ��������
//==========================================================================
CDelusionPoint *CDelusionPoint::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CDelusionPoint *pScore = NULL;

	if (pScore == NULL)
	{// NULL��������

		// �������̊m��
		pScore = DEBUG_NEW CDelusionPoint;

		if (pScore != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pScore->Init();
		}

		return pScore;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CDelusionPoint::Init(void)
{
	//*********************************
	// �e�L�X�g
	//*********************************
	// �e��ϐ��̏�����
	SetSize(D3DXVECTOR2(100.0f, 40.0f));			// �T�C�Y
	SetPosition(D3DXVECTOR3(580.0f, 400.0f, 0.0f));	// �ʒu
	SetType(CObject::TYPE_DELUSION_POINT);			// ��ނ̐ݒ�

	// ����������
	HRESULT hr = CObject2D::Init();

	if (FAILED(hr))
	{// ���s�����Ƃ�

		return E_FAIL;
	}

	// �e�N�X�`���̓ǂݍ���
	m_nTexIdx = CManager::GetTexture()->Regist(TEXT_TEXTURE);

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx);


	//*********************************
	// ����
	//*********************************
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// ��������
	m_apNumber = CNumber::Create();

	// �e��ϐ��̏�����
	m_apNumber->GetObject2D()->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// �T�C�Y
	m_apNumber->GetObject2D()->SetPosition(D3DXVECTOR3(pos.x + 120.0f, pos.y, pos.z));					// �ʒu

	// ��ނ̐ݒ�
	m_apNumber->GetObject2D()->SetType(CObject::TYPE_SCORE);

	// �����̃e�N�X�`���ǂݍ���
	int nTexNumberIdx = CManager::GetTexture()->Regist(TEXTURE);

	// �e�N�X�`���̊��蓖��
	m_apNumber->GetObject2D()->BindTexture(nTexNumberIdx);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CDelusionPoint::Uninit(void)
{
	// �I������
	if (m_apNumber != NULL)
	{// �������̊m�ۂ�����Ă�����

		// �I������
		m_apNumber->GetObject2D()->Uninit();
		m_apNumber->Uninit();
		delete m_apNumber;
		m_apNumber = NULL;
	}

	// �J������
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CDelusionPoint::Update(void)
{
	//SetVtx();

	// �l�̐ݒ菈��
	//SetValue(m_nNum);
}

//==========================================================================
// �l�̐ݒ菈��
//==========================================================================
void CDelusionPoint::SetValue(int nNum)
{
	int aTexU;
	int nDigit = 10;		// aTexU�v�Z�p
	m_nNum = nNum;

	// �e�N�X�`�����W�ɑ������
	aTexU = nNum % nDigit / (nDigit / 10);

	if (m_apNumber != NULL)
	{// NULL����Ȃ�������

		// �e�N�X�`�����W�擾
		D3DXVECTOR2 *pTex = m_apNumber->GetObject2D()->GetTex();

		// �e�N�X�`�����W�̐ݒ�
		pTex[0] = D3DXVECTOR2(aTexU * TEX_U, 0.0f);
		pTex[1] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 0.0f);
		pTex[2] = D3DXVECTOR2(aTexU * TEX_U, 1.0f);
		pTex[3] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		SetTex(pTex);

		// ���_�ݒ�
		m_apNumber->GetObject2D()->SetVtx();
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CDelusionPoint::Draw(void)
{
	// �`�揈��
	CObject2D::Draw();
}
