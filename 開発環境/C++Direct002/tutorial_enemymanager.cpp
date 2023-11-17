//=============================================================================
// 
//  �G�̃}�l�[�W������ [tutorial_enemymanager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "debugproc.h"
#include "manager.h"
#include "scene.h"
#include "tutorial.h"
#include "tutorial_enemymanager.h"
#include "tutorialstep.h"
#include "enemy.h"

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTutorialEnemyManager::CTutorialEnemyManager()
{
	// �l�̃N���A
	
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTutorialEnemyManager::~CTutorialEnemyManager()
{

}

//==========================================================================
// �X�V����
//==========================================================================
void CTutorialEnemyManager::Update(void)
{
	if (CTutorial::GetStep()->GetNowStep() < CTutorialStep::STEP_IMPACTWAVE)
	{
		return;
	}

	if (m_nNumAll <= 0)
	{// �G�����Ȃ��Ȃ�����

		int nWave = 0;
		if (CTutorial::GetStep()->GetNowStep() == CTutorialStep::STEP_MAX)
		{
			nWave = 1;
		}
		// �G�ݒu���Ȃ���
		SetSpawnWave(0, nWave);
	}

	// �C���^�[�o�����Z
	m_nIntervalKill--;

	if (m_nIntervalKill <= 0 && m_state != STATE_COMBOANIM)
	{// �Œ�l�ŌŒ�

		if (m_bHitStop == true)
		{// �q�b�g�X�g�b�v�܂ł����Ă�����
			m_state = STATE_COMBOANIM;
		}

		m_nNumTotalKill = 0;
		m_nIntervalKill = 0;
		m_bHitStop = false;
	}

	if (m_state == STATE_COMBOANIM)
	{// �R���{���̃A�j���[�V������Ԃ̎�
		UpdateComboAnimation();
	}
}

//==========================================================================
// �X�|�[���̋N���ݒ�
//==========================================================================
void CTutorialEnemyManager::SetSpawnWave(int nStage, int nWave)
{
	// �X�|�[�����̋N���ݒ�
	CElevation *pElevation = CManager::GetScene()->GetObject3DMesh();


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

		default:
			SetSpawnWaveNormal(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;
		}

		nToatalCount += m_aPattern[nPattern].nNumEnemy;
	}
}

//==========================================================================
// �ʏ�N��
//==========================================================================
void CTutorialEnemyManager::SetSpawnWaveNormal(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// �X�|�[�����̋N���ݒ�
	CElevation *pElevation = CManager::GetScene()->GetObject3DMesh();

	// ����̏��ꎞ�ۑ�
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
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
			D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin,info.rot.y, 0.0f, 0.0f);

			// �����𔽉f����
			D3DXMatrixRotationYawPitchRoll(&mtxRot, info.rot.y, info.rot.x, info.rot.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

			// �p�^�[�����̈ʒu�𔽉f����
			D3DXMatrixTranslation(&mtxTrans, enemyData.pos.x, enemyData.pos.y, enemyData.pos.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);

			// �X�|�[�����̌������|�����킹��
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
			D3DXVECTOR3 spawnRot = D3DXVECTOR3(0.0f, info.rot.y, 0.0f);

			// �ʒu�ݒ�
			spawnPos = D3DXVECTOR3(spawnPos.x + info.pos.x, 0.0f, spawnPos.z + info.pos.z);

			// �^�C�v�ʂ͈̔�
			D3DXVECTOR2 range = D3DXVECTOR2(80.0f, 80.0f);
			float fStrength = 70.0f;

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
// �G�z�u
//==========================================================================
void CTutorialEnemyManager::SetEnemy(int nIdx, int nPattern, int nCntEnemy, int nParent, CWaveManager::SetInfo setinfo, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int movetype)
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
	m_pEnemy[nIdx]->SetOriginRotation(rot);

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
