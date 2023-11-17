//=============================================================================
// 
//  �w�i���� [score.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "score.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "object2D.h"
#include "number.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\number_05.png"	// �e�N�X�`���̃t�@�C��
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
CScore::CScore(int nPriority)
{
	// �l�̃N���A
	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		m_apNumber[nCntScore] = NULL;
	}
	m_nNum = 0;		// �l
	m_nTexIdx = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CScore::~CScore()
{

}

//==========================================================================
// ��������
//==========================================================================
CScore *CScore::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CScore *pScore = NULL;

	if (pScore == NULL)
	{// NULL��������

		// �������̊m��
		pScore = DEBUG_NEW CScore;

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
// ��������
//==========================================================================
CScore *CScore::Create(D3DXVECTOR3 pos)
{
	// �����p�̃I�u�W�F�N�g
	CScore *pScore = NULL;

	if (pScore == NULL)
	{// NULL��������

		// �������̊m��
		pScore = DEBUG_NEW CScore;

		if (pScore != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pScore->Init(pos);
		}

		return pScore;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CScore::Init(void)
{
	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		// ��������
		m_apNumber[nCntScore] = CNumber::Create();

		// �e��ϐ��̏�����
		m_apNumber[nCntScore]->GetObject2D()->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// �T�C�Y
		m_apNumber[nCntScore]->GetObject2D()->SetPosition(D3DXVECTOR3(640.0f, 360.0f, 0.0f));	// �ʒu

		// ��ނ̐ݒ�
		m_apNumber[nCntScore]->GetObject2D()->SetType(CObject::TYPE_SCORE);

		// �e�N�X�`���̊��蓖��
		m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

		// �e�N�X�`���̊��蓖��
		m_apNumber[nCntScore]->GetObject2D()->BindTexture(m_nTexIdx);

	}
	return S_OK;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CScore::Init(D3DXVECTOR3 pos)
{

	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		// ��������
		m_apNumber[nCntScore] = CNumber::Create();

		// �e��ϐ��̏�����
		m_apNumber[nCntScore]->GetObject2D()->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// �T�C�Y
		m_apNumber[nCntScore]->GetObject2D()->SetPosition(D3DXVECTOR3(pos.x + DIS_X * nCntScore, pos.y, pos.z));	// �ʒu

		// ��ނ̐ݒ�
		m_apNumber[nCntScore]->GetObject2D()->SetType(CObject::TYPE_SCORE);

		// �e�N�X�`���̊��蓖��
		m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

		// �e�N�X�`���̊��蓖��
		m_apNumber[nCntScore]->GetObject2D()->BindTexture(m_nTexIdx);

	}
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CScore::Uninit(void)
{
	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		// �I������
		if (m_apNumber[nCntScore] != NULL)
		{// �������̊m�ۂ�����Ă�����

			// �I������
			m_apNumber[nCntScore]->Uninit();
			delete m_apNumber[nCntScore];
			m_apNumber[nCntScore] = NULL;
		}
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CScore::Update(void)
{
	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		if (m_apNumber[nCntScore] != NULL)
		{// NULL����Ȃ�������
			m_apNumber[nCntScore]->Update();
		}
	}
	// �l�̐ݒ菈��
	SetValue();
}

//==========================================================================
// �l�̐ݒ菈��
//==========================================================================
void CScore::SetValue(void)
{
	int aTexU[NUM_SCORE];
	int nDigit = 1;		// aTexU�v�Z�p

	// �v�Z�p����o��
	for (int nCntDigit = 0; nCntDigit < NUM_SCORE; nCntDigit++)
	{
		nDigit *= 10;
	}

	// �e�N�X�`�����W�ɑ������
	for (int nCntTex = 0; nCntTex < NUM_SCORE; nCntTex++)
	{// �������ݒ�

		aTexU[nCntTex] = m_nNum % nDigit / (nDigit / 10);
		nDigit /= 10;
	}

	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		if (m_apNumber[nCntScore] != NULL)
		{// NULL����Ȃ�������

			D3DXVECTOR2 *pTex = m_apNumber[nCntScore]->GetObject2D()->GetTex();

			// �e�N�X�`�����W�̐ݒ�
			pTex[0] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 1.0f);

			// ���_�ݒ�
			m_apNumber[nCntScore]->GetObject2D()->SetVtx();
		}
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CScore::Draw(void)
{

}

//==========================================================================
// ���Z����
//==========================================================================
void CScore::Add(int nValue)
{
	// �l���Z
	m_nNum += nValue;

	// �X�R�A�̉��Z
	CManager::GetResultManager()->AddScore(nValue);

	if (m_nNum >= MAX_VALUE)
	{// �ő�l�ŌŒ�
		m_nNum = MAX_VALUE;
	}

	// �l�̐ݒ菈��
	SetValue();
}

//==========================================================================
// ���Z����
//==========================================================================
void CScore::Add(CResultManager::ADDTYPE AddType)
{
	// �l���Z
	/*switch (AddType)
	{
	case CResultManager::ADDTYPE_NONE:
		break;

	case CResultManager::ADDTYPE_NORMAL:
		m_nNum += mylib_const::COMBOBONUS_1;
		break;

	case CResultManager::ADDTYPE_BLUE:
		m_nNum += mylib_const::COMBOBONUS_2;
		break;

	case CResultManager::ADDTYPE_GOLD:
		m_nNum += mylib_const::COMBOBONUS_3;
		break;

	default:
		break;
	}*/

	// �X�R�A�̉��Z
	CManager::GetResultManager()->AddScoreNum(AddType);

	if (m_nNum >= MAX_VALUE)
	{// �ő�l�ŌŒ�
		m_nNum = MAX_VALUE;
	}

	// �l�̐ݒ菈��
	SetValue();
}
