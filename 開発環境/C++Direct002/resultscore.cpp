//=============================================================================
// 
//  �w�i���� [resultscore.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "resultscore.h"
#include "result.h"
#include "manager.h"
#include "resultmanager.h"
#include "texture.h"
#include "renderer.h"
#include "object2D.h"
#include "number.h"
#include "calculation.h"
#include "result_screen.h"
#include "rankingmanager.h"
#include "sound.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define BASETEXT_TEXTURE	"data\\TEXTURE\\resultscoretext_02.png"	// �e�N�X�`���̃t�@�C��
#define TEXTURE				"data\\TEXTURE\\resultscoretext_00.png"	// �e�N�X�`���̃t�@�C��
#define NUMBERTEXTURE		"data\\TEXTURE\\number_blackclover_01.png"	// �e�N�X�`���̃t�@�C��
#define MAX_VALUE			(999999)			// �l�̍ő�l
#define TEX_U				(0.1f)				// U�̕���
#define BASE_WIDTH			(40.0f * 0.75f)		// ����
#define BASE_HEIGHT			(50.0f * 0.75f)		// �c��
#define BASE_DIS_X			(50.0f * 0.75f)		// �Ԋu
#define WIDTH				(40.0f)				// ����
#define HEIGHT				(50.0f)				// �c��
#define DIS_X				(50.0f)				// �Ԋu
#define POSY_BASE			(330.0f)			// Y�ʒu
#define POSY				(650.0f)			// Y�ʒu
#define BASETEXT_POSX		(180.0f)			// X�ʒu
#define TEXT_POSX			(450.0f)			// X�ʒu
#define INT_BASESCORESET	(60)				// �X�R�A�̕ϓ�����
#define INT_SCORESET		(120)				// �X�R�A�̕ϓ�����
#define TEXT_MOVETIME		(20)				// �ړ�����
#define INIT_POSX			(1600.0f)			// �����ʒu

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CResultScore::CResultScore(int nPriority)
{
	// �l�̃N���A
	m_nBaseNum = 0;					// ���̃X�R�A
	m_nBaseNumDest = 0;				// ���̃X�R�A�̖ڕW�l
	m_nToatalNum = 0;					// �l
	m_nToatalNumDest = 0;				// �l
	m_nTexIdx = 0;			// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_nTexIdxNumber = 0;	// �����e�N�X�`���̃C���f�b�N�X�ԍ�
	m_pBaseObj2D = NULL;		// ���̃X�R�A2D�̃I�u�W�F�N�g
	m_pToatalObj2D = NULL;		// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
	m_fBasePosDest_X = 0.0f;		// ���X�R�A�̖ڕW�ʒu
	m_fToatalPosDest_X = 0.0f;		// �ŏI�X�R�A�̖ڕW�ʒu
	m_bArrivalBase = false;			// ���̃X�R�A�̓�������
	m_bArrivalToatal = false;		// �ŏI�X�R�A�̓�������
	m_bSetBase = false;				// ���̃X�R�A�̐ݒ蔻��
	m_bSetToatal = false;			// �ŏI�X�R�A�̐ݒ蔻��
	memset(&m_pToatalScore[0], NULL, sizeof(m_pToatalScore));	// �����̃I�u�W�F�N�g
	memset(&m_pBaseScore[0], NULL, sizeof(m_pBaseScore));	// ���̃X�R�A
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CResultScore::~CResultScore()
{

}

//==========================================================================
// ��������
//==========================================================================
CResultScore *CResultScore::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CResultScore *pScore = NULL;

	if (pScore == NULL)
	{// NULL��������

		// �������̊m��
		pScore = DEBUG_NEW CResultScore;

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
HRESULT CResultScore::Init(void)
{
	// ��ސݒ�
	SetType(CObject::TYPE_SCORE);

	m_fBasePosDest_X = BASETEXT_POSX;	// ���X�R�A�̖ڕW�ʒu
	m_fToatalPosDest_X = TEXT_POSX;		// �ŏI�X�R�A�̖ڕW�ʒu

	// ���̃X�R�A�̐���
	CreateBaseScore();

	// �ŏI�X�R�A�̐���
	CreateToatalScore();


	// ���̃X�R�A�擾
	m_nBaseNumDest = CManager::GetResultManager()->GetBaseScore();

	// �ڕW�̃X�R�A�擾
	m_nToatalNumDest = CManager::GetResultManager()->GetToatalScore();
	CManager::GetRankingManager()->SetNowScore(m_nToatalNumDest);
	
	return S_OK;
}

//==========================================================================
// ���̃X�R�A�̐���
//==========================================================================
void CResultScore::CreateBaseScore(void)
{
	// ��������
	m_pBaseObj2D = CObject2D::Create(7);

	// �e�N�X�`���̊��蓖��
	m_nTexIdx = CManager::GetTexture()->Regist(BASETEXT_TEXTURE);

	// �e�N�X�`���̊��蓖��
	m_pBaseObj2D->GetObject2D()->BindTexture(m_nTexIdx);

	// �e��ϐ��̏�����
	m_pBaseObj2D->GetObject2D()->SetSize(CManager::GetTexture()->GetImageSize(m_nTexIdx) * 0.3f);	// �T�C�Y
	m_pBaseObj2D->GetObject2D()->SetPosition(D3DXVECTOR3(INIT_POSX, POSY_BASE, 0.0f));	// �ʒu

	// ��ނ̐ݒ�
	m_pBaseObj2D->GetObject2D()->SetType(CObject::TYPE_SCORE);


	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		// ��������
		m_pBaseScore[nCntScore] = CNumber::Create();

		// �e��ϐ��̏�����
		m_pBaseScore[nCntScore]->GetObject2D()->SetSize(D3DXVECTOR2(BASE_WIDTH, BASE_HEIGHT));	// �T�C�Y
		m_pBaseScore[nCntScore]->GetObject2D()->SetPosition(D3DXVECTOR3(INIT_POSX + m_pBaseObj2D->GetSize().x + nCntScore * BASE_DIS_X, POSY_BASE, 0.0f));	// �ʒu

		// ��ނ̐ݒ�
		m_pBaseScore[nCntScore]->GetObject2D()->SetType(CObject::TYPE_SCORE);

		// �e�N�X�`���̊��蓖��
		m_nTexIdxNumber = CManager::GetTexture()->Regist(NUMBERTEXTURE);

		// �e�N�X�`���̊��蓖��
		m_pBaseScore[nCntScore]->GetObject2D()->BindTexture(m_nTexIdxNumber);
	}
}

//==========================================================================
// �ŏI�X�R�A�̐���
//==========================================================================
void CResultScore::CreateToatalScore(void)
{
	// ��������
	m_pToatalObj2D = CObject2D::Create(7);

	// �e�N�X�`���̊��蓖��
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// �e�N�X�`���̊��蓖��
	m_pToatalObj2D->GetObject2D()->BindTexture(m_nTexIdx);

	// �e��ϐ��̏�����
	m_pToatalObj2D->GetObject2D()->SetSize(CManager::GetTexture()->GetImageSize(m_nTexIdx) * 0.4f);	// �T�C�Y
	m_pToatalObj2D->GetObject2D()->SetPosition(D3DXVECTOR3(INIT_POSX, POSY, 0.0f));	// �ʒu

	// ��ނ̐ݒ�
	m_pToatalObj2D->GetObject2D()->SetType(CObject::TYPE_SCORE);


	float fPosX = INIT_POSX + m_pToatalObj2D->GetSize().x + 50.0f;
	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		// ��������
		m_pToatalScore[nCntScore] = CNumber::Create();

		// �e��ϐ��̏�����
		m_pToatalScore[nCntScore]->GetObject2D()->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// �T�C�Y
		m_pToatalScore[nCntScore]->GetObject2D()->SetPosition(D3DXVECTOR3(fPosX + nCntScore * DIS_X, POSY, 0.0f));	// �ʒu

		// ��ނ̐ݒ�
		m_pToatalScore[nCntScore]->GetObject2D()->SetType(CObject::TYPE_SCORE);

		// �e�N�X�`���̊��蓖��
		m_nTexIdxNumber = CManager::GetTexture()->Regist(NUMBERTEXTURE);

		// �e�N�X�`���̊��蓖��
		m_pToatalScore[nCntScore]->GetObject2D()->BindTexture(m_nTexIdxNumber);
	}
}

