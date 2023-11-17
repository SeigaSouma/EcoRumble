//=============================================================================
// 
//  �G�̃}�l�[�W������ [enemymanager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "debugproc.h"
#include "enemymanager.h"
#include "enemybase.h"
#include "wavemanager.h"
#include "calculation.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "enemy.h"
#include "boss.h"
#include "power_gauge.h"
#include "particle.h"
#include "fade.h"
#include "elevation.h"
#include "player.h"
#include "impactwave.h"
#include "camera.h"
#include "sound.h"

// �p����
#include "tutorial_enemymanager.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define SPAWN_STRENGTH	(100.0f)	// �X�|�[�����̍���
#define KILL_INT		(30)		// �|�����C���^�[�o��

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CEnemyManager::Pattern CEnemyManager::m_aPattern[mylib_const::MAX_PATTEN_ENEMY] = {};			// �ǂݍ��݃f�[�^
CWaveManager::SetStageInfo CEnemyManager::m_aSetEnemyInfo[mylib_const::MAX_WAVE_ENEMY] = {};	// �G�̔z�u���
std::string CEnemyManager::sMotionFileName[mylib_const::MAX_PATTEN_ENEMY] = {};					// ���[�V�����t�@�C����
int CEnemyManager::m_nPatternNum = 0;	// �o���p�^�[����
CEnemy *CEnemyManager::m_pEnemy[mylib_const::MAX_OBJ] = {};
CBoss *CEnemyManager::m_pBoss[mylib_const::MAX_OBJ] = {};
int CEnemyManager::m_nNumAll = 0;		// ����
int CEnemyManager::m_nNumChara = 0;		// �G�̎�ނ̑���
int CEnemyManager::m_nCntSpawn = 0;		// �o���J�E���g
int CEnemyManager::m_nParent[mylib_const::MAX_OBJ] = {};	// �G�̐e�|�C���^
bool CEnemyManager::m_bLoadPattern = false;	// �p�^�[���ǂݍ��ݔ���
int CEnemyManager::m_nNumTotalKill = 0;	// �܂Ƃ߂ē|������
int CEnemyManager::m_nIntervalKill = 0;	// �|�����J�E���g�̃C���^�[�o��
bool CEnemyManager::m_bHitStop = false;	// �q�b�g�X�g�b�v�̔���
CEnemyManager::STATE CEnemyManager::m_state = CEnemyManager::STATE_NONE;		// ���

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyManager::CEnemyManager()
{
	// �l�̃N���A
	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_OBJ; nCntEnemy++)
	{
		// �e��ID
		m_nParent[nCntEnemy] = -1;
	}
	m_state = STATE_NONE;	// ���

	// �������Z�b�g
	m_nNumAll = 0;
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyManager::~CEnemyManager()
{

}

//==========================================================================
// ��������
//==========================================================================
CEnemyManager *CEnemyManager::Create(const std::string pTextFile)
{
	// �����p�̃I�u�W�F�N�g
	CEnemyManager *pModel = NULL;

	if (pModel == NULL)
	{// NULL��������

		// �������̊m��
		switch (CManager::GetMode())
		{
		case CScene::MODE_TUTORIAL:
			pModel = DEBUG_NEW CTutorialEnemyManager;
			break;

		default:
			pModel = DEBUG_NEW CEnemyManager;
			break;
		}

		if (pModel != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			HRESULT hr = pModel->ReadText(pTextFile);

			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}

			// ����������
			hr = pModel->Init();

			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}
		}

		return pModel;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyManager::Init(void)
{
	// �o�����ǂݍ���
	//HRESULT hr;

	//if (FAILED(hr))
	//{// ���s���Ă�����
	//	return E_FAIL;
	//}

	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_OBJ; nCntEnemy++)
	{
		// �e��ID
		m_nParent[nCntEnemy] = -1;
	}

	// �������Z�b�g
	m_nNumAll = 0;
	m_nCntSpawn = 0;

	//SetBoss(4, D3DXVECTOR3(0.0f, 50.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyManager::Uninit(void)
{
	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_OBJ; nCntEnemy++)
	{
		if (m_pEnemy[nCntEnemy] != NULL)
		{
			//delete m_pEnemy[nCntEnemy];
			m_pEnemy[nCntEnemy] = NULL;
			m_nParent[nCntEnemy] = -1;
		}
	}
}

//==========================================================================
// �j��
//==========================================================================
void CEnemyManager::Release(int nIdx)
{
	if (m_pEnemy[nIdx] != NULL)
	{
		//delete m_pEnemy[nIdx];
		//memset(&m_pEnemy[nIdx], NULL, sizeof(m_pEnemy[nIdx]));
		m_pEnemy[nIdx] = NULL;
		m_nParent[nIdx] = -1;
	}

	// �������Z
	m_nNumAll--;
}

//==========================================================================
// �j��
//==========================================================================
void CEnemyManager::ReleaseBoss(int nIdx)
{
	if (m_pBoss[nIdx] != NULL)
	{
		m_pBoss[nIdx] = NULL;
	}

	m_nNumAll--;
}

//==========================================================================
// ���o�^
//==========================================================================
void CEnemyManager::SetEnemyInfo(CWaveManager::SetInfo *info, int nStage, int nWave, int nSpawnNum)
{
	for (int nCntSpawn = 0; nCntSpawn < nSpawnNum; nCntSpawn++)
	{
		// ���n��
		m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntSpawn] = info[nCntSpawn];
	}

	// �o�����鐔���Z
	m_aSetEnemyInfo[nStage].aWaveInfo[nWave].nSetNum = nSpawnNum;
}

