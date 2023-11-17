//=============================================================================
// 
//  �Q�̓G���� [enemy_crowd.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "enemy_crowd.h"
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
CEnemyCrowd::CEnemyCrowd(int nPriority) : CEnemy(nPriority)
{
	// �l�̃N���A
	m_nCntEmission = 0;	// �������̃J�E���^�[
	m_nIntEmission = 0;	// �������̊Ԋu
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyCrowd::~CEnemyCrowd()
{

}


//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyCrowd::Init(void)
{
	// ����������
	CEnemy::Init();

	// �̗͎擾
	int nLife = GetLife();

	//// �̗̓Q�[�W
	//m_pHPGauge = CHP_Gauge::Create(80.0f, nLife, 0.8f);
	m_state = STATE_SPAWN;	// �e�ǂ��|�����

	if (m_SpawnType == CEnemyManager::SPAWNTYPE_CIRCLE || 
		m_SpawnType == CEnemyManager::SPAWNTYPE_CLOCK ||
		m_SpawnType == CEnemyManager::SPAWNTYPE_BOTHCLOCK)
	{// �~ or ���v��肾������
		m_Oldstate = STATE_PLAYERCHASE;
	}
	else
	{
		m_Oldstate = STATE_PARENTCHASE;
	}

	//if (m_pHPGauge == NULL)
	//{// NULL��������
	//	m_pHPGauge = NULL;
	//}

	m_nIntEmission = 3;	// �������̊Ԋu
	switch (m_colorType)
	{
	case COLORTYPE_NORMAL:
		break;

	case COLORTYPE_POISON:

		m_nIntEmission = 3;	// �������̊Ԋu
		break;

	case COLORTYPE_LUCKY:
		m_nIntEmission = 30;	// �������̊Ԋu
		break;

	case COLORTYPE_CONFUSION:
		m_nIntEmission = 90;	// �������̊Ԋu
		break;

	case COLORTYPE_SWIFTFOOT:
		m_nIntEmission = 1;	// �������̊Ԋu
		break;

	case COLORTYPE_TUTORIAL:
		m_Oldstate = STATE_WAIT;
		break;

	default:
		break;
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyCrowd::Uninit(void)
{
	// �I������
	CEnemy::Uninit();
}

//==========================================================================
// ���S����
//==========================================================================
void CEnemyCrowd::Kill(void)
{
	// ���S����
	CEnemy::Kill();
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyCrowd::Update(void)
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

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 rot = GetRotation();

	// �������̃J�E���^�[���Z
	m_nCntEmission = (m_nCntEmission + 1) % m_nIntEmission;

	if (m_nCntEmission == 0)
	{
		CEffect3D *pEffect = NULL;
		D3DXVECTOR3 spawnmove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		float fRadius = 0.0f;
		int nCntEffect = 0;

		switch (m_colorType)
		{
		case COLORTYPE_NORMAL:
			break;

		case COLORTYPE_POISON:

			// �o���ړ���
			spawnmove.x = sinf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 90) * 0.01f);
			spawnmove.z = cosf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 90) * 0.01f);
			spawnmove.y = (float)Random(20, 40) * 0.1f;

			fRadius = (float)Random(600, 800) * 0.1f;

			// �G�t�F�N�g����
			pEffect = CEffect3D::Create(
				pos,
				spawnmove,
				D3DXCOLOR(0.6f, 0.2f, 0.6f, 0.35f),
				fRadius,
				60,
				CEffect3D::MOVEEFFECT_SUB,
				CEffect3D::TYPE_SMOKE);

			if (pEffect != NULL)
			{
				// �Z�b�g�A�b�v�ʒu�ݒ�
				pEffect->SetUp(D3DXVECTOR3(0.0f, 20.0f, 0.0f), CObject::GetObject(), SetEffectParent(pEffect));
			}

			// ���a�����_������
			fRadius = (float)Random(200, 400) * 0.1f;

			// �G�t�F�N�g����
			pEffect = CEffect3D::Create(
				pos,
				spawnmove,
				D3DXCOLOR(0.8f, 0.8f, 0.2f, 0.2f),
				fRadius,
				60,
				CEffect3D::MOVEEFFECT_SUB,
				CEffect3D::TYPE_SMOKE);

			if (pEffect != NULL)
			{
				// �Z�b�g�A�b�v�ʒu�ݒ�
				pEffect->SetUp(D3DXVECTOR3(0.0f, 20.0f, 0.0f), CObject::GetObject(), SetEffectParent(pEffect));
			}
			break;

		case COLORTYPE_LUCKY:

			// �o���ړ���
			spawnmove.x = sinf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 100) * 0.01f);
			spawnmove.z = cosf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 100) * 0.01f);
			spawnmove.y = (float)Random(20, 100) * 0.01f;

			fRadius = (float)Random(100, 200) * 0.1f;

			pos.x += ((float)Random(-100, 100) * 0.1f) + sinf(D3DX_PI + rot.y) * 30.0f;
			pos.z += ((float)Random(-100, 100) * 0.1f) + cosf(D3DX_PI + rot.y) * 30.0f;
			pos.y += (float)Random(30, 50);

			// ��
			CEffect3D::Create(
				pos,
				spawnmove,
				D3DXCOLOR(1.0f, 0.8f, 0.1f, 1.0f),
				fRadius,
				90,
				CEffect3D::MOVEEFFECT_ADD,
				CEffect3D::TYPE_JUJI,
				fRadius * 0.05f);
			break;

		case COLORTYPE_CONFUSION:
			// �o���ړ���
			spawnmove.x = sinf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 80) * 0.01f);
			spawnmove.z = cosf((float)Random(-314, 314) * 0.01f) * ((float)Random(0, 80) * 0.01f);
			spawnmove.y = (float)Random(20, 100) * 0.01f;

			fRadius = (float)Random(300, 500) * 0.1f;

			pos.x += ((float)Random(-100, 100) * 0.1f) + sinf(D3DX_PI + rot.y) * 20.0f;
			pos.z += ((float)Random(-100, 100) * 0.1f) + cosf(D3DX_PI + rot.y) * 20.0f;
			pos.y += (float)Random(40, 60);

			// ��
			pEffect = CEffect3D::Create(
				pos,
				spawnmove,
				D3DXCOLOR(0.7f, 0.2f, 0.7f, 0.8f),
				fRadius,
				90,
				CEffect3D::MOVEEFFECT_ADD,
				CEffect3D::TYPE_SMOKEBLACK,
				fRadius * 0.025f);

			if (pEffect != NULL)
			{
				// �Z�b�g�A�b�v�ʒu�ݒ�
				pEffect->SetUp(D3DXVECTOR3(0.0f, 20.0f, 0.0f), CObject::GetObject(), SetEffectParent(pEffect));
			}


			// �o���ړ���
			fRadius = (float)Random(200, 400) * 0.1f;

			// ��
			pEffect = CEffect3D::Create(
				pos,
				spawnmove,
				D3DXCOLOR(0.2f, 0.9f, 0.9f, 0.3f),
				fRadius,
				90,
				CEffect3D::MOVEEFFECT_ADD,
				CEffect3D::TYPE_SMOKE,
				fRadius * 0.025f);

			if (pEffect != NULL)
			{
				// �Z�b�g�A�b�v�ʒu�ݒ�
				pEffect->SetUp(D3DXVECTOR3(0.0f, 20.0f, 0.0f), CObject::GetObject(), SetEffectParent(pEffect));
			}
			break;

		case COLORTYPE_SWIFTFOOT:
			// �o���ړ���
			spawnmove.x = sinf(D3DX_PI + rot.y) * -((float)Random(20, 40) * 0.1f);
			spawnmove.z = cosf(D3DX_PI + rot.y) * -((float)Random(20, 40) * 0.1f);
			spawnmove.y = 0.0f;

			fRadius = (float)Random(300, 350) * 0.1f;

			// ��
			CEffect3D::Create(
				D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z),
				spawnmove,
				D3DXCOLOR(0.3f, 0.3f, 0.8f, 0.3f),
				fRadius,
				25,
				CEffect3D::MOVEEFFECT_SUB,
				CEffect3D::TYPE_NORMAL,
				0.0f);
			break;

		default:
			break;
		}
	}

}

//==========================================================================
// ���n���̏���
//==========================================================================
void CEnemyCrowd::ProcessLanding(void)
{
	// ���n���̏���
	CEnemy::ProcessLanding();
}

//==========================================================================
// �U����Ԉڍs����
//==========================================================================
void CEnemyCrowd::ChangeToAttackState(void)
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

	float fRadius = 50.0f;

	if (m_colorType == COLORTYPE_POISON ||
		m_colorType == COLORTYPE_CONFUSION)
	{// �ŃL�m�R�͍U���I
		fRadius = 150.0f;
	}

	if (CircleRange(pos, posPlayer, fRadius, pPlayer->GetRadius()) == true && m_sMotionFrag.bJump == false)
	{// ��苗���ԂɃv���C���[����������

		// �U����Ԃɂ���
		m_state = STATE_ATTACK;
		m_sMotionFrag.bATK = true;
	}
}

//==========================================================================
// �o��
//==========================================================================
void CEnemyCrowd::Spawn(void)
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
// �U������
//==========================================================================
void CEnemyCrowd::StateAttack(void)
{
	// �U������
	CEnemy::StateAttack();

	// ���[�V�����̏��擾
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// �U�����̑����擾
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	bool bAtkWait = true;	// �U���ҋ@��
	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (aInfo.AttackInfo[nCntAttack] == NULL)
		{// NULL��������
			continue;
		}

		// ���[�V�����J�E���^�[�擾
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt)
		{// �U�����蒆

			// �U�����蒆�ɂ���
			bAtkWait = false;
		}
	}

	if (bAtkWait == false)
	{// ���蒆�̎�
		return;
	}


	// �v���C���[���
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{// NULL��������
		return;
	}

	// �e�̈ʒu�擾
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// �ڕW�̊p�x�����߂�
	fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

	// �ڕW�Ƃ̍���
	float fRotDiff = fRotDest - rot.y;

	//�p�x�̐��K��
	RotNormalize(fRotDiff);

	//�p�x�̕␳������
	rot.y += fRotDiff * 0.025f;

	// �p�x�̐��K��
	RotNormalize(rot.y);

	// �����ݒ�
	SetRotation(rot);
}

//==========================================================================
// �ǂ��|���ړ�
//==========================================================================
void CEnemyCrowd::ChaseMove(float fMove)
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
void CEnemyCrowd::MotionSet(void)
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
void CEnemyCrowd::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
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

	// ��ޕ�
	switch (m_pMotion->GetType())
	{
	case MOTION_WALK:
		break;

	case MOTION_ATK:
		break;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEnemyCrowd::Draw(void)
{
	// �`�揈��
	CEnemy::Draw();
}

//==========================================================================
// �G�̏��擾
//==========================================================================
CEnemyCrowd *CEnemyCrowd::GetEnemy(void)
{
	// �������g�̃|�C���^���擾
	return this;
}