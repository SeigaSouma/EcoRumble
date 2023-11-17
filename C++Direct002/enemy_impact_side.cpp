//=============================================================================
// 
//  �g�N�����G���� [enemy_impact_side.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "enemy_impact_side.h"
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
CEnemyImpactSide::CEnemyImpactSide(int nPriority) : CEnemy(nPriority)
{
	// �l�̃N���A
	
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyImpactSide::~CEnemyImpactSide()
{

}


//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyImpactSide::Init(void)
{
	// ����������
	CEnemy::Init();

	// �̗͎擾
	int nLife = GetLife();

	// �̗̓Q�[�W
	m_pHPGauge = CHP_Gauge::Create(350.0f, nLife, 3.0f);
	m_state = STATE_SPAWN;
	m_Oldstate = STATE_TURRETWAIT;
	m_nCntState = 0;

	if (m_pHPGauge == NULL)
	{// NULL��������
		m_pHPGauge = NULL;
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyImpactSide::Uninit(void)
{
	// �I������
	CEnemy::Uninit();
}

//==========================================================================
// ���S����
//==========================================================================
void CEnemyImpactSide::Kill(void)
{
	// ���S����
	CEnemy::Kill();
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyImpactSide::Update(void)
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
}

//==========================================================================
// ���n���̏���
//==========================================================================
void CEnemyImpactSide::ProcessLanding(void)
{
	// ���n���̏���
	CEnemy::ProcessLanding();
}

//==========================================================================
// �U����Ԉڍs����
//==========================================================================
void CEnemyImpactSide::ChangeToAttackState(void)
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
	}
}

//==========================================================================
// ��ޕʏ�ԍX�V����
//==========================================================================
void CEnemyImpactSide::UpdateStateByType(void)
{
	return;
}

//==========================================================================
// �o��
//==========================================================================
void CEnemyImpactSide::Spawn(void)
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
		m_nCntState = 60 * 1;	// �J�ڃJ�E���^�[�ݒ�
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
void CEnemyImpactSide::ChaseMove(float fMove)
{
	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �ړ��ʉ��Z
	move.x += sinf(D3DX_PI + rot.y) * fMove;
	move.z += cosf(D3DX_PI + rot.y) * fMove;

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// ���[�V�����̐ݒ�
//==========================================================================
void CEnemyImpactSide::MotionSet(void)
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
void CEnemyImpactSide::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
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

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// ��ޕ�
	switch (m_pMotion->GetType())
	{
	case MOTION_WALK:
		break;

	case MOTION_ATK:
		// �U��
		CManager::GetCamera()->SetShake(20, 5.0f, 0.0f);

		// �g����
		CGame::GetObject3DMesh()->SetWave
		(
			D3DXVECTOR3(
				weponpos.x + sinf(D3DX_PI + rot.y) * 250.0f,
				weponpos.y,
				weponpos.z + cosf(D3DX_PI + rot.y) * 250.0f),	// �ʒu
			D3DXVECTOR3(
				0.0f,
				rot.y,
				0.0f),									// ����
			D3DXVECTOR3(
				sinf(D3DX_PI + rot.y) * 20.0f,
				0.0f,
				cosf(D3DX_PI + rot.y) * 20.0f),					// �ړ���
			D3DXVECTOR2(70.0f, 300.0f),									// ����͈�
			CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// �g�����̕p�x
			-300.0f,														// �g�̍���
			D3DX_PI * 0.0f,													// �g�łp�x
			60,															// ��������
			CElevation::WAVETYPE_DIRECT,										// ���
			CElevation::TYPE_ENEMY,								// �v���C���[�̎��
			0
		);
		break;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEnemyImpactSide::Draw(void)
{
	// �`�揈��
	CEnemy::Draw();
}

//==========================================================================
// �G�̏��擾
//==========================================================================
CEnemyImpactSide *CEnemyImpactSide::GetEnemy(void)
{
	// �������g�̃|�C���^���擾
	return this;
}