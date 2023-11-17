//=============================================================================
// 
//  �`���[�g���A���v���C���[���� [tutorialplayer.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "tutorial.h"
#include "tutorialplayer.h"
#include "tutorialstep.h"
#include "camera.h"
#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "input.h"
#include "motion.h"
#include "sound.h"
#include "particle.h"
#include "grassparticle.h"
#include "scene.h"
#include "wavemanager.h"
#include "calculation.h"

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTutorialPlayer::CTutorialPlayer(int nPriority) : CPlayer(nPriority)
{
	// �l�̃N���A
	
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTutorialPlayer::~CTutorialPlayer()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTutorialPlayer::Init(void)
{
	// �e�̏���������
	CPlayer::Init();

	return S_OK;
}

//==========================================================================
// ���[�h�ʏI������
//==========================================================================
void CTutorialPlayer::UninitByMode(void)
{

	// �v���C���[���
	//CPlayer **ppPlayer = CTutorial::GetPlayer(0);
	CScene *pScene = CManager::GetScene();
	if (pScene != NULL)
	{
		CPlayer **ppPlayer = CManager::GetScene()->GetPlayer(0);

		// �v���C���[��NULL
		*ppPlayer = NULL;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CTutorialPlayer::Update(void)
{
	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �e�̍X�V����
	CPlayer::Update();

	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �X�e�b�v���Ƃ̍X�V
	UpdateByStep();
}

//==========================================================================
// �X�e�b�v���Ƃ̍X�V����
//==========================================================================
void CTutorialPlayer::UpdateByStep(void)
{
	// �X�e�b�v�̐ݒ�
	//CTutorial::GetStep()->SetStep(CTutorialStep::STEPFRAG_SPEEDUP);
}


//==========================================================================
// ���쏈��
//==========================================================================
void CTutorialPlayer::Controll(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	// �J�����̏��擾
	CCamera *pCamera = CManager::GetCamera();

	// �J�����̌����擾
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// ���݂̎�ގ擾
	int nType = m_pMotion->GetType();

	// �ړ��ʎ擾
	float fMove = GetVelocity();

	// �����̊����擾
	m_fMoveRatio = pInputGamepad->GetStickPositionRatioL(0).y;

	if (m_fMoveRatio > 0.85f)
	{
		CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDUP);
	}

	if (m_fMoveRatio < -0.85f)
	{
		CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDDOWN);
	}

	static int nWalk = 0;
	if (nType == MOTION_WALK)
	{// ���s��

		if (m_fMoveSlideDest >= SLIDEMOVE * 0.35f)
		{// ������
			m_nCntWalk = (m_nCntWalk + 1) % 2;
		}
		else if (m_fMoveSlideDest < 0.0f)
		{
			m_nCntWalk = (m_nCntWalk + 1) % 30;
		}
		else
		{
			m_nCntWalk = (m_nCntWalk + 1) % 25;
		}

		nWalk = (nWalk + 1) % 25;
		if (nWalk == 0)
		{
			// ���s���Đ�
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_WALK);
		}

		if (m_nCntWalk == 0)
		{
			my_particle::Create(pos, my_particle::TYPE_MOVESMOKE);
			CGrassParticle::Create(pos, rot, D3DXVECTOR2(3.5f, 3.0f), 10);
		}
	}
	else
	{
		m_nCntWalk = 0;
	}


	if (m_pMotion->IsGetMove(nType) == 1 && m_state != STATE_CONFUSION && CTutorial::GetStep()->IsEndStep() == false)
	{// �ړ��\���[�V�����̎� && ��������Ȃ��Ƃ�

#ifndef AUTO_MOVE
		// �ړ����ɂ���
		m_bMove = true;
#endif

		//*************************************************
		// �����̕ύX
		//*************************************************
		// �X�e�B�b�N�̌����擾
		float fStickRot = pInputGamepad->GetStickRot(0);

		if (fStickRot > D3DX_PI * 0.75f || fStickRot < -D3DX_PI * 0.75f)
		{// �^���͖���
			fStickRot = 0.0f;
		}

		fRotDest = D3DX_PI + fStickRot + Camerarot.y;

		// �ڕW�̌����ݒ�
		SetRotDest(fRotDest);

		if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0 ||
			pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0 ||
			pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0 ||
			pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0 ||
			pInputKeyboard->GetPress(DIK_O) == true || pInputGamepad->GetPress(CInputGamepad::BUTTON_Y, 0) == true ||
			pInputKeyboard->GetPress(DIK_RETURN) == true || pInputGamepad->GetPress(CInputGamepad::BUTTON_A, 0) == true)
		{//�Ȃɂ��L�[�������ꂽ

			// �s�����Ă�ݒ�ɂ���
			CTutorial::SetEnableMovingPlayer();
		}
		else
		{
			// �s�����ĂȂ��ݒ�ɂ���
			CTutorial::SetDesableMovingPlayer();
		}

		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//���L�[�������ꂽ,���ړ�

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//A+W,����ړ�
				fRotDest = D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//A+S,�����ړ�
				fRotDest = D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//A,���ړ�
				fRotDest = D3DX_PI * 0.5f + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//D�L�[�������ꂽ,�E�ړ�

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//D+W,�E��ړ�
				fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//D+S,�E���ړ�
				fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//D,�E�ړ�
				fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
			}
		}

		// �����x�ݒ�
		if (pInputKeyboard->GetPress(DIK_W) == true)
		{// ��ړ�

			if (pInputKeyboard->GetPress(DIK_A) == true || pInputKeyboard->GetPress(DIK_D) == true)
			{// A or D
				m_fMoveRatio = 0.5f;
				CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDUP);
			}
			else
			{
				m_fMoveRatio = 1.0f;
				CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDUP);
			}
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{// ���ړ�

			if (pInputKeyboard->GetPress(DIK_A) == true || pInputKeyboard->GetPress(DIK_D) == true)
			{// A or D
				m_fMoveRatio = -0.5f;
				CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDDOWN);
			}
			else
			{
				m_fMoveRatio = -1.0f;
				CTutorial::GetStep()->SetStep(CTutorialStep::STEP_SPEEDDOWN);
			}
		}
	}
	else if (m_state != STATE_CONFUSION)
	{// �ړ��\���[�V��������Ȃ��ꍇ
#if 1
		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//���L�[�������ꂽ,���ړ�

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//A+W,����ړ�
				fRotDest = D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//A+S,�����ړ�
				fRotDest = D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//A,���ړ�
				fRotDest = D3DX_PI * 0.5f + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//D�L�[�������ꂽ,�E�ړ�

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//D+W,�E��ړ�
				fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//D+S,�E���ړ�
				fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//D,�E�ړ�
				fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
			}
		}
#endif
	}

	// �ړ��̊����ňړ���ޕς���
	if (m_fMoveRatio >= 0.5f)
	{
		// �����ړ�
		m_walkType = WALKTYPE_SPEED;
	}
	else if (m_fMoveRatio <= -0.75f)
	{
		// �x���ړ�
		m_walkType = WALKTYPE_SLOW;
	}
	else
	{
		// �ʏ�ړ�
		m_walkType = WALKTYPE_NORMAL;
	}

	// �ړ��ʐݒ�
#ifndef AUTO_MOVE
	fMove = fMove + ((fMove * 0.5f) * m_fMoveRatio);
#else
	fMove = fMove + ((fMove * 0.5f) * 1.0f);
	fMove *= 1.5f;
#endif

	if (m_state == STATE_DMG || m_state == STATE_KNOCKBACK)
	{// ������я�Ԃ̂Ƃ�
		fRotDest = rot.y;
	}

	//�p�x�̐��K��
	RotNormalize(fRotDest);

	//���݂ƖڕW�̍��������߂�
	float fRotDiff = fRotDest - rot.y;

	//�p�x�̐��K��
	RotNormalize(fRotDiff);

	//�p�x�̕␳������
	if (m_state != STATE_CONFUSION)
	{// ������я�Ԃ̂Ƃ�
		rot.y += fRotDiff * 0.014f;
	}

	//�p�x�̐��K��
	RotNormalize(rot.y);

	// �X���C�h
	m_fMoveSlideDest = CScene::GetObject3DMesh()->Slide(pos);
	m_fMoveSlideDest *= SLIDEMOVE;

	m_fMoveSlide += (m_fMoveSlideDest - m_fMoveSlide) * 0.1f;

#ifndef AUTO_MOVE
	if (CTutorial::GetStep()->IsEndStep() == false)
	{// �I�����͈ꎞ��~

		if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_CONFUSION)
		{
			move.x += sinf(D3DX_PI + rot.y) * (fMove + m_fMoveSlide);
			move.z += cosf(D3DX_PI + rot.y) * (fMove + m_fMoveSlide);
		}
	}
#endif

	// �d�͏���
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		move.y -= mylib_const::GRAVITY;
	}

	// �ʒu�X�V
	pos += move;

	// �����␳
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);

	//**********************************
	// �����蔻��
	//**********************************
	Collision();

#if _DEBUG
	if (pInputKeyboard->GetPress(DIK_H) == true)
	{//���L�[�������ꂽ,���ړ�

		// �U���^�C�v�ύX
		CTutorial::GetStep()->SetStep(CTutorial::GetStep()->GetNowStep());
		//CTutorial::GetStep()->AddStep();
	}
#endif

	if (m_state != STATE_ATTACK && m_state != STATE_CONFUSION && m_state != STATE_DMG && m_state != STATE_KNOCKBACK)
	{
		if (CTutorial::GetStep()->GetNowStep() >= CTutorialStep::STEP_DIRECTWAVE&&
			(pInputKeyboard->GetTrigger(DIK_O) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_Y, 0) == true))
		{//���L�[�������ꂽ,���ړ�

			// �U���^�C�v�ύX
			m_Shotmode = ATK_DIRECT;
			m_bATK = true;		// �U������ON
		}

		if (CTutorial::GetStep()->GetNowStep() >= CTutorialStep::STEP_IMPACTWAVE &&
			(pInputKeyboard->GetTrigger(DIK_RETURN) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true) &&
			m_bATK == false && nType != MOTION_DIRECTATK && nType != MOTION_SUPERATK)
		{// �U��

			// �U���^�C�v�ύX
			m_Shotmode = ATK_DEBUG_POWER;
			m_bATK = true;		// �U������ON

			// �ڕW�̋����ݒ�
			pCamera->SetLenDest(pCamera->GetOriginDistance() + 200.0f);
		}
	}
}