//==========================================================================
// �I������
//==========================================================================
void CResultScore::Uninit(void)
{
	// �I������
	if (m_pBaseObj2D != NULL)
	{// �������̊m�ۂ�����Ă�����

		// �I������
		m_pBaseObj2D->Uninit();
		m_pBaseObj2D = NULL;
	}

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		// �I������
		if (m_pBaseScore[nCntScore] != NULL)
		{// �������̊m�ۂ�����Ă�����

			// �I������
			m_pBaseScore[nCntScore]->Uninit();
			delete m_pBaseScore[nCntScore];
			m_pBaseScore[nCntScore] = NULL;
		}
	}

	// �I������
	if (m_pToatalObj2D != NULL)
	{// �������̊m�ۂ�����Ă�����

		// �I������
		m_pToatalObj2D->Uninit();
		m_pToatalObj2D = NULL;
	}

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		// �I������
		if (m_pToatalScore[nCntScore] != NULL)
		{// �������̊m�ۂ�����Ă�����

			// �I������
			m_pToatalScore[nCntScore]->Uninit();
			delete m_pToatalScore[nCntScore];
			m_pToatalScore[nCntScore] = NULL;
		}
	}

	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CResultScore::Update(void)
{
	// ���̃X�R�A�̈ړ�����
	if (m_bSetBase == false)
	{
		MoveBaseScore();
	}

	// �ŏI�X�R�A�̈ړ�����
	if (m_bSetToatal == false)
	{
		MoveToatalScore();
	}

	// ���̃X�R�A�̐ݒ菈��
	SetBaseValue();

	// �ŏI�X�R�A�̐ݒ菈��
	SetToatalValue();
}

