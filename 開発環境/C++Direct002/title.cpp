//=============================================================================
// 
//  �^�C�g������ [title.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "title_screen.h"
#include "wavemanager.h"
#include "stageswitch.h"
#include "enemymanager.h"
#include "enemy.h"
#include "sound.h"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTitle::CTitle()
{
	// �l�̃N���A
	m_nSwitchWave = 0;		// �E�F�[�u�̑J��
	m_nCntSwitch = 0;		// �؂�ւ��̃J�E���^�[
	m_bStageSwitch = false;		// �X�e�[�W�؂�ւ��̃t���O
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTitle::~CTitle()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTitle::Init(void)
{

	// BGM�Đ�
	CManager::GetSound()->PlaySound(CSound::LABEL_BGM_TITLE);

	//int nMax = CScene::GetWaveManager()->GetMaxStage() - 1;
	// �ŏ��̃X�e�[�W�����_��
	m_nSwitchWave = Random(0, 2);

	// ����������
	if (FAILED(CScene::Init()))
	{// ���s�����ꍇ
		return E_FAIL;
	}

	// �^�C�g�����
	CTitleScreen::Create();

	// �G�z�u
	CScene::GetEnemyManager()->SetSpawnWave(m_nSwitchWave, 0);
	m_bStageSwitch = false;		// �X�e�[�W�؂�ւ��̃t���O
	m_nCntSwitch = 0;		// �؂�ւ��̃J�E���^�[

	// �X�e�[�W�؂�ւ�����
	CStageSwitch::Create();

	// ����
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CTitle::Uninit(void)
{
	// �I������
	CScene::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CTitle::Update(void)
{
	CManager::GetDebugProc()->Print(
		"���݂̃��[�h�F�y�^�C�g���z\n"
		"�؂�ւ��F�y F �z\n\n");

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	// �؂�ւ��̃J�E���^�[���Z
	m_nCntSwitch++;

	if (pInputKeyboard->GetTrigger(DIK_RETURN) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true)
	{
		// ���[�h�ݒ�
		CManager::GetFade()->SetFade(CScene::MODE_TUTORIAL);
	}

	if (m_nCntSwitch >= 60 * 40)
	{// �����J��

		// ���[�h�ݒ�
		CManager::GetFade()->SetFade(CScene::MODE_RANKING);
	}

//#if _DEBUG
	if (pInputKeyboard->GetTrigger(DIK_R) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true)
	{

		// ���[�h�ݒ�
		CManager::GetFade()->SetFade(CScene::MODE_RANKING);
	}
//#endif

	if (CScene::GetWaveManager()->IsStageFrag() == true)
	{// �X�e�[�W�؂�ւ����I����Ă�����

		if (m_bStageSwitch == true)
		{// �I����Ă�����

			// �؂�ւ�����I�t
			m_bStageSwitch = false;

			// �I������ɂ���
			CScene::GetWaveManager()->SetDisableStageEnd();

			// �X�e�[�W�؂�ւ�
			m_nSwitchWave = (m_nSwitchWave + 1) % CScene::GetWaveManager()->GetMaxStage();


			// �擪��ۑ�
			CObject *pObj = CObject::GetTop(mylib_const::ENEMY_PRIORITY);

			while (pObj != NULL)
			{// NULL������܂Ŗ������[�v

				// ���̃I�u�W�F�N�g���ꎞ�ۑ�
				CObject *pObjNext = pObj->GetNext();

				// ���S�̔���
				if (pObj->IsDeath() == true)
				{// ���S�t���O�������Ă�����

					// ���̃I�u�W�F�N�g����
					pObj = pObjNext;
					continue;
				}

				// ��ނ̎擾
				if (pObj->GetType() != CObject::TYPE_ENEMY)
				{// �G����Ȃ�������

					// ���̃I�u�W�F�N�g����
					pObj = pObjNext;
					continue;
				}

				// ��Ԑݒ�
				pObj->GetObjectChara()->SetState(CEnemy::STATE_FADEOUT);

				// ���̃I�u�W�F�N�g����
				pObj = pObjNext;
			}


			// �G�z�u
			CScene::GetEnemyManager()->SetSpawnWave(m_nSwitchWave, 0);

			// �X�e�[�W�؂�ւ�����
			CStageSwitch::Create();
		}
	}

}

//==========================================================================
// �`�揈��
//==========================================================================
void CTitle::Draw(void)
{

}

//==========================================================================
// �X�e�[�W�̐؂�ւ��ݒ�
//==========================================================================
void CTitle::SetStageSwitch(void)
{
	m_bStageSwitch = true;
}

//==========================================================================
// �E�F�[�u�J�ڎ擾
//==========================================================================
int CTitle::GetSwitchWave(void)
{
	return m_nSwitchWave;
}