//==========================================================================
// �|�����t���O��ݒ�
//==========================================================================
void CEnemyManager::SetKillFlag(void)
{
	// �܂Ƃ߂ē|���������Z
	m_nNumTotalKill++;

	// �|�������̃C���^�[�o�����Z�b�g
	m_nIntervalKill = KILL_INT;

	if (m_nNumTotalKill >= mylib_const::COMBONUM_3)
	{// 3�i�K�ڂ𒴂�����
		// �ڕW�̋����ݒ�
		CManager::GetCamera()->SetLenDest(CManager::GetCamera()->GetOriginDistance() + 700.0f, 90, 5.0f);

		if (m_bHitStop == false)
		{
			//CManager::SetEnableHitStop(8);

			// �U��
			CManager::GetCamera()->SetShake(20, 30.0f, 0.0f);

			// �X���b�V���Ռ��g�ݒ�
			D3DXVECTOR3 pos = CManager::GetScene()->GetPlayer()->GetPosition();
			pos.y += 200.0f;
			SetSlashImpact(pos);

			m_bHitStop = true;
		}
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyManager::Update(void)
{
	// �E�F�[�u���擾
	int nNowWave = CManager::GetScene()->GetWaveManager()->GetNowWave();
	int nMaxWave = CManager::GetScene()->GetWaveManager()->GetMaxWave();

	int nSpawnNum = 0;
	if (nNowWave < nMaxWave - 1)
	{
		nSpawnNum = CManager::GetScene()->GetWaveManager()->GetRespawnThreshold();
	}

	if (m_nNumAll <= nSpawnNum && CManager::GetScene()->GetWaveManager()->IsWaveFrag() == false)
	{// �G�����Ȃ��Ȃ�����

		// �E�F�[�u�I���t���O�𗧂Ă�
		CManager::GetScene()->GetWaveManager()->SetEnableWaveEnd();
	}

	// �C���^�[�o�����Z
	m_nIntervalKill--;

	if (m_nIntervalKill <= 0 && m_state != STATE_COMBOANIM)
	{// �Œ�l�ŌŒ�

		if (m_bHitStop == true)
		{// �q�b�g�X�g�b�v�܂ł����Ă�����
			m_state = STATE_COMBOANIM;

			// �R���{����
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_COMBOIN);
		}

		m_nNumTotalKill = 0;
		m_nIntervalKill = 0;
		m_bHitStop = false;
	}

	if (m_state == STATE_COMBOANIM)
	{// �R���{���̃A�j���[�V������Ԃ̎�
		UpdateComboAnimation();
	}

	// �e�L�X�g�̕`��
	CManager::GetDebugProc()->Print(
		"---------------- �G��� ----------------\n"
		"�y�c��l���z[%d]\n", m_nNumAll);
}

//==========================================================================
// �E�F�[�u���̔z�u����
//==========================================================================
void CEnemyManager::SetWaveEnemy(int nStage, int nWave)
{
	// �G���_�f�[�^�擾
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	for (int nCntEnemy = 0; nCntEnemy < m_aSetEnemyInfo[nStage].aWaveInfo[nWave].nSetNum; nCntEnemy++)
	{
		// ����̏��ꎞ�ۑ�
		CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntEnemy];

		// �G�o��
		SetEnemy(info.nType, pEnemyBase[info.nBase].pos + info.pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), pEnemyBase[info.nBase].rot, info.nMoveType);
	}
}

