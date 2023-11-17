//=============================================================================
// 
//  ���U���g���� [result.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "result.h"
#include "input.h"
#include "fade.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "result_screen.h"
#include "player.h"
#include "sound.h"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CResultScreen *CResult::m_pResultScreen = NULL;	// ���U���g�X�N���[���̃I�u�W�F�N�g
bool CResult::m_bAllArrival = false;		// �S�ē�����������

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CResult::CResult()
{
	// �l�̃N���A
	m_pPlayer = NULL;		// �v���C���[�̃I�u�W�F�N�g
	m_bAllArrival = false;	// �S�ē�����������
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CResult::~CResult()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CResult::Init(void)
{

	// ����������
	if (FAILED(CScene::Init()))
	{// ���s�����ꍇ
		return E_FAIL;
	}

	// BGM�Đ�
	CManager::GetSound()->PlaySound(CSound::LABEL_BGM_RESULT);

	// ���U���g���
	m_pResultScreen = CResultScreen::Create();

	//**********************************
	// �v���C���[
	//**********************************
	m_pPlayer = CPlayer::Create();
	m_pPlayer->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// ����
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CResult::Uninit(void)
{
	m_pResultScreen = NULL;

	// �v���C���[�̔j��
	if (m_pPlayer != NULL)
	{// �������̊m�ۂ��o���Ă�����

		m_pPlayer->Kill();
		m_pPlayer = NULL;
	}

	// �I������
	CScene::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CResult::Update(void)
{
	CManager::GetDebugProc()->Print(
		"���݂̃��[�h�F�y���U���g�z\n"
		"�؂�ւ��F�y F �z\n\n");

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	static int n = 0;
	n = (n + 1) % 5;
	if (n == 0)
	{
		// ���[�h�ݒ�
		//CManager::GetFade()->SetFade(CScene::MODE_TITLE);
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true)
	{
		if (m_bAllArrival == true)
		{
			// ���[�h�ݒ�
			CManager::GetFade()->SetFade(CScene::MODE_RANKING);
		}

		if (CManager::GetFade()->GetState() == CFade::STATE_NONE)
		{
			// �S�Ă̓�������
			m_pResultScreen->SetAllArrival();
			m_bAllArrival = true;
		}
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CResult::Draw(void)
{

}

//==========================================================================
// ���U���g�X�N���[���̏��擾
//==========================================================================
CResultScreen *CResult::GetResultScreen(void)
{
	return m_pResultScreen;
}

//==========================================================================
// �v���C���[�̎擾
//==========================================================================
CPlayer *CResult::GetPlayer(void)
{
	return m_pPlayer;
}

//==========================================================================
// �v���C���[�̎擾
//==========================================================================
CPlayer **CResult::GetPlayer(int n)
{
	return &m_pPlayer;
}

//==========================================================================
// �����ݒ�ON
//==========================================================================
void CResult::SetEnableArrival(void)
{
	m_bAllArrival = true;
}
