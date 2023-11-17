//=============================================================================
// 
//  �v���C���[���� [player.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "game.h"
#include "player.h"
#include "camera.h"
#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "input.h"
#include "bullet.h"
#include "enemy.h"
#include "calculation.h"
#include "collision.h"
#include "score.h"
#include "texture.h"
#include "Xload.h"
#include "model.h"
#include "motion.h"
#include "hp_gauge.h"
#include "objectChara.h"
#include "elevation.h"
#include "shadow.h"
#include "particle.h"
#include "3D_Effect.h"
#include "ballast.h"
#include "magic_ice.h"
#include "speed_rush.h"
#include "magiccircle.h"
#include "targetpoint.h"
#include "explosion_sphere.h"
#include "impactwave.h"
#include "power_gauge.h"
#include "sound.h"
#include "grassparticle.h"
#include "confusion_effect.h"
#include "enemymanager.h"
#include "delusion_window.h"
#include "delusion_meter.h"

// �h����
#include "tutorialplayer.h"
#include "resultplayer.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define CHARAFILE		"data\\TEXT\\motion_set_player.txt"
#define JUMP			(20.0f * 1.5f)	// �W�����v�͏����l
#define MAX_LIFE		(20)			// �̗�
#define TARGET_LEN		(400.0f)		// �ڕW�܂ł̋���
#define WALK_INT		(30)			// ���s�̎���
#define INVINCIBLE_INT	(2)				// ���G�̊Ԋu
#define INVINCIBLE_TIME	(90)			// ���G�̎���
#define CONFUSIONTIME	(60 * 2)		// ��������

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CPlayer::CPlayer(int nPriority) : CObjectChara(nPriority)
{
	// �l�̃N���A
	m_state = STATE_NONE;	// ���
	m_Oldstate = m_state;	// �O��̏��
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// �}�e���A���̐F
	m_rotConfusion = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �����̌���
	m_fMoveSlide = 0.0f;			// �X���C�h�ړ���
	m_fMoveSlideDest = 0.0f;		// �ڕW�̃X���C�h�ړ���
	m_fMoveRatio = 0.0f;			// �ړ��̊���
	m_bJump = false;				// �W�����v�����ǂ���
	m_bKnockback = false;			// �m�b�N�o�b�N�����ǂ���
	m_bMove = false;				// �ړ������ǂ���
	m_bATK = false;					// �U�������ǂ���
	m_bMWait = false;				// ���@�ҋ@�����ǂ���
	m_bCharge = false;				// �`���[�W�����ǂ���
	m_bConfusion = false;			// ���������ǂ���
	m_bInWater = false;				// ���̒��ɂ��锻��
	m_nCntState = 0;				// ��ԑJ�ڃJ�E���^�[
	m_nCntWalk = 0;					// ���s�J�E���^�[
	m_nCntConfusion = 0;			// �����J�E���^�[
	m_nLife = 0;					// �̗�
	m_nTexIdx = 0;					// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_nIdxXFile = 0;				// X�t�@�C���̃C���f�b�N�X�ԍ�
	m_walkType = WALKTYPE_NORMAL;	// �ړ����
	m_pMotion = NULL;				// ���[�V�����̏��
	m_pShadow = NULL;				// �e�̏��
	m_pMCircle = NULL;				// ���@�w�̏��
	m_pTargetP = NULL;				// �ڕW�̒n�_
	m_pHPGauge = NULL;				// HP�Q�[�W�̏��
	m_pDelusionWindow = NULL;		// �ϑz�E�B���h�E
	m_pConfusionEffect = NULL;		// �����G�t�F�N�g�̏��

#if ACTION_MODE
	m_Shotmode = SHOTMODE_TRIGGER;	// �ˌ��̎��

#else
	m_Shotmode = SHOTMODE_NONE;	// �ˌ��̎��
#endif
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CPlayer::~CPlayer()
{

}

//==========================================================================
// ��������
//==========================================================================
CPlayer *CPlayer::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CPlayer *pPlayer = NULL;

	if (pPlayer == NULL)
	{// NULL��������

		// �������̊m��
		switch (CManager::GetMode())
		{
		case CScene::MODE_GAME:
			pPlayer = DEBUG_NEW CPlayer;
			break;

		case CScene::MODE_TUTORIAL:
			pPlayer = DEBUG_NEW CTutorialPlayer;
			break;

		case CScene::MODE_RESULT:
			pPlayer = DEBUG_NEW CResultPlayer;
			break;

		default:
			pPlayer = DEBUG_NEW CPlayer;
			break;
		}

		if (pPlayer != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pPlayer->Init();
		}

		return pPlayer;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CPlayer::Init(void)
{
	// ��ނ̐ݒ�
	SetType(TYPE_PLAYER);

	m_state = STATE_NONE;	// ���
	m_nCntState = 0;		// ��ԑJ�ڃJ�E���^�[
	m_nLife = MAX_LIFE;		// �̗�
	m_Shotmode = ATK_DEBUG_POWER;

	// �L�����쐬
	HRESULT hr = SetCharacter(CHARAFILE);

	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	// ���[�V�����̐�������
	m_pMotion = CMotion::Create();

	// ���[�V�����̃t�@�C���ǂݍ���
	m_pMotion->ReadText(CHARAFILE);

	// �I�u�W�F�N�g�L�����N�^�[�̏��擾
	CObjectChara *pObjChar = GetObjectChara();

	// ���[�V�����̐ݒ�
	m_pMotion->SetModel(pObjChar->GetModel(), pObjChar->GetNumModel(), CObjectChara::GetObjectChara());

	// �̗̓Q�[�W
	//m_pHPGauge = CHP_Gauge::Create(120.0f, m_nLife);

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �e�̐���
	m_pShadow = CShadow::Create(pos, 50.0f);

	// �|�[�Y�̃��Z�b�g
	m_pMotion->ResetPose(MOTION_DEF);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CPlayer::Uninit(void)
{
	if (m_pMotion != NULL)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// ���@�w������
	if (m_pMCircle != NULL)
	{
		m_pMCircle->Uninit();
		m_pMCircle = NULL;
	}

	// HP�Q�[�W������
	if (m_pHPGauge != NULL)
	{
		//m_pHPGauge->Uninit();
		m_pHPGauge = NULL;
	}

	// �ϑz�E�B���h�E������
	if (m_pDelusionWindow != NULL)
	{
		//m_pDelusionWindow->Uninit();
		m_pDelusionWindow = NULL;
	}

	// �e������
	if (m_pShadow != NULL)
	{
		//m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	// �ڕW�̒n�_����
	if (m_pTargetP != NULL)
	{
		m_pTargetP->Uninit();
		m_pTargetP = NULL;
	}

	// �����G�t�F�N�g����
	if (m_pConfusionEffect != NULL)
	{
		m_pConfusionEffect = NULL;
	}

	// �I������
	CObjectChara::Uninit();

	// ���[�h�ʏI������
	UninitByMode();
}

//==========================================================================
// ���[�h�ʏI������
//==========================================================================
void  CPlayer::UninitByMode(void)
{
	CScene *pScene = CManager::GetScene();
	if (pScene != NULL)
	{
		// �v���C���[���
		//CPlayer **ppPlayer = CGame::GetPlayer(0);
		CPlayer **ppPlayer = CManager::GetScene()->GetPlayer(0);

		// �v���C���[��NULL
		*ppPlayer = NULL;
	}
}

//==========================================================================
// �I������
//==========================================================================
void CPlayer::Kill(void)
{

	my_particle::Create(GetPosition(), my_particle::TYPE_OFFSETTING);

	// HP�Q�[�W������
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->Uninit();
		m_pHPGauge = NULL;
	}

	// �ϑz�E�B���h�E������
	if (m_pDelusionWindow != NULL)
	{
		m_pDelusionWindow->Uninit();
		m_pDelusionWindow = NULL;
	}

	// �e������
	if (m_pShadow != NULL)
	{
		m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	// �����G�t�F�N�g����
	if (m_pConfusionEffect != NULL)
	{
		m_pConfusionEffect->Uninit();
		m_pConfusionEffect = NULL;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CPlayer::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();
	if (pInputKeyboard->GetTrigger(DIK_F5) == true)
	{// F5�Ń��Z�b�g
		SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1000.0f));
		SetMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}


	// �G�f�B�b�g���͔�����
	if (CGame::GetObject3DMesh()->IsEdit())
	{
		return;
	}

	// �R���{���o���͔�����
	if (CGame::GetEnemyManager()->GetState() == CEnemyManager::STATE_COMBOANIM)
	{
		return;
	}


	// �ߋ��̈ʒu�ۑ�
	SetOldPosition(GetPosition());

	// ����
	Controll();

	// ���[�V�����̐ݒ菈��
	MotionSet();

	// ���[�V�����̍X�V����
	m_pMotion->Update();

	// ���_���ݒ�
	SetVtx();

	// �U������
	Atack();

	// ��ԍX�V
	UpdateState();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �e�̈ʒu�X�V
	if (m_pShadow != NULL)
	{
		m_pShadow->SetPosition(D3DXVECTOR3(pos.x, m_pShadow->GetPosition().y, pos.z));
	}

	// HP�Q�[�W�̈ʒu�X�V
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->UpdatePosition(GetPosition(), m_nLife);
	}

	// �f�o�b�O�\��
	CManager::GetDebugProc()->Print(
		"------------------[�v���C���[�̑���]------------------\n"
		"�ړ��F�yX�F%f, Y�F%f, Z�F%f�z �yW / A / S / D�z\n"
		"�����F�yX�F%f, Y�F%f, Z�F%f�z �yZ / C�z\n"
		"�ړ��ʁF�yX�F%f, Y�F%f, Z�F%f�z\n", pos.x, pos.y, pos.z, rot.x, rot.y, rot.y, move.x, move.y, move.z);
}

//==========================================================================
// ���쏈��
//==========================================================================
void CPlayer::Controll(void)
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


#if 0
	// ��
	CEffect3D *pEffect = CEffect3D::Create(
		pos,
		D3DXVECTOR3(0.0f, 2.0f,0.0f),
		D3DXCOLOR(0.6f, 0.2f, 0.6f, 0.15f),
		40.0f,
		60,
		CEffect3D::MOVEEFFECT_SUB,
		CEffect3D::TYPE_SMOKE);

	if (pEffect != NULL)
	{
		// �Z�b�g�A�b�v�ʒu�ݒ�
		pEffect->SetUp(D3DXVECTOR3(50.0f, 150.0f, 50.0f), CObject::GetObject());
	}
#endif

	static int nWalk = 0;
	if (m_bInWater == false && (nType == MOTION_WALK || nType == MOTION_SPEEDWALK || nType == MOTION_SLOWWALK))
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

		int nWalkFrame = 25;

		switch (nType)
		{
		case MOTION_SPEEDWALK:
			nWalkFrame = 20;
			break;

		case MOTION_SLOWWALK:
			nWalkFrame = 30;
			break;

		default:
			break;
		}
		nWalk = (nWalk + 1) % nWalkFrame;
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

	if (m_pMotion->IsGetMove(nType) == 1 && m_state != STATE_CONFUSION)
	{// �ړ��\���[�V�����̎� && ��������Ȃ��Ƃ�

#ifndef AUTO_MOVE
		// �ړ����ɂ���
		if (CManager::GetScene()->GetWaveManager()->IsStageFrag() == true)
		{
			m_bMove = true;
		}
#endif

#if AUTO_MOVE
		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//���L�[�������ꂽ,���ړ�

			// �ړ����ɂ���
			m_bMove = true;

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//A+W,����ړ�

				move.x += sinf(-D3DX_PI * 0.25f + Camerarot.y) * fMove;
				move.z += cosf(-D3DX_PI * 0.25f + Camerarot.y) * fMove;
				fRotDest = D3DX_PI * 0.75f/* + rot.y*/ + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//A+S,�����ړ�

				move.x += sinf(-D3DX_PI * 0.75f + Camerarot.y) * fMove;
				move.z += cosf(-D3DX_PI * 0.75f + Camerarot.y) * fMove;
				fRotDest = D3DX_PI * 0.25f /*+ rot.y */ + Camerarot.y;
			}
			else
			{//A,���ړ�

				move.x += sinf(-D3DX_PI * 0.5f + Camerarot.y) * fMove;
				move.z += cosf(-D3DX_PI * 0.5f + Camerarot.y) * fMove;
				fRotDest = D3DX_PI * 0.5f/* + rot.y*/ + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//D�L�[�������ꂽ,�E�ړ�

			// �ړ����ɂ���
			m_bMove = true;

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//D+W,�E��ړ�

				move.x += sinf(D3DX_PI * 0.25f + Camerarot.y) * fMove;
				move.z += cosf(D3DX_PI * 0.25f + Camerarot.y) * fMove;
				fRotDest = -D3DX_PI * 0.75f/* + rot.y*/ + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//D+S,�E���ړ�

				move.x += sinf(D3DX_PI * 0.75f + Camerarot.y) * fMove;
				move.z += cosf(D3DX_PI * 0.75f + Camerarot.y) * fMove;
				fRotDest = -D3DX_PI * 0.25f/* + rot.y*/ + Camerarot.y;
			}
			else
			{//D,�E�ړ�

				move.x += sinf(D3DX_PI * 0.5f + Camerarot.y) * fMove;
				move.z += cosf(D3DX_PI * 0.5f + Camerarot.y) * fMove;
				fRotDest = -D3DX_PI * 0.5f /*+ rot.y*/ + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
		{//W�������ꂽ�A��ړ�

			// �ړ����ɂ���
			m_bMove = true;

			move.x += sinf(D3DX_PI * 0.0f + Camerarot.y) * fMove;
			move.z += cosf(D3DX_PI * 0.0f + Camerarot.y) * fMove;
			fRotDest = D3DX_PI * 1.0f/* + rot.y */ + Camerarot.y;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
		{//S�������ꂽ�A���ړ�

			// �ړ����ɂ���
			m_bMove = true;

			move.x += sinf(D3DX_PI * 1.0f + Camerarot.y) * fMove;
			move.z += cosf(D3DX_PI * 1.0f + Camerarot.y) * fMove;
			fRotDest = D3DX_PI * 0.0f /*+ rot.y */ + Camerarot.y;
		}
		else
		{
			// �ړ������ǂ���
			m_bMove = false;
		}
#else

		//*************************************************
		// �����̕ύX
		//*************************************************
		// �X�e�B�b�N�̌����擾
		float fStickRot = pInputGamepad->GetStickRotR(0);

		if (fStickRot > D3DX_PI * 0.75f || fStickRot < -D3DX_PI * 0.75f)
		{// �^���͖���
			fStickRot = 0.0f;
		}

		fRotDest = D3DX_PI + fStickRot + Camerarot.y;

		// �ڕW�̌����ݒ�
		SetRotDest(fRotDest);

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
		//else if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
		//{//W�������ꂽ�A��ړ�
		//	fRotDest = D3DX_PI * 1.0f + Camerarot.y;
		//}
		//else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
		//{//S�������ꂽ�A���ړ�
		//	fRotDest = D3DX_PI * 0.0f + Camerarot.y;
		//}

#endif

		// �����x�ݒ�
		if (pInputKeyboard->GetPress(DIK_W) == true)
		{// ��ړ�

			if (pInputKeyboard->GetPress(DIK_A) == true || pInputKeyboard->GetPress(DIK_D) == true)
			{// A or D
				m_fMoveRatio = 0.5f;
			}
			else
			{
				m_fMoveRatio = 1.0f;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{// ���ړ�

			if (pInputKeyboard->GetPress(DIK_A) == true || pInputKeyboard->GetPress(DIK_D) == true)
			{// A or D
				m_fMoveRatio = -0.5f;
			}
			else
			{
				m_fMoveRatio = -1.0f;
			}
		}

		if (pInputKeyboard->GetTrigger(DIK_SPACE) == true)
		{//SPACE�������ꂽ,�W�����v

#if 0
			// �ϑz�E�B���h�E����, �j��
			if (m_pDelusionWindow == NULL)
			{
				m_pDelusionWindow = CDelusionWindow::Create();
				CDelusionMeter::Create();
			}
#endif
		}

		// �`���[�W
#if 0
		if (nType == MOTION_CHARGE)
		{// �`���[�W�̎�

			if (m_pMotion->GetAllCount() <= 20)
			{// �n�߂��Ă͋����ړ�����
				m_bMove = false;	// �ړ�����OFF
				move.x = 0.0f;
				move.z = 0.0f;
			}

			if (CGame::GetPowerGauge()->GetLevel() == CPowerGauge::LEVEL_MAX)
			{
				m_bCharge = false;	// �`���[�WOFF
			}

			float fMove = 0.0f;		// �ړ���
			float fMoveY = 0.0f;	// �ړ���

			switch (CGame::GetPowerGauge()->GetLevel())
			{
			case CPowerGauge::LEVEL_1:

				fMove = (float)Random(0, 150) * 0.01f;	// �ړ���
				fMoveY = (float)Random(200, 350) * 0.1f;	// �ړ���

				// �G�t�F�N�g�̐ݒ�
				CEffect3D::Create(
					pos,
					D3DXVECTOR3(sinf((float)Random(-314, 314) * 0.01f) * fMove, sinf((float)Random(0, 314) * 0.01f) * fMoveY, cosf((float)Random(-314, 314) * 0.01f) * fMove),
					D3DXCOLOR(0.2f, 0.2f, 1.0f, 0.6f),
					60.0f,
					10,
					CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_SMOKE);
				break;

			case CPowerGauge::LEVEL_2:

				fMove = (float)Random(0, 150) * 0.01f;	// �ړ���
				fMoveY = (float)Random(200, 350) * 0.1f;	// �ړ���

				// �G�t�F�N�g�̐ݒ�
				CEffect3D::Create(
					pos,
					D3DXVECTOR3(sinf((float)Random(-314, 314) * 0.01f) * fMove, sinf((float)Random(0, 314) * 0.01f) * fMoveY, cosf((float)Random(-314, 314) * 0.01f) * fMove),
					D3DXCOLOR(0.2f, 1.0f, 0.2f, 0.4f),
					80.0f,
					15,
					CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_SMOKE);
				break;

			case CPowerGauge::LEVEL_3:

				fMove = (float)Random(0, 150) * 0.01f;	// �ړ���
				fMoveY = (float)Random(200, 350) * 0.1f;	// �ړ���

				// �G�t�F�N�g�̐ݒ�
				CEffect3D::Create(
					pos,
					D3DXVECTOR3(sinf((float)Random(-314, 314) * 0.01f) * fMove, sinf((float)Random(0, 314) * 0.01f) * fMoveY, cosf((float)Random(-314, 314) * 0.01f) * fMove),
					D3DXCOLOR(1.0f, 0.2f, 1.0f, 0.6f),
					120.0f,
					20,
					CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_SMOKE);
				break;

			case CPowerGauge::LEVEL_MAX:
				break;
			}

		}
#endif
	}
	else if(m_state != STATE_CONFUSION)
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
		//else if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
		//{//W�������ꂽ�A��ړ�
		//	fRotDest = D3DX_PI * 1.0f + Camerarot.y;
		//}
		//else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
		//{//S�������ꂽ�A���ړ�
		//	fRotDest = D3DX_PI * 0.0f + Camerarot.y;
		//}
#endif
	}

	if (/*m_state == STATE_CONFUSION ||*/ m_state == STATE_DMG || m_state == STATE_KNOCKBACK)
	{// ������я�Ԃ̂Ƃ�
		fRotDest = rot.y;
	}

	//*******************************
	// ���E�ŕ���`�F���W
	//*******************************
#if 0
	if (pInputKeyboard->GetTrigger(DIK_LEFT) == true)
	{// ���������ꂽ�A���[�h�ύX

		// ���@�w������
		if (m_pMCircle != NULL)
		{
			m_pMCircle->Uninit();
			m_pMCircle = NULL;
		}

		// �ڕW�̒n�_����
		if (m_pTargetP != NULL)
		{
			m_pTargetP->Uninit();
			m_pTargetP = NULL;
		}
		m_bMWait = false;		// ���@�ҋ@OFF

		m_Shotmode = (m_Shotmode + 1) % ATK_DEBUG_MAX;

		// ���Z�b�g
		for (int nCntParts = 16; nCntParts < GetObjectChara()->GetNumModel(); nCntParts++)
		{
			ChangeObject(nCntParts, -1);
		}

		// ����ς���
		switch (m_Shotmode)
		{
		case ATK_DEBUG_POWER:
			SetObject(16);
			break;

		case ATK_DEBUG_POWER2_SPEED1:
			SetObject(18);
			SetObject(19);
			break;

		case ATK_DEBUG_POWER2_CROWD1:
			SetObject(20);
			break;

		case ATK_DEBUG_CROWD:
			SetObject(17);
			break;

		case ATK_DEBUG_CROWD2_POWER1:
			break;

		case ATK_DEBUG_SPEED:
			break;
		}
	}

	if (pInputKeyboard->GetTrigger(DIK_RIGHT) == true)
	{// ���������ꂽ�A���[�h�ύX

		// ���@�w������
		if (m_pMCircle != NULL)
		{
			m_pMCircle->Uninit();
			m_pMCircle = NULL;
		}

		// �ڕW�̒n�_����
		if (m_pTargetP != NULL)
		{
			m_pTargetP->Uninit();
			m_pTargetP = NULL;
		}
		m_bMWait = false;		// ���@�ҋ@OFF

		m_Shotmode = (m_Shotmode + (ATK_DEBUG_MAX - 1)) % ATK_DEBUG_MAX;

		// ���Z�b�g
		for (int nCntParts = 16; nCntParts < GetObjectChara()->GetNumModel(); nCntParts++)
		{
			ChangeObject(nCntParts, -1);
		}

		// ����ς���
		switch (m_Shotmode)
		{
		case ATK_DEBUG_POWER:
			SetObject(16);
			break;

		case ATK_DEBUG_POWER2_SPEED1:
			SetObject(18);
			SetObject(19);
			break;

		case ATK_DEBUG_POWER2_CROWD1:
			SetObject(20);
			break;

		case ATK_DEBUG_CROWD:
			SetObject(17);
			break;

		case ATK_DEBUG_CROWD2_POWER1:
			break;

		case ATK_DEBUG_SPEED:
			break;
		}
	}
#endif

	switch (m_Shotmode)
	{
	case ATK_DEBUG_POWER:
		// �f�o�b�O�\�L
		CManager::GetDebugProc()->Print("�U���̎�ޕύX�F�y�p���[+3�z �ύX�F�ySPACE�z\n", m_Shotmode);
		break;

	case ATK_DEBUG_POWER2_SPEED1:
		// �f�o�b�O�\�L
		CManager::GetDebugProc()->Print("�U���̎�ޕύX�F�y�p���[+2, �X�s�[�h+1�z �ύX�F�ySPACE�z\n", m_Shotmode);
		break;

	case ATK_DEBUG_POWER2_CROWD1:
		// �f�o�b�O�\�L
		CManager::GetDebugProc()->Print("�U���̎�ޕύX�F�y�p���[+2, �͈�+1�z �ύX�F�ySPACE�z\n", m_Shotmode);
		break;

	case ATK_DEBUG_CROWD:
		// �f�o�b�O�\�L
		CManager::GetDebugProc()->Print("�U���̎�ޕύX�F�y�͈�+3�z �ύX�F�ySPACE�z\n", m_Shotmode);
		break;

	case ATK_DEBUG_CROWD2_POWER1:
		// �f�o�b�O�\�L
		CManager::GetDebugProc()->Print("�U���̎�ޕύX�F�y�͈�+2, �p���[+1�z �ύX�F�ySPACE�z\n", m_Shotmode);
		break;

	case ATK_DEBUG_CROWD2_SPEED1:
		// �f�o�b�O�\�L
		CManager::GetDebugProc()->Print("�U���̎�ޕύX�F�y�͈�+2, �X�s�[�h+1�z �ύX�F�ySPACE�z\n", m_Shotmode);
		break;

	case ATK_DEBUG_SPEED:
		// �f�o�b�O�\�L
		CManager::GetDebugProc()->Print("�U���̎�ޕύX�F�y�X�s�[�h+3�z �ύX�F�ySPACE�z\n", m_Shotmode);
		break;

	case ATK_DEBUG_SPEED2_POWER1:
		// �f�o�b�O�\�L
		CManager::GetDebugProc()->Print("�U���̎�ޕύX�F�y�X�s�[�h+2, �p���[+1�z �ύX�F�ySPACE�z\n", m_Shotmode);
		break;

	case ATK_DEBUG_SPEED2_CROWD1:
		// �f�o�b�O�\�L
		CManager::GetDebugProc()->Print("�U���̎�ޕύX�F�y�X�s�[�h+2, �͈�+1�z �ύX�F�ySPACE�z\n", m_Shotmode);
		break;
	}
	CManager::GetDebugProc()->Print(
		"\n"
		"====================================================================\n"
		"�n�`�j��F�yO or Y�z\n"
		"====================================================================\n\n", m_Shotmode);

	// ���@�̍U��
#if 0

	if ((pInputKeyboard->GetTrigger(DIK_RETURN) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true) &&
		m_bATK == false && nType != MOTION_DIRECTATK && nType != MOTION_SUPERATK)
	{// �U��

		// �U���^�C�v�ύX
		m_Shotmode = ATK_DEBUG_POWER;
		m_bATK = true;		// �U������ON

		// �ڕW�̋����ݒ�
		pCamera->SetLenDest(pCamera->GetOriginDistance() + 200.0f);

		if (m_Shotmode == ATK_DEBUG_CROWD && nType != MOTION_MAGIC_ICE)
		{// ���@�̎�

			// �ڕW�̒n�_����
			m_pTargetP = CTargetPoint::Create(
				D3DXVECTOR3(pos.x + sinf(D3DX_PI + GetRotation().y) * TARGET_LEN, pos.y, pos.z + cosf(D3DX_PI + GetRotation().y) * TARGET_LEN),
				400.0f,
				800.0f);

			m_bMWait = true;		// ���@�ҋ@ON
			m_bMove = false;		// �ړ�OFF

			// �ҋ@���[�V����
			m_pMotion->Set(MOTION_MAGIC_WAIT);
		}
		else if (m_Shotmode == ATK_DEBUG_CROWD2_POWER1 && nType != MOTION_MAGIC_EXPLOSION)
		{// �������@�̎�

			// �ڕW�̒n�_����
			m_pTargetP = CTargetPoint::Create(
				D3DXVECTOR3(pos.x + sinf(D3DX_PI + GetRotation().y) * TARGET_LEN, pos.y, pos.z + cosf(D3DX_PI + GetRotation().y) * TARGET_LEN),
				200.0f, 800.0f);

			m_bMWait = true;		// ���@�ҋ@ON
			m_bMove = false;		// �ړ�OFF

			// �ҋ@���[�V����
			m_pMotion->Set(MOTION_MAGIC_WAIT);
		}
		else if (m_Shotmode == ATK_DEBUG_POWER2_SPEED1 && nType != MOTION_CLOWWAIT)
		{// �܂̎�

			m_bMWait = true;		// ���@�ҋ@ON
			m_bMove = false;		// �ړ�OFF
			//m_bATK = true;		// �U������ON

			// �ҋ@���[�V����
			//m_pMotion->Set(MOTION_CLOWWAIT);
		}
		else if (m_Shotmode == ATK_DEBUG_POWER)
		{
			//if (m_bCharge == false && CGame::GetPowerGauge()->GetLevel() != CPowerGauge::LEVEL_MAX)
			//{// �`���[�W���ĂȂ��Ƃ�����
			//	m_bMove = false;	// �ړ�����OFF
			//	m_bCharge = true;
			//}
			//else if (m_bCharge == false && CGame::GetPowerGauge()->GetLevel() == CPowerGauge::LEVEL_MAX)
			//{
			//	m_bATK = true;		// �U������ON
			//	m_bCharge = false;	// �`���[�WOFF
			//}
			m_bATK = true;		// �U������ON
			m_bCharge = false;	// �`���[�WOFF

			//if (m_bCharge == true && nType == MOTION_CHARGE)
			{// �`���[�W���̎�

				m_bATK = true;		// �U������ON
				m_bCharge = false;	// �`���[�WOFF
			}

		}
		else
		{
			// �U������ON
			m_bATK = true;
		}

		if (m_Shotmode == ATK_DEBUG_SPEED)
		{// ���b�V���̎�

			// ���@�w����
			m_pMCircle = CMagicCircle::Create(
				D3DXVECTOR3(pos.x, pos.y + 150.0f, pos.z),
				D3DXVECTOR3(-0.5f, rot.y, 0.0f),
				D3DXVECTOR3(100.0f, 100.0f, 0.0f));
		}
	}


	if (m_Shotmode == ATK_DEBUG_SPEED || m_Shotmode == ATK_DEBUG_CROWD || m_Shotmode == ATK_DEBUG_CROWD2_POWER1 || m_Shotmode == ATK_DEBUG_POWER2_SPEED1)
	{// ���b�V���̎�

		if (pInputKeyboard->GetPress(DIK_RETURN) == true)
		{// �U��

			if (m_Shotmode == ATK_DEBUG_SPEED)
			{// �X�s�[�h�n�̎�����

				// ���b�V������
				if (nType == MOTION_SPEED_RUSH_MOVE)
				{// �ړ����b�V����
					CSpeedRush::Create(pos, rot, 1);
				}
				else
				{
					CSpeedRush::Create(pos, rot, 3);
				}

				// �U������ON
				m_bATK = true;
			}

			if (m_Shotmode == ATK_DEBUG_POWER2_SPEED1)
			{
				// �U������ON
				m_bATK = true;
			}
		}

		if (pInputKeyboard->GetRelease(DIK_RETURN) == true)
		{// Enter�������ꂽ��

			// ���@�w������
			if (m_pMCircle != NULL)
			{
				m_pMCircle->Uninit();
				m_pMCircle = NULL;
			}

			if (nType == MOTION_MAGIC_WAIT)
			{// ���@�̎�

				// �U������ON
				m_bATK = true;

				// �ҋ@����OFF
				m_bMWait = false;
			}

			if (nType == MOTION_CLOWATK)
			{// �܍U���̎�

				m_bATK = false;	// �U������OFF
			}
		}
	}

	// ���@�w�X�V
	if (m_pMCircle != NULL)
	{// NULL����Ȃ�������

		// �ʒu�E�����擾
		D3DXVECTOR3 MagisSRot = m_pMCircle->GetRotation();
		m_pMCircle->SetRotation(D3DXVECTOR3(MagisSRot.x, rot.y, MagisSRot.z));
		m_pMCircle->SetPosition(D3DXVECTOR3(pos.x, pos.y + 150.0f, pos.z));
	}

	// �ڕW�n�_�X�V
	if (m_pTargetP != NULL)
	{
		// �����擾
		D3DXVECTOR3 TargetRot = m_pTargetP->GetRotation();
		m_pTargetP->SetRotation(D3DXVECTOR3(TargetRot.x, rot.y, TargetRot.z));
	}

#else

#endif

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

	//�p�x�̐��K��
	RotNormalize(fRotDest);

	//���݂ƖڕW�̍��������߂�
	float fRotDiff = fRotDest - rot.y;

	//�p�x�̐��K��
	RotNormalize(fRotDiff);

	//�p�x�̕␳������
#ifndef AUTO_MOVE
	if (m_state != STATE_CONFUSION)
	{// ������Ԃ̂Ƃ�
		// ���s���Đ�
		CManager::GetSound()->StopSound(CSound::LABEL_SE_CONFUSION);

		rot.y += fRotDiff * 0.014f;
	}
	
#else
	rot.y += fRotDiff * 0.1f;
#endif

	//�p�x�̐��K��
	RotNormalize(rot.y);

	// �X���C�h
	m_fMoveSlideDest = CGame::GetObject3DMesh()->Slide(pos);
	m_fMoveSlideDest *= SLIDEMOVE;

	m_fMoveSlide += (m_fMoveSlideDest - m_fMoveSlide) * 0.1f;

#ifndef AUTO_MOVE
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_CONFUSION && CManager::GetScene()->GetWaveManager()->IsStageFrag() == true)
	{
		move.x += sinf(D3DX_PI + rot.y) * (fMove + m_fMoveSlide);
		move.z += cosf(D3DX_PI + rot.y) * (fMove + m_fMoveSlide);
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

	if (m_state != STATE_ATTACK && m_state != STATE_CONFUSION && m_state != STATE_DMG && m_state != STATE_KNOCKBACK &&
		CManager::GetScene()->GetWaveManager()->IsStageFrag() == true)
	{
		if (pInputKeyboard->GetTrigger(DIK_O) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_Y, 0) == true)
		{//���L�[�������ꂽ,���ړ�

			// �U���^�C�v�ύX
			m_Shotmode = ATK_DIRECT;
			m_bATK = true;		// �U������ON
		}

		if ((pInputKeyboard->GetTrigger(DIK_RETURN) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true) &&
			m_bATK == false && nType != MOTION_DIRECTATK && nType != MOTION_SUPERATK)
		{// �U��

			// �U���^�C�v�ύX
			m_Shotmode = ATK_DEBUG_POWER;
			m_bATK = true;		// �U������ON

			// �ڕW�̋����ݒ�
			pCamera->SetLenDest(pCamera->GetOriginDistance() + 200.0f);
		}
	}
	else if(CManager::GetScene()->GetWaveManager()->IsStageFrag() == false)
	{// �X�e�[�W�؂�ւ���

		m_bATK = false;		// �U������OFF
		m_bMove = false;	// �ړ�����OFF

		// �j���[�g�������[�V�����ݒ�
		m_pMotion->Set(MOTION_DEF);
	}

#if _DEBUG
	if (pInputKeyboard->GetTrigger(DIK_I) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_X, 0) == true)
	{//���L�[�������ꂽ,���ړ�

		// �g����
		CGame::GetObject3DMesh()->SetWave
		(
			D3DXVECTOR3(
				pos.x + sinf(D3DX_PI * 0.75f + GetRotation().y) * 800.0f,
				pos.y,
				pos.z + cosf(D3DX_PI * 0.75f + GetRotation().y) * 800.0f),	// �ʒu
			GetRotation(),										// ����
			D3DXVECTOR3(
				0.0f,
				0.0f,
				0.0f),											// �ړ���
			D3DXVECTOR2(200.0f, 200.0f),						// ����͈�
			0.0f,							// �g�����̕p�x
			180.0f,											// �g�̍���
			0.0f,												// �g�̍L����
			60,													// ��������
			CElevation::WAVETYPE_RAISE,						// ���
			CElevation::TYPE_SPAWN,							// �v���C���[�̎��
			0												// ���x��
		);

		// �g����
		CGame::GetObject3DMesh()->SetWave
		(
			D3DXVECTOR3(
				pos.x + sinf(-D3DX_PI * 0.75f + GetRotation().y) * 800.0f,
				pos.y,
				pos.z + cosf(-D3DX_PI * 0.75f + GetRotation().y) * 800.0f),	// �ʒu
			GetRotation(),									// ����
			D3DXVECTOR3(
				sinf(D3DX_PI + GetRotation().y) * 10.0f,
				0.0f,
				cosf(D3DX_PI + GetRotation().y) * 10.0f),					// �ړ���
			D3DXVECTOR2(200.0f, 300.0f),									// ����͈�
			CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// �g�����̕p�x
			-300.0f,														// �g�̍���
			D3DX_PI * 0.0f,													// �g�łp�x
			150,															// ��������
			CElevation::WAVETYPE_DIRECT,										// ���
			CElevation::TYPE_NATURAL,								// �v���C���[�̎��
			CPowerGauge::LEVEL_1
		);
	}
#endif
}

//==========================================================================
// ����`�F���W����
//==========================================================================
void CPlayer::ChangeWeapon(const int *pPoint)
{
	// �莝�����탊�Z�b�g
	for (int nCntParts = 16; nCntParts < GetObjectChara()->GetNumModel(); nCntParts++)
	{
		ChangeObject(nCntParts, -1);
	}

	// �ϑz�G�E�B���h�E������
	if (m_pDelusionWindow != NULL)
	{// �g���Ă�����
		m_pDelusionWindow->Uninit();
		m_pDelusionWindow = NULL;
	}

	// ���@�w������
	if (m_pMCircle != NULL)
	{
		m_pMCircle->Uninit();
		m_pMCircle = NULL;
	}

	// �ڕW�̒n�_����
	if (m_pTargetP != NULL)
	{
		m_pTargetP->Uninit();
		m_pTargetP = NULL;
	}
	m_bMWait = false;		// ���@�ҋ@OFF

	//*****************************
	// �p���[�n
	//*****************************
	if (pPoint[CDelusionMeter::TYPE_POWER] == 4)
	{// �p���[3
		m_Shotmode = ATK_DEBUG_POWER;
		SetObject(16);
	}
	else if (pPoint[CDelusionMeter::TYPE_POWER] == 3 && pPoint[CDelusionMeter::TYPE_SPEED] == 2)
	{// �p���[2, �X�s�[�h1
		m_Shotmode = ATK_DEBUG_POWER2_SPEED1;
		SetObject(18);
		SetObject(19);
	}
	else if (pPoint[CDelusionMeter::TYPE_POWER] == 3 && pPoint[CDelusionMeter::TYPE_CROWD] == 2)
	{// �p���[2, �Q��1
		m_Shotmode = ATK_DEBUG_POWER2_CROWD1;
		SetObject(20);
	}
	//*****************************
	// �X�s�[�h�n
	//*****************************
	else if (pPoint[CDelusionMeter::TYPE_SPEED] == 4)
	{// �X�s�[�h3
		m_Shotmode = ATK_DEBUG_SPEED;
	}
	else if (pPoint[CDelusionMeter::TYPE_SPEED] == 3 && pPoint[CDelusionMeter::TYPE_POWER] == 2)
	{// �X�s�[�h2, �p���[1
		m_Shotmode = ATK_DEBUG_SPEED2_POWER1;
	}
	else if (pPoint[CDelusionMeter::TYPE_SPEED] == 3 && pPoint[CDelusionMeter::TYPE_CROWD] == 2)
	{// �X�s�[�h2, �Q��1
		m_Shotmode = ATK_DEBUG_SPEED2_CROWD1;
	}
	//*****************************
	// �Q�̌n
	//*****************************
	else if (pPoint[CDelusionMeter::TYPE_CROWD] == 4)
	{// �Q��3
		m_Shotmode = ATK_DEBUG_CROWD;
		SetObject(17);
	}
	else if (pPoint[CDelusionMeter::TYPE_CROWD] == 3 && pPoint[CDelusionMeter::TYPE_POWER] == 2)
	{// �Q��2, �p���[1
		m_Shotmode = ATK_DEBUG_CROWD2_POWER1;
	}
	else if (pPoint[CDelusionMeter::TYPE_CROWD] == 3 && pPoint[CDelusionMeter::TYPE_SPEED] == 2)
	{// �Q��2, �X�s�[�h1
		m_Shotmode = ATK_DEBUG_CROWD2_SPEED1;
	}
	else
	{
		m_Shotmode = ATK_DEBUG_POWER1;
	}
}

//==========================================================================
// ���[�V�����̐ݒ�
//==========================================================================
void CPlayer::MotionSet(void)
{
	if (m_pMotion->IsFinish() == true)
	{// �I�����Ă�����

		// ���݂̎�ގ擾
		int nType = m_pMotion->GetType();
		int nOldType = m_pMotion->GetOldType();

		if (m_bMove == true && m_bATK == false && m_bKnockback == false)
		{// �ړ����Ă�����

			m_bMove = false;	// �ړ�����OFF
			m_bCharge = false;	// �`���[�W����OFF

			// �ړ����[�V����
			switch (m_walkType)
			{
			case CPlayer::WALKTYPE_NORMAL:
				m_pMotion->Set(MOTION_WALK);
				break;

			case CPlayer::WALKTYPE_SPEED:
				m_pMotion->Set(MOTION_SPEEDWALK);
				break;

			case CPlayer::WALKTYPE_SLOW:
				m_pMotion->Set(MOTION_SLOWWALK);
				break;

			default:
				m_pMotion->Set(MOTION_WALK);
				break;
			}
		}
		else if (m_bKnockback == true)
		{// ���ꒆ��������

			// ���ꃂ�[�V����
			m_pMotion->Set(MOTION_KNOCKBACK);
		}
		else if (m_state == STATE_CONFUSION)
		{// ��������������

			// ���ꃂ�[�V����
			m_pMotion->Set(MOTION_CONFUSION);
		}
		else if (m_bATK == true)
		{// �U�����Ă�����

			m_bATK = false;		// �U������OFF

			switch (m_Shotmode)
			{
			case ATK_DEBUG_POWER:

				m_pMotion->Set(MOTION_SUPERATK);
				break;

			case ATK_DIRECT:
				// ���s�g�U��
				m_pMotion->Set(MOTION_DIRECTATK);
				break;
			}
		}
		else
		{
			// �j���[�g�������[�V����
			m_pMotion->Set(MOTION_DEF);
		}
	}
}

//==========================================================================
// �U��
//==========================================================================
void CPlayer::Atack(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �J�����̏��擾
	CCamera *pCamera = CManager::GetCamera();

	// �J�����̌����擾
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// ���[�V�����̏��擾
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// �U�����̑����擾
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	int nLevel = 0;
	//int nLevel = CGame::GetPowerGauge()->GetLevel();

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
			case MOTION_SUPERATK:

				//// �p�[�e�B�N������
				//my_particle::Create(weponpos, my_particle::TYPE_SUPERATTACK);

				//// �`���[�W�J�E���g���Z�b�g
				////CGame::GetPowerGauge()->SetChargeCount(0);

				// ���I����
				CBallast::Create(weponpos, D3DXVECTOR3(2.0f, 8.0f, 2.0f), 16);

				// �Ռ��g����
				CImpactWave::Create
				(
					D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// �ʒu
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// ����
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f),			// �F
					100.0f,										// ��
					20.0f,										// ����
					20,											// ����
					28.0f,										// ���̈ړ���
					CImpactWave::TYPE_BLACK2,					// �e�N�X�`���^�C�v
					true										// ���Z�������邩
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(pos.x, pos.y + 150.0f, pos.z),	// �ʒu
					D3DXVECTOR3(0.0f, 0.0f, D3DX_PI),				// ����
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f),			// �F
					180.0f,										// ��
					150.0f,										// ����
					14,											// ����
					4.0f,										// ���̈ړ���
					CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
					false										// ���Z�������邩
				);

				// �U��
				CManager::GetCamera()->SetShake(20, 10.0f, 0.0f);


				// ���s���Đ�
				CManager::GetSound()->PlaySound(CSound::LABEL_SE_IMPACT01);

				// �g����
				switch (nLevel)
				{
				case CPowerGauge::LEVEL_1:

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
						CGame::GetObject3DMesh()->GetWidthLen() * 1.0f,		// �g�����̕p�x
						-180.0f,											// �g�̍���
						12.5f,												// �g�̍L����
						30,													// ��������
						CElevation::WAVETYPE_IMPACT,						// ���
						CElevation::TYPE_PLAYER,							// �v���C���[�̎��
						nLevel												// ���x��
					);
					break;

				case CPowerGauge::LEVEL_2:

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
						CGame::GetObject3DMesh()->GetWidthLen() * 1.0f,		// �g�����̕p�x
						-180.0f,											// �g�̍���
						19.0f,												// �g�̍L����
						30,													// ��������
						CElevation::WAVETYPE_IMPACT,						// ���
						CElevation::TYPE_PLAYER,							// �v���C���[�̎��
						nLevel												// ���x��
					);
					break;

				case CPowerGauge::LEVEL_3:

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
						CGame::GetObject3DMesh()->GetWidthLen() * 1.0f,		// �g�����̕p�x
						-180.0f,											// �g�̍���
						30.0f,												// �g�̍L����
						25,													// ��������
						CElevation::WAVETYPE_IMPACT,						// ���
						CElevation::TYPE_PLAYER,							// �v���C���[�̎��
						nLevel												// ���x��
					);
					break;

				case CPowerGauge::LEVEL_MAX:
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
						CGame::GetObject3DMesh()->GetWidthLen() * 1.5f,		// �g�����̕p�x
						-200.0f,											// �g�̍���
						40.0f,												// �g�̍L����
						30,													// ��������
						CElevation::WAVETYPE_IMPACT,						// ���
						CElevation::TYPE_PLAYER,							// �v���C���[�̎��
						nLevel												// ���x��
					);
					break;
				}
				break;

			case MOTION_DIRECTATK:

				// �n�ʂ̔���
				CGame::GetObject3DMesh()->IsHit(weponpos);

				// �ڕW�̋����ݒ�
				pCamera->SetLenDest(pCamera->GetOriginDistance() + 100.0f);

				// �U��
				CManager::GetCamera()->SetShake(120, 8.0f, 0.0f);

				// ���s���Đ�
				CManager::GetSound()->PlaySound(CSound::LABEL_SE_SWING);

				CImpactWave::Create
				(
					D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * 80.0f, pos.y + 50.0f, pos.z + cosf(D3DX_PI + rot.y) * 80.0f),	// �ʒu
					D3DXVECTOR3(D3DX_PI * 0.5f, rot.y, D3DX_PI),				// ����
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.2f),			// �F
					80.0f,										// ��
					80.0f,										// ����
					20,											// ����
					16.0f,										// ���̈ړ���
					CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
					false										// ���Z�������邩
				);

				// �g����
				switch (nLevel)
				{
				case CPowerGauge::LEVEL_1:

					// �g����
					CGame::GetObject3DMesh()->SetWave
					(
						D3DXVECTOR3(
							pos.x + sinf(D3DX_PI + rot.y) * 250.0f,
							pos.y,
							pos.z + cosf(D3DX_PI + rot.y) * 250.0f),	// �ʒu
						D3DXVECTOR3(
							0.0f,
							rot.y,
							0.0f),									// ����
						D3DXVECTOR3(
							sinf(D3DX_PI + rot.y) * 25.0f,
							0.0f,
							cosf(D3DX_PI + rot.y) * 25.0f),					// �ړ���
						D3DXVECTOR2(70.0f, 300.0f),									// ����͈�
						CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// �g�����̕p�x
						-300.0f,														// �g�̍���
						D3DX_PI * 0.0f,													// �g�łp�x
						60,															// ��������
						CElevation::WAVETYPE_DIRECT,										// ���
						CElevation::TYPE_PLAYER,								// �v���C���[�̎��
						CPowerGauge::LEVEL_1
					);
					break;

				case CPowerGauge::LEVEL_2:

					// �g����
					CGame::GetObject3DMesh()->SetWave
					(
						D3DXVECTOR3(
							pos.x + sinf(Camerarot.y) * 400.0f,
							pos.y,
							pos.z + cosf(Camerarot.y) * 400.0f),	// �ʒu
						D3DXVECTOR3(
							0.0f,
							D3DX_PI + Camerarot.y,
							0.0f),									// ����
						D3DXVECTOR3(
							sinf(Camerarot.y) * 17.0f,
							0.0f,
							cosf(Camerarot.y) * 17.0f),					// �ړ���
						D3DXVECTOR2(70.0f, 300.0f),									// ����͈�
						CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// �g�����̕p�x
						-375.0f,														// �g�̍���
						D3DX_PI * 0.0f,													// �g�łp�x
						100,															// ��������
						CElevation::WAVETYPE_DIRECT,										// ���
						CElevation::TYPE_PLAYER,								// �v���C���[�̎��
						CPowerGauge::LEVEL_1
					);
					break;

				case CPowerGauge::LEVEL_3:

					// �g����
					CGame::GetObject3DMesh()->SetWave
					(
						D3DXVECTOR3(
							pos.x + sinf(Camerarot.y) * 400.0f,
							pos.y,
							pos.z + cosf(Camerarot.y) * 400.0f),	// �ʒu
						D3DXVECTOR3(
							0.0f,
							D3DX_PI + Camerarot.y,
							0.0f),									// ����
						D3DXVECTOR3(
							sinf(Camerarot.y) * 20.0f,
							0.0f,
							cosf(Camerarot.y) * 20.0f),					// �ړ���
						D3DXVECTOR2(70.0f, 300.0f),									// ����͈�
						CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// �g�����̕p�x
						-450.0f,														// �g�̍���
						D3DX_PI * 0.0f,													// �g�łp�x
						75,															// ��������
						CElevation::WAVETYPE_DIRECT,										// ���
						CElevation::TYPE_PLAYER,								// �v���C���[�̎��
						CPowerGauge::LEVEL_1
					);
					break;

				case CPowerGauge::LEVEL_MAX:

					// �g����
					CGame::GetObject3DMesh()->SetWave
					(
						D3DXVECTOR3(
							pos.x + sinf(Camerarot.y) * 400.0f,
							pos.y,
							pos.z + cosf(Camerarot.y) * 400.0f),	// �ʒu
						D3DXVECTOR3(
							0.0f,
							D3DX_PI + Camerarot.y,
							0.0f),									// ����
						D3DXVECTOR3(
							sinf(Camerarot.y) * 25.0f,
							0.0f,
							cosf(Camerarot.y) * 25.0f),				// �ړ���
						D3DXVECTOR2(140.0f, 400.0f),									// ����͈�
						CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// �g�����̕p�x
						-500.0f,														// �g�̍���
						D3DX_PI * 0.0f,													// �g�łp�x
						90,															// ��������
						CElevation::WAVETYPE_DIRECT,									// ���
						CElevation::TYPE_PLAYER,										// �v���C���[�̎��
						CPowerGauge::LEVEL_MAX											// ���x��
					);
					break;
				}
				break;
			}
		}

		// ���[�V�����J�E���^�[�擾
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt && m_pMotion->GetAllCount() < aInfo.AttackInfo[nCntAttack]->nMaxCnt)
		{// �U�����蒆

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

#if TOPCUR

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

				// �G�̈ʒu�擾
				D3DXMATRIX mtxOrigin = pObj->GetObjectChara()->GetModel()[0]->GetWorldMtx();
				D3DXVECTOR3 TargetPos = D3DXVECTOR3(mtxOrigin._41, mtxOrigin._42, mtxOrigin._43);

				// ����T�C�Y�擾
				float fRadius = pObj->GetObjectChara()->GetRadius();

				if (SphereRange(weponpos, TargetPos, aInfo.AttackInfo[nCntAttack]->fRangeSize, fRadius))
				{// ���̔���

					if (pObj->Hit(aInfo.AttackInfo[nCntAttack]->nDamage) == true)
					{// ����ł���

						my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
					}
				}

				// ���̃I�u�W�F�N�g����
				pObj = pObjNext;
			}