//==========================================================================
// ���X�R�A�̈ړ�����
//==========================================================================
void CResultScore::MoveBaseScore(void)
{
	if (m_pBaseObj2D == NULL)
	{// ���̃X�R�A����������Ă�����
		return;
	}

	// �ʒu�擾
	D3DXVECTOR3 pos = m_pBaseObj2D->GetPosition();

	if (pos.x == INIT_POSX)
	{
		// �T�E���h�Đ�
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_NUMBERMOVE);
	}

	// �ړ�
	pos.x += (m_fBasePosDest_X - INIT_POSX) / (float)TEXT_MOVETIME;
	if (pos.x <= m_fBasePosDest_X)
	{// �␳����
		pos.x = m_fBasePosDest_X;
		m_bArrivalBase = true;		// ���̃X�R�A�̓�������
	}

	// �ʒu�ݒ�
	m_pBaseObj2D->SetPosition(pos);

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pBaseScore[nCntScore] == NULL)
		{// NULL��������
			continue;
		}

		// �ʒu�擾
		D3DXVECTOR3 posNumber = m_pBaseScore[nCntScore]->GetObject2D()->GetPosition();

		// �ʒu�X�V
		m_pBaseScore[nCntScore]->GetObject2D()->SetPosition(
			D3DXVECTOR3(
				pos.x + m_pBaseObj2D->GetSize().x + nCntScore * BASE_DIS_X,
				posNumber.y,
				0.0f));
	}

	if (m_bArrivalBase == true)
	{// �������Ă�����

		// ���̃X�R�A�擾
#if _DEBUG
		m_nBaseNumDest = 1000;
#else
		m_nBaseNumDest = CManager::GetResultManager()->GetBaseScore();
#endif

		if (m_nBaseNumDest > m_nBaseNum)
		{// �K�莞�Ԃ����ĕ␳
			m_nBaseNum += (int)((float)m_nBaseNumDest / (float)INT_BASESCORESET);

			// �T�E���h�Đ�
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_NUMBER);
		}
		else
		{// �ڕW�̒l�ŌŒ�
			m_nBaseNum = m_nBaseNumDest;
			m_bSetBase = true;				// ���̃X�R�A�̐ݒ蔻��
		}
	}
}

