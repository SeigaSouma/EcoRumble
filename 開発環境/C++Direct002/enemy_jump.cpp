//=============================================================================
// 
//  �W�����v�G���� [enemy_jump.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "enemy_jump.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "bullet.h"
#include "explosion.h"
#include "particle.h"
#include "collision.h"
#include "motion.h"
#include "model.h"
#include "elevation.h"
#include "player.h"
#include "enemymanager.h"
#include "camera.h"
#include "ballast.h"
#include "3D_Effect.h"
#include "hp_gauge.h"
#include "shadow.h"
#include "sound.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define WIDTH			(60.0f)							// ����
#define HEIGHT			(60.0f)							// �c��
#define MAX_LIFE		(5)								// �̗�
#define PLAYER_SERCH	(800.0f)	// �v���C���[�T���͈�
#define CHACE_DISTABCE	(50.0f)		// �ǂ��|���鎞�̊Ԋu
#define JUMP			(18.0f)					// �W�����v�͏����l

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyJump::CEnemyJump(int nPriority) : CEnemy(nPriority)
{
	// �l�̃N���A
	m_rotParts = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �p�[�c��]�p
	m_nCntWaitJump = 0;		// �W�����v�̑ҋ@����

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyJump::~CEnemyJump()
{

}


//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyJump::Init(void)
{
	// ����������
	CEnemy::Init();

	// �̗͎擾
	int nLife = GetLife();

	// �̗̓Q�[�W
	m_pHPGauge = CHP_Gauge::Create(200.0f, nLife);

	if (m_pHPGauge == NULL)
	{// NULL��������
		m_pHPGauge = NULL;
	}

	// �W�����v�ҋ@����
	m_nCntWaitJump = 30;

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyJump::Uninit(void)
{
	// �I������
	CEnemy::Uninit();
}

//==========================================================================
// ���S����
//==========================================================================
void CEnemyJump::Kill(void)
{
	// ���S����
	CEnemy::Kill();
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyJump::Update(void)
{
	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �X�V����
	CEnemy::Update();

	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// ���݂̃��[�V������ގ擾
	int nType = m_pMotion->GetType();

	// �W�����v�ҋ@���Ԍ��Z
	m_nCntWaitJump--;
	if (m_nCntWaitJump <= 0)
	{
		m_nCntWaitJump = 0;
	}

	if (nType == MOTION_WALK)
	{// �ړ����[�V������

		// ���p�[�c
		CModel *pModel = GetObjectChara()->GetModel()[1];

		// �����擾
		D3DXVECTOR3 rotParts = pModel->GetRotation();

		// ��]������
		m_rotParts.y += D3DX_PI * 0.2f;

		// �p�x�̐��K��
		RotNormalize(m_rotParts.y);

		// �����ݒ�
		pModel->SetRotation(D3DXVECTOR3(rotParts.x, m_rotParts.y, rotParts.z));

	}
	else
	{// �C�ӂ̃��[�V�����ȊO

		// ���p�[�c
		CModel *pModel = GetObjectChara()->GetModel()[1];

		// �����擾
		D3DXVECTOR3 rotParts = pModel->GetRotation();

		// ���ɖ߂�
		InertiaCorrection(m_rotParts.y, 0.0f, 0.25f);

		// �p�x�̐��K��
		RotNormalize(m_rotParts.y);

		// �����ݒ�
		pModel->SetRotation(D3DXVECTOR3(rotParts.x, m_rotParts.y, rotParts.z));
	}

}

//==========================================================================
// ���n���̏���
//==========================================================================
void CEnemyJump::ProcessLanding(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �g����
	if (m_state != STATE_NONE && m_state != STATE_DEAD  && m_sMotionFrag.bJump == true)
	{
		if (m_state == STATE_ATTACK)
		{
			CGame::GetObject3DMesh()->SetWave
			(
				pos,												// �ʒu
				GetRotation(),										// ����
				D3DXVECTOR3(
					0.0f,
					0.0f,
					0.0f),											// �ړ���
				D3DXVECTOR2(2000.0f, 2000.0f),						// ����͈�
				CGame::GetObject3DMesh()->GetWidthLen() * 1.0f,		// �g�����̕p�x
				-120.0f,											// �g�̍���
				10.0f,												// �g�̍L����
				30,													// ��������
				CElevation::WAVETYPE_IMPACT,						// ���
				CElevation::TYPE_ENEMY,							// �v���C���[�̎��
				0													// ���x��
			);
		}
		else
		{
			CGame::GetObject3DMesh()->SetWave
			(
				pos,												// �ʒu
				GetRotation(),										// ����
				D3DXVECTOR3(
					0.0f,
					0.0f,
					0.0f),											// �ړ���
				D3DXVECTOR2(2000.0f, 2000.0f),						// ����͈�
				CGame::GetObject3DMesh()->GetWidthLen() * 1.0f,		// �g�����̕p�x
				-60.0f,											// �g�̍���
				4.0f,												// �g�̍L����
				20,													// ��������
				CElevation::WAVETYPE_IMPACT,						// ���
				CElevation::TYPE_EFFECT,							// �v���C���[�̎��
				0													// ���x��
			);
		}

		if (m_state == STATE_ATTACK)
		{
			m_nCntWaitJump = 100;
		}
		else
		{
			m_nCntWaitJump = 30;
		}
	}

	// ���n���̏���
	CEnemy::ProcessLanding();
}

//==========================================================================
// �U����Ԉڍs����
//==========================================================================
void CEnemyJump::ChangeToAttackState(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �v���C���[���
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{
		return;
	}

	// �e�̈ʒu�擾
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	if (CircleRange(pos, posPlayer, 400.0f, pPlayer->GetRadius()) == true && m_sMotionFrag.bJump == false)
	{// ��苗���ԂɃv���C���[����������

		// �U����Ԃɂ���
		m_state = STATE_ATTACK;
		m_sMotionFrag.bATK = true;

		if (m_nCntWaitJump > 0)
		{// �W�����v�ҋ@���͍U�����Ȃ�
			m_sMotionFrag.bATK = false;
		}
	}
}

//==========================================================================
// �o��
//==========================================================================
void CEnemyJump::Spawn(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �ʒu�X�V
	pos = (D3DXVECTOR3(0.0f, SPAWN_GRAVITY, 0.0f) * (float)(m_nCntState * m_nCntState) + move * (float)m_nCntState) + m_posOrigin;

	// �N���Ƃ̔���
	if (CGame::GetObject3DMesh()->IsHit(pos))
	{// �n�ʂƓ������Ă�����
		m_state = m_Oldstate;	// �ߋ��̏��ۑ�
		m_nCntState = 0;	// �J�ڃJ�E���^�[�ݒ�
		move.y = 0.0f;			// �ړ��ʃ[��
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// �ǂ��|���ړ�
//==========================================================================
void CEnemyJump::ChaseMove(float fMove)
{
	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	if (m_nCntWaitJump <= 0)
	{// �W�����v�ҋ@���Ԃ��I����Ă�����

		// �ړ��ʉ��Z
		move.x += sinf(D3DX_PI + rot.y) * fMove;
		move.z += cosf(D3DX_PI + rot.y) * fMove;
	}

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// ���[�V�����̐ݒ�
//==========================================================================
void CEnemyJump::MotionSet(void)
{
	if (m_pMotion->IsFinish() == true)
	{// �I�����Ă�����

		// ���݂̎�ގ擾
		int nType = m_pMotion->GetType();

		if (m_sMotionFrag.bMove == true && m_sMotionFrag.bKnockback == false)
		{// �ړ����Ă�����

			m_sMotionFrag.bMove = false;	// �ړ�����OFF

			// �ړ����[�V����
			m_pMotion->Set(MOTION_WALK);
		}
		else if (m_sMotionFrag.bKnockback == true)
		{// ���ꒆ��������

			// ���ꃂ�[�V����
			m_pMotion->Set(MOTION_KNOCKBACK);
		}
		else if (m_sMotionFrag.bATK == true)
		{// �U�����Ă�����

			m_sMotionFrag.bATK = false;		// �U������OFF

			// �U�����[�V����
			m_pMotion->Set(MOTION_ATK);
		}
		else
		{
			// �j���[�g�������[�V����
			m_pMotion->Set(MOTION_DEF);
		}
	}
}

//==========================================================================
// �U��������
//==========================================================================
void CEnemyJump::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
{
	D3DXMATRIX mtxTrans;	// �v�Z�p�}�g���b�N�X�錾

	// �U��������
	CEnemy::AttackAction(nModelNum, ATKInfo);

	if (GetObjectChara()->GetModel()[nModelNum] == NULL)
	{// NULL��������
		return;
	}

	// ���肷��p�[�c�̃}�g���b�N�X�擾
	D3DXMATRIX mtxWepon = GetObjectChara()->GetModel()[nModelNum]->GetWorldMtx();

	// �ʒu�𔽉f����
	D3DXMatrixTranslation(&mtxTrans, ATKInfo.Offset.x, ATKInfo.Offset.y, ATKInfo.Offset.z);
	D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

	// ����̈ʒu
	D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// ��ޕ�
	switch (m_pMotion->GetType())
	{
	case MOTION_WALK:
		// �W�����v������
		m_sMotionFrag.bJump = true;
		move.y = 0.0f;
		move.y += JUMP;

		// �ړ��ʐݒ�
		SetMove(move);
		break;

	case MOTION_ATK:

		if (m_sMotionFrag.bJump == false)
		{
			// �W�����v������
			m_sMotionFrag.bJump = true;
			move.y = 0.0f;
			move.y += JUMP;

			// �ړ��ʐݒ�
			SetMove(move);
		}
		break;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEnemyJump::Draw(void)
{
	// �`�揈��
	CEnemy::Draw();
}

//==========================================================================
// �G�̏��擾
//==========================================================================
CEnemyJump *CEnemyJump::GetEnemy(void)
{
	// �������g�̃|�C���^���擾
	return this;
}