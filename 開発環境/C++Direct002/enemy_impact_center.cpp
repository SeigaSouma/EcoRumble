//=============================================================================
// 
//  �g�N�����G���� [enemy_impact_center.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "enemy_impact_center.h"
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
#include "impactwave.h"

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
CEnemyImpactCenter::CEnemyImpactCenter(int nPriority) : CEnemy(nPriority)
{
	// �l�̃N���A
	m_bInvicible = false;	// ���G��Ԃ��̔���
	m_nCntInvicible = 0;	// ���G�J�E���^�[

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyImpactCenter::~CEnemyImpactCenter()
{

}


//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyImpactCenter::Init(void)
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
	m_bInvicible = false;	// ���G��Ԃ��̔���

	if (m_pHPGauge == NULL)
	{// NULL��������
		m_pHPGauge = NULL;
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyImpactCenter::Uninit(void)
{
	// �I������
	CEnemy::Uninit();
}

//==========================================================================
// ���S����
//==========================================================================
void CEnemyImpactCenter::Kill(void)
{
	// ���S����
	CEnemy::Kill();
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyImpactCenter::Update(void)
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

	if (m_bInvicible == true)
	{
		// ���G���
		Invincible();
	}
}

//==========================================================================
// ���n���̏���
//==========================================================================
void CEnemyImpactCenter::ProcessLanding(void)
{
	// ���n���̏���
	CEnemy::ProcessLanding();
}

//==========================================================================
// �U����Ԉڍs����
//==========================================================================
void CEnemyImpactCenter::ChangeToAttackState(void)
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
void CEnemyImpactCenter::UpdateStateByType(void)
{
	return;
}

//==========================================================================
// �q�b�g����
//==========================================================================
bool CEnemyImpactCenter::Hit(const int nValue)
{
	// �����擾
	D3DXVECTOR3 rot = GetRotation();
	D3DXVECTOR3 pos = GetPosition();

	// �̗͎擾
	int nLife = GetLife();

	if (nValue == 0 && m_state != STATE_KNOCKBACK)
	{// �m�[�_���[�W�̏ꍇ

		// �ߋ��̏�ԕۑ�
		m_Oldstate = m_state;

		// �m�b�N�o�b�N���
		m_state = STATE_KNOCKBACK;

		// �J�ڃJ�E���^�[�ݒ�
		m_nCntState = 30;

		D3DXVECTOR3 move = GetMove();
		move.y = 0.0f;
		move.y += 10.5f;
		SetMove(move);
	}
	else if (nValue == 0 && m_state == STATE_KNOCKBACK)
	{
		return true;
	}
	else
	{
		if (m_bInvicible == false &&
			m_state != STATE_DMG && m_state != STATE_KNOCKBACK && m_state != STATE_DEAD && m_state != STATE_SPAWN)
		{// �Ȃɂ��Ȃ���Ԃ̎�

			if (nValue == mylib_const::DIRECTWAVE_DMG)
			{
				CManager::SetEnableHitStop(5);

				// �U��
				CManager::GetCamera()->SetShake(5, 10.0f, 0.0f);
			}
			else if (nValue == mylib_const::IMPACTWAVE_DMG)
			{
				// �U��
				CManager::GetCamera()->SetShake(5, 10.0f, 0.0f);
			}

			// �̗͌��炷
			nLife -= nValue;

			// �̗͐ݒ�
			SetLife(nLife);

			if (nLife > 0)
			{// �̗͂��Ȃ��Ȃ��ĂȂ�������

				// �_���[�W���Đ�
				CManager::GetSound()->PlaySound(CSound::LABEL_SE_DMG01);

				// �ړ���
				SetMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

				CImpactWave::Create
				(
					D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * 80.0f, pos.y + 80.0f, pos.z + cosf(D3DX_PI + rot.y) * 80.0f),	// �ʒu
					D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// ����
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f),			// �F
					80.0f,										// ��
					80.0f,										// ����
					40,											// ����
					9.0f,										// ���̈ړ���
					CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
					false										// ���Z�������邩
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(pos.x + sinf(rot.y) * 200.0f, pos.y + 80.0f, pos.z + cosf(rot.y) * 200.0f),	// �ʒu
					D3DXVECTOR3(D3DX_PI * 0.5f, rot.y, D3DX_PI),				// ����
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f),			// �F
					40.0f,										// ��
					200.0f,										// ����
					20,											// ����
					1.0f,										// ���̈ړ���
					CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
					false										// ���Z�������邩
				);
			}

			if (nLife <= 0)
			{// �̗͂��Ȃ��Ȃ�����

				// �Ԋu���ɓ|�������擾
				int nNumKill = CGame::GetEnemyManager()->GetNumIntervalKill();

				// �Ռ��g����
				if (nNumKill >= 0 && nNumKill < mylib_const::COMBONUM_1)
				{
					CImpactWave::Create
					(
						D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// �ʒu
						D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// ����
						D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.2f),			// �F
						80.0f,										// ��
						80.0f,										// ����
						20,											// ����
						9.0f,										// ���̈ړ���
						CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
						false										// ���Z�������邩
					);

					// �_���[�W���Đ�
					CManager::GetSound()->PlaySound(CSound::LABEL_SE_ENEMYDMG_SMALL);

					// ���Z�X�R�A�ݒ�
					m_AddType = CResultManager::ADDTYPE_NORMAL;
				}
				else if (nNumKill >= mylib_const::COMBONUM_1 && nNumKill < mylib_const::COMBONUM_2)
				{
					CImpactWave::Create
					(
						D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// �ʒu
						D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// ����
						D3DXCOLOR(0.2f, 0.2f, 1.0f, 0.5f),			// �F
						80.0f,										// ��
						80.0f,										// ����
						30,											// ����
						13.0f,										// ���̈ړ���
						CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
						true										// ���Z�������邩
					);

					// �_���[�W���Đ�
					CManager::GetSound()->PlaySound(CSound::LABEL_SE_ENEMYDMG_NORMAL);

					// ���Z�X�R�A�ݒ�
					m_AddType = CResultManager::ADDTYPE_BLUE;
				}
				else
				{
					CImpactWave::Create
					(
						D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// �ʒu
						D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// ����
						D3DXCOLOR(1.0f, 0.6f, 0.3f, 0.6f),			// �F
						80.0f,										// ��
						80.0f,										// ����
						40,											// ����
						13.0f,										// ���̈ړ���
						CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
						true										// ���Z�������邩
					);

					// �_���[�W���Đ�
					CManager::GetSound()->PlaySound(CSound::LABEL_SE_ENEMYDMG_BIG);

					// ���Z�X�R�A�ݒ�
					m_AddType = CResultManager::ADDTYPE_GOLD;
				}

				// ���S��Ԃɂ���
				m_state = STATE_DEAD;

				// �J�ڃJ�E���^�[�ݒ�
				m_nCntState = 0;

				// �m�b�N�o�b�N�̈ʒu�X�V
				m_posKnokBack = GetPosition();

				// �m�b�N�o�b�N����ɂ���
				m_sMotionFrag.bKnockback = true;

				// ���ꃂ�[�V����
				m_pMotion->Set(MOTION_KNOCKBACK);

				// �|��������ݒ�
				CGame::GetEnemyManager()->SetKillFlag();

				// ����
				return true;
			}

			// �␳
			ValueNormalize(nLife, GetLifeOrigin(), 0);

			// �ߋ��̏�ԕۑ�
			m_Oldstate = m_state;

			// �_���[�W��Ԃɂ���
			m_state = STATE_DMG;

			// �J�ڃJ�E���^�[�ݒ�
			m_nCntState = 0;

			// �m�b�N�o�b�N�̈ʒu�X�V
			m_posKnokBack = GetPosition();

			// �m�b�N�o�b�N����ɂ���
			m_sMotionFrag.bKnockback = true;

			// ���ꃂ�[�V����
			m_pMotion->Set(MOTION_KNOCKBACK);

		}

		if (m_bInvicible == true)
		{
			// �ړ���
			SetMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}
	}

	// ����łȂ�
	return false;
}