//==========================================================================
// �ŏI�X�R�A�̈ړ�����
//==========================================================================
void CResultScore::MoveToatalScore(void)
{
	if (CResult::GetResultScreen()->IsSetNumber() == false || m_pToatalObj2D == NULL)
	{// �ŏI�X�R�A����������Ă�����
		return;
	}

	// �ʒu�擾
	D3DXVECTOR3 pos = m_pToatalObj2D->GetPosition();

	if (pos.x == INIT_POSX)
	{
		// �T�E���h�Đ�
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_NUMBERMOVE);
	}

	// �ړ�
	pos.x += (m_fToatalPosDest_X - INIT_POSX) / (float)TEXT_MOVETIME;
	if (pos.x <= m_fToatalPosDest_X)
	{// �␳����
		pos.x = m_fToatalPosDest_X;
		m_bArrivalToatal = true;	// �ŏI�X�R�A�̓�������
	}

	// �ʒu�ݒ�
	m_pToatalObj2D->SetPosition(pos);

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pToatalScore[nCntScore] == NULL)
		{// NULL��������
			continue;
		}

		// �ʒu�擾
		D3DXVECTOR3 posNumber = m_pToatalScore[nCntScore]->GetObject2D()->GetPosition();

		// �ʒu�X�V
		m_pToatalScore[nCntScore]->GetObject2D()->SetPosition(
			D3DXVECTOR3(
				pos.x + m_pToatalObj2D->GetSize().x + nCntScore * DIS_X + 50.0f,
				posNumber.y,
				0.0f));
	}

	if (m_bArrivalToatal == true)
	{// �������Ă�����

		// �ڕW�̃X�R�A�擾
#if _DEBUG
		m_nToatalNumDest = 1000;
#else
		m_nToatalNumDest = CManager::GetResultManager()->GetToatalScore();
#endif
		CManager::GetRankingManager()->SetNowScore(m_nToatalNumDest);

		if (m_nToatalNumDest > m_nToatalNum)
		{// �K�莞�Ԃ����ĕ␳
			m_nToatalNum += (int)((float)m_nToatalNumDest / (float)INT_SCORESET);

			// �T�E���h�Đ�
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_NUMBER);
		}
		else
		{// �ڕW�̒l�ŌŒ�
			m_nToatalNum = m_nToatalNumDest;
			m_bSetToatal = true;			// �ŏI�X�R�A�̐ݒ蔻��
			CResult::SetEnableArrival();
		}
	}
}

//==========================================================================
// ���̃X�R�A�̐ݒ菈��
//==========================================================================
void CResultScore::SetBaseValue(void)
{
	int aTexU[RESULTSCORE_DIGIT];
	int nDigit = 1;		// aTexU�v�Z�p

	// �v�Z�p����o��
	for (int nCntDigit = 0; nCntDigit < RESULTSCORE_DIGIT; nCntDigit++)
	{
		nDigit *= 10;
	}

	// �e�N�X�`�����W�ɑ������
	for (int nCntTex = 0; nCntTex < RESULTSCORE_DIGIT; nCntTex++)
	{// �������ݒ�

		aTexU[nCntTex] = m_nBaseNum % nDigit / (nDigit / 10);
		nDigit /= 10;
	}

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pBaseScore[nCntScore] != NULL)
		{// NULL����Ȃ�������

			D3DXVECTOR2 *pTex = m_pBaseScore[nCntScore]->GetObject2D()->GetTex();

			// �e�N�X�`�����W�̐ݒ�
			pTex[0] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 1.0f);

			// ���_�ݒ�
			m_pBaseScore[nCntScore]->GetObject2D()->SetVtx();
		}
	}
}