#endif
		}
	}

	CManager::GetDebugProc()->Print(
		"���[�V�����J�E���^�[�F%d\n", m_pMotion->GetAllCount());
}

//==========================================================================
// �����蔻��
//==========================================================================
void CPlayer::Collision(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// ���n�������ǂ���
	bool bLand = false;
	float fHeight = 0.0f;

	// �����擾
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		fHeight = CManager::GetScene()->GetObject3DMesh()->GetHeight(pos, move, bLand, GetType());
	}
	else
	{
		fHeight = pos.y;
	}

	//if (fHeight > pos.y)
	{// �n�ʂ̕���������荂��������

		// �n�ʂ̍����ɕ␳
		pos.y = fHeight;

		if (bLand == true)
		{// ���n���Ă���

			// �W�����v�g�p�\�ɂ���
			m_bJump = false;
			move.y = 0.0f;
		}
	}

	// �ʒu�擾
	D3DXVECTOR3 posOld = GetPosition();

	// ��
	float fLen = CGame::GetObject3DMesh()->GetWidthLen();
	int nBlock = CGame::GetObject3DMesh()->GetWidthBlock();
	nBlock /= 2;
	if (pos.x + GetRadius() >= fLen * nBlock) { pos.x = fLen * nBlock - GetRadius(); }
	if (pos.x - GetRadius() <= -fLen * nBlock) { pos.x = -fLen * nBlock + GetRadius(); }
	if (pos.z + GetRadius() >= fLen * nBlock) { pos.z = fLen * nBlock - GetRadius(); }
	if (pos.z - GetRadius() <= -fLen * nBlock) { pos.z = -fLen * nBlock + GetRadius(); }
	
	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);
}

