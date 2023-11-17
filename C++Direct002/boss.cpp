//=============================================================================
// 
//  �{�X���� [boss.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "boss.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "bullet.h"
#include "explosion.h"
#include "particle.h"
#include "motion.h"
#include "model.h"
#include "elevation.h"
#include "enemymanager.h"
#include "camera.h"
#include "ballast.h"
#include "3D_Effect.h"
#include "hp_gauge.h"
#include "player.h"
#include "enemy.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define WIDTH			(60.0f)							// ����
#define HEIGHT			(60.0f)							// �c��
#define MAX_LIFE		(30)							// �̗�
#define PLAYER_SERCH	(400.0f)	// �v���C���[�T���͈�
#define CHACE_DISTABCE	(50.0f)		// �ǂ��|���鎞�̊Ԋu
#define JUMP			(15.0f)					// �W�����v�͏����l

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CBoss::CBoss(int nPriority) : CObjectChara(nPriority)
{
	// �l�̃N���A
	m_action = ACTION_TACKLE;	// �s��
	m_ImpalseMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ������Ԉړ���
	m_nCntAction = 0;			// �s���J�E���^�[
	m_nCntHit = 0;				// �q�b�g�J�E���^�[
	m_nCntHitAfter = 0;			// �q�b�g��̃J�E���g

	m_state = STATE_NONE;	// ���
	m_Oldstate = m_state;	// �O��̏��
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// �}�e���A���̐F
	m_nCntState = 0;		// ��ԑJ�ڃJ�E���^�[
	m_nBeforCntState = 0;	// �O�̏�ԑJ�ڃJ�E���^�[
	m_nLife = 0;			// �̗�
	m_nTexIdx = 0;			// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_nIdxManager = 0;		// �}�l�[�W���̃C���f�b�N�X�ԍ�
	m_bJump = false;		// �W�����v�����ǂ���
	m_bMove = false;		// �ړ������ǂ���
	m_bATK = false;			// �U�������ǂ���
	m_pHPGauge = NULL;		// HP�Q�[�W�̏��
	m_pMotion = NULL;		// ���[�V�����̏��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CBoss::~CBoss()
{

}

//==========================================================================
// ��������
//==========================================================================
CBoss *CBoss::Create(int nIdx, const char *pFileName, D3DXVECTOR3 pos)
{
	// �����p�̃I�u�W�F�N�g
	CBoss *pEnemy = NULL;

	if (pEnemy == NULL)
	{// NULL��������

		// �������̊m��
		pEnemy = DEBUG_NEW CBoss;

		if (pEnemy != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �C���f�b�N�X�ԍ�
			pEnemy->m_nIdxManager = nIdx;

			// �ʒu�ݒ�
			pEnemy->SetPosition(pos);

			// ����������
			pEnemy->Init(pFileName);
		}

		return pEnemy;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CBoss::Init(void)
{
	return E_FAIL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CBoss::Init(const char *pFileName)
{

	// �e��ϐ��̏�����
	m_state = STATE_NONE;	// ���
	m_nCntState = 0;		// ��ԑJ�ڃJ�E���^�[
	m_nLife = MAX_LIFE;		// �̗�

	// �̗̓Q�[�W
	m_pHPGauge = CHP_Gauge::Create(350.0f, m_nLife, 3.0f);
		
	if (m_pHPGauge == NULL)
	{// NULL��������
		
		m_pHPGauge = NULL;
	}

	// ��ނ̐ݒ�
	SetType(TYPE_BOSS);

	// �L�����쐬
	HRESULT hr = SetCharacter(pFileName);

	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	// ���[�V�����̐�������
	if (m_pMotion == NULL)
	{
		m_pMotion = CMotion::Create();
	}

	if (m_pMotion == NULL)
	{
		return E_FAIL;
	}

	// ���[�V�����̃t�@�C���ǂݍ���
	m_pMotion->ReadText(pFileName);

	// �I�u�W�F�N�g�L�����N�^�[�̏��擾
	CObjectChara *pObjChar = GetObjectChara();

	// ���[�V�����̐ݒ�
	m_pMotion->SetModel(pObjChar->GetModel(), pObjChar->GetNumModel(), CObjectChara::GetObjectChara());

	// �����ݒ�
	SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CBoss::Uninit(void)
{
	int nIdx = m_nIdxManager;

	// �I������
	if (m_pMotion != NULL)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// �I������
	CObjectChara::Uninit();

	// �}�l�[�W���̏I��
	CEnemyManager::Release(nIdx);
}

//==========================================================================
// ���S����
//==========================================================================
void CBoss::Kill(void)
{
	// HP�Q�[�W������
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->Uninit();
		m_pHPGauge = NULL;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CBoss::Update(void)
{
	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �ߋ��̈ʒu�ݒ�
	SetOldPosition(GetPosition());

	// �����蔻��
	Collision();

	// ���[�V�����̐ݒ菈��
	MotionSet();

	// ��ԍX�V
	UpdateState();

	// �U������
	Atack();

	// ���[�V�����̍X�V
	m_pMotion->Update();

	// HP�Q�[�W�̈ʒu�X�V
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->UpdatePosition(GetPosition(), m_nLife);
	}

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();
	if (pInputKeyboard->GetPress(DIK_SPACE) == true)
	{
		// �U����Ԃɂ���
		m_state = STATE_ATTACK;
		m_nCntState = 60 * 10;
		m_action = (ACTION)(rand() % ACTION_MAX);
	}

	//if (GetPosition().y <= -800.0f)
	//{
	//	// �G�̏I������
	//	Kill();
	//	Uninit();
	//}
}

//==========================================================================
// �����蔻��
//==========================================================================
void CBoss::Collision(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �d�͏���
	move.y -= mylib_const::GRAVITY * 0.25f;

	// �ʒu�X�V
	pos += move;

	// �����␳
	move.x += (0.0f - move.x) * 0.25f;
	move.z += (0.0f - move.z) * 0.25f;

	if (move.x >= 0.1f || move.x <= -0.1f ||
		move.z >= 0.1f || move.z <= -0.1f)
	{// �ړ���
		m_bMove = true;
	}
	else
	{
		m_bMove = false;
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);


	// ���n�������ǂ���
	bool bLand = false;

	// �����擾
	float fHeight = CGame::GetObject3DMesh()->GetHeight(pos, move, bLand, GetType());


	if (fHeight > pos.y)
	{// �n�ʂ̕���������荂��������

		// �n�ʂ̍����ɐݒ�
		pos.y = fHeight;

		if (bLand == true)
		{// ���n���Ă���

			if (m_bJump == true)
			{// �W�����v���璅�n�̎�

				switch (m_action)
				{
				case CBoss::ACTION_JUMP:
					
					// �s���J�E���^�[���Z�b�g
					m_nCntAction = 0;

					// �{���G�X�|�[��
					CGame::GetEnemyManager()->SetEnemy(CEnemyManager::PATTERN_BOMB, D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

					// �g����
					CGame::GetObject3DMesh()->SetWave
					(
						pos,												// �ʒu
						GetRotation(),										// ����
						D3DXVECTOR3(
							0.0f,
							0.0f,
							0.0f),											// �ړ���
						D3DXVECTOR2(2000.0f, 2000.0f),						// ����͈�
						CGame::GetObject3DMesh()->GetWidthLen() * 2.0f,		// �g�����̕p�x
						-180.0f,											// �g�̍���
						9.375f,												// �g�̍L����
						40,													// ��������
						CElevation::WAVETYPE_IMPACT,						// ���
						CElevation::TYPE_ENEMY,								// �v���C���[�̎��
						1
					);
					break;

				case CBoss::ACTION_SP_JUMP:

					// ��ԑJ�ڃJ�E���^�[
					m_nCntState = 0;

					// �{���G�X�|�[��
					CGame::GetEnemyManager()->SetEnemy(CEnemyManager::PATTERN_BOMB, D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

					// �{���G�X�|�[��
					CGame::GetEnemyManager()->SetEnemy(CEnemyManager::PATTERN_BOMB, D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

					// �{���G�X�|�[��
					CGame::GetEnemyManager()->SetEnemy(CEnemyManager::PATTERN_BOMB, D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

					// �g����
					CGame::GetObject3DMesh()->SetWave
					(
						pos,												// �ʒu
						GetRotation(),										// ����
						D3DXVECTOR3(
							0.0f,
							0.0f,
							0.0f),											// �ړ���
						D3DXVECTOR2(2000.0f, 2000.0f),						// ����͈�
						CGame::GetObject3DMesh()->GetWidthLen() * 3.0f,		// �g�����̕p�x
						-200.0f,											// �g�̍���
						13.0f,												// �g�̍L����
						90,													// ��������
						CElevation::WAVETYPE_IMPACT,						// ���
						CElevation::TYPE_ENEMY,								// �v���C���[�̎��
						3
					);
					break;
				}
			}

			// �W�����v�g�p�\�ɂ���
			m_bJump = false;
			move.y = 0.0f;
		}
	}


	if (pos.y <= -800.0f)
	{
		pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	
	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);
}


//==========================================================================
// ���[�V�����̐ݒ�
//==========================================================================
void CBoss::MotionSet(void)
{
	if (m_pMotion->IsFinish() == true)
	{// �I�����Ă�����

		// ���݂̎�ގ擾
		int nType = m_pMotion->GetType();

		if (m_bMove == true)
		{// �ړ����Ă�����

			m_bMove = false;	// �ړ�����OFF

			// �ړ����[�V����
			m_pMotion->Set(MOTION_WALK);
		}
		else if (m_bATK == true)
		{// �U�����Ă�����

			m_bATK = false;		// �U������OFF

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
// �q�b�g����
//==========================================================================
bool CBoss::Hit(const int nValue)
{

	if (nValue == 0 && m_state != STATE_KNOCKBACK && m_state != STATE_ATTACK && m_state != STATE_DMG)
	{// �m�[�_���[�W�̏ꍇ

		// �q�b�g�񐔉��Z
		if (m_nCntHitAfter > 0)
		{
			m_nCntHit++;
		}

		// �q�b�g�J�E���^�[���Z�b�g
		m_nCntHitAfter = 120;

		// �ߋ��̏�ԕۑ�
		m_Oldstate = m_state;

		// �m�b�N�o�b�N���
		m_state = STATE_KNOCKBACK;

		// ������Ԉړ���
		m_ImpalseMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// �r���̃J�E���^�[�ۑ�
		m_nBeforCntState = m_nCntState;

		// �J�ڃJ�E���^�[�ݒ�
		m_nCntState = 40;

		D3DXVECTOR3 move = GetMove();
		move.y += 5.5f;
		SetMove(move);

	}
	else if (m_state == STATE_KNOCKBACK)
	{
		return true;
	}
	else if(nValue != 0)
	{
		if (m_state != STATE_DEAD && m_state != STATE_DMG)
		{// �Ȃɂ��Ȃ���Ԃ̎�

			// �̗͌��炷
			m_nLife -= nValue;

			if (m_nLife <= 0)
			{// �̗͂��Ȃ��Ȃ�����

				// �����
				m_state = STATE_DEAD;

				// �G�̏I������
				Kill();
				Uninit();

				// ����
				return true;
			}

			// �␳
			ValueNormalize(m_nLife, MAX_LIFE, 0);

			// �_���[�W��Ԃɂ���
			m_state = STATE_DMG;

			// �J�ڃJ�E���^�[�ݒ�
			m_nCntState = 60 * 3;

			// ��ɔ�΂�
			D3DXVECTOR3 move = GetMove();
			move.y += 25.0f;
			SetMove(move);
		}
	}

	// ����łȂ�
	return false;
}


//==========================================================================
// �q�b�g����
//==========================================================================
bool CBoss::Hit(const int nValue, D3DXVECTOR3 move)
{

	if (nValue == 0 && m_state != STATE_KNOCKBACK && m_state != STATE_ATTACK && m_state != STATE_DMG)
	{// �m�[�_���[�W�̏ꍇ

		// �q�b�g�񐔉��Z
		if (m_nCntHitAfter > 0)
		{
			m_nCntHit++;
		}

		// �q�b�g�J�E���^�[���Z�b�g
		m_nCntHitAfter = 120;

		// �ߋ��̏�ԕۑ�
		m_Oldstate = m_state;

		// �m�b�N�o�b�N���
		m_state = STATE_KNOCKBACK;

		// ������Ԉړ���
		m_ImpalseMove = move;

		// �r���̃J�E���^�[�ۑ�
		m_nBeforCntState = m_nCntState;

		// �J�ڃJ�E���^�[�ݒ�
		m_nCntState = 40;

		D3DXVECTOR3 move = GetMove();
		move.y += 5.5f;
		SetMove(move);

	}
	else if (m_state == STATE_KNOCKBACK)
	{
		return true;
	}
	else
	{
		if (m_state != STATE_DEAD && m_state != STATE_DMG)
		{// �Ȃɂ��Ȃ���Ԃ̎�

			// �̗͌��炷
			m_nLife -= nValue;

			if (m_nLife <= 0)
			{// �̗͂��Ȃ��Ȃ�����

				// �����
				m_state = STATE_DEAD;

				// �G�̏I������
				Kill();
				Uninit();

				// ����
				return true;
			}

			// �␳
			ValueNormalize(m_nLife, MAX_LIFE, 0);

			// �_���[�W��Ԃɂ���
			m_state = STATE_DMG;

			// �J�ڃJ�E���^�[�ݒ�
			m_nCntState = 60 * 3;

			// ��ɔ�΂�
			D3DXVECTOR3 move = GetMove();
			move.y += 25.0f;
			SetMove(move);
		}
	}

	// ����łȂ�
	return false;
}

//==========================================================================
// ��ԍX�V����
//==========================================================================
void CBoss::UpdateState(void)
{
#if TOPCUR
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 pos11 = D3DXVECTOR3(GetObjectChara()->GetModel()[0]->GetWorldMtx()._41, GetObjectChara()->GetModel()[0]->GetWorldMtx()._42, GetObjectChara()->GetModel()[0]->GetWorldMtx()._43);

	CEffect3D::Create(pos11, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), GetRadius(), 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// �ړ��ʎ擾
	float fMove = GetVelocity();

	// ���݂ƖڕW�̍���
	float fRotDiff = 0.0f;

	// �����̔���
	bool bLen = false;

	// �v���C���[���
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	// �I�u�W�F�N�g���
	CObject *pObj = NULL;

	// ���݂̎�ގ擾
	int nType = m_pMotion->GetType();


	// �q�b�g�J�E���^�[���Z
	m_nCntHitAfter--;

	// �q�b�g�񐔉��Z
	if (m_nCntHitAfter < 0)
	{
		m_nCntHitAfter = 0;
		m_nCntHit = 0;
	}

	if (m_nCntHit >= 2 && m_state != STATE_ATTACK && m_state != STATE_KNOCKBACK)
	{
		m_state = STATE_ATTACK;
	}

	switch (m_state)
	{
	case STATE_NONE:
		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		{
			static int nCnt = 0;

			float fRotDiff = 0.0f;	// ���݂ƖڕW�̍���

			nCnt = (nCnt + 1) % 60;

			// �ڕW�̊p�x�����߂�
			if (nCnt == 0)
			{
				fRotDest = Random(-31, 31) * 0.1f;
			}
		}

		// �ڕW�Ƃ̍���
		fRotDiff = fRotDest - rot.y;

		//�p�x�̐��K��
		RotNormalize(fRotDiff);

		//�p�x�̕␳������
		rot.y += fRotDiff * 0.025f;

		// �p�x�̐��K��
		RotNormalize(rot.y);

		// �����Ă�����Ƀ_�b�V��
		move.x += sinf(D3DX_PI + rot.y) * fMove;
		move.z += cosf(D3DX_PI + rot.y) * fMove;

		if (pPlayer != NULL)
		{// NULL����Ȃ��Ƃ�

			if (CircleRange(pos, pPlayer->GetPosition(), 200.0f, PLAYER_SERCH) == true)
			{// �v���C���[���͈͂ɓ����
				m_state = STATE_PLAYERCHASE;
			}
		}

		break;

	case STATE_DMG:

		// ��ԑJ�ڃJ�E���^�[���Z
		m_nCntState--;

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		if (m_nCntState <= 0)
		{// �J�ڃJ�E���^�[��0�ɂȂ�����

			// �e�ǂ��|����Ԃɂ���
			m_state = STATE_PLAYERCHASE;
			m_nCntState = 0;
			// �{�����j
			BombExplosion();
		}

		// �ڕW�̊p�x�����߂�
		fRotDest = atan2f((pos.x - 0.0f), (pos.z - 0.0f));

		// �ڕW�Ƃ̍���
		fRotDiff = fRotDest - rot.y;

		//�p�x�̐��K��
		RotNormalize(fRotDiff);

		//�p�x�̕␳������
		rot.y += fRotDiff * 0.2f;

		// �p�x�̐��K��
		RotNormalize(rot.y);

		if (CircleRange(pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), 100.0f, 100.0f) == false)
		{// �v���C���[���͈͂ɓ����

			// �����Ă�����Ƀ_�b�V��
			move.x += sinf(D3DX_PI + rot.y) * 8.0f;
			move.z += cosf(D3DX_PI + rot.y) * 8.0f;
		}
		break;

	case STATE_KNOCKBACK:

		// ��ԑJ�ڃJ�E���^�[���Z
		m_nCntState--;

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

		if (m_nCntState <= 0)
		{// �J�ڃJ�E���^�[��0�ɂȂ�����

			m_nCntState = m_nBeforCntState;
			
			if (m_Oldstate != STATE_ATTACK)
			{
				// �v���C���[�ǂ��|����Ԃɂ���
				m_state = STATE_PLAYERCHASE;
			}
			else
			{
				m_state = STATE_ATTACK;
				m_action = (ACTION)(rand() % ACTION_MAX);
			}
			break;
		}

		// �����Ă�����Ƌt�����Ɉړ�
		move.x += sinf(D3DX_PI + rot.y) * -fMove * 0.7f;
		move.z += cosf(D3DX_PI + rot.y) * -fMove * 0.7f;

		// ������Ԉړ��ʉ��Z
		pos += m_ImpalseMove;

		break;

	case STATE_PLAYERCHASE:

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// ��ԑJ�ڃJ�E���^�[���Z
		m_nCntState--;

		if (m_nCntState <= 0)
		{// �J�ڃJ�E���^�[��0�ɂȂ�����
			m_nCntState = 0;

			// �U����Ԃɂ���
			m_state = STATE_ATTACK;
			m_nCntState = 60 * 10;
			m_action = (ACTION)(rand() % ACTION_MAX);
		}

		if (pPlayer != NULL)
		{// NULL����Ȃ��Ƃ�

			// �e�̈ʒu�擾
			D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

			// �ڕW�̊p�x�����߂�
			fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

			// �ڕW�Ƃ̍���
			fRotDiff = fRotDest - rot.y;

			//�p�x�̐��K��
			RotNormalize(fRotDiff);

			//�p�x�̕␳������
			rot.y += fRotDiff * 0.05f;

			// �p�x�̐��K��
			RotNormalize(rot.y);

			if (m_nCntState == 60 * 2)
			{
				// �{�����j
				BombExplosion();
			}

			// �����Ă�����Ƀ_�b�V��
			move.x += sinf(D3DX_PI + rot.y) * fMove;
			move.z += cosf(D3DX_PI + rot.y) * fMove;
		}

		break;

	case STATE_ATTACK:

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);

		// ��ԑJ�ڃJ�E���^�[���Z
		m_nCntState--;

		if (m_nCntState <= 0)
		{// �J�ڃJ�E���^�[��0�ɂȂ�����
			m_nCntState = 60 * 15;
			m_state = STATE_PLAYERCHASE;
			break;
		}

		// �s���J�E���^�[���Z
		m_nCntAction++;

		if (m_nCntAction <= 0)
		{// �s���J�E���^�[��0�ɂȂ�����
			m_nCntAction = 0;
		}

		if (m_bATK == false && nType != MOTION_ATK)
		{// �U��������Ȃ����
			m_bATK = true;
		}

		// �U�����
		switch (m_action)
		{
		case CBoss::ACTION_TACKLE:

			if (m_nCntAction >= 0 && m_nCntAction < 60)
			{// ��莞�ԓˌ�

				// �����Ă�����Ƀ_�b�V��
				move.x += sinf(D3DX_PI + rot.y) * fMove * 4.5f;
				move.z += cosf(D3DX_PI + rot.y) * fMove * 4.5f;

				if (m_nCntAction == 5)
				{
					// �{���G�X�|�[��
					CGame::GetEnemyManager()->SetEnemy(CEnemyManager::PATTERN_BOMB, D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
				}

			}
			else if (m_nCntAction >= 60)
			{// �v���C���[�̕�����

				// �e�̈ʒu�擾
				D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

				// �ڕW�̊p�x�����߂�
				fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

				// �ڕW�Ƃ̍���
				fRotDiff = fRotDest - rot.y;

				//�p�x�̐��K��
				RotNormalize(fRotDiff);

				//�p�x�̕␳������
				rot.y += fRotDiff * 0.05f;

				// �p�x�̐��K��
				RotNormalize(rot.y);
			}

			if (m_nCntAction >= 120)
			{// �K��l�𒴂�����

				// �s���J�E���^�[���Z�b�g
				m_nCntAction = 0;
			}

			break;

		case CBoss::ACTION_JUMP:

			if (m_nCntAction >= 60 && m_bJump == false)
			{// �W�����v������Ȃ��Ƃ�

				// �W�����v������
				m_bJump = true;
				move.y += JUMP;
			}

			if (pPlayer != NULL)
			{// NULL����Ȃ��Ƃ�

				if (CircleRange(pos, pPlayer->GetPosition(), 50.0f, 100.0f) == false)
				{// �v���C���[���͈͂ɓ����

					// �e�̈ʒu�擾
					D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

					// �ڕW�̊p�x�����߂�
					fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

					// �ڕW�Ƃ̍���
					fRotDiff = fRotDest - rot.y;

					//�p�x�̐��K��
					RotNormalize(fRotDiff);

					//�p�x�̕␳������
					rot.y += fRotDiff * 0.05f;

					// �p�x�̐��K��
					RotNormalize(rot.y);
				}
			}

			// �����Ă�����Ƀ_�b�V��
			if (m_bJump == true)
			{
				move.x += sinf(D3DX_PI + rot.y) * fMove * 3.0f;
				move.z += cosf(D3DX_PI + rot.y) * fMove * 3.0f;
			}
			break;

		case CBoss::ACTION_SP_JUMP:

			if (m_bJump == false)
			{// �W�����v������Ȃ��Ƃ�

				// �W�����v������
				m_bJump = true;
				move.y += JUMP * 2.0f;
			}

			break;
		}
		break;

	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
#endif
}


//==========================================================================
// �U��
//==========================================================================
void CBoss::Atack(void)
{

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// ���[�V�����̏��擾
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// �U�����̑����擾
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (aInfo.AttackInfo[nCntAttack] == NULL)
		{// NULL��������
			continue;
		}

		if (m_pMotion->GetAllCount() == aInfo.AttackInfo[nCntAttack]->nInpactCnt)
		{// �Ռ��̃J�E���g�Ɠ����Ƃ�

			D3DXMATRIX mtxTrans;	// �v�Z�p�}�g���b�N�X�錾

			if (GetObjectChara()->GetModel()[aInfo.AttackInfo[nCntAttack]->nCollisionNum] == NULL)
			{// NULL��������
				return;
			}

			// ���肷��p�[�c�̃}�g���b�N�X�擾
			D3DXMATRIX mtxWepon = GetObjectChara()->GetModel()[aInfo.AttackInfo[nCntAttack]->nCollisionNum]->GetWorldMtx();

			// �ʒu�𔽉f����
			D3DXMatrixTranslation(&mtxTrans, aInfo.AttackInfo[nCntAttack]->Offset.x, aInfo.AttackInfo[nCntAttack]->Offset.y, aInfo.AttackInfo[nCntAttack]->Offset.z);
			D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

			// ����̈ʒu
			D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

			// ��ޕ�
			switch (m_pMotion->GetType())
			{
			case MOTION_ATK:

				// �p�[�e�B�N������
				my_particle::Create(weponpos, my_particle::TYPE_SUPERATTACK);

				// ���I����
				CBallast::Create(weponpos, DEFAULTMOVE_BALLAST, 5);

				// �U��
				CManager::GetCamera()->SetShake(20, 5.0f, 0.0f);
				break;
			}
		}

		// ���[�V�����J�E���^�[�擾
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt && m_pMotion->GetAllCount() < aInfo.AttackInfo[nCntAttack]->nMaxCnt)
		{// �U�����蒆

			// �v���C���[���
			CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

			if (pPlayer == NULL)
			{// NULL��������
				return;
			}

			// �G�̈ʒu�擾
			D3DXVECTOR3 TargetPos = pPlayer->GetPosition();

			// ���f�����擾
			CModel **pModel = GetObjectChara()->GetModel();
			D3DXMATRIX mtxTrans;	// �v�Z�p�}�g���b�N�X�錾

			// ����̃}�g���b�N�X�擾
			D3DXMATRIX mtxWepon;
			D3DXMatrixIdentity(&mtxWepon);

			if (pModel[aInfo.AttackInfo[nCntAttack]->nCollisionNum] == NULL)
			{// NULL��������
				return;
			}

			// ���肷��p�[�c�̃}�g���b�N�X�擾
			mtxWepon = pModel[aInfo.AttackInfo[nCntAttack]->nCollisionNum]->GetWorldMtx();

			// �ʒu�𔽉f����
			D3DXMatrixTranslation(&mtxTrans, aInfo.AttackInfo[nCntAttack]->Offset.x, aInfo.AttackInfo[nCntAttack]->Offset.y, aInfo.AttackInfo[nCntAttack]->Offset.z);
			D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

			// ����̈ʒu
			D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

			CEffect3D::Create(weponpos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), aInfo.AttackInfo[nCntAttack]->fRangeSize * 0.5f, 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

			// ����T�C�Y�擾
			float fRadius = pPlayer->GetObjectChara()->GetRadius();

			if (SphereRange(weponpos, TargetPos, aInfo.AttackInfo[nCntAttack]->fRangeSize, fRadius))
			{// ���̔���

				if (pPlayer->Hit(aInfo.AttackInfo[nCntAttack]->nDamage) == true)
				{// ����ł���

					my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);

					// �v���C���[���
					CPlayer **ppPlayer = CManager::GetScene()->GetPlayer(0);

					// �v���C���[��NULL
					*ppPlayer = NULL;
					int m = 0;
				}
			}
		}
	}
}

//==========================================================================
// �{�����j
//==========================================================================
void CBoss::BombExplosion(void)
{
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

		if (pObj->GetObjectChara()->GetCharaType() == CEnemy::TYPE_BOMB)
		{// �{��
			pObj->GetObjectChara()->SetState(CEnemy::STATE_DEAD, 120);
		}

		// ���̃I�u�W�F�N�g����
		pObj = pObjNext;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CBoss::Draw(void)
{
	if (m_mMatcol != D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))
	{
		// �I�u�W�F�N�g�L�����̕`��
		CObjectChara::Draw(m_mMatcol);
	}
	else
	{
		// �I�u�W�F�N�g�L�����̕`��
		CObjectChara::Draw();
	}
}

//==========================================================================
// ��Ԏ擾
//==========================================================================
int CBoss::GetState(void)
{
	return m_state;
}

//==========================================================================
// ��ގ擾
//==========================================================================
int CBoss::GetCharaType(void)
{
	return 0;
}

//==========================================================================
// �G�̏��擾
//==========================================================================
CBoss *CBoss::GetBoss(void)
{
	// �������g�̃|�C���^���擾
	return this;
}