//==========================================================================
// �ŏI�X�R�A�̐ݒ菈��
//==========================================================================
void CResultScore::SetToatalValue(void)
{
	int aTexU[RESULTSCORE_DIGIT];
	int nDigit = 1;		// aTexU�v�Z�p

	// �v�Z�p����o��
	for (int nCntDigit = 0; nCntDigit < RESULTSCORE_DIGIT; nCntDigit++)
	{
		nDigit *= 10;
	}

	// �e�N�X�`�����W�ɑ������
	for (int nCntTex = 0; nCntTex < RESULTSCORE_DIGIT; nCntTex++)
	{// �������ݒ�

		aTexU[nCntTex] = m_nToatalNum % nDigit / (nDigit / 10);
		nDigit /= 10;
	}

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pToatalScore[nCntScore] != NULL)
		{// NULL����Ȃ�������

			D3DXVECTOR2 *pTex = m_pToatalScore[nCntScore]->GetObject2D()->GetTex();

			// �e�N�X�`�����W�̐ݒ�
			pTex[0] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 1.0f);

			// ���_�ݒ�
			m_pToatalScore[nCntScore]->GetObject2D()->SetVtx();
		}
	}
}

//==========================================================================
// �S�Ă̓����ݒ�
//==========================================================================
void CResultScore::SetAllArrival(void)
{
	m_bArrivalBase = true;			// ���̃X�R�A�̓�������
	m_bArrivalToatal = true;		// �ŏI�X�R�A�̓�������
	m_bSetBase = true;				// ���̃X�R�A�̐ݒ蔻��
	m_bSetToatal = true;			// �ŏI�X�R�A�̐ݒ蔻��

	// �ʒu�擾
	D3DXVECTOR3 pos = m_pBaseObj2D->GetPosition();
	
	pos.x = m_fBasePosDest_X;

	// �ʒu�ݒ�
	m_pBaseObj2D->SetPosition(pos);

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pBaseScore[nCntScore] == NULL)
		{// NULL��������
			continue;
		}

		// �ʒu�擾
		D3DXVECTOR3 posNumber = m_pBaseScore[nCntScore]->GetObject2D()->GetPosition();

		// �ʒu�X�V
		m_pBaseScore[nCntScore]->GetObject2D()->SetPosition(
			D3DXVECTOR3(
				pos.x + m_pBaseObj2D->GetSize().x + nCntScore * BASE_DIS_X,
				posNumber.y,
				0.0f));
	}

	// �ڕW�̒l�ɐݒ�
	m_nBaseNum = m_nBaseNumDest;
	m_nToatalNum = m_nToatalNumDest;


	// �ʒu�擾
	pos = m_pToatalObj2D->GetPosition();

	// �ړ�
	pos.x = m_fToatalPosDest_X;

	// �ʒu�ݒ�
	m_pToatalObj2D->SetPosition(pos);

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pToatalScore[nCntScore] == NULL)
		{// NULL��������
			continue;
		}

		// �ʒu�擾
		D3DXVECTOR3 posNumber = m_pToatalScore[nCntScore]->GetObject2D()->GetPosition();

		// �ʒu�X�V
		m_pToatalScore[nCntScore]->GetObject2D()->SetPosition(
			D3DXVECTOR3(
				pos.x + m_pToatalObj2D->GetSize().x + nCntScore * DIS_X + 50.0f,
				posNumber.y,
				0.0f));
	}

}

//==========================================================================
// �`�揈��
//==========================================================================
void CResultScore::Draw(void)
{

}

//==========================================================================
// �l�̐ݒ菈��
//==========================================================================
void CResultScore::SetValue(int nScore)
{
	m_nToatalNumDest = nScore;
}