//==========================================================================
// �X�|�[���̋N���ݒ�
//==========================================================================
void CEnemyManager::SetSpawnWave(int nStage, int nWave)
{
	// �X�|�[�����̋N���ݒ�
	CElevation *pElevation = CGame::GetObject3DMesh();

	// �G���_�f�[�^�擾
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	int aParent[mylib_const::MAX_PATTEN_ENEMY][mylib_const::MAX_ENEMY] = {};
	int nToatalCount = 0;

	for (int nCntPattern = 0; nCntPattern < m_aSetEnemyInfo[nStage].aWaveInfo[nWave].nSetNum; nCntPattern++)
	{
		// ����̏��ꎞ�ۑ�
		CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
		int nPattern = info.nType;

		switch (info.nSpawnType)
		{
		case SPAWNTYPE_NORMAL:
			SetSpawnWaveNormal(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;

		case SPAWNTYPE_ORDER:
			SetSpawnWaveOrder(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;

		case SPAWNTYPE_CIRCLE:
			SetSpawnWaveCircle(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;

		case SPAWNTYPE_CLOCK:
			SetSpawnWaveClock(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;

		case SPAWNTYPE_BOTHCLOCK:
			SetSpawnWaveBothClock(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;

		default:
			SetSpawnWaveNormal(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;
		}

#if 0
		// ����̏��ꎞ�ۑ�
		CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
		CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
		int nPattern = info.nType;
		int nNumSpawn = m_aPattern[nPattern].nNumEnemy;	// �X�|�[�����鐔

		if (info.nSpawnType == SPAWNTYPE_CIRCLE ||
			info.nSpawnType ==  SPAWNTYPE_CLOCK)
		{// �����w�肳��Ă���Ƃ�
			nNumSpawn = info.nSpawnNum;
		}

		// 1�����̊p�x
		int nNumDivision = nNumSpawn;

		if (info.nSpawnType == SPAWNTYPE_BOTHCLOCK)
		{// �����̎�

			// �o�����̔���
			nNumDivision = nNumSpawn / 2;
		}

		// 1����������̊p�x
		float fAngleOneDgree = (D3DX_PI * 2.0f) / nNumDivision;

		int nCntNULL = 0;
		for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
		{// 1�̂��Z�b�g

			for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
			{
				if (m_nParent[nCntNULL] != -1)
				{// ��񂪓����Ă���
					continue;
				}

				// �e�̔ԍ��ۑ�
				m_nParent[m_nNumAll] = nCntNULL;

				// �e�ԍ��ۑ�
				aParent[nCntPattern][nCntEnemy] = nCntNULL;

				// �G�̏��
				int nWaitTime = (int)(45.0f / (float)nNumDivision);

				// �G�̏��
				EnemyData enemyData = m_aPattern[nPattern].EnemyData[nCntEnemy];

				if (info.nSpawnType == SPAWNTYPE_CIRCLE ||
					info.nSpawnType == SPAWNTYPE_CLOCK ||
					info.nSpawnType == SPAWNTYPE_BOTHCLOCK)
				{// �����w�肳��Ă���Ƃ�
					enemyData = m_aPattern[nPattern].EnemyData[0];
				}

				// �o���ʒu
				D3DXVECTOR3 spawnPos;

				if (info.nSpawnType == SPAWNTYPE_CIRCLE || info.nSpawnType == SPAWNTYPE_CLOCK || info.nSpawnType == SPAWNTYPE_BOTHCLOCK)
				{// �~ or ���v��肾������

					// �v���C���[�̈ʒu�擾
					D3DXVECTOR3 PlayerPos = CManager::GetScene()->GetPlayer()->GetPosition();

					// �v���C���[�̎��͂ɐݒ�
					spawnPos = D3DXVECTOR3(
						PlayerPos.x + sinf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x,
						0.0f,
						PlayerPos.z + cosf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x);

					switch (info.nSpawnType)
					{
					case SPAWNTYPE_CIRCLE:
						nWaitTime = 0;
						break;

					case SPAWNTYPE_CLOCK:
						nWaitTime = nWaitTime * nCntEnemy;
						break;

					case SPAWNTYPE_BOTHCLOCK:
						nWaitTime = nWaitTime * nCntEnemy;
						break;

					default:
						break;
					}

				}
				else
				{
					// �ҋ@����
					if (info.nSpawnType == SPAWNTYPE_ORDER)
					{
						nWaitTime = nWaitTime * nCntEnemy;
					}
					else
					{
						nWaitTime = enemyData.nWaitTime;
					}

					// �v�Z�p�}�g���b�N�X
					D3DXMATRIX mtxRot, mtxTrans, mtxWorld;
					D3DXMATRIX mtxRotOrigin;

					// �}�g���b�N�X�̏�����
					D3DXMatrixIdentity(&mtxWorld);
					D3DXMatrixIdentity(&mtxRotOrigin);

					// ���̌����𔽉f����
					D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, EnemyBase.rot.y, EnemyBase.rot.x, EnemyBase.rot.z);

					// �����𔽉f����
					D3DXMatrixRotationYawPitchRoll(&mtxRot, 0.0f, 0.0f, 0.0f);
					D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

					// �p�^�[�����̈ʒu�𔽉f����
					D3DXMatrixTranslation(&mtxTrans, enemyData.pos.x, enemyData.pos.y, enemyData.pos.z);
					D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);
					D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);

					// �X�|�[�����̌������|�����킹��
					spawnPos = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);

					// �ʒu�ݒ�
					spawnPos = D3DXVECTOR3(spawnPos.x + EnemyBase.pos.x + info.pos.x, 0.0f, spawnPos.z + EnemyBase.pos.z + info.pos.z);
				}

				// �^�C�v�ʂ͈̔�
				D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
				float fStrength = SPAWN_STRENGTH;

				switch ((CEnemy::TYPE)enemyData.nType)
				{
				case CEnemy::TYPE_POWER:
					break;

				case CEnemy::TYPE_CROWD:
					range = D3DXVECTOR2(80.0f, 80.0f);
					fStrength = SPAWN_STRENGTH * 0.5f;
					break;

				case CEnemy::TYPE_JUMP:
					range = D3DXVECTOR2(125.0f, 125.0f);
					fStrength = SPAWN_STRENGTH * 0.75f;
					break;

				case CEnemy::TYPE_BOMB:
					break;

				case CEnemy::TYPE_SPEED:
					break;

				case CEnemy::TYPE_IMPACT:
					break;

				case CEnemy::TYPE_IMPACTCENTER:
					break;
				}

				// �e��ID�v�Z
				int nParent = -1;
				if (enemyData.nParent >= 0)
				{
					nParent = aParent[nCntPattern][enemyData.nParent]/* + nToatalCount*/;
					//nParent = m_nParent[nParent];
				}
				else
				{
					nParent = -1;
				}

				// �g����
				pElevation->SetSpawnWave(
					nCntNULL,				// �����̃C���f�b�N�X�ԍ�
					nParent,				// �e�̃C���f�b�N�X�ԍ�
					nCntEnemy,				// ���Ԗڂ̓G��
					nWaitTime,				// �ҋ@����
					info,					// �E�F�[�u�}�l�[�W���̏��
					spawnPos,				// �����n�_
					range,					// �͈�
					fStrength,				// ����
					60						// ��������
				);

				// �������Z
				m_nNumAll++;

				// �e�ԍ��ۑ�
				aParent[nCntPattern][nCntEnemy] = nCntNULL;
				break;
			}
		}
#endif
		nToatalCount += m_aPattern[nPattern].nNumEnemy;
	}
}

//==========================================================================
// �ʏ�N��
//==========================================================================
void CEnemyManager::SetSpawnWaveNormal(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// �X�|�[�����̋N���ݒ�
	CElevation *pElevation = CGame::GetObject3DMesh();

	// �G���_�f�[�^�擾
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// ����̏��ꎞ�ۑ�
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
	CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
	int nPattern = info.nType;
	int nNumSpawn = m_aPattern[nPattern].nNumEnemy;	// �X�|�[�����鐔

	int nCntNULL = 0;
	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{// 1�̂��Z�b�g

		for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_nParent[nCntNULL] != -1)
			{// ��񂪓����Ă���
				continue;
			}

			// �e�̔ԍ��ۑ�
			m_nParent[nCntNULL] = nCntNULL;

			// �e�ԍ��ۑ�
			pParent[nCntEnemy] = nCntNULL;

			// �G�̏��
			EnemyData enemyData = m_aPattern[nPattern].EnemyData[nCntEnemy];

			// �ҋ@����
			int nWaitTime = enemyData.nWaitTime;

			// �v�Z�p�}�g���b�N�X
			D3DXMATRIX mtxRot, mtxTrans, mtxWorld;
			D3DXMATRIX mtxRotOrigin;

			// �}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&mtxWorld);
			D3DXMatrixIdentity(&mtxRotOrigin);

			
			// �����𔽉f����
			D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, EnemyBase.rot.y + info.rot.y, EnemyBase.rot.x, EnemyBase.rot.z);

			// �����𔽉f����
			D3DXMatrixRotationYawPitchRoll(&mtxRot, info.rot.y, info.rot.x, info.rot.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

			// �p�^�[�����̈ʒu�𔽉f����
			D3DXMatrixTranslation(&mtxTrans, enemyData.pos.x, enemyData.pos.y, enemyData.pos.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);

			// �X�|�[�����̌������|�����킹��
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
			D3DXVECTOR3 spawnRot = D3DXVECTOR3(EnemyBase.rot.x, EnemyBase.rot.y + info.rot.y, EnemyBase.rot.z);

			// �ʒu�ݒ�
			spawnPos = D3DXVECTOR3(spawnPos.x + EnemyBase.pos.x + info.pos.x, 0.0f, spawnPos.z + EnemyBase.pos.z + info.pos.z);

			// �^�C�v�ʂ͈̔�
			D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
			float fStrength = SPAWN_STRENGTH;

			switch ((CEnemy::TYPE)enemyData.nType)
			{
			case CEnemy::TYPE_POWER:
				break;

			case CEnemy::TYPE_CROWD:
				range = D3DXVECTOR2(80.0f, 80.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_JUMP:
				range = D3DXVECTOR2(125.0f, 125.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_BOMB:
				break;

			case CEnemy::TYPE_SPEED:
				break;

			case CEnemy::TYPE_IMPACT:
				break;

			case CEnemy::TYPE_IMPACTCENTER:
				break;
			}

			// �e��ID�v�Z
			int nParent = -1;
			if (enemyData.nParent >= 0)
			{
				nParent = pParent[enemyData.nParent]/* + nToatalCount*/;
				//nParent = m_nParent[nParent];
			}
			else
			{
				nParent = -1;
			}

			// �g����
			pElevation->SetSpawnWave(
				nCntNULL,				// �����̃C���f�b�N�X�ԍ�
				nParent,				// �e�̃C���f�b�N�X�ԍ�
				nCntEnemy,				// ���Ԗڂ̓G��
				nWaitTime,				// �ҋ@����
				info,					// �E�F�[�u�}�l�[�W���̏��
				spawnPos,				// �����n�_
				spawnRot,				// ����
				range,					// �͈�
				fStrength,				// ����
				60						// ��������
			);

			// �������Z
			m_nNumAll++;

			// �e�ԍ��ۑ�
			pParent[nCntEnemy] = nCntNULL;
			break;
		}
	}
}

//==========================================================================
// ���ԗN��
//==========================================================================
void CEnemyManager::SetSpawnWaveOrder(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// �X�|�[�����̋N���ݒ�
	CElevation *pElevation = CGame::GetObject3DMesh();

	// �G���_�f�[�^�擾
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// ����̏��ꎞ�ۑ�
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
	CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
	int nPattern = info.nType;
	int nNumSpawn = m_aPattern[nPattern].nNumEnemy;	// �X�|�[�����鐔

	int nCntNULL = 0;
	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{// 1�̂��Z�b�g

		for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_nParent[nCntNULL] != -1)
			{// ��񂪓����Ă���
				continue;
			}

			// �e�̔ԍ��ۑ�
			m_nParent[nCntNULL] = nCntNULL;

			// �e�ԍ��ۑ�
			pParent[nCntEnemy] = nCntNULL;

			// �G�̏��
			EnemyData enemyData = m_aPattern[nPattern].EnemyData[nCntEnemy];

			// �ҋ@����
			int nWaitTime = (int)(45.0f / (float)nNumSpawn) * nCntEnemy;

			// �v�Z�p�}�g���b�N�X
			D3DXMATRIX mtxRot, mtxTrans, mtxWorld;
			D3DXMATRIX mtxRotOrigin;

			// �}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&mtxWorld);
			D3DXMatrixIdentity(&mtxRotOrigin);

			// ���̌����𔽉f����
			D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, EnemyBase.rot.y + info.rot.y, EnemyBase.rot.x, EnemyBase.rot.z);

			// �����𔽉f����
			D3DXMatrixRotationYawPitchRoll(&mtxRot, info.rot.y, info.rot.x, info.rot.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

			// �p�^�[�����̈ʒu�𔽉f����
			D3DXMatrixTranslation(&mtxTrans, enemyData.pos.x, enemyData.pos.y, enemyData.pos.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);

			// �X�|�[�����̌������|�����킹��
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
			D3DXVECTOR3 spawnRot = D3DXVECTOR3(EnemyBase.rot.x, EnemyBase.rot.y + info.rot.y, EnemyBase.rot.z);

			// �ʒu�ݒ�
			spawnPos = D3DXVECTOR3(spawnPos.x + EnemyBase.pos.x + info.pos.x, 0.0f, spawnPos.z + EnemyBase.pos.z + info.pos.z);

			// �^�C�v�ʂ͈̔�
			D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
			float fStrength = SPAWN_STRENGTH;

			switch ((CEnemy::TYPE)enemyData.nType)
			{
			case CEnemy::TYPE_POWER:
				break;

			case CEnemy::TYPE_CROWD:
				range = D3DXVECTOR2(80.0f, 80.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_JUMP:
				range = D3DXVECTOR2(125.0f, 125.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_BOMB:
				break;

			case CEnemy::TYPE_SPEED:
				break;

			case CEnemy::TYPE_IMPACT:
				break;

			case CEnemy::TYPE_IMPACTCENTER:
				break;
			}

			// �e��ID�v�Z
			int nParent = -1;
			if (enemyData.nParent >= 0)
			{
				nParent = pParent[enemyData.nParent]/* + nToatalCount*/;
				//nParent = m_nParent[nParent];
			}
			else
			{
				nParent = -1;
			}

			// �g����
			pElevation->SetSpawnWave(
				nCntNULL,				// �����̃C���f�b�N�X�ԍ�
				nParent,				// �e�̃C���f�b�N�X�ԍ�
				nCntEnemy,				// ���Ԗڂ̓G��
				nWaitTime,				// �ҋ@����
				info,					// �E�F�[�u�}�l�[�W���̏��
				spawnPos,				// �����n�_
				spawnRot,				// ����
				range,					// �͈�
				fStrength,				// ����
				60						// ��������
			);

			// �������Z
			m_nNumAll++;

			// �e�ԍ��ۑ�
			pParent[nCntEnemy] = nCntNULL;
			break;
		}
	}
}

//==========================================================================
// �~��C�N��
//==========================================================================
void CEnemyManager::SetSpawnWaveCircle(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// �X�|�[�����̋N���ݒ�
	CElevation *pElevation = CGame::GetObject3DMesh();

	// �G���_�f�[�^�擾
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// ����̏��ꎞ�ۑ�
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
	CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
	int nPattern = info.nType;
	int nNumSpawn = info.nSpawnNum;	// �X�|�[�����鐔

	// 1����������̊p�x
	float fAngleOneDgree = (D3DX_PI * 2.0f) / nNumSpawn;

	int nCntNULL = 0;
	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{// 1�̂��Z�b�g

		for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_nParent[nCntNULL] != -1)
			{// ��񂪓����Ă���
				continue;
			}

			// �e�̔ԍ��ۑ�
			m_nParent[nCntNULL] = nCntNULL;

			// �e�ԍ��ۑ�
			pParent[nCntEnemy] = nCntNULL;

			// �ҋ@����
			int nWaitTime = 0;

			// �G�̏��
			EnemyData enemyData = m_aPattern[nPattern].EnemyData[0];

			// �v���C���[�̈ʒu�擾
			D3DXVECTOR3 PlayerPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			if (CManager::GetScene()->GetPlayer() != NULL)
			{
				PlayerPos = CManager::GetScene()->GetPlayer()->GetPosition();
			}

			// �v���C���[�̎��͂ɐݒ�
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(
				PlayerPos.x + sinf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x,
				0.0f,
				PlayerPos.z + cosf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x);

			// �^�C�v�ʂ͈̔�
			D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
			float fStrength = SPAWN_STRENGTH;

			switch ((CEnemy::TYPE)enemyData.nType)
			{
			case CEnemy::TYPE_POWER:
				break;

			case CEnemy::TYPE_CROWD:
				range = D3DXVECTOR2(80.0f, 80.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_JUMP:
				range = D3DXVECTOR2(125.0f, 125.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_BOMB:
				break;

			case CEnemy::TYPE_SPEED:
				break;

			case CEnemy::TYPE_IMPACT:
				break;

			case CEnemy::TYPE_IMPACTCENTER:
				break;
			}

			// �e��ID�v�Z
			int nParent = -1;
			if (enemyData.nParent >= 0)
			{
				nParent = pParent[enemyData.nParent]/* + nToatalCount*/;
				//nParent = m_nParent[nParent];
			}
			else
			{
				nParent = -1;
			}

			D3DXVECTOR3 SetRot = D3DXVECTOR3(0.0f, D3DX_PI + fAngleOneDgree * nCntEnemy, 0.0f);

			// �g����
			pElevation->SetSpawnWave(
				nCntNULL,				// �����̃C���f�b�N�X�ԍ�
				nParent,				// �e�̃C���f�b�N�X�ԍ�
				nCntEnemy,				// ���Ԗڂ̓G��
				nWaitTime,				// �ҋ@����
				info,					// �E�F�[�u�}�l�[�W���̏��
				spawnPos,				// �����n�_
				SetRot,					// ����
				range,					// �͈�
				fStrength,				// ����
				60						// ��������
			);

			// �������Z
			m_nNumAll++;

			// �e�ԍ��ۑ�
			pParent[nCntEnemy] = nCntNULL;
			break;
		}
	}
}

//==========================================================================
// ���v���N��
//==========================================================================
void CEnemyManager::SetSpawnWaveClock(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// �X�|�[�����̋N���ݒ�
	CElevation *pElevation = CGame::GetObject3DMesh();

	// �G���_�f�[�^�擾
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// ����̏��ꎞ�ۑ�
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
	CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
	int nPattern = info.nType;
	int nNumSpawn = info.nSpawnNum;	// �X�|�[�����鐔

	// 1����������̊p�x
	float fAngleOneDgree = (D3DX_PI * 2.0f) / nNumSpawn;

	int nCntNULL = 0;
	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{// 1�̂��Z�b�g

		for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_nParent[nCntNULL] != -1)
			{// ��񂪓����Ă���
				continue;
			}

			// �e�̔ԍ��ۑ�
			m_nParent[nCntNULL] = nCntNULL;

			// �e�ԍ��ۑ�
			pParent[nCntEnemy] = nCntNULL;

			// �ҋ@����
			int nWaitTime = (int)(45.0f / (float)nNumSpawn) * nCntEnemy;

			// �G�̏��
			EnemyData enemyData = m_aPattern[nPattern].EnemyData[0];

			// �v���C���[�̈ʒu�擾
			D3DXVECTOR3 PlayerPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			if (CManager::GetScene()->GetPlayer() != NULL)
			{
				PlayerPos = CManager::GetScene()->GetPlayer()->GetPosition();
			}

			// �v���C���[�̎��͂ɐݒ�
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(
				PlayerPos.x + sinf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x,
				0.0f,
				PlayerPos.z + cosf(D3DX_PI + fAngleOneDgree * nCntEnemy) * info.pos.x);

			// �^�C�v�ʂ͈̔�
			D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
			float fStrength = SPAWN_STRENGTH;

			switch ((CEnemy::TYPE)enemyData.nType)
			{
			case CEnemy::TYPE_POWER:
				break;

			case CEnemy::TYPE_CROWD:
				range = D3DXVECTOR2(80.0f, 80.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_JUMP:
				range = D3DXVECTOR2(125.0f, 125.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_BOMB:
				break;

			case CEnemy::TYPE_SPEED:
				break;

			case CEnemy::TYPE_IMPACT:
				break;

			case CEnemy::TYPE_IMPACTCENTER:
				break;
			}

			// �e��ID�v�Z
			int nParent = -1;
			if (enemyData.nParent >= 0)
			{
				nParent = pParent[enemyData.nParent];
			}
			else
			{
				nParent = -1;
			}

			D3DXVECTOR3 SetRot = D3DXVECTOR3(0.0f, D3DX_PI + fAngleOneDgree * nCntEnemy, 0.0f);
			// �g����
			pElevation->SetSpawnWave(
				nCntNULL,				// �����̃C���f�b�N�X�ԍ�
				nParent,				// �e�̃C���f�b�N�X�ԍ�
				nCntEnemy,				// ���Ԗڂ̓G��
				nWaitTime,				// �ҋ@����
				info,					// �E�F�[�u�}�l�[�W���̏��
				spawnPos,				// �����n�_
				SetRot,					// ����
				range,					// �͈�
				fStrength,				// ����
				60						// ��������
			);

			// �������Z
			m_nNumAll++;

			// �e�ԍ��ۑ�
			pParent[nCntEnemy] = nCntNULL;
			break;
		}
	}
}

//==========================================================================
// �����N��
//==========================================================================
void CEnemyManager::SetSpawnWaveBothClock(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// �X�|�[�����̋N���ݒ�
	CElevation *pElevation = CGame::GetObject3DMesh();

	// �G���_�f�[�^�擾
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// ����̏��ꎞ�ۑ�
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
	CEnemyBase::BaseData EnemyBase = pEnemyBase[info.nBase];
	int nPattern = info.nType;
	int nNumSpawn = info.nSpawnNum;	// �X�|�[�����鐔

	// ������
	int nNumDivision = nNumSpawn / 2;

	// 1����������̊p�x
	float fAngleOneDgree = D3DX_PI / (float)nNumDivision;

	int nCntNULL = 0, nCntDivision = 0;
	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{// 1�̂��Z�b�g

		for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_nParent[nCntNULL] != -1)
			{// ��񂪓����Ă���
				continue;
			}

			// �e�̔ԍ��ۑ�
			m_nParent[nCntNULL] = nCntNULL;

			// �e�ԍ��ۑ�
			pParent[nCntEnemy] = nCntNULL;

			// �ҋ@����
			int nWaitTime = (int)(50.0f / (float)nNumDivision) * nCntDivision;

			// �G�̏��
			EnemyData enemyData = m_aPattern[nPattern].EnemyData[0];

			// �v���C���[�̈ʒu�擾
			D3DXVECTOR3 PlayerPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			if (CManager::GetScene()->GetPlayer() != NULL)
			{
				PlayerPos = CManager::GetScene()->GetPlayer()->GetPosition();
			}

			// ����
			int nAngle = 1;
			if (nCntEnemy % 2 == 0)
			{
				nAngle = -1;
			}

			// �v���C���[�̎��͂ɐݒ�
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(
				PlayerPos.x + sinf(D3DX_PI + (fAngleOneDgree * nCntDivision) * nAngle) * info.pos.x,
				0.0f,
				PlayerPos.z + cosf(D3DX_PI + (fAngleOneDgree * nCntDivision) * nAngle) * info.pos.x);

			// �^�C�v�ʂ͈̔�
			D3DXVECTOR2 range = D3DXVECTOR2(200.0f, 200.0f);
			float fStrength = SPAWN_STRENGTH;

			switch ((CEnemy::TYPE)enemyData.nType)
			{
			case CEnemy::TYPE_POWER:
				break;

			case CEnemy::TYPE_CROWD:
				range = D3DXVECTOR2(80.0f, 80.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_JUMP:
				range = D3DXVECTOR2(125.0f, 125.0f);
				fStrength = SPAWN_STRENGTH * 0.75f;
				break;

			case CEnemy::TYPE_BOMB:
				break;

			case CEnemy::TYPE_SPEED:
				break;

			case CEnemy::TYPE_IMPACT:
				break;

			case CEnemy::TYPE_IMPACTCENTER:
				break;
			}

			// �e��ID�v�Z
			int nParent = -1;
			if (enemyData.nParent >= 0)
			{
				nParent = pParent[enemyData.nParent];
			}
			else
			{
				nParent = -1;
			}

			D3DXVECTOR3 SetRot = D3DXVECTOR3(0.0f, D3DX_PI + (fAngleOneDgree * nCntDivision) * nAngle, 0.0f);
			// �g����
			pElevation->SetSpawnWave(
				nCntNULL,				// �����̃C���f�b�N�X�ԍ�
				nParent,				// �e�̃C���f�b�N�X�ԍ�
				nCntEnemy,				// ���Ԗڂ̓G��
				nWaitTime,				// �ҋ@����
				info,					// �E�F�[�u�}�l�[�W���̏��
				spawnPos,				// �����n�_
				SetRot,					// ����
				range,					// �͈�
				fStrength,				// ����
				60						// ��������
			);

			// �������Z
			m_nNumAll++;

			// �e�ԍ��ۑ�
			pParent[nCntEnemy] = nCntNULL;

			if (nCntEnemy % 2 == 0)
			{
				nCntDivision++;
			}
			break;
		}
	}
}

//==========================================================================
// �G�z�u
//==========================================================================
void CEnemyManager::SetEnemy(int nPattern, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 rotBase, int movetype)
{
	if (mylib_const::MAX_PATTEN_ENEMY <= nPattern)
	{
		return;
	}

	int nCntNULL = 0;
	int aParent[mylib_const::MAX_PATTEN_ENEMY] = {};

	for (int nCntEnemy = 0; nCntEnemy < m_aPattern[nPattern].nNumEnemy; nCntEnemy++)
	{
		for (int nCntNULL = 0; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_pEnemy[nCntNULL] == NULL)
			{// NULL��������

				// �v�Z�p�}�g���b�N�X
				D3DXMATRIX mtxRot, mtxTrans, mtxWorld;
				D3DXMATRIX mtxRotOrigin;

				// �}�g���b�N�X�̏�����
				D3DXMatrixIdentity(&mtxWorld);
				D3DXMatrixIdentity(&mtxRotOrigin);

				// ���̌����𔽉f����
				D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, rotBase.y, rotBase.x, rotBase.z);

				// �����𔽉f����
				D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
				D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

				// �X�|�[���ʒu�𔽉f����
				/*D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
				D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);*/

				// �p�^�[�����̈ʒu�𔽉f����
				D3DXMatrixTranslation(&mtxTrans, m_aPattern[nPattern].EnemyData[nCntEnemy].pos.x, m_aPattern[nPattern].EnemyData[nCntEnemy].pos.y, m_aPattern[nPattern].EnemyData[nCntEnemy].pos.z);
				D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);
				D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);

				// �X�|�[�����̌������|�����킹��
				//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);
				D3DXVECTOR3 spawnPos = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);

				// �G�̐���
				m_pEnemy[nCntNULL] = CEnemy::Create(
					nCntNULL,																	// �C���f�b�N�X�ԍ�
					sMotionFileName[m_aPattern[nPattern].EnemyData[nCntEnemy].nType].c_str(),	// �t�@�C����
					D3DXVECTOR3(spawnPos.x + pos.x, -200.0f, spawnPos.z + pos.z),				// �ʒu
					(CEnemy::TYPE)m_aPattern[nPattern].EnemyData[nCntEnemy].nType);				// ���

				if (m_pEnemy[nCntNULL] == NULL)
				{// ���s���Ă�����

					delete m_pEnemy[nCntNULL];
					m_pEnemy[nCntNULL] = NULL;
					break;
				}

				// �����ݒ�
				m_pEnemy[nCntNULL]->SetRotation(rotBase);
				m_pEnemy[nCntNULL]->SetRotDest(rotBase.y);

				// �s���̎�ސݒ�
				m_pEnemy[nCntNULL]->SetMoveType((CEnemy::MOVETYPE)movetype);

				// �e���f���̐ݒ�
				if (m_aPattern[nPattern].EnemyData[nCntEnemy].nParent >= 0)
				{
					m_pEnemy[nCntNULL]->SetParent(m_pEnemy[aParent[m_aPattern[nPattern].EnemyData[nCntEnemy].nParent]]);
				}
				else
				{// �e�������̏ꍇ

					// NULL�ݒ�
					m_pEnemy[nCntNULL]->SetParent(NULL);
				}

				// �e�ԍ��ۑ�
				aParent[nCntEnemy] = nCntNULL;

				break;
			}
		}
	}
}