//==========================================================================
// �q�b�g����
//==========================================================================
bool CPlayer::Hit(const int nValue)
{
	if (m_state != STATE_DEAD && m_state != STATE_DMG && m_state != STATE_KNOCKBACK && m_state != STATE_INVINCIBLE && m_state != STATE_CONFUSION)
	{// �_���[�W��t��Ԃ̎�

		// �̗͌��炷
		//m_nLife -= nValue;

		if (m_nLife <= 0)
		{// �̗͂��Ȃ��Ȃ�����

			// �����
			m_state = STATE_DEAD;

			// ���S����
			Kill();

			// �I������
			Uninit();

			// ����
			return true;
		}

		// �␳
		ValueNormalize(m_nLife, MAX_LIFE, 0);

		// �ߋ��̏�ԕۑ�
		m_Oldstate = m_state;

		// �_���[�W��Ԃɂ���
		m_state = STATE_DMG;

		// �J�ڃJ�E���^�[�ݒ�
		m_nCntState = 0;

		// �m�b�N�o�b�N�̈ʒu�X�V
		D3DXVECTOR3 pos = GetPosition();
		D3DXVECTOR3 rot = GetRotation();
		m_posKnokBack = pos;

		// �m�b�N�o�b�N����ɂ���
		m_bKnockback = true;

		// ���ꃂ�[�V����
		m_pMotion->Set(MOTION_KNOCKBACK);

		// �Ռ��g����
		CImpactWave::Create
		(
			D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// �ʒu
			D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// ����
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f),			// �F
			80.0f,										// ��
			80.0f,										// ����
			20,											// ����
			10.0f,										// ���̈ړ���
			CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
			false										// ���Z�������邩
		);

		if (nValue == mylib_const::DIRECTWAVE_DMG)
		{
			CManager::SetEnableHitStop(12);

			// �U��
			CManager::GetCamera()->SetShake(12, 25.0f, 0.0f);

			// �T�E���h�Đ�
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_PLAYERDMG_BIG);
		}
		else if (nValue == mylib_const::IMPACTWAVE_DMG)
		{
			//CManager::SetEnableHitStop(8);

			// �U��
			CManager::GetCamera()->SetShake(8, 15.0f, 0.0f);

			// �T�E���h�Đ�
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_PLAYERDMG);
		}
		else
		{
			CManager::SetEnableHitStop(12);

			// �U��
			//CManager::GetCamera()->SetShake(8, 20.0f, 0.0f);
			CManager::GetCamera()->SetShake(12, 25.0f, 0.0f);

			// �T�E���h�Đ�
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_PLAYERDMG);
		}
	}

	// ����łȂ�
	return false;
}