//==========================================================================
// �o��
//==========================================================================
void CEnemyImpactCenter::Spawn(void)
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
// �_���[�W
//==========================================================================
void CEnemyImpactCenter::Damage(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// ���݂ƖڕW�̍���
	float fRotDiff = 0.0f;

	// �����̔���
	bool bLen = false;

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �ő�t���[���擾
	int nAllFrame = m_pMotion->GetMaxAllCount();

	// �}�e���A���̐F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, m_mMatcol.a);

	if (m_nCntState == 60)
	{
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);
		m_bInvicible = true;	// ���G��Ԃ��̔���
		m_nCntInvicible = 80;	// ���G�J�E���^�[
	}

	// �N���Ƃ̔���
	if (m_nCntState >= nAllFrame)
	{// �t���[���𒴂��Ă�����

		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);
		m_state = STATE_TURRETATK;
		m_nCntState = 0;
		move.y = 0.0f;			// �ړ��ʃ[��

		// �m�b�N�o�b�N�������
		m_sMotionFrag.bKnockback = false;

		// �U�����[�V����
		m_pMotion->Set(MOTION_ATK);
	}

	// �n�ʂ̍����擾
	bool bLand = false;
	pos.y = CManager::GetScene()->GetObject3DMesh()->GetHeight(pos, move, bLand, GetType());

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
}

//==========================================================================
// ���G
//==========================================================================
void CEnemyImpactCenter::Invincible(void)
{
	// ���G�J�E���^�[���Z
	m_nCntInvicible--;

	if (m_nCntInvicible % 4 == 0)
	{// �K��Ԋu

		// �F�ݒ�
		if (m_mMatcol.a == 1.0f)
		{
			m_mMatcol.a = 0.4f;
		}
		else
		{
			m_mMatcol.a = 1.0f;
		}
	}

	if (m_nCntInvicible <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		m_bInvicible = false;	// ���G��Ԃ��̔���
	}

}

//==========================================================================
// �ǂ��|���ړ�
//==========================================================================
void CEnemyImpactCenter::ChaseMove(float fMove)
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
void CEnemyImpactCenter::MotionSet(void)
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
void CEnemyImpactCenter::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
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
void CEnemyImpactCenter::Draw(void)
{
	// �`�揈��
	CEnemy::Draw();
}

//==========================================================================
// �G�̏��擾
//==========================================================================
CEnemyImpactCenter *CEnemyImpactCenter::GetEnemy(void)
{
	// �������g�̃|�C���^���擾
	return this;
}