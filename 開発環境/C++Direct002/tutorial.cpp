//=============================================================================
// 
//  �`���[�g���A������ [tutorial.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "tutorial.h"
#include "input.h"
#include "fade.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "enemymanager.h"
#include "player.h"
#include "tutorialstep.h"
#include "tutorialtext.h"
#include "sound.h"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CTutorialStep *CTutorial::m_pStep = NULL;	// �X�e�b�v
bool CTutorial::m_bMovingPlayer = false;	// �v���C���[�������Ă锻��

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTutorial::CTutorial()
{
	// �l�̃N���A
	m_pPlayer = NULL;	// �v���C���[�̃I�u�W�F�N�g
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTutorial::~CTutorial()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTutorial::Init(void)
{

	// �e��ϐ��̏�����


	// ����������
	if (FAILED(CScene::Init()))
	{// ���s�����ꍇ
		return E_FAIL;
	}

	// BGM�Đ�
	CManager::GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

	//**********************************
	// �X�e�b�v
	//**********************************
	m_pStep = CTutorialStep::Create();

	//**********************************
	// �e�L�X�g
	//**********************************
	CTutorialText::Create();

	//**********************************
	// �v���C���[
	//**********************************
	m_pPlayer = CPlayer::Create();
	m_pPlayer->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1000.0f));
	m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));

	// ����
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CTutorial::Uninit(void)
{
	// �v���C���[�̔j��
	if (m_pPlayer != NULL)
	{// �������̊m�ۂ��o���Ă�����

		m_pPlayer->Kill();
		m_pPlayer = NULL;
	}

	// �X�e�b�v�̔j��
	if (m_pStep != NULL)
	{// �������̊m�ۂ��o���Ă�����

		m_pStep->Uninit();
		delete m_pStep;
		m_pStep = NULL;
	}

	// �I������
	CScene::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CTutorial::Update(void)
{
	CManager::GetDebugProc()->Print(
		"���݂̃��[�h�F�y�`���[�g���A���z\n"
		"�؂�ւ��F�y F �z\n\n");

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	if (/*m_pStep->GetNowStep() >= CTutorialStep::STEP_MAX - 1 &&*/
		(pInputKeyboard->GetTrigger(DIK_BACKSPACE) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_START, 0) == true))
	{
		// ���[�h�ݒ�
		CManager::GetFade()->SetFade(CScene::MODE_GAME);
	}

	// �G�}�l�[�W���̍X�V����
	GetEnemyManager()->Update();

	// �X�e�b�v�̍X�V����
	if (m_pStep != NULL)
	{// NULL����Ȃ�������
		m_pStep->Update();
	}

}

//==========================================================================
// �`�揈��
//==========================================================================
void CTutorial::Draw(void)
{

}

//==========================================================================
// �v���C���[�������Ă锻��
//==========================================================================
void CTutorial::SetEnableMovingPlayer(void)
{
	m_bMovingPlayer = true;
}

//==========================================================================
// �v���C���[�������Ă锻��OFF
//==========================================================================
void CTutorial::SetDesableMovingPlayer(void)
{
	m_bMovingPlayer = false;
}

//==========================================================================
// �v���C���[�������Ă锻��擾
//==========================================================================
bool CTutorial::IsMovingPlayer(void)
{
	return m_bMovingPlayer;
}

//==========================================================================
// �v���C���[�̎擾
//==========================================================================
CPlayer *CTutorial::GetPlayer(void)
{
	return m_pPlayer;
}

//==========================================================================
// �v���C���[�̎擾
//==========================================================================
CPlayer **CTutorial::GetPlayer(int n)
{
	return &m_pPlayer;
}

//==========================================================================
// �X�e�b�v�擾
//==========================================================================
CTutorialStep *CTutorial::GetStep(void)
{
	return m_pStep;
}