//==========================================================================
// ��ԍX�V����
//==========================================================================
void CPlayer::UpdateState(void)
{
	switch (m_state)
	{
	case STATE_NONE:
		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		break;

	case STATE_INVINCIBLE:
		Invincible();
		break;

	case STATE_DMG:
		Damage();
		break;

	case STATE_CONFUSION:
		Confusion();	// ����
		break;

	case STATE_KNOCKBACK:
		KnockBack();
		break;
	}
}

//==========================================================================
// ���G
//==========================================================================
void CPlayer::Invincible(void)
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState--;

	if (m_nCntState % INVINCIBLE_INT == 0)
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

	if (m_nCntState <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����

		// �Ȃɂ��Ȃ���Ԃɂ���
		m_state = STATE_NONE;
		m_nCntState = 0;

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

}

//==========================================================================
// �_���[�W
//==========================================================================
void CPlayer::Damage(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	// �ʒu�X�V
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + move.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;

	// �N���Ƃ̔���
	if (CGame::GetObject3DMesh()->IsHit(pos) && m_nCntState >= 20)
	{// �n�ʂƓ������Ă�����
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		move.y = 0.0f;			// �ړ��ʃ[��
		
		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �m�b�N�o�b�N�������
		m_bKnockback = false;
		m_pMotion->ToggleFinish(true);

		//if (m_bConfusion == true)
		//{// ��������������
		//	

		//	// �����G�t�F�N�g�ݒ�
		//	if (m_pConfusionEffect == NULL)
		//	{
		//		m_pConfusionEffect = CConfusionEffect::Create(D3DXVECTOR3(pos.x, pos.y + 100.0f, pos.z), 60.0f, m_nCntConfusion);
		//	}

		//	// �����ݒ�
		//	m_state = STATE_CONFUSION;
		//	m_pMotion->Set(MOTION_CONFUSION);
		//}
	}
	else if (m_nCntState >= 30)
	{// �J�ڃJ�E���^�[��30�ɂȂ�����

		// �m�b�N�o�b�N��Ԃɂ���
		m_state = STATE_KNOCKBACK;
	}


	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);
}

