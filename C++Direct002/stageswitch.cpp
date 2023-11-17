//=============================================================================
// 
// �X�e�[�W�؂�ւ����� [stageswitch.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "stageswitch.h"
#include "renderer.h"
#include "texture.h"
#include "camera.h"
#include "manager.h"
#include "number.h"
#include "game.h"
#include "wavemanager.h"
#include "elevation.h"
#include "title.h"
#include "sound.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXT_SIZE		(250.0f)	// ����
#define NUMBER_SIZE		(80.0f)		// �����̃T�C�Y
#if _DEBUG
#define MAX_LIFE		(200)		// �ő����
#else
#define MAX_LIFE		(360)		// �ő����
#endif
#define NUMBER_TEXTURE	"data\\TEXTURE\\number_blackclover_01.png"	// �e�N�X�`���̃t�@�C��

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
const char *CStageSwitch::m_apTextureFile[] =	// �t�@�C���ǂݍ���
{
	"data\\TEXTURE\\stagetext_01.png",		// �ʏ�G�t�F�N�g
};
int CStageSwitch::m_nTexIdx[VTX_MAX] = {};		// �e�N�X�`���̃C���f�b�N�X�ԍ�
int CStageSwitch::m_nNumberTexIdx = 0;				// �����e�N�X�`���̃C���f�b�N�X�ԍ�

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CStageSwitch::CStageSwitch(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_nLife = 0;	// ����

	for (int nCntNumber = 0; nCntNumber < NUM_STAGE; nCntNumber++)
	{
		m_apNumber[nCntNumber] = {};	// �����̃I�u�W�F�N�g
	}
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CStageSwitch::~CStageSwitch()
{

}

//==========================================================================
// ��������
//==========================================================================
CStageSwitch *CStageSwitch::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CStageSwitch *pStageSwitch = NULL;

	if (pStageSwitch == NULL)
	{// NULL��������

		// �������̊m��
		pStageSwitch = DEBUG_NEW CStageSwitch;

		if (pStageSwitch != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pStageSwitch->Init();
		}

		return pStageSwitch;
	}

	return NULL;
}

//==================================================================================
// ����������
//==================================================================================
HRESULT CStageSwitch::Init(void)
{
	HRESULT hr;

	// �e��ϐ��̏�����
	SetPosition(D3DXVECTOR3(450.0f, SCREEN_HEIGHT * 0.5f, 0.0f));
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// �F
	SetSize(D3DXVECTOR2(TEXT_SIZE * 1.0f, TEXT_SIZE * 0.45f));		// �T�C�Y�ݒ�
	m_nLife = MAX_LIFE;	// ����

	// ��ނ̐ݒ�
	SetType(TYPE_STAGESWITCH);

	// �e�N�X�`���f�[�^�̔z�񕪌J��Ԃ�
	for (int nCntTex = 0; nCntTex < VTX_MAX; nCntTex++)
	{
		if (m_nTexIdx[nCntTex] == 0)
		{
			// �e�N�X�`���̊��蓖��
			m_nTexIdx[nCntTex] = CManager::GetTexture()->Regist(m_apTextureFile[nCntTex]);
		}

		// �e�N�X�`���̊��蓖��
		BindTexture(m_nTexIdx[nCntTex]);
	}

	// ����������
	hr = CObject2D::Init();

	if (FAILED(hr))
	{// ���s�����Ƃ�
		return E_FAIL;
	}

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

#if 0
	for (int nCntNumber = 0; nCntNumber < NUM_STAGE; nCntNumber++)
	{
		// ��������
		m_apNumber[nCntNumber] = CNumber::Create();

		// �e��ϐ��̏�����
		m_apNumber[nCntNumber]->GetObject2D()->SetSize(D3DXVECTOR2(NUMBER_SIZE, NUMBER_SIZE));	// �T�C�Y
		m_apNumber[nCntNumber]->GetObject2D()->SetPosition(D3DXVECTOR3(pos.x + TEXT_SIZE + (nCntNumber + 1) * NUMBER_SIZE, pos.y, 0.0f));	// �ʒu

		// ��ނ̐ݒ�
		m_apNumber[nCntNumber]->GetObject2D()->SetType(CObject::TYPE_STAGESWITCH);

		// �e�N�X�`���̊��蓖��
		m_nNumberTexIdx = CManager::GetTexture()->Regist(NUMBER_TEXTURE);

		// �e�N�X�`���̊��蓖��
		m_apNumber[nCntNumber]->GetObject2D()->BindTexture(m_nNumberTexIdx);
	}
#endif


	// �T�E���h�Đ�
	CManager::GetSound()->PlaySound(CSound::LABEL_SE_STAGESWITCH);

	CCamera *pCamera = CManager::GetCamera();
	switch (CManager::GetMode())
	{
	case CScene::MODE_GAME:
		// �ڕW�̋����ݒ�
		pCamera->SetLenDest(pCamera->GetOriginDistance() + 1000.0f, MAX_LIFE / 2, 2.5f, 0.01f);

		break;

	case CScene::MODE_TITLE:
		break;

	default:
		break;
	}

	return S_OK;
}

//==================================================================================
// �I������
//==================================================================================
void CStageSwitch::Uninit(void)
{
	for (int nCntScore = 0; nCntScore < NUM_STAGE; nCntScore++)
	{
		// �I������
		if (m_apNumber[nCntScore] != NULL)
		{// �������̊m�ۂ�����Ă�����

			// �I������
			m_apNumber[nCntScore]->Release();
			m_apNumber[nCntScore]->Uninit();
			delete m_apNumber[nCntScore];
			m_apNumber[nCntScore] = NULL;
		}
	}

	// �I������
	CObject2D::Uninit();
}

//==================================================================================
// �������
//==================================================================================
void CStageSwitch::Release(void)
{
	for (int nCntScore = 0; nCntScore < NUM_STAGE; nCntScore++)
	{
		// �I������
		if (m_apNumber[nCntScore] != NULL)
		{// �������̊m�ۂ�����Ă�����

			// �������
			m_apNumber[nCntScore]->Release();
			m_apNumber[nCntScore]->Uninit();
			delete m_apNumber[nCntScore];
			m_apNumber[nCntScore] = NULL;
		}
	}
}

//==================================================================================
// �X�V����
//==================================================================================
void CStageSwitch::Update(void)
{
	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	switch (CManager::GetMode())
	{
	case CScene::MODE_GAME:
		UpdateGame();
		break;

	case CScene::MODE_TITLE:
		UpdateTitle();
		break;

	default:
		break;
	}
}

//==================================================================================
// �Q�[���̍X�V
//==================================================================================
void CStageSwitch::UpdateGame(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// ���݂̃X�e�[�W�擾
	int nNowStage = CManager::GetScene()->GetWaveManager()->GetNowStage();

	// �X�e�[�W�؂�ւ�����
	nNowStage = CManager::GetScene()->GetWaveManager()->GetNowStage();
	CGame::GetObject3DMesh()->StageSwitch(nNowStage, nNowStage + 1, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_UP)->StageSwitch(nNowStage, nNowStage + 1, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_RIGHT)->StageSwitch(nNowStage, nNowStage + 1, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_DOWN)->StageSwitch(nNowStage, nNowStage + 1, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_LEFT)->StageSwitch(nNowStage, nNowStage + 1, MAX_LIFE - m_nLife, MAX_LIFE);

	// �������炷
	m_nLife--;

	if (m_nLife <= 60)
	{
		nNowStage += 1;
	}

	if (m_nLife <= 0)
	{
		// �T�E���h��~
		CManager::GetSound()->StopSound(CSound::LABEL_SE_STAGESWITCH);

		CManager::GetScene()->GetWaveManager()->SetEnableStageEnd();
		// �I������
		Release();
		Uninit();
		return;
	}

	// �l�̐ݒ�
	SetValue(nNowStage);

	// �ʒu�X�V
	pos += move;

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// ���_���W�̐ݒ�
	SetVtx();
}

//==================================================================================
// �^�C�g���̍X�V
//==================================================================================
void CStageSwitch::UpdateTitle(void)
{
	// ���݂̃X�e�[�W�擾
	CTitle *pTitle = (CTitle*)CManager::GetScene();
	int nNowStage = pTitle->GetSwitchWave();
	int nNextStage = (nNowStage + 1)/* % CScene::GetWaveManager()->GetMaxStage()*/;

	if (nNextStage > CManager::GetScene()->GetWaveManager()->GetMaxStage())
	{
		nNextStage = 0;
	}

	// �X�e�[�W�؂�ւ�����
	CScene::GetObject3DMesh()->StageSwitch(nNowStage, nNextStage, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_UP)->StageSwitch(nNowStage, nNextStage, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_RIGHT)->StageSwitch(nNowStage, nNextStage, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_DOWN)->StageSwitch(nNowStage, nNextStage, MAX_LIFE - m_nLife, MAX_LIFE);
	CScene::GetElevationOutside(CElevation::POSTYPE_LEFT)->StageSwitch(nNowStage, nNextStage, MAX_LIFE - m_nLife, MAX_LIFE);

	// �������炷
	m_nLife--;
	if (m_nLife <= 0)
	{
		// �T�E���h��~
		CManager::GetSound()->StopSound(CSound::LABEL_SE_STAGESWITCH);

		CManager::GetScene()->GetWaveManager()->SetEnableStageEnd();
		// �I������
		Release();
		Uninit();
		return;
	}
}

//==================================================================================
// �l�̐ݒ菈��
//==================================================================================
void CStageSwitch::SetValue(int nValue)
{
	int aTexU[NUM_STAGE];
	int nDigit = 1;		// aTexU�v�Z�p

	// �v�Z�p����o��
	for (int nCntDigit = 0; nCntDigit < NUM_STAGE; nCntDigit++)
	{
		nDigit *= 10;
	}

	// �e�N�X�`�����W�ɑ������
	for (int nCntTex = 0; nCntTex < NUM_STAGE; nCntTex++)
	{// �������ݒ�

		aTexU[nCntTex] = nValue % nDigit / (nDigit / 10);
		nDigit /= 10;
	}

	for (int nCntNumber = 0; nCntNumber < NUM_STAGE; nCntNumber++)
	{
		if (m_apNumber[nCntNumber] != NULL)
		{// NULL����Ȃ�������

			// ���_�ݒ�
			m_apNumber[nCntNumber]->GetObject2D()->SetVtx();

			D3DXVECTOR2 *pTex = m_apNumber[nCntNumber]->GetObject2D()->GetTex();

			// �e�N�X�`�����W�̐ݒ�
			pTex[0] = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f + 0.1f, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU[nCntNumber] * 0.1f + 0.1f, 1.0f);
		}
	}
}

//==================================================================================
// �`�揈��
//==================================================================================
void CStageSwitch::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// �`�揈��
	//CObject2D::Draw();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CStageSwitch::SetVtx(void)
{
	// ���_�ݒ�
	CObject2D::SetVtx();
}