//==========================================================================
// �G�z�u
//==========================================================================
void CEnemyManager::SetEnemy(int nIdx, int nPattern, int nCntEnemy, int nParent, CWaveManager::SetInfo setinfo, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int movetype)
{
	if (m_pEnemy[nIdx] != NULL)
	{// ���ɏ�񂪓����Ă���
		return;
	}

	int nType = m_aPattern[nPattern].EnemyData[nCntEnemy].nType;

	if (setinfo.nChara != -1)
	{// ��������ݒ肳��Ă���
		nType = setinfo.nChara;
	}

	// �G�̐���
	m_pEnemy[nIdx] = CEnemy::Create(
		nIdx,							// �C���f�b�N�X�ԍ�
		sMotionFileName[nType].c_str(),	// �t�@�C����
		pos,							// �ʒu
		(CEnemy::TYPE)m_aPattern[nPattern].EnemyData[nCntEnemy].nType,	// ���
		nType							// �L�����̎��
	);


	if (m_pEnemy[nIdx] == NULL)
	{// ���s���Ă�����

		delete m_pEnemy[nIdx];
		m_pEnemy[nIdx] = NULL;
		return;
	}

	// �����ݒ�
	m_pEnemy[nIdx]->SetRotation(rot);
	m_pEnemy[nIdx]->SetRotDest(rot.y);

	if ((CEnemy::MOVETYPE)movetype == CEnemy::MOVETYPE_PLAYERRUSH)
	{// �v���C���[�ˌ���������

		// �v���C���[���
		CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

		if (pPlayer != NULL)
		{// NULL����Ȃ��Ƃ�

			// �e�̈ʒu�擾
			D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

			// �ڕW�̊p�x�����߂�
			float fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

			m_pEnemy[nIdx]->SetOriginRotation(D3DXVECTOR3(rot.x, fRotDest, rot.z));
			m_pEnemy[nIdx]->SetRotation(D3DXVECTOR3(rot.x, fRotDest, rot.z));
			m_pEnemy[nIdx]->SetRotDest(fRotDest);
		}
	}
	else
	{// ����ȊO
		m_pEnemy[nIdx]->SetOriginRotation(rot);
	}

	// �s���̎�ސݒ�
	m_pEnemy[nIdx]->SetMoveType((CEnemy::MOVETYPE)movetype);

	// �s���̎�ސݒ�
	m_pEnemy[nIdx]->SetSpawnType((SPAWNTYPE)setinfo.nSpawnType);

	// ����ݒ�
	if (setinfo.nSpawnType == 0)
	{
		int n = 0;
	}

	// �e���f���̐ݒ�
	if (nParent >= 0)
	{
		m_pEnemy[nIdx]->SetParent(m_pEnemy[nParent]);
	}
	else
	{// �e�������̏ꍇ

		// NULL�ݒ�
		m_pEnemy[nIdx]->SetParent(NULL);
	}

	// �G�̏��
	EnemyData enemyData = m_aPattern[nPattern].EnemyData[nCntEnemy];
	D3DXVECTOR3 posParent = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (enemyData.nParent != -1)
	{// �e������ꍇ
		posParent = m_aPattern[nPattern].EnemyData[enemyData.nParent].pos;
	}

	// ����̈ʒu�ݒ�
	m_pEnemy[nIdx]->SetFormationPos(m_aPattern[nPattern].EnemyData[nCntEnemy].pos, posParent);
}

//==========================================================================
// �{�X�z�u
//==========================================================================
void CEnemyManager::SetBoss(int nPattern, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	if (mylib_const::MAX_PATTEN_ENEMY <= nPattern)
	{
		return;
	}

	int nCntNULL = 0;

	for (int nCntEnemy = 0; nCntEnemy < m_aPattern[nPattern].nNumEnemy; nCntEnemy++)
	{
		for (int nCntNULL = 0; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_pBoss[nCntNULL] == NULL)
			{// NULL��������

				// �G�̐���
				m_pBoss[nCntNULL] = CBoss::Create(
					nCntNULL,																	// �C���f�b�N�X�ԍ�
					sMotionFileName[m_aPattern[nPattern].EnemyData[nCntEnemy].nType].c_str(),	// �t�@�C����
					m_aPattern[nPattern].EnemyData[nCntEnemy].pos + pos							// �ʒu
					);

				if (m_pBoss[nCntNULL] == NULL)
				{// ���s���Ă�����

					delete m_pBoss[nCntNULL];
					m_pBoss[nCntNULL] = NULL;
					break;
				}

				// �����ݒ�
				m_pBoss[nCntNULL]->SetRotation(rot);

				// �������Z
				m_nNumAll++;
				break;
			}
		}
	}
}


//==========================================================================
// �R���{���̉��o
//==========================================================================
void CEnemyManager::UpdateComboAnimation(void)
{

	// �ڕW�̋����ݒ�
	CManager::GetCamera()->SetLenDest(CManager::GetCamera()->GetOriginDistance() + 1000.0f, 90, 5.0f);

	static int nnn = 0;
	static int nIntKill = 0;

	nnn = (nnn + 1) % 2;
	nIntKill = (nIntKill + 1) % 4;

	if (nnn != 0)
	{
		return;
	}


	// �U��
	CManager::GetCamera()->SetShake(3, 10.0f, 0.0f);

	int nCntKill = 0;	// �|���ꂽ�G�̐�

	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_OBJ; nCntEnemy++)
	{
		if (m_pEnemy[nCntEnemy] == NULL)
		{// NULL��������
			continue;
		}

		if (m_pEnemy[nCntEnemy]->IsDeath() == true)
		{// ���S�t���O�������Ă�����
			continue;
		}

		if (m_pEnemy[nCntEnemy]->GetState() != CEnemy::STATE_DEAD)
		{// ���S��ԈȊO�̓G
			continue;
		}

		// �|���ꂽ�G�̐����Z
		nCntKill++;

		// �X���b�V���Ռ��g�ݒ�
		SetSlashImpact(m_pEnemy[nCntEnemy]->GetPosition());

		// �p�[�e�B�N������
		my_particle::Create(m_pEnemy[nCntEnemy]->GetPosition(), my_particle::TYPE_ENEMYKILL_COMBO);

		// �I������
		m_pEnemy[nCntEnemy]->Kill();
		m_pEnemy[nCntEnemy]->Uninit();

		// �T�E���h�Đ�
		if (nIntKill == 0)
		{
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_COMBOKILL);
		}
		break;
	}

	if (nCntKill == 0)
	{// �I����Ă�����
		m_state = STATE_NONE;
	}

}

//==========================================================================
// �X���b�V���Ռ��g�ݒ�
//==========================================================================
void CEnemyManager::SetSlashImpact(D3DXVECTOR3 pos)
{
	D3DXCOLOR col = D3DXCOLOR(1.0f, 0.6f, 0.3f, 0.5f);
	CImpactWave::TYPE type = CImpactWave::TYPE_SMOKE;
	float fMove = 20.0f;
	float fWidth = 100.0f;

#if 1
	// �Ռ��g����
	CImpactWave::Create
	(
		D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
		D3DXVECTOR3(D3DX_PI * 0.75f, 0.0f, 0.0f),	// ����
		col,			// �F
		fWidth,										// ��
		1.0f,										// ����
		30,											// ����
		fMove,										// ���̈ړ���
		type,					// �e�N�X�`���^�C�v
		true										// ���Z�������邩
	);

	// �Ռ��g����
	CImpactWave::Create
	(
		D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// ����
		col,			// �F
		fWidth,										// ��
		1.0f,										// ����
		30,											// ����
		fMove,										// ���̈ړ���
		type,					// �e�N�X�`���^�C�v
		true										// ���Z�������邩
	);

	// �Ռ��g����
	CImpactWave::Create
	(
		D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
		D3DXVECTOR3(D3DX_PI * -0.75f, 0.0f, 0.0f),	// ����
		col,			// �F
		fWidth,										// ��
		1.0f,										// ����
		30,											// ����
		fMove,										// ���̈ړ���
		type,					// �e�N�X�`���^�C�v
		true										// ���Z�������邩
	);
#endif
}

//==========================================================================
// �p�^�[����
//==========================================================================
int CEnemyManager::GetPatternNum(void)
{
	return m_nPatternNum;
}

//==========================================================================
// �|�����Ԋu�擾
//==========================================================================
int CEnemyManager::GetIntervalKill(void)
{
	return m_nIntervalKill;
}

//==========================================================================
// �Ԋu���ɓ|�������擾
//==========================================================================
int CEnemyManager::GetNumIntervalKill(void)
{
	return m_nNumTotalKill;
}

//==========================================================================
// �G�̑����擾
//==========================================================================
int CEnemyManager::GetNumAll(void)
{
	return m_nNumAll;
}

//==========================================================================
// �e�L�X�g�ǂݍ��ݏ���
//==========================================================================
HRESULT CEnemyManager::ReadText(const std::string pTextFile)
{

	if (m_bLoadPattern == true)
	{// ���ɓǂݍ��܂�Ă�����
		return S_OK;
	}

	FILE *pFile = NULL;	// �t�@�C���|�C���^��錾

	// �t�@�C�����J��
	pFile = fopen(pTextFile.c_str(), "r");

	if (pFile == NULL)
	{//�t�@�C�����J�����ꍇ
		return E_FAIL;
	}

	// �ǂݍ��ݔ���ON
	m_bLoadPattern = true;

	char aComment[MAX_COMMENT];	// �R�����g
	int nType = 0;				// �z�u������
	int nCntPatten = 0;			// �p�^�[���̃J�E���g
	int nCntFileName = 0;

	while (1)
	{// END_SCRIPT������܂ŌJ��Ԃ�

		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		// �L�����N�^�[���̐ݒ�
		if (strcmp(aComment, "NUM_CHARACTER") == 0)
		{// NUM_CHARACTER��������

			fscanf(pFile, "%s", &aComment[0]);	// =�̕�
			fscanf(pFile, "%d", &m_nNumChara);	// �L�����N�^�[��
		}

		while (nCntFileName != m_nNumChara)
		{// ���f���̐����ǂݍ��ނ܂ŌJ��Ԃ�

			// ������̓ǂݍ���
			fscanf(pFile, "%s", &aComment[0]);

			// ���f�����̐ݒ�
			if (strcmp(aComment, "MOTION_FILENAME") == 0)
			{// NUM_MODEL��������

				fscanf(pFile, "%s", &aComment[0]);	// =�̕�
				fscanf(pFile, "%s", &aComment[0]);	// �t�@�C����

				// �t�@�C�����ۑ�
				sMotionFileName[nCntFileName] = aComment;

				nCntFileName++;	// �t�@�C�������Z
			}
		}

		// �e�p�^�[���̐ݒ�
		if (strcmp(aComment, "PATTERNSET") == 0)
		{// �z�u���̓ǂݍ��݂��J�n

			int nCntEnemy = 0;			// �G�̔z�u�J�E���g

			while (strcmp(aComment, "END_PATTERNSET") != 0)
			{// END_PATTERNSET������܂ŌJ��Ԃ�

				fscanf(pFile, "%s", &aComment[0]);	//�m�F����

				if (strcmp(aComment, "ENEMYSET") == 0)
				{// ENEMYSET�œG���̓ǂݍ��݊J�n

					while (strcmp(aComment, "END_ENEMYSET") != 0)
					{// END_ENEMYSET������܂ŌJ��Ԃ�

						fscanf(pFile, "%s", &aComment[0]);	// �m�F����

						if (strcmp(aComment, "TYPE") == 0)
						{// TYPE��������L�����t�@�C���ԍ��ǂݍ���

							fscanf(pFile, "%s", &aComment[0]);	// =�̕�
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nType);	// �L�����t�@�C���ԍ�
						}

						if (strcmp(aComment, "PARENT") == 0)
						{// PARENT��������e�̔ԍ��ǂݍ���

							fscanf(pFile, "%s", &aComment[0]);	// =�̕�
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nParent);	// �e�̔ԍ�
						}

						if (strcmp(aComment, "WAITTIME") == 0)
						{// WAITTIME��������ҋ@���ԓǂݍ���

							fscanf(pFile, "%s", &aComment[0]);	// =�̕�
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nWaitTime);	// �ҋ@����
						}

						if (strcmp(aComment, "POS") == 0)
						{// POS��������ʒu�ǂݍ���

							fscanf(pFile, "%s", &aComment[0]);		// =�̕�
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.x);	// X���W
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.y);	// Y���W
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.z);	// Z���W
						}

					}// END_ENEMYSET�̂�����

					nCntEnemy++;	// �G�̃J�E���g�����Z
					m_aPattern[nCntPatten].nNumEnemy++;	// �G�̃J�E���g�����Z
				}
			}// END_PATTERNSET�̂�����

			nCntPatten++;	// �p�^�[�����Z
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���
			break;
		}
	}

	// �p�^�[����
	m_nPatternNum = nCntPatten - 1;

	// �t�@�C�������
	fclose(pFile);

	return S_OK;
}