//==========================================================================
// �m�b�N�o�b�N
//==========================================================================
void CPlayer::KnockBack(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

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


	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �ʒu�X�V
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + move.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;

	// �N���Ƃ̔���
	if (CGame::GetObject3DMesh()->IsHit(pos) && m_nCntState >= 20)
	{// �n�ʂƓ������Ă�����
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		move.y = 0.0f;	// �ړ��ʃ[��
		
		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		
		// �m�b�N�o�b�N�������
		m_bKnockback = false;
		m_pMotion->ToggleFinish(true);

		//if (m_bConfusion == true)
		//{// ��������������

		//	// �����G�t�F�N�g�ݒ�
		//	if (m_pConfusionEffect == NULL)
		//	{
		//		m_pConfusionEffect = CConfusionEffect::Create(D3DXVECTOR3(pos.x, pos.y + 100.0f, pos.z), 60.0f, m_nCntConfusion);
		//	}

		//	// �����ݒ�
		//	m_state = STATE_CONFUSION;
		//	m_pMotion->Set(MOTION_CONFUSION);
		//}
	}


	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
}

//==========================================================================
// ����
//==========================================================================
void CPlayer::Confusion(void)
{

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ő�t���[���擾
	int nAllFrame = m_pMotion->GetMaxAllCount();
	nAllFrame *= 2;

	//float fDiff = m_rotConfusion.y - rot.y;
	rot.y += m_rotConfusion.y / (float)(nAllFrame);

	// �p�x�̐��K��
	RotNormalize(rot.y);

	// �ʒu�X�V
	pos.x += sinf(rot.y) * 4.0f;
	pos.z += cosf(rot.y) * 4.0f;

	// �J�E���^�[���Z
	m_nCntConfusion--;

	if (m_nCntConfusion <= 0)
	{
		m_nCntConfusion = 0;
		m_bConfusion = false;

		// ���G��Ԃɂ���
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;

		// �����G�t�F�N�g�폜
		if (m_pConfusionEffect != NULL)
		{
			m_pConfusionEffect->Uninit();
			m_pConfusionEffect = NULL;
		}
	}

	if (m_pConfusionEffect != NULL)
	{
		m_pConfusionEffect->UpdatePosition(GetPosition());
	}

	// �����ݒ�
	SetRotation(rot);

	// �ʒu�ݒ�
	SetPosition(pos);
}

//==========================================================================
// �`�揈��
//==========================================================================
void CPlayer::Draw(void)
{
#if _DEBUG
	CObjectChara::Draw(m_mMatcol);
#else
	CObjectChara::Draw();
#endif
}

//==========================================================================
// ������L���ɂ���
//==========================================================================
void CPlayer::EnableConfusion(void)
{
	// �����J�E���^�[�ݒ�
	m_nCntConfusion = m_pMotion->GetMaxAllCount(MOTION_CONFUSION) * 2;
	m_bConfusion = true;

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �����G�t�F�N�g�ݒ�
	/*if (m_pConfusionEffect == NULL)
	{
		m_pConfusionEffect = CConfusionEffect::Create(D3DXVECTOR3(pos.x, pos.y + 100.0f, pos.z), 60.0f, m_nCntConfusion);
	}*/

	// �����ݒ�
	m_state = STATE_CONFUSION;
	m_pMotion->Set(MOTION_CONFUSION);


	// �����擾
	D3DXVECTOR3 rot = GetRotation();
	//while (1)
	//{// �͈͊O�����܂Ŗ������[�v

	//	// ���������_������
	//	fRotDest = (float)Random(-314, 314) * 0.01f;

	//	float fMax = rot.y + D3DX_PI * 0.2f;
	//	float fMin = rot.y - D3DX_PI * 0.2f;
	//	/*RotNormalize(fMax);
	//	RotNormalize(fMin);*/

	//	if (fRotDest <= fMax &&
	//		fRotDest >= fMin)
	//	{
	//		break;
	//	}

	//}

	// ���s���Đ�
	CManager::GetSound()->PlaySound(CSound::LABEL_SE_CONFUSION);

	m_rotConfusion.y = (rot.y - D3DX_PI * 2.0f) + ((float)Random(-30, 30) * 0.01f);	// �����̌����ݒ�
}

//==========================================================================
// ���̒��ɂ��锻��
//==========================================================================
void CPlayer::IsSwitchInWater(bool bFrag)
{
	m_bInWater = bFrag;
}

//==========================================================================
// ��Ԏ擾
//==========================================================================
int CPlayer::GetState(void)
{
	return m_state;
}