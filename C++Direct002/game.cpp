//=============================================================================
// 
//  �Q�[������ [game.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "fade.h"
#include "camera.h"

#include "input.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "timer.h"
#include "map.h"
#include "elevation.h"
#include "magma.h"
#include "damage_gate.h"
#include "power_gauge.h"
#include "grassblades.h"
#include "stageswitch.h"
#include "sound.h"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CScore *CGame::m_pScore = NULL;					// �X�R�A�̃I�u�W�F�N�g
CTimer *CGame::m_pTimer = NULL;					// �^�C�}�[�̃I�u�W�F�N�g
CPowerGauge *CGame::m_pPowerGauge = NULL;		// �p���[�Q�[�W�̃I�u�W�F�N�g

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CGame::CGame()
{
	// �l�̃N���A
	m_pPlayer = NULL;	// �v���C���[�̃I�u�W�F�N�g
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CGame::~CGame()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CGame::Init(void)
{

	// ����������
	if (FAILED(CScene::Init()))
	{// ���s�����ꍇ
		return E_FAIL;
	}

	// BGM�Đ�
	CManager::GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

	//**********************************
	// �v���C���[
	//**********************************
	// �������m��
	m_pPlayer = CPlayer::Create();
	m_pPlayer->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1000.0f));
	m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));

	// �X�R�A�̐�������
	m_pScore = CScore::Create(D3DXVECTOR3(1000.0f, 50.0f, 0.0f));

	// �^�C�}�[�̐�������
	m_pTimer = CTimer::Create(D3DXVECTOR3(200.0f, 50.0f, 0.0f));

	CGrassBlades::Create(D3DXVECTOR3(-160.0f, 0.0f, -20.0f));
	CGrassBlades::Create(D3DXVECTOR3(-1220.0f, 0.0f, -1016.0f));
	CGrassBlades::Create(D3DXVECTOR3(-20.0f, 200.0f, 280.0f));
	CGrassBlades::Create(D3DXVECTOR3(200.0f, 200.0f, 580.0f));

	CManager::GetCamera()->Reset(CScene::MODE_GAME);

	// �X�e�[�W�؂�ւ�����
	CStageSwitch::Create();

	// ����
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CGame::Uninit(void)
{
	// �v���C���[�̔j��
	if (m_pPlayer != NULL)
	{// �������̊m�ۂ��o���Ă�����

		m_pPlayer->Kill();
		m_pPlayer = NULL;
	}

	// �X�R�A�̔j��
	if (m_pScore != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pScore->Uninit();

		// �������̊J��
		delete m_pScore;
		m_pScore = NULL;
	}

	// �^�C�}�[�̔j��
	if (m_pTimer != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pTimer->Uninit();

		// �������̊J��
		delete m_pTimer;
		m_pTimer = NULL;
	}

	// �I������
	CScene::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CGame::Update(void)
{
	CManager::GetDebugProc()->Print(
		"���݂̃��[�h�F�y�Q�[���z\n"
		"�؂�ւ��F�y F �z\n\n");

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

#if 1
	// �X�R�A�̍X�V����
	m_pScore->Update();

	if (CManager::GetEdit() == NULL &&
		CScene::GetObject3DMesh()->IsEdit() == false &&
		GetWaveManager()->IsStageFrag() == true &&
		GetEnemyManager()->GetState() != CEnemyManager::STATE_COMBOANIM)
	{
		// �^�C�}�[�̍X�V����
		m_pTimer->Update();
	}
#endif
	// �G�}�l�[�W���̍X�V����
	GetEnemyManager()->Update();

	// �E�F�[�u�Ǘ��̍X�V����
	if (GetWaveManager() != NULL)
	{
		GetWaveManager()->Update();
	}

#if _DEBUG
	if (/*pInputKeyboard->GetTrigger(DIK_P) || */pInputGamepad->GetTrigger(CInputGamepad::BUTTON_BACK, 0) == true)
	{
		for (int nCnt = 0; nCnt < 20; nCnt++)
		{
			Reset();
		}
	}

	if (pInputKeyboard->GetTrigger(DIK_F))
	{
		// ���[�h�ݒ�
		CManager::GetFade()->SetFade(CScene::MODE_RESULT);
	}
#endif
}

//==========================================================================
// �`�揈��
//==========================================================================
void CGame::Draw(void)
{

}

//==========================================================================
// �v���C���[�̎擾
//==========================================================================
CPlayer *CGame::GetPlayer(void)
{
	return m_pPlayer;
}

//==========================================================================
// �v���C���[�̎擾
//==========================================================================
CPlayer **CGame::GetPlayer(int n)
{
	return &m_pPlayer;
}

//==========================================================================
// �X�R�A�̎擾
//==========================================================================
CScore *CGame::GetScore(void)
{
	return m_pScore;
}

//==========================================================================
// �^�C�}�[�̎擾
//==========================================================================
CTimer *CGame::GetTimer(void)
{
	return m_pTimer;
}

//==========================================================================
// �p���[�Q�[�W�̎擾
//==========================================================================
CPowerGauge *CGame::GetPowerGauge(void)
{
	return m_pPowerGauge;
}

//==========================================================================
// ���Z�b�g����
//==========================================================================
void CGame::Reset(void)
{
	// �G
	GetEnemyManager()->SetEnemy(
		Random(0, 4),
		D3DXVECTOR3(Random(-200, 200) * 10.0f, 0.0f, Random(-200, 200) * 10.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}