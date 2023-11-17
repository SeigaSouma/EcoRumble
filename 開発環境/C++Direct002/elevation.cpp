//=============================================================================
// 
//  �N������ [elevation.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "elevation.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "debugproc.h"
#include "particle.h"
#include "3D_effect.h"
#include "player.h"
#include "model.h"
#include "ballast.h"
#include "power_gauge.h"
#include "boss.h"
#include "enemy.h"
#include "targetpoint.h"
#include "impactwave.h"
#include "camera.h"
#include "enemymanager.h"
#include "enemybase.h"
#include "sound.h"
#include "tutorial.h"
#include "tutorialstep.h"
#include "ranking.h"

// �h����
#include "elevation_outside.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\glass.jpg"	// �e�N�X�`��
#define MAX_MOVE	(8.0f)		// �ړ���
#define MAX_RANGE	(200.0f)	// �͈�
#define MOVE_LEN	(1.0f)		// ���ړ���
#define MIN_HEIGHT	(-400.0f)	// �Œ�̍���
#define MOVE		(5.0f)
#define IMPACTSHAKE_DIRECT	(30.0f)
#define IMPACTSHAKE_IMPACT	(20.0f)
#define SHAKETIME_DIRECT	(15 + 5)
#define SHAKETIME_IMPACT	(10 + 5)
#define SPAWN_WAITTIME		(20)
#if 1
#define CHANGE	(0)
#endif

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CWaveManager::SetStageInfo CElevation::m_aSetInfo[mylib_const::MAX_WAVE_ENEMY] = {};	// �G�̔z�u���
bool CElevation::m_bLoadInfo = false;		// ���ǂݍ��ݔ���
bool CElevation::m_bSetSpawnCamera = false;	// �X�|�[�����̃J�����ݒ蔻��
int CElevation::m_nCntSetSpawn = 0;			// �X�|�[�����̃J�����J�E���g

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CElevation::CElevation(int nPriority) : CObject3DMesh(nPriority)
{
	memset(&m_aWave, NULL, sizeof(m_aWave));	// �N���̏��
	m_fBrushStrength = 0.0f;					// �u���V����
	m_fBrushRange = 0.0f;						// �͈�
	m_bEdit = false;							// �G�f�B�b�gON/OFF
	m_nTexIdx = 0;								// �e�N�X�`���̃C���f�b�N�X�ԍ�
	memset(&m_VtxPos, 0, sizeof(m_VtxPos));		// ���_���W
	memset(&m_sFadeInfo, NULL, sizeof(m_sFadeInfo));		// �t�F�[�h���̏��

	m_aInfo.TextureFileName = {};				// �e�N�X�`���t�@�C����
	m_aInfo.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ʒu
	m_aInfo.fWidthLength = 0.0f;				// ������
	m_aInfo.fHeightLength = 0.0f;				// �c����
	m_aInfo.nWidthBlock = 0;					// ��������
	m_aInfo.nHeightBlock = 0;					// �c������
	m_aInfo.pVtxPos = NULL;						// ���_���W

	for (int nCntStage = 0; nCntStage < mylib_const::MAX_STAGE; nCntStage++)
	{
		m_aInfo.pStageVtxPos[nCntStage] = NULL;	// �X�e�[�W���̒��_���W
	}
	m_pTargetP = NULL;							// �ڕW�̒n�_
	m_nSpawnByFrame = 0;						// �t���[�����̃X�|�[���J�E���g
	m_bSpawnByFrame = false;					// �t���[�����̃X�|�[������
	m_bHitStop_Direct = false;					// �����g�̃q�b�g�X�g�b�v����
	m_bHitStop_Impact = false;					// �~�`�g�̃q�b�g�X�g�b�v����
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CElevation::~CElevation()
{

}

//==========================================================================
// ��������
//==========================================================================
CElevation *CElevation::Create(const char *pText, CObject::TYPE type)
{
	// �����p�̃I�u�W�F�N�g
	CElevation *pObjMeshField = NULL;

	if (pObjMeshField == NULL)
	{// NULL��������

		// �������̊m��
		switch (type)
		{
		case CObject::TYPE_ELEVATION:
			pObjMeshField = DEBUG_NEW CElevation;
			break;

		case CObject::TYPE_ELEVATION_OUTSIDE:
			pObjMeshField = DEBUG_NEW CElevationOutside;
			break;

		default:
			break;
		}

		if (pObjMeshField != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pObjMeshField->Init(pText);
		}

		return pObjMeshField;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CElevation::Init(void)
{
	//for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock; nCntHeight++)
	//{// �c�̕������J��Ԃ�

	//	for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock; nCntWidth++)
	//	{// ���̕������J��Ԃ�

	//		int nNowVtx = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));

	//		// 0�X�e�[�W�̏��ɂ���
	//		m_aInfo.pVtxPos[nNowVtx] = m_aInfo.pStageVtxPos[0][nNowVtx];
	//		pVtxPos[nNowVtx] = m_aInfo.pVtxPos[nNowVtx];
	//	}
	//}

	return E_FAIL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CElevation::Init(const char *pText)
{
	HRESULT hr;

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���̊��蓖��
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx);

	// ��ސݒ�
	SetType(TYPE_ELEVATION);

	// ���[�h����
	hr = Load(pText);

	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	// �e��ϐ�������
	SetPosition(m_aInfo.pos);				// �ʒu
	SetWidthBlock(m_aInfo.nWidthBlock);		// ������
	SetHeightBlock(m_aInfo.nHeightBlock);	// �c����
	SetWidthLen(m_aInfo.fWidthLength);		// �c����
	SetHeightLen(m_aInfo.fHeightLength);	// ������
	m_fBrushStrength = MAX_MOVE;			// �u���V����
	m_fBrushRange = MAX_RANGE;				// �͈�
	m_bHitStop_Direct = false;				// �q�b�g�X�g�b�v�̔���
	m_bHitStop_Impact = false;				// �~�`�g�̃q�b�g�X�g�b�v����

	// �I�u�W�F�N�g3D���b�V���̏���������
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_FIELD);

	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	// ���_�����������m��
	int nVtxNum = GetNumVertex();
	m_sFadeInfo.pRot = DEBUG_NEW float[nVtxNum];
	memset(&m_sFadeInfo.pRot[0], 0, sizeof(float) * nVtxNum);

	if (CManager::GetMode() == CScene::MODE_RESULT)
	{
		for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
		{// �c�̕������J��Ԃ�

			for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
			{// ���̕������J��Ԃ�
				m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = m_aInfo.pStageVtxPos[3][nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;
			}
		}
	}

	if (CManager::GetMode() == CScene::MODE_RANKING)
	{
		int nStage = CRanking::GetRandStage();
		for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
		{// �c�̕������J��Ԃ�

			for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
			{// ���̕������J��Ԃ�
				m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = m_aInfo.pStageVtxPos[nStage][nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = m_aInfo.pStageVtxPos[nStage][nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;
			}
		}
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CElevation::Uninit(void)
{
	// ���_���W�̔j��
	if (m_aInfo.pVtxPos != NULL)
	{
		delete[] m_aInfo.pVtxPos;
		m_aInfo.pVtxPos = NULL;
	}

	if (m_sFadeInfo.pRot != NULL)
	{
		delete[] m_sFadeInfo.pRot;
		m_sFadeInfo.pRot = NULL;
	}


	// �X�e�[�W�����_���W�̔j��
	for (int nCntStage = 0; nCntStage < mylib_const::MAX_STAGE; nCntStage++)
	{
		if (m_aInfo.pStageVtxPos[nCntStage] != NULL)
		{
			delete[] m_aInfo.pStageVtxPos[nCntStage];
			m_aInfo.pStageVtxPos[nCntStage] = NULL;
		}
	}

	// �ڕW�̒n�_����
	if (m_pTargetP != NULL)
	{
		m_pTargetP->Uninit();
		m_pTargetP = NULL;
	}

	// �I������
	CObject3DMesh::Uninit();
}

//==========================================================================
// �������
//==========================================================================
void CElevation::Release(void)
{
	// ���_���W�̔j��
	if (m_aInfo.pVtxPos != NULL)
	{
		delete[] m_aInfo.pVtxPos;
		m_aInfo.pVtxPos = NULL;
	}

	// �X�e�[�W�����_���W�̔j��
	for (int nCntStage = 0; nCntStage < mylib_const::MAX_STAGE; nCntStage++)
	{
		if (m_aInfo.pStageVtxPos[nCntStage] != NULL)
		{
			delete[] m_aInfo.pStageVtxPos[nCntStage];
			m_aInfo.pStageVtxPos[nCntStage] = NULL;
		}
	}
}

//==========================================================================
// ���o�^
//==========================================================================
void CElevation::SetNaturalWaveInfo(CWaveManager::SetInfo *info, int nStage, int nWave, int nSpawnNum)
{
	for (int nCntSpawn = 0; nCntSpawn < nSpawnNum; nCntSpawn++)
	{
		// ���n��
		m_aSetInfo[nStage].aWaveInfo[nWave].Info[nCntSpawn] = info[nCntSpawn];
	}

	// �o�����鐔���Z
	m_aSetInfo[nStage].aWaveInfo[nWave].nSetNum = nSpawnNum;
}

//==========================================================================
// �X�V����
//==========================================================================
void CElevation::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// �R���{���o���͔�����
	if (CGame::GetEnemyManager()->GetState() == CEnemyManager::STATE_COMBOANIM)
	{
		return;
	}

	// �������Z�b�g
	if (CManager::GetMode() != CScene::MODE_RESULT)
	{
		memset(&m_VtxPos[0], 0, sizeof(m_VtxPos));
	}

	if (m_bSpawnByFrame == true)
	{
		// �t���[�����̃X�|�[���J�E���g���Z
		m_nSpawnByFrame++;
		if (m_nSpawnByFrame >= 5)
		{
			m_nSpawnByFrame = 0;
			m_bSpawnByFrame = false;
		}
	}

	if (pInputKeyboard->GetTrigger(DIK_F3) == true)
	{// �؂�ւ�
		m_bEdit = m_bEdit ? false : true;

		// �ڕW�̒n�_����
		if (m_pTargetP != NULL)
		{
			m_pTargetP->Uninit();
			m_pTargetP = NULL;
		}
		// �ڕW�̒n�_����
		else if (m_pTargetP == NULL)
		{
			m_pTargetP = CTargetPoint::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 400.0f, 800.0f);
		}
	}

	// ���_����
	if (m_bEdit == true)
	{// �G�f�B�b�g���Ȃ�
		UPVtxField(m_pTargetP->GetPosition());
	}
	else
	{
		// ��ԍX�V����
		UpdateState();
	}

	if (m_bSetSpawnCamera == true)
	{// �ݒ蔻�肪�����

		m_nCntSetSpawn = (m_nCntSetSpawn + 1) % 3;
		if (m_nCntSetSpawn == 0)
		{
			// �J�����h��ǉ�
			CManager::GetCamera()->SetShake(10, 5.0f, 0.0f);
		}
	}

#if 0
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// �擪��ۑ�
		CObject *pObj = CObject::GetTop(nCntPriority);

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
			if (pObj->GetType() == CObject::TYPE_XFILE ||
				pObj->GetType() == CObject::TYPE_EDIT)
			{// ����̃^�C�v��������

				D3DXVECTOR3 pos = pObj->GetPosition();
				D3DXVECTOR3 move = pObj->GetMove();
				bool bLand = false, bIn = false;

				// �����擾
				float fHeight = GetHeight(pos, move, bIn, pObj->GetType());

				if (fHeight > pos.y)
				{// �n�ʂ̕���������荂��������
					pos.y = fHeight;
					pObj->SetPosition(pos);
					move.y = 0.0f;

					if (bIn == true)
					{
						bLand = true;
					}
				}

				if (bLand == true)
				{// ����Ă�����
					move.y = 0.0f;
				}

				// �ړ��ʐݒ�
				pObj->SetMove(move);
			}

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
		}
	}
#endif
	// ���_���ݒ�
	SetVtx();

	// �q�b�g�X�g�b�v�̔���
	if (m_bHitStop_Direct == true || m_bHitStop_Impact == true)
	{
		static int nCntHitStop = 0;
		nCntHitStop = (nCntHitStop + 1) % 5;

		// �q�b�g�X�g�b�v�L��
		if (nCntHitStop == 0)
		{
			if (m_bHitStop_Direct == true)
			{
				CManager::SetEnableHitStop(SHAKETIME_DIRECT - 5);
			}
			else if (m_bHitStop_Impact == true)
			{
				CManager::SetEnableHitStop(SHAKETIME_IMPACT - 5);
			}
			m_bHitStop_Direct = false;
			m_bHitStop_Impact = false;
		}
	}

}

//==========================================================================
// �X�e�[�W�؂�ւ�
//==========================================================================
void CElevation::StageSwitch(int nNowStage, int nNextStage, int nNowLife, int nMaxLife)
{

	// ���_�J���[�擾
	D3DXCOLOR *pVtxCol = GetVtxCol();
	*pVtxCol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	if (nNowStage <= 0)
	{
		nNowStage = 0;
	}

	// ���_���W�擾
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	int nNoneTime = 60;
	float fHalfMaxLife = (float)((nMaxLife / 2) - nNoneTime);
	float fHalfAfterLife = (float)((nMaxLife / 2) + nNoneTime);
	float fRotMove = D3DX_PI * 0.05f;

	// �U��
	if (nNowLife % 3 == 0)
	{
		CManager::GetCamera()->SetShake(15, 5.0f, 0.0f);
	}

	if (GetType() == TYPE_ELEVATION)
	{
		int nRand;
		for (int i = 0; i < 10; i++)
		{
			nRand = Random(0, GetNumVertex() - 1);
			my_particle::Create(D3DXVECTOR3(pVtxPos[nRand].x, pVtxPos[nRand].y - 5.0f, pVtxPos[nRand].z), my_particle::TYPE_WAVESMOKE);
		}
	}

	float fMoveTime = (float)(fHalfMaxLife - nNowLife) / fHalfMaxLife;

	if (nNowLife < (nMaxLife / 2) - nNoneTime)
	{// �O��

		fMoveTime = (float)(fHalfMaxLife - nNowLife) / fHalfMaxLife;

		m_sFadeInfo.state = FADESTATE_INCREMENT;
		fRotMove *= -1;
	}
	else if (nNowLife >= (nMaxLife / 2) - nNoneTime && nNowLife < (nMaxLife / 2) + nNoneTime)
	{// ����Ȏ���

		fMoveTime = (float)(fHalfMaxLife - nNowLife) / fHalfMaxLife;

		m_sFadeInfo.state = FADESTATE_NONE;
		int nStartCnt = -nNoneTime;
		int nHalfCnt = 0;
		int nEndCnt = nNoneTime;
		int nLife = nNowLife - ((nMaxLife / 2) - nNoneTime);

		fRotMove *= -1;
		float fRotMoveDest = D3DX_PI * 0.05f;
		float fRotDiff = fRotMoveDest - fRotMove;
		fRotMove = (fRotDiff / (float)(nNoneTime * 2)) * nLife;
		RotNormalize(fRotMove);
		fRotMove -= D3DX_PI * 0.05f;
	}
	else
	{// �㔼
		int nEndCnt = (nMaxLife - 1) - ((nMaxLife / 2) + nNoneTime);
		int nLife = nNowLife - ((nMaxLife / 2) + nNoneTime);

		fMoveTime = (float)(nLife) / (float)nEndCnt;
		m_sFadeInfo.state = FADESTATE_DECREMENT;
	}

	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{// �c�̕������J��Ԃ�

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// ���̕������J��Ԃ�

			int nNowVtx = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			float fHeightDest = m_aInfo.pStageVtxPos[nNextStage][nNowVtx].y;

			if (m_sFadeInfo.state == FADESTATE_INCREMENT)
			{// �O��
				fHeightDest = 200.0f;
				m_sFadeInfo.fLength = (5040.0f - 0.0f) * (1.0f - fMoveTime);
			}
			else if (m_sFadeInfo.state == FADESTATE_NONE)
			{// ����Ȏ���
				fHeightDest = 200.0f;
				m_sFadeInfo.fLength = 5040.0f;
			}
			else
			{// �㔼
				int nLife = nNowLife - ((nMaxLife / 2) + nNoneTime);
				int nEnd = nMaxLife - ((nMaxLife / 2) + nNoneTime);
				float fDiff = (5040.0f - 0.0f) / (float)nEnd;
				m_sFadeInfo.fLength = fDiff * (nEnd - nLife);
			}

			float fDiffY;


			// ���_�̍����␳
			if (m_sFadeInfo.state == FADESTATE_INCREMENT)
			{// �O��
				fDiffY = fHeightDest - m_aInfo.pStageVtxPos[nNowStage][nNowVtx].y;
				float f = 200.0f + fDiffY * -fMoveTime;
				m_aInfo.pVtxPos[nNowVtx].y = f;
			}
			else if (m_sFadeInfo.state == FADESTATE_NONE)
			{// ����Ȏ���
				m_aInfo.pVtxPos[nNowVtx].y = fHeightDest;
			}
			else
			{// �㔼
				fDiffY = fHeightDest - 200.0f;
				m_aInfo.pVtxPos[nNowVtx].y = 200.0f + fDiffY * fMoveTime;
			}

			// ���_�ƒ��_�̋���
			float fNowLength =
				sqrtf((0.0f - pVtxPos[nNowVtx].x) * (0.0f - pVtxPos[nNowVtx].x)
					+ (0.0f - pVtxPos[nNowVtx].z) * (0.0f - pVtxPos[nNowVtx].z));

			// ����
			float Wariai = fNowLength / (GetWidthLen() * 8.0f);
			float WaveWariai = fNowLength / m_sFadeInfo.fLength;

#ifdef CHANGE
			float fStrength = 300.0f;
#else
			float fStrength = 80.0f;
#endif

			if (m_sFadeInfo.state == FADESTATE_INCREMENT)
			{// �O��
				fStrength = fStrength * (1.0f - fMoveTime);
			}
			else if (m_sFadeInfo.state == FADESTATE_NONE)
			{// ����Ȏ���
				int n = 0;
			}
			else
			{// �㔼
				fStrength = fStrength * (1.0f - fMoveTime);
			}

			// �g�̍���
			float fWaveStrength = fStrength;

			if (WaveWariai >= 1.0f)
			{// �͈͊O�͈ړ��ʃ[��
				fWaveStrength = 0.0f;
			}

			// �g�̌���
			float fRot = D3DX_PI * Wariai;

			// �E�F�[�u������
			m_sFadeInfo.pRot[nNowVtx] += fRotMove;

			// �p�x�̐��K��
			RotNormalize(fRot);
			RotNormalize(m_sFadeInfo.pRot[nNowVtx]);

			// �g�̍����ݒ�
			float fHeight = sinf(fRot + m_sFadeInfo.pRot[nNowVtx]) * fWaveStrength;

			if (fHeightDest >= 0.0f && fHeight <= 0.0f)
			{// �O�� && �}�C�i�X�̍���
				fHeight = 0.0f;
			}

			if (m_sFadeInfo.state != FADESTATE_DECREMENT && m_aInfo.pVtxPos[nNowVtx].y >= 0.0f)
			{// �O��

				if (fHeight > m_aInfo.pVtxPos[nNowVtx].y)
				{// �g�̕�������������E�F�[�u�Ώ�
					m_aInfo.pVtxPos[nNowVtx].y = fHeight;
				}

			}
			else if(m_sFadeInfo.state == FADESTATE_DECREMENT)
			{
				if (m_aInfo.pVtxPos[nNowVtx].y >= 0.0f)
				{// ��

					if (fHeight != 0.0f)
					{
						int n = 0;
					}

					if (fHeight > m_aInfo.pVtxPos[nNowVtx].y)
					{// �g�̕�������������E�F�[�u�Ώ�
						m_aInfo.pVtxPos[nNowVtx].y = fHeight;
					}

				}
			}

		}
	}
}

//==========================================================================
// ��ԍX�V����
//==========================================================================
void CElevation::UpdateState(void)
{
	for (int nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
	{

		// ���_�J���[�擾
		D3DXCOLOR *pVtxCol = GetVtxCol();
		*pVtxCol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		switch (m_aWave[nCntWave].state)
		{
		case STATE_NONE:

			break;

		case STATE_SET:

			// �����Ȃ���Ԃɖ߂�
			m_aWave[nCntWave].state = STATE_NONE;
			break;

		case STATE_WAVE:

			// �g�̋��������炵�Ă���
			if (m_aWave[nCntWave].sSetWave.type != WAVETYPE_RAISE && m_aWave[nCntWave].nCntState >= 0)
			{// ���R�̔g�ȊO

				if (m_aWave[nCntWave].sSetWave.playertype == TYPE_NATURAL)
				{// ���R�̔g
					m_aWave[nCntWave].sSetWave.fStrength =
						m_aWave[nCntWave].sSetWave.fStrengthOrigin * ((float)(m_aWave[nCntWave].nCntState) / (float)(m_aWave[nCntWave].nOriginCntState));
				}
				else if (m_aWave[nCntWave].nMoveLife > 0)
				{// �ړ��������s���Ă���

					if (m_aWave[nCntWave].sSetWave.type == WAVETYPE_RIPPLE)
					{// �g��̎�
						m_aWave[nCntWave].nCntState -= m_aWave[nCntWave].nMoveLife;
						m_aWave[nCntWave].nMoveLife = 0;
					}

					m_aWave[nCntWave].sSetWave.fStrength =
						m_aWave[nCntWave].sSetWave.fStrengthOrigin * ((float)(m_aWave[nCntWave].nCntState - 180) / (float)(m_aWave[nCntWave].nOriginCntState - 180));

					if (m_aWave[nCntWave].sSetWave.fStrength >= m_aWave[nCntWave].sSetWave.fStrengthOrigin * 0.5f)
					{// �K��l�������Ȃ��悤�ɕ␳
						m_aWave[nCntWave].sSetWave.fStrength = m_aWave[nCntWave].sSetWave.fStrengthOrigin * 0.5f;
					}
				}
				else
				{
					m_aWave[nCntWave].sSetWave.fStrength =
						(m_aWave[nCntWave].sSetWave.fStrengthOrigin * 0.5f) * ((float)m_aWave[nCntWave].nCntState / (float)m_aWave[nCntWave].nOriginCntState);
				}
			}
			else if(m_aWave[nCntWave].sSetWave.type == WAVETYPE_RAISE && m_aWave[nCntWave].nCntState >= 0)
			{// ����グ�̔g

				switch (m_aWave[nCntWave].sSetWave.playertype)
				{
				case TYPE_NATURAL:
					m_aWave[nCntWave].sSetWave.fStrength =
						m_aWave[nCntWave].sSetWave.fStrengthOrigin * (1.0f - ((float)(m_aWave[nCntWave].nCntState / (float)m_aWave[nCntWave].nOriginCntState)));
					break;

				case TYPE_SPAWN:
				if(m_aWave[nCntWave].nMoveLife > 0)
				{// �ړ����������鎞

					float fOrigin = (float)m_aWave[nCntWave].nOriginCntState * 0.5f;	// ���̃J�E���g�̔���
					m_aWave[nCntWave].sSetWave.fStrength =
						m_aWave[nCntWave].sSetWave.fStrengthOrigin * (1.0f - ((float)(m_aWave[nCntWave].nMoveLife / (float)m_aWave[nCntWave].nOriginMoveLife)));

					if (m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_CIRCLE ||
						m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_CLOCK ||
						m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_BOTHCLOCK)
					{// �����w�肳��Ă���Ƃ�
						// �ڕW�̋����ݒ�
						CManager::GetCamera()->SetLenDest(CManager::GetCamera()->GetOriginDistance() + 300.0f);
					}
				}
				else if (m_aWave[nCntWave].nCntState <= m_aWave[nCntWave].nOriginCntState / 2 && m_aWave[nCntWave].nWaitTime >= 0)
				{// �o������J�E���g�̎�

					// �ҋ@���Ԍ��Z
					m_aWave[nCntWave].nWaitTime--;

					// �J�����ݒ�̔���I�t
					m_bSetSpawnCamera = false;
					m_nCntSetSpawn = 0;

					if (m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_CIRCLE ||
						m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_CLOCK ||
						m_aWave[nCntWave].WaveManagerInfo.nSpawnType == CEnemyManager::SPAWNTYPE_BOTHCLOCK)
					{// �����w�肳��Ă���Ƃ�
						// �ڕW�̋����ݒ�
						CManager::GetCamera()->SetLenDest(CManager::GetCamera()->GetOriginDistance() + 300.0f);
					}

					if (m_aWave[nCntWave].nWaitTime < 0)
					{// �ҋ@���Ԃ��Ȃ��Ȃ�����

						// �G�ݒ�
						SetEnemy(nCntWave);
					}
				}
				else if(m_aWave[nCntWave].nCntState <= m_aWave[nCntWave].nOriginCntState / 2 - SPAWN_WAITTIME && m_aWave[nCntWave].nWaitTime <= 0)
				{// �ړ��������Ȃ��Ȃ�����
					float fOrigin = (float)m_aWave[nCntWave].nOriginCntState * 0.5f - SPAWN_WAITTIME;	// ���̃J�E���g�̔���
					m_aWave[nCntWave].sSetWave.fStrength =
						m_aWave[nCntWave].sSetWave.fStrengthOrigin * (((float)(m_aWave[nCntWave].nCntState / fOrigin)));
				}
					break;

				default:
					break;
				}
				
			}

			// �g����
			Wave(nCntWave);

			if (m_aWave[nCntWave].nMoveLife > 0 && m_aWave[nCntWave].sSetWave.type != WAVETYPE_RAISE)
			{// �ړ����������邤��

				// �����蔻��
				Collision(nCntWave);
			}

			// ��������
			int nDecrementTime = m_aWave[nCntWave].nOriginCntState / 2 - SPAWN_WAITTIME;

			// ��ԑJ�ڃJ�E���^�[���Z
			if (m_aWave[nCntWave].nWaitTime <= 0 || nDecrementTime <= m_aWave[nCntWave].nCntState)
			{
				m_aWave[nCntWave].nCntState--;
			}

			if (m_aWave[nCntWave].nCntState <= 0 && m_aWave[nCntWave].sSetWave.playertype != TYPE_NATURAL)
			{
				// ���_�ۑ���ԂɈڍs
				m_aWave[nCntWave].state = STATE_SET;
			}

			if (m_aWave[nCntWave].sSetWave.type == WAVETYPE_RAISE && m_aWave[nCntWave].sSetWave.playertype == TYPE_NATURAL &&
				m_aWave[nCntWave].nCntState % 15 == 0)
			{// ���R�̔g

				// �ڈ�
				my_particle::Create(D3DXVECTOR3(
					m_aWave[nCntWave].sSetWave.pos.x + Random(-40, 40),
					m_aWave[nCntWave].sSetWave.pos.y,
					m_aWave[nCntWave].sSetWave.pos.z + Random(-40, 40)), my_particle::TYPE_NATURALWAVE_SPAWN);
			}

			// �ړ��������Z
			if (m_aWave[nCntWave].nWaitTime <= 0 || nDecrementTime <= m_aWave[nCntWave].nCntState)
			{
				m_aWave[nCntWave].nMoveLife--;
			}

			if (m_aWave[nCntWave].nMoveLife <= 0)
			{// �ړ��������Ȃ��Ȃ�����

				m_aWave[nCntWave].nMoveLife = 0;

				if (m_aWave[nCntWave].sSetWave.playertype == TYPE_NATURAL && m_aWave[nCntWave].sSetWave.type == WAVETYPE_DIRECT)
				{// ���R�̔g && ����

					m_aWave[nCntWave].nCntState = m_aWave[nCntWave].nOriginCntState;
					m_aWave[nCntWave].nMoveLife = m_aWave[nCntWave].nOriginMoveLife;
					m_aWave[nCntWave].sSetWave.pos = m_aWave[nCntWave].sSetWave.posOrigin;
				}
			}

			break;
		}
	}
}

//==========================================================================
// �G�̐ݒ�
//==========================================================================
void CElevation::SetEnemy(int nCntWave)
{

	// ���݂̃X�e�[�W�擾
	int nStage = CManager::GetScene()->GetWaveManager()->GetNowStage();

	CWaveManager::SetInfo info = m_aWave[nCntWave].WaveManagerInfo;

	int nNumEnemy = m_aWave[nCntWave].nNumEnemy;
	// �G�o��
	if (info.nSpawnType == CEnemyManager::SPAWNTYPE_CIRCLE ||
		info.nSpawnType == CEnemyManager::SPAWNTYPE_CLOCK ||
		info.nSpawnType == CEnemyManager::SPAWNTYPE_BOTHCLOCK)
	{// �����w�肳��Ă���Ƃ�
		nNumEnemy = 0;
	}

	if (info.nSpawnType == CEnemyManager::SPAWNTYPE_NORMAL)
	{
		int n = 0;
	}

	if (info.nSpawnType != CEnemyManager::SPAWNTYPE_CIRCLE &&
		info.nSpawnType != CEnemyManager::SPAWNTYPE_CLOCK &&
		info.nSpawnType != CEnemyManager::SPAWNTYPE_BOTHCLOCK && nNumEnemy == 0)
	{
		// �Ռ��g����
		CImpactWave::Create
		(
			D3DXVECTOR3(m_aWave[nCntWave].sSetWave.pos.x, m_aWave[nCntWave].sSetWave.pos.y + 50.0f, m_aWave[nCntWave].sSetWave.pos.z),	// �ʒu
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// ����
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f),			// �F
			100.0f,										// ��
			10.0f,										// ����
			80,											// ����
			40.0f,										// ���̈ړ���
			CImpactWave::TYPE_GREEN,					// �e�N�X�`���^�C�v
			false										// ���Z�������邩
		);
	}

	if (m_bSpawnByFrame == false &&
		(CManager::GetMode() == CScene::MODE_GAME || CManager::GetMode() == CScene::MODE_TUTORIAL))
	{
		// �T�E���h�Đ�
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_SPAWNEN);

		// �T�E���h�Đ�
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_SPAWN);
		m_bSpawnByFrame = true;
	}

	// �G�z�u
	CGame::GetEnemyManager()->SetEnemy(m_aWave[nCntWave].nMyIdx, info.nType, nNumEnemy, m_aWave[nCntWave].nParentIdx, info, m_aWave[nCntWave].sSetWave.pos, m_aWave[nCntWave].sSetWave.rot, info.nMoveType);
}

//==========================================================================
// �`�揈��
//==========================================================================
void CElevation::Draw(void)
{
	//  �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (CManager::IsWireframe() == true)
	{
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);	// ���C���[�t���[�����[�h
	}

	// �`�揈��
	CObject3DMesh::Draw();

	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);	// ���߂郂�[�h
}

//==========================================================================
// �����擾
//==========================================================================
float CElevation::GetHeight(D3DXVECTOR3& pos, D3DXVECTOR3& move, bool &bLand, CObject::TYPE type)
{
	// �x�N�g���Ɩ@��
	D3DXVECTOR3 vec1, vec2, nor;
	float fHeight = 0.0f;
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

#if 0
	//for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock; nCntHeight++)
	//{// �c�̕������J��Ԃ�

	//	for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock; nCntWidth++)
	//	{// ���̕������J��Ԃ�

	//		if (bLand == true)
	//		{
	//			break;
	//		}

	//		// ����̒��_
	//		int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
	//		int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

	//		int nLeft	= nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
	//		int nRight	= nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

	//		if (CollisionTriangle(pVtxPos[nNowPoint], pVtxPos[nLeft], pVtxPos[nRight], pos, pos) == true)
	//		{// �O�p�ɓ����Ă�����

	//			// �x�N�g�����v�Z
	//			vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
	//			vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

	//			// �O�ς����߂�
	//			D3DXVec3Cross(&nor, &vec1, &vec2);

	//			// �O�ς̐��K�������Ė@���ɂ���
	//			D3DXVec3Normalize(&nor, &nor);

	//			if (nor.y != 0.0f)
	//			{// �@����0.0f����Ȃ�������

	//				// ���������߂�
	//				fHeight = ((pos.x - pVtxPos[nNowPoint].x) * nor.x + (pos.z - pVtxPos[nNowPoint].z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
	//				fHeight += GetPosition().y;
	//				bLand = true;
	//				break;
	//			}
	//		}

	//		if (CollisionTriangle(pVtxPos[nNowPoint + m_aInfo.nWidthBlock], pVtxPos[nRight], pVtxPos[nLeft], pos, pos) == true)
	//		{// �O�p�ɓ����Ă�����

	//			// �x�N�g�����v�Z
	//			vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
	//			vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

	//			// �O�ς����߂�
	//			D3DXVec3Cross(&nor, &vec1, &vec2);

	//			// �O�ς̐��K�������Ė@���ɂ���
	//			D3DXVec3Normalize(&nor, &nor);

	//			if (nor.y != 0.0f)
	//			{// �@����0.0f����Ȃ�������

	//				// ���������߂�
	//				fHeight = ((pos.x - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].x) * nor.x + (pos.z - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].z) * nor.z + (pVtxPos[nNowPoint + m_aInfo.nWidthBlock].y * -nor.y)) / -nor.y;
	//				fHeight += GetPosition().y;
	//				bLand = true;
	//				break;
	//			}
	//		}
	//	}
	//}
#endif
	// �t�B�[���h�̈ʒu
	D3DXVECTOR3 posfield = GetPosition();

	// �����擾
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// �ő�̒���
	float fMaxWidthLen = fWidthLen * GetWidthBlock();
	float fMaxHeightLen = -fWidthLen * GetHeightBlock();

	// ���肷�钸�_�̌v�Z
	int nCntWidth = 0;
	int nCntHeight = 0;
	int nWidthPoint = ((pos.x - posfield.x) - fMaxWidthLen * 0.5f) / fWidthLen + GetWidthBlock();
	int nHeightPoint = GetHeightBlock() - ((pos.z - posfield.z) - fMaxHeightLen * 0.5f) / fHeightLen;

	for (int nCntH = 0; nCntH < 2; nCntH++)
	{
		for (int nCntW = 0; nCntW < 2; nCntW++)
		{
			// �����_
			nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);

			if (nCntWidth < 0 || m_aInfo.nWidthBlock < nCntWidth)
			{// �͈͊O�Ő擪
				continue;
			}

			if (nCntHeight < 0 || m_aInfo.nHeightBlock < nCntHeight)
			{// �͈͊O�Ő擪
				continue;
			}

			// ����̒��_
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (nLeft >= GetNumVertex() || nRight >= GetNumVertex())
			{
				continue;
			}

			if (CollisionTriangle(pVtxPos[nNowPoint] + posfield, pVtxPos[nLeft] + posfield, pVtxPos[nRight] + posfield, pos, pos) == true)
			{// �O�p�ɓ����Ă�����

				// �x�N�g�����v�Z
				vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
				vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

				// �O�ς����߂�
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// �O�ς̐��K�������Ė@���ɂ���
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// �@����0.0f����Ȃ�������

					// ���������߂�
					fHeight = ((pos.x - pVtxPos[nNowPoint].x - posfield.x) * nor.x + (pos.z - pVtxPos[nNowPoint].z - posfield.z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					bLand = true;


					if (nor.y < 0.0f)
					{
						int n = 0;
					}
					/*if (nor.y <= 0.5f && nor.y >= -0.5f && fHeight > pos.y && (type == CObject::TYPE_PLAYER || type == CObject::TYPE_ENEMY))
					{
						pos.x -= move.x;
						pos.z -= move.z;
						move.x = 0.0f;
						move.z = 0.0f;
						int n = 0;
					}*/
				}
				break;
			}

			if (CollisionTriangle(pVtxPos[nNowPoint + GetWidthBlock()] + posfield, pVtxPos[nRight] + posfield, pVtxPos[nLeft] + posfield, pos, pos) == true)
			{// �O�p�ɓ����Ă�����

				// �x�N�g�����v�Z
				vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
				vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

				// �O�ς����߂�
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// �O�ς̐��K�������Ė@���ɂ���
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// �@����0.0f����Ȃ�������

					// ���������߂�
					fHeight = ((pos.x - pVtxPos[nNowPoint + GetWidthBlock()].x - posfield.x) * nor.x + (pos.z - pVtxPos[nNowPoint + GetWidthBlock()].z - posfield.z) * nor.z + (pVtxPos[nNowPoint + GetWidthBlock()].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					bLand = true;

					if (nor.y < 0.0f)
					{
						int n = 0;
					}


					/*if (nor.y <= 0.5f && nor.y >= -0.5f && fHeight >= pos.y && (type == CObject::TYPE_PLAYER || type == CObject::TYPE_ENEMY))
					{
						pos.x -= move.x;
						pos.z -= move.z;
						move.x = 0.0f;
						move.z = 0.0f;
						int n = 0;
					}*/
					break;
				}
			}

		}
	}

	if (bLand == true)
	{// ���n���Ă�����

		return fHeight;
	}

	// �������擾
	return pos.y;
}

//==========================================================================
// �n�ʂɓ���������
//==========================================================================
bool CElevation::IsHit(D3DXVECTOR3& pos)
{
	bool bHit = false;

	// �x�N�g���Ɩ@��
	D3DXVECTOR3 vec1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f), vec2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f), nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	float fHeight = pos.y;
	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	D3DXVECTOR3 posfield = GetPosition();

	// �����擾
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// �ő�̒���
	float fMaxWidthLen = fWidthLen * m_aInfo.nWidthBlock + posfield.x;
	float fMaxHeightLen = -fWidthLen * m_aInfo.nHeightBlock + posfield.z;

	// ���_���ߑł�
	int nCntWidth = 0;
	int nCntHeight = 0;
	int nWidthPoint = (int)((pos.x - fMaxWidthLen * 0.5f) / fWidthLen) + m_aInfo.nWidthBlock;
	int nHeightPoint = m_aInfo.nHeightBlock - (int)((pos.z - fMaxHeightLen * 0.5f) / fHeightLen);

	for (int nCntH = 0; nCntH < 2; nCntH++)
	{
		for (int nCntW = 0; nCntW < 2; nCntW++)
		{
			// �����_
			nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);

			if (nCntWidth < 0 || m_aInfo.nWidthBlock < nCntWidth)
			{// �͈͊O�Ő擪
				continue;
			}

			if (nCntHeight < 0 || m_aInfo.nHeightBlock < nCntHeight)
			{// �͈͊O�Ő擪
				continue;
			}

			// ����̒��_
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (CollisionTriangle(pVtxPos[nNowPoint], pVtxPos[nLeft], pVtxPos[nRight], pos, pos) == true)
			{// �O�p�ɓ����Ă�����

				// �x�N�g�����v�Z
				vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
				vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

				// �O�ς����߂�
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// �O�ς̐��K�������Ė@���ɂ���
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// �@����0.0f����Ȃ�������

					// ���������߂�
					fHeight = ((pos.x - pVtxPos[nNowPoint].x) * nor.x + (pos.z - pVtxPos[nNowPoint].z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;

					/*if (nor.y <= 0.5f && nor.y >= -0.5f && fHeight >= pos.y)
					{
						pos.x -= move.x;
						pos.z -= move.z;
						move.x = 0.0f;
						move.z = 0.0f;
						int n = 0;
					}*/
					break;
				}
			}

			if (CollisionTriangle(pVtxPos[nNowPoint + m_aInfo.nWidthBlock], pVtxPos[nRight], pVtxPos[nLeft], pos, pos) == true)
			{// �O�p�ɓ����Ă�����

				// �x�N�g�����v�Z
				vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
				vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

				// �O�ς����߂�
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// �O�ς̐��K�������Ė@���ɂ���
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// �@����0.0f����Ȃ�������

					// ���������߂�
					fHeight = ((pos.x - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].x) * nor.x + (pos.z - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].z) * nor.z + (pVtxPos[nNowPoint + m_aInfo.nWidthBlock].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					break;
				}
			}
		}
	}

	if (fHeight > pos.y)
	{// �O��̈ʒu��������������(�������Ă�����)
		bHit = true;
		pos.y = fHeight;
	}

	// �����Ԃ�
	return bHit;
}

//==========================================================================
// �X���C�h�̔���
//==========================================================================
float CElevation::Slide(D3DXVECTOR3& pos)
{

	float fVecY = 1.0f;

	// �x�N�g���Ɩ@��
	D3DXVECTOR3 vec1, vec2, nor;
	float fHeight = pos.y;
	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	D3DXVECTOR3 posfield = GetPosition();

	// �����擾
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// �ő�̒���
	float fMaxWidthLen = fWidthLen * m_aInfo.nWidthBlock + posfield.x;
	float fMaxHeightLen = -fWidthLen * m_aInfo.nHeightBlock + posfield.z;

	// ���_���ߑł�
	int nCntWidth = 0;
	int nCntHeight = 0;
	int nWidthPoint = (int)((pos.x - fMaxWidthLen * 0.5f) / fWidthLen) + m_aInfo.nWidthBlock;
	int nHeightPoint = m_aInfo.nHeightBlock - (int)((pos.z - fMaxHeightLen * 0.5f) / fHeightLen);

	for (int nCntH = 0; nCntH < 2; nCntH++)
	{
		for (int nCntW = 0; nCntW < 2; nCntW++)
		{
			// �����_
			nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);

			if (nCntWidth < 0 || m_aInfo.nWidthBlock < nCntWidth)
			{// �͈͊O�Ő擪
				continue;
			}

			if (nCntHeight < 0 || m_aInfo.nHeightBlock < nCntHeight)
			{// �͈͊O�Ő擪
				continue;
			}

			// ����̒��_
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (CollisionTriangle(pVtxPos[nNowPoint], pVtxPos[nLeft], pVtxPos[nRight], pos, pos) == true)
			{// �O�p�ɓ����Ă�����

				// �x�N�g�����v�Z
				vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
				vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

				// �O�ς����߂�
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// �O�ς̐��K�������Ė@���ɂ���
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// �@����0.0f����Ȃ�������

					// ���������߂�
					fHeight = ((pos.x - pVtxPos[nNowPoint].x) * nor.x + (pos.z - pVtxPos[nNowPoint].z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;

					if (fHeight >= pos.y)
					{// �n�`�̕���������(�o���Ă�)
						fVecY = nor.y - 1.0f;
					}
					else
					{// �����Ă�
						fVecY = 1.0f - nor.y + 0.25f;
					}

					/*if (nor.y <= 0.5f && nor.y >= -0.5f && fHeight >= pos.y)
					{
						pos.x -= move.x;
						pos.z -= move.z;
						move.x = 0.0f;
						move.z = 0.0f;
						int n = 0;
					}*/
					break;
				}
			}

			if (CollisionTriangle(pVtxPos[nNowPoint + m_aInfo.nWidthBlock], pVtxPos[nRight], pVtxPos[nLeft], pos, pos) == true)
			{// �O�p�ɓ����Ă�����

				// �x�N�g�����v�Z
				vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];
				vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + m_aInfo.nWidthBlock];

				// �O�ς����߂�
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// �O�ς̐��K�������Ė@���ɂ���
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// �@����0.0f����Ȃ�������

					// ���������߂�
					fHeight = ((pos.x - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].x) * nor.x + (pos.z - pVtxPos[nNowPoint + m_aInfo.nWidthBlock].z) * nor.z + (pVtxPos[nNowPoint + m_aInfo.nWidthBlock].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;

					if (fHeight >= pos.y)
					{// �n�`�̕���������(�o���Ă�)
						fVecY = nor.y - 1.0f;
					}
					else
					{// �����Ă�
						fVecY = 1.0f - nor.y + 0.25f;
					}
					break;
				}
			}
		}
	}

	// �����Ԃ�
	return fVecY;
}

//==========================================================================
// ���_�グ����
//==========================================================================
void CElevation::UPVtxField(D3DXVECTOR3 pos)
{

	float m_fWidthLen = GetWidthLen();
	float m_fHeightLen = GetHeightLen();
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// �f�o�b�O�\��
	CManager::GetDebugProc()->Print(
		"------------------[ �N���G�f�B�b�g��� ]------------------\n"
		"�u���V�̃T�C�Y�F[1, 2] �y%f�z\n"
		"�u���V�̋����F  [3, 4] �y%f�z\n"
		"���_�グ�����F  [5, 6]\n"
		"���g�k�F        [7, 8] �y%f, %f�z\n"
		"�܂����F        [ �X ]\n"
		"���_���Z�b�g�F  [delete]\n"
		"�N�����ۑ��F  [F9]\n\n", m_fBrushRange, m_fBrushStrength, m_fWidthLen, m_fHeightLen);

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// �Z�[�u
		Save();
	}

	// �u���V�̃T�C�Y
	if (pInputKeyboard->GetPress(DIK_1))
	{// 1�ŉ�����
		m_fBrushRange -= 1.0f;
	}

	if (pInputKeyboard->GetPress(DIK_2))
	{// 2�ŏグ��
		m_fBrushRange += 1.0f;
	}

	// �u���V�̋���
	if (pInputKeyboard->GetPress(DIK_3))
	{// 3�ŉ�����
		m_fBrushStrength -= 0.01f;
	}

	if (pInputKeyboard->GetPress(DIK_4))
	{// 4�ŏグ��
		m_fBrushStrength += 0.01f;
	}

	// �t�B�[���h�̕�
	if (pInputKeyboard->GetPress(DIK_7))
	{// 7�ŏグ��
		m_fWidthLen -= MOVE_LEN;
		m_fHeightLen -= MOVE_LEN;
	}

	if (pInputKeyboard->GetPress(DIK_8))
	{// 8�ŉ�����
		m_fWidthLen += MOVE_LEN;
		m_fHeightLen += MOVE_LEN;
	}

	// �l�̐��K��
	ValueNormalize(m_fWidthLen, 99999999.0f, 10.0f);
	ValueNormalize(m_fHeightLen, 99999999.0f, 10.0f);

	// �l�̐��K��
	ValueNormalize(m_fBrushStrength, 99999999.0f, 0.1f);
	ValueNormalize(m_fBrushRange, 99999999.0f, 1.0f);

	// ���_���̐ݒ�
	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{//�c�̒��_�����J��Ԃ�

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// ���̒��_�����J��Ԃ�

			if (pInputKeyboard->GetTrigger(DIK_DELETE))
			{// delete�Ń��Z�b�g
				for (int nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
				{
					m_aWave[nCntWave].VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
				}
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
			}

			float fNowLength =
				sqrtf((pos.x - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x) * (pos.x - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x)
					+ (pos.z - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z) * (pos.z - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z));

			float Wariai = fNowLength / m_fBrushRange;

			if (Wariai >= 1.0f)
			{// ������1.0f�ȉ�����
				continue;
			}

			D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			if (pInputKeyboard->GetPress(DIK_5))
			{// 5�ŉ�����
				move.y = -m_fBrushStrength - (-m_fBrushStrength * Wariai);
			}

			if (pInputKeyboard->GetPress(DIK_6))
			{// 6�ŏグ��
				move.y = m_fBrushStrength - (m_fBrushStrength * Wariai);
			}

			if (pInputKeyboard->GetTrigger(DIK_9))
			{// 9�ł܂���
				m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = pos.y;
				move.y = 0.0f;
			}

			m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += move.y;
			// �e���_���W�m�F
			//pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += move.y;
		}
	}

	// ���_�ݒ�
	SetVtxPos(pVtxPos);
	SetWidthLen(m_fWidthLen);
	SetHeightLen(m_fHeightLen);

	if (m_pTargetP != NULL)
	{
		// �����ύX
		m_pTargetP->SetWidthLen(m_fBrushRange);

		// �J�����̏��擾
		CCamera *pCamera = CManager::GetCamera();

		// �J�����̌����擾
		D3DXVECTOR3 Camerarot = pCamera->GetRotation();

		if (pInputKeyboard->GetPress(DIK_LEFT) == true)
		{// ���L�[�������ꂽ,���ړ�

			if (pInputKeyboard->GetPress(DIK_UP) == true)
			{// A+W,����ړ�

				pos.x += sinf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			}
			else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
			{// A+S,�����ړ�

				pos.x += sinf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			}
			else
			{// A,���ړ�

				pos.x += sinf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
				pos.z += cosf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
		{// D�L�[�������ꂽ,�E�ړ�

			if (pInputKeyboard->GetPress(DIK_UP) == true)
			{// D+W,�E��ړ�

				pos.x += sinf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			}
			else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
			{// D+S,�E���ړ�

				pos.x += sinf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			}
			else
			{// D,�E�ړ�

				pos.x += sinf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
				pos.z += cosf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_UP) == true)
		{// W�������ꂽ�A���ړ�

			pos.x += sinf(Camerarot.y) * MOVE;
			pos.z += cosf(Camerarot.y) * MOVE;
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// S�������ꂽ�A��O�ړ�

			pos.x += sinf(D3DX_PI + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI + Camerarot.y) * MOVE;
		}

		// �ʒu�ݒ�
		m_pTargetP->SetPosition(pos);
	}

}

//==========================================================================
// �_���[�W����
//==========================================================================
void CElevation::Damage(D3DXVECTOR3 pos, float fRange, float fStrength, DAMAGE damage)
{
	int nCntWave = 0;
	for (nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
	{
		if (m_aWave[nCntWave].state == STATE_NONE)
		{
			m_aWave[nCntWave].sSetWave.pos = pos;
			m_aWave[nCntWave].sSetWave.fStrength = 0.0f;
			m_aWave[nCntWave].sSetWave.fStrengthOrigin = 0.0f;
			m_aWave[nCntWave].nCntState = 30;
			m_aWave[nCntWave].state = STATE_WAVE;
			m_aWave[nCntWave].nOriginCntState = 30;	// ��ԑJ�ڃJ�E���^�[
			break;
		}
	}

	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// ���_���̐ݒ�
	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{//�c�̒��_�����J��Ԃ�

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// ���̒��_�����J��Ԃ�

			float fNowLength = 
				sqrtf((pos.x - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x) * (pos.x - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x)
				+ (pos.z - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z) * (pos.z - pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z));

			// �����v�Z
			float Wariai = fNowLength / fRange;

			if (Wariai >= 1.0f)
			{// ������1.0f�ȉ�����
				continue;
			}

			D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			// ���_���ړ�������
			switch (damage)
			{
			case DAMAGE_RATIO:
				move.y = fStrength - (fStrength * Wariai);
				break;

			case DAMAGE_EQUAL:
				move.y = fStrength;
				break;
			}

			// �e���_���W�m�F
			//pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += move.y;

			// �g�̍����ݒ�
			m_aWave[nCntWave].VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = move.y;

			//if (pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y <= MIN_HEIGHT)
			//{// �Œ�����������

			//	// �Œ�ŌŒ�
			//	pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = MIN_HEIGHT;
			//}
			int n = 0;
		}
	}

	// ���_�ݒ�
	SetVtxPos(pVtxPos);

}

//==========================================================================
// �g����
//==========================================================================
void CElevation::SetWave(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR2 fRange, float fRatio, float fStrength, float fWave, int nCntState, WAVETYPE type, TYPE playertype, int nLevel)
{
	for (int nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
	{
		if (m_aWave[nCntWave].state == STATE_NONE)
		{
			m_aWave[nCntWave].sSetWave.pos = pos;
			m_aWave[nCntWave].sSetWave.posOrigin = pos;
			m_aWave[nCntWave].sSetWave.rot = rot;
			m_aWave[nCntWave].sSetWave.move = move;
			m_aWave[nCntWave].sSetWave.fRange = fRange;
			m_aWave[nCntWave].sSetWave.fRatio = fRatio;
			m_aWave[nCntWave].sSetWave.fStrength = fStrength;
			m_aWave[nCntWave].sSetWave.fStrengthOrigin = fStrength;
			m_aWave[nCntWave].sSetWave.type = type;
			m_aWave[nCntWave].sSetWave.playertype = playertype;
			m_aWave[nCntWave].sSetWave.fWave = fWave;
			m_aWave[nCntWave].sSetWave.fLength = m_aWave[nCntWave].sSetWave.fRatio * 1.5f;
			m_aWave[nCntWave].sSetWave.nLevel = nLevel;
			m_aWave[nCntWave].nMoveLife = nCntState;				// �ړ�����
			m_aWave[nCntWave].nOriginMoveLife = nCntState;				// �ړ�����
			m_aWave[nCntWave].nCntState = nCntState;			// ��ԑJ�ڃJ�E���^�[
			m_aWave[nCntWave].nOriginCntState = nCntState;	// ��ԑJ�ڃJ�E���^�[

			if (type == WAVETYPE_IMPACT && (playertype == TYPE_PLAYER || playertype == TYPE_ENEMY))
			{
				CManager::GetSound()->PlaySound(CSound::LABEL_SE_FIELD);
			}

			if (playertype != TYPE_NATURAL)
			{// ���R�g�̎�
				m_aWave[nCntWave].nCntState += 180;			// ��ԑJ�ڃJ�E���^�[
				m_aWave[nCntWave].nOriginCntState += 180;	// ��ԑJ�ڃJ�E���^�[
			}

			if (playertype == TYPE_SPAWN)
			{// �X�|�[���̋N���̎�
				m_aWave[nCntWave].nCntState = nCntState * 2 + SPAWN_WAITTIME;		// ��ԑJ�ڃJ�E���^�[
				m_aWave[nCntWave].nOriginCntState = nCntState * 2 + SPAWN_WAITTIME;	// ��ԑJ�ڃJ�E���^�[
			}

			m_aWave[nCntWave].state = STATE_WAVE;
			m_aWave[nCntWave].fMaxWaveHeight = 0.0f;		// �g�̍ő卂��

			memset(&m_aWave[nCntWave].fWave[0], 0, sizeof(float) * 5600);
			break;
		}
	}
}

//==========================================================================
// �X�|�[���g�̐ݒ�
//==========================================================================
void CElevation::SetSpawnWave(int nIdx, int nParentID, int nNumEnemy, int nWaitTime, CWaveManager::SetInfo info, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 Range, float fStrength, int nCntState)
{
	// �J�����ݒ�̔���I��
	m_bSetSpawnCamera = true;

	for (int nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
	{
		if (m_aWave[nCntWave].state != STATE_NONE)
		{// ��������̏�Ԃ̓X�L�b�v
			continue;
		}

		m_aWave[nCntWave].WaveManagerInfo = info;						// �E�F�[�u�}�l�[�W���̏��
		m_aWave[nCntWave].nMyIdx = nIdx;								// �����̃C���f�b�N�X�ԍ�
		m_aWave[nCntWave].nParentIdx = nParentID;						// �e�̃C���f�b�N�X�ԍ�
		m_aWave[nCntWave].nNumEnemy = nNumEnemy;						// ���Ԗڂ̓G��
		m_aWave[nCntWave].nWaitTime = nWaitTime;						// �ҋ@����
		m_aWave[nCntWave].sSetWave.pos = pos;							// �ʒu
		m_aWave[nCntWave].sSetWave.posOrigin = pos;						// ���̈ʒu
		m_aWave[nCntWave].sSetWave.rot = rot;							// ����
		m_aWave[nCntWave].sSetWave.move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);// �ړ���
		m_aWave[nCntWave].sSetWave.fRange = Range;						// �͈�
		m_aWave[nCntWave].sSetWave.fRatio = 0.0f;						// �Ԋu
		m_aWave[nCntWave].sSetWave.fStrength = fStrength;				// ����
		m_aWave[nCntWave].sSetWave.fStrengthOrigin = fStrength;			// ���̍���
		m_aWave[nCntWave].sSetWave.type = WAVETYPE_RAISE;				// �g�̎��
		m_aWave[nCntWave].sSetWave.playertype = TYPE_SPAWN;				// �o���҂̎��
		m_aWave[nCntWave].sSetWave.fWave = 0.0f;						// �g�̍L����
		m_aWave[nCntWave].sSetWave.fLength = 0.0f;						// ���S�_����̒���
		m_aWave[nCntWave].sSetWave.nLevel = 0;							// ���x��
		m_aWave[nCntWave].nMoveLife = nCntState;						// �ړ�����
		m_aWave[nCntWave].nOriginMoveLife = nCntState;					// �ړ�����
		m_aWave[nCntWave].nCntState = nCntState * 2 + SPAWN_WAITTIME;		// ��ԑJ�ڃJ�E���^�[
		m_aWave[nCntWave].nOriginCntState = nCntState * 2 + SPAWN_WAITTIME;	// ��ԑJ�ڃJ�E���^�[
		m_aWave[nCntWave].state = STATE_WAVE;								// ���
		m_aWave[nCntWave].fMaxWaveHeight = 0.0f;							// �g�̍ő卂��

		// �������Z�b�g
		memset(&m_aWave[nCntWave].fWave[0], 0, sizeof(float) * 5600);
		break;
	}
}

//==========================================================================
// �g����
//==========================================================================
void CElevation::Wave(int nCntWave)
{
	if (m_aWave[nCntWave].nMoveLife > 0)
	{// ���������邤��
		// ���_�ړ�
		m_aWave[nCntWave].sSetWave.pos += m_aWave[nCntWave].sSetWave.move;
	}

#if 1
	// �t�B�[���h�̏��
	D3DXVECTOR3 posfield = GetPosition();
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;
	D3DXVECTOR2 Range = m_aWave[nCntWave].sSetWave.fRange;
	float fLength = sqrtf(Range.x * Range.x + Range.y * Range.y);	// �Ίp���̒���
	float fAngle = atan2f(Range.x, Range.y);					// �Ίp���̌���

	// �����擾
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	// �ő�̒���
	float fMaxWidthLen = fWidthLen * m_aInfo.nWidthBlock + posfield.x;
	float fMaxHeightLen = -fWidthLen * m_aInfo.nHeightBlock + posfield.z;

	// ���_���ߑł�
	int nWidthPoint = (int)((m_aWave[nCntWave].sSetWave.pos.x - fMaxWidthLen * 0.5f) / fWidthLen) + m_aInfo.nWidthBlock;
	int nHeightPoint = m_aInfo.nHeightBlock - (int)((m_aWave[nCntWave].sSetWave.pos.z - fMaxHeightLen * 0.5f) / fHeightLen);

	// ���肷��͈�
	float fLeftLen = 0.0f;
	if (m_aWave[nCntWave].sSetWave.type == WAVETYPE_IMPACT)
	{// �~�`�g�̎�
		fLeftLen = m_aWave[nCntWave].sSetWave.fLength * 2.0f + m_aWave[nCntWave].sSetWave.fRatio;
	}
	else
	{// �~�`�g�ȊO
		if (Range.x >= Range.y)
		{
			fLeftLen = Range.x * 2.0f;
		}
		else
		{
			fLeftLen = Range.y * 2.0f;
		}
	}

	// ���������͈�
	int nNumVtx = (int)(fLeftLen / 63.0f);
	nNumVtx += 1;

	// �͈͊O�̂��̂͑S��0
	memset(&m_aWave[nCntWave].VtxPos[0], 0, sizeof(D3DXVECTOR3) * mylib_const::MAX_VTX);

	int nCntWidth = 0;
	int nCntHeight = 0;
	for (int nCntH = 0; nCntH < nNumVtx; nCntH++)
	{
		for (int nCntW = 0; nCntW < nNumVtx; nCntW++)
		{
			// �����_
			nCntWidth = -(nNumVtx / 2) + nWidthPoint + nCntW;
			nCntHeight = -(nNumVtx / 2) + nHeightPoint + nCntH;
			/*nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);*/

			if (nCntWidth < 0 || m_aInfo.nWidthBlock < nCntWidth)
			{// �͈͊O�Ő擪
				continue;
			}

			if (nCntHeight < 0 || m_aInfo.nHeightBlock < nCntHeight)
			{// �͈͊O�Ő擪
				continue;
			}

			switch (m_aWave[nCntWave].sSetWave.type)
			{
			case WAVETYPE_RIPPLE:	// �g��
				UpdateRippleWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_DIRECT:	// ���s

				if (nCntH == 0 && nCntW == 0 &&
					m_aWave[nCntWave].nMoveLife > 0 &&
					m_aWave[nCntWave].sSetWave.playertype != TYPE_NATURAL)
				{// ���������邤�� && ���R�̔g�ȊO�S��

					// �n�ʉ��Đ�
					if (m_aWave[nCntWave].nMoveLife >= 50 && m_aWave[nCntWave].nMoveLife % 6 == 0)
					{
						CManager::GetSound()->PlaySound(CSound::LABEL_SE_FIELD);
					}

					// �Ռ��g����
					if (m_aWave[nCntWave].nMoveLife % 12 == 0)
					{
						// �o���ʒu
						D3DXVECTOR3 spawnPos = D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * Range.y, m_aWave[nCntWave].fFrameMaxWaveHeight + 15.0f, pos.z + cosf(D3DX_PI + rot.y) * Range.y);

						// �Ռ��g����
						switch (m_aWave[nCntWave].sSetWave.playertype)
						{
						case TYPE_PLAYER:
							CImpactWave::Create
							(
								spawnPos,	// �ʒu
								D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, 0.0f),				// ����
								D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.2f),			// �F
								40.0f,										// ��
								30.0f,										// ����
								36,											// ����
								10.0f,										// ���̈ړ���
								CImpactWave::TYPE_BLACK2,					// �e�N�X�`���^�C�v
								false										// ���Z�������邩
							);
							break;

						case TYPE_ENEMY:
							CImpactWave::Create
							(
								spawnPos,	// �ʒu
								D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, 0.0f),				// ����
								D3DXCOLOR(1.0f, 0.0f, 1.0f, 0.3f),			// �F
								40.0f,										// ��
								30.0f,										// ����
								36,											// ����
								10.0f,										// ���̈ړ���
								CImpactWave::TYPE_PURPLE3,					// �e�N�X�`���^�C�v
								false										// ���Z�������邩
							);
							break;

						}
					}
#if MODE_SCREENSHOT
					// ���I����
					CBallast::Create(D3DXVECTOR3(pos.x + sinf(rot.y - fAngle) * fLength, pos.y + 15.0f, pos.z + cosf(rot.y - fAngle) * fLength), DEFAULTMOVE_BALLAST, 1);
					CBallast::Create(D3DXVECTOR3(pos.x + sinf(rot.y + fAngle) * fLength, pos.y + 15.0f, pos.z + cosf(rot.y + fAngle) * fLength), DEFAULTMOVE_BALLAST, 1);

					// ����ƉE��̈ʒu�ɉ�
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y - 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y - 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);

#else
					// ���I����
					CBallast::Create(D3DXVECTOR3(pos.x + sinf(rot.y - fAngle) * fLength, pos.y + 5.0f, pos.z + cosf(rot.y - fAngle) * fLength), 4);
					CBallast::Create(D3DXVECTOR3(pos.x + sinf(rot.y + fAngle) * fLength, pos.y + 5.0f, pos.z + cosf(rot.y + fAngle) * fLength), 4);

					// ����ƉE��̈ʒu�ɉ�
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y + 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y + 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y + 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);
					my_particle::Create(D3DXVECTOR3(pos.x + Random(-200, 200), pos.y + 5.0f, pos.z + Random(-200, 200)), my_particle::TYPE_WAVESMOKE);
#endif

				}

				// �t���[�����̍ő卂�����Z�b�g
				m_aWave[nCntWave].fFrameMaxWaveHeight = 0;

				// �����g�̍X�V����
				UpdateDirectWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_IMPACT:	// �Ռ��g

				if (nCntH == 0 && nCntW == 0 && m_aWave[nCntWave].nMoveLife > 0)
				{// ���������邤��

					// �n�ʉ��Đ�
					/*if (m_aWave[nCntWave].nMoveLife % 4 == 0)
					{
						CManager::GetSound()->PlaySound(CSound::LABEL_SE_FIELD);
					}*/

					// �L���点��
					m_aWave[nCntWave].sSetWave.fLength += m_aWave[nCntWave].sSetWave.fWave;

					for (int nCnt = 0; nCnt < 16; nCnt++)
					{
						// �Ռ��g�ƓG�Ƃ̌���
						float fRot = (D3DX_PI * 2.0f) / 16.0f;
						fRot *= nCnt;

						D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;

						// �G�̈ʒu�������o��
						D3DXVECTOR3 particlePos;
						particlePos.x = pos.x + sinf(D3DX_PI + fRot) * (m_aWave[nCntWave].sSetWave.fLength + m_aWave[nCntWave].sSetWave.fRatio + m_aWave[nCntWave].sSetWave.fRatio);
						particlePos.z = pos.z + cosf(D3DX_PI + fRot) * (m_aWave[nCntWave].sSetWave.fLength + m_aWave[nCntWave].sSetWave.fRatio + m_aWave[nCntWave].sSetWave.fRatio);

						// ����ƉE��̈ʒu�ɉ�
						if (m_aWave[nCntWave].sSetWave.playertype == TYPE_PLAYER)
						{
							my_particle::Create(D3DXVECTOR3(particlePos.x + Random(-50, 50), pos.y - 5.0f, particlePos.z + Random(-50, 50)), my_particle::TYPE_IMPACTWAVESMOKE);
						}
					}
				}

				UpdateImpactWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_THRUST:	// �˂��グ
				UpdateThrustWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_RAISE:	// ����グ
				UpdateRaiseField(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;
			}
		}
	}

#else


	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock; nCntHeight++)
	{// �c�̕������J��Ԃ�

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock; nCntWidth++)
		{// ���̕������J��Ԃ�

			switch (m_aWave[nCntWave].sSetWave.type)
			{
			case WAVETYPE_RIPPLE:	// �g��
				UpdateRippleWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_DIRECT:	// ���s
				UpdateDirectWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_IMPACT:	// �Ռ��g
				UpdateImpactWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_THRUST:	// �˂��グ
				UpdateThrustWave(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;

			case WAVETYPE_RAISE:	// ����グ
				UpdateRaiseField(nCntWave, nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)));
				break;
			}

		}
	}
#endif
}

//==========================================================================
// �g��X�V����
//==========================================================================
void CElevation::UpdateRippleWave(int nCntWave, int nVtxPoint)
{
	// ���_���W�擾
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// ���_�ƒ��_�̋���
	float fNowLength =
		sqrtf((m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x) * (m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x)
			+ (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z) * (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z));

	// ����
	float Wariai = fNowLength / m_aWave[nCntWave].sSetWave.fRatio;
	if (m_aWave[nCntWave].sSetWave.fRatio == 0.0f)
	{
		return;
	}

	float WaveWariai = fNowLength / m_aWave[nCntWave].sSetWave.fRange.x;

	// �g�̍���
	float fWaveStrength;
	fWaveStrength = -m_aWave[nCntWave].sSetWave.fStrength - (-m_aWave[nCntWave].sSetWave.fStrength * WaveWariai);

	if (WaveWariai >= 1.0f)
	{// �͈͊O�͈ړ��ʃ[��
		fWaveStrength = 0.0f;
	}

	// �g�̌���
	float fRot = D3DX_PI * Wariai;

	// �E�F�[�u������
	m_aWave[nCntWave].fWave[nVtxPoint] -= m_aWave[nCntWave].sSetWave.fWave;

	// �p�x�̐��K��
	RotNormalize(fRot);
	RotNormalize(m_aWave[nCntWave].fWave[nVtxPoint]);

	// �g�̍����ݒ�
	m_aWave[nCntWave].VtxPos[nVtxPoint].y =
		sinf(fRot + m_aWave[nCntWave].fWave[nVtxPoint]) * fWaveStrength;

	// �������Z�b�g
	m_VtxPos[nVtxPoint].y += m_aWave[nCntWave].VtxPos[nVtxPoint].y;
}

//==========================================================================
// ���s�g�X�V����
//==========================================================================
void CElevation::UpdateDirectWave(int nCntWave, int nVtxPoint)
{

	// ���_���W�擾
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// ���_�J���[�擾
	D3DXCOLOR *pVtxCol = GetVtxCol();

	// ���Â炢����v�Z�p�ɑ��
	D3DXVECTOR2 fRange = m_aWave[nCntWave].sSetWave.fRange;
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;

	// �����ɂȂ�2�_
	D3DXVECTOR2 Start = D3DXVECTOR2(pos.x + sinf(-D3DX_PI * 0.5f + rot.y) * fRange.x, pos.z + cosf(-D3DX_PI * 0.5f + rot.y) * fRange.x);
	D3DXVECTOR2 End = D3DXVECTOR2(pos.x + sinf(D3DX_PI * 0.5f + rot.y) * fRange.x, pos.z + cosf(D3DX_PI * 0.5f + rot.y) * fRange.x);

	// ���肷�钸�_
	D3DXVECTOR2 Point = D3DXVECTOR2(pVtxPos[nVtxPoint].x, pVtxPos[nVtxPoint].z);

	// ����
	D3DXVECTOR2 Line = D3DXVECTOR2(End.x - Start.x, End.y - Start.y);

	// ���_�Ɛ����Ƃ̍ŒZ����
	float fNowLength = Line.x * (Start.y - Point.y) - Line.y * (Start.x - Point.x);
	fNowLength = sqrt((fNowLength * fNowLength) / ((Line.x * Line.x) + (Line.y * Line.y)));

	// ����
	float WaveWariai = fNowLength / m_aWave[nCntWave].sSetWave.fRange.y;

	// �g�̍���
	float fWaveStrength;
	fWaveStrength = -m_aWave[nCntWave].sSetWave.fStrength - (-m_aWave[nCntWave].sSetWave.fStrength * WaveWariai);

	// �����̔���
	if (fWaveStrength <= 0.0f)
	{
		fWaveStrength = 0.0f;

		// �f�t�H���g�F�ɖ߂�
		//if (pVtxCol[nVtxPoint] != mylib_const::FIELDCOLOR_01)
		{
			pVtxCol[nVtxPoint] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
		return;
	}

	float fLength = sqrtf(fRange.x * fRange.x + fRange.y * fRange.y);	// �Ίp���̒���
	float fAngle = atan2f(fRange.x, fRange.y);					// �Ίp���̌���

	// �l�p�̔���
	if (CollisionSquare(pos, fRange, rot.y, pVtxPos[nVtxPoint]))
	{
		// �O�͍��
		float fff = -fLength * 0.5f;
		if (CollisionSquare
		(
			D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * fff + sinf(rot.y - fAngle) * fLength, pos.y, pos.z + cosf(D3DX_PI + rot.y) * fff + cosf(rot.y - fAngle) * fLength),	// ����
			D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * fff + sinf(rot.y + fAngle) * fLength, pos.y, pos.z + cosf(D3DX_PI + rot.y) * fff + cosf(rot.y + fAngle) * fLength),		// �E��
			D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * fff + sinf(rot.y - D3DX_PI + fAngle) * fLength, pos.y, pos.z + cosf(D3DX_PI + rot.y) * fff + cosf(rot.y - D3DX_PI + fAngle) * fLength),	// ����
			D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * fff + sinf(rot.y + D3DX_PI - fAngle) * fLength, pos.y, pos.z + cosf(D3DX_PI + rot.y) * fff + cosf(rot.y + D3DX_PI - fAngle) * fLength),		// �E��
			pVtxPos[nVtxPoint]											// ���肷��ʒu
		))
		{

		}
		else
		{
			fWaveStrength = 0.0f;

			// �f�t�H���g�F�ɖ߂�
			//if (pVtxCol[nVtxPoint] != mylib_const::FIELDCOLOR_01)
			{
				pVtxCol[nVtxPoint] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}
			return;
		}

	}
	else
	{
		// ������0
		fWaveStrength = 0.0f;

		// �f�t�H���g�F�ɖ߂�
		//if (pVtxCol[nVtxPoint] != mylib_const::FIELDCOLOR_01)
		{
			pVtxCol[nVtxPoint] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
		return;
	}

	// �g�̍����ݒ�
	m_aWave[nCntWave].VtxPos[nVtxPoint].y = fWaveStrength;
	
	// �ő�l����ւ�
	if (m_aWave[nCntWave].fMaxWaveHeight < m_aWave[nCntWave].VtxPos[nVtxPoint].y)
	{// ����̕�������������
		m_aWave[nCntWave].fMaxWaveHeight = m_aWave[nCntWave].VtxPos[nVtxPoint].y + m_aInfo.pVtxPos[nVtxPoint].y;
	}

	if (m_aWave[nCntWave].fFrameMaxWaveHeight < m_aWave[nCntWave].VtxPos[nVtxPoint].y)
	{
		m_aWave[nCntWave].fFrameMaxWaveHeight = m_aWave[nCntWave].VtxPos[nVtxPoint].y + m_aInfo.pVtxPos[nVtxPoint].y;
	}

	// �n�ʐF�ɐݒ�
	pVtxCol[nVtxPoint] = mylib_const::FIELDCOLOR_01;

	// �������Z�b�g
	m_VtxPos[nVtxPoint].y += m_aWave[nCntWave].VtxPos[nVtxPoint].y;
}

//==========================================================================
// �Ռ��g�X�V����
//==========================================================================
void CElevation::UpdateImpactWave(int nCntWave, int nVtxPoint)
{
	// ���_���W�擾
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// ���_�J���[�擾
	D3DXCOLOR *pVtxCol = GetVtxCol();

	// ���Â炢����v�Z�p�ɑ��
	D3DXVECTOR2 fRange = m_aWave[nCntWave].sSetWave.fRange;
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;
	float fRatio = m_aWave[nCntWave].sSetWave.fRatio;
	float fLength = m_aWave[nCntWave].sSetWave.fLength;

	// ���_�ƒ��_�̋���
	float fNowLength =
		sqrtf((pos.x - pVtxPos[nVtxPoint].x) * (pos.x - pVtxPos[nVtxPoint].x)
			+ (pos.z - pVtxPos[nVtxPoint].z) * (pos.z - pVtxPos[nVtxPoint].z));

	// ���_�ƍő�͈͂܂ł̋���
	float fMaxLength = fLength + fRatio;

	// ���_�ƍŏ��͈͂܂ł̋���
	float fMinLength = fLength - fRatio;

	// �͈͂̒���
	float fRangeLength = fMaxLength - fMinLength;

	// ���莞�̒���
	float fNowCollisionLen = fNowLength - fLength;

	// ����
	float Wariai = fNowLength / fRatio;
	float WaveWariai = fNowCollisionLen / fRangeLength;

	if (WaveWariai < 0.0f)
	{// �͈͊O�͋t�]
		WaveWariai *= -1;
	}

	// �g�̍���
	float fWaveStrength;
	fWaveStrength = -m_aWave[nCntWave].sSetWave.fStrength - (-m_aWave[nCntWave].sSetWave.fStrength * WaveWariai);

	if (fNowLength > fMaxLength || fNowLength < fMinLength)
	{// �͈͊O�͈ړ��ʃ[��
		fWaveStrength = 0.0f;

		// �f�t�H���g�F�ɖ߂�
		//if (pVtxCol[nVtxPoint] != mylib_const::FIELDCOLOR_01)
		{
			pVtxCol[nVtxPoint] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
		return;
	}

	// �g�̍����ݒ�
	m_aWave[nCntWave].VtxPos[nVtxPoint].y = fWaveStrength;

	// �ő�l����ւ�
	if (m_aWave[nCntWave].fMaxWaveHeight < m_aWave[nCntWave].VtxPos[nVtxPoint].y)
	{// ����̕�������������
		m_aWave[nCntWave].fMaxWaveHeight = m_aWave[nCntWave].VtxPos[nVtxPoint].y + m_aInfo.pVtxPos[nVtxPoint].y;
	}

	// �n�ʐF�ɐݒ�
	pVtxCol[nVtxPoint] = mylib_const::FIELDCOLOR_01;

	// �������Z�b�g
	m_VtxPos[nVtxPoint].y += m_aWave[nCntWave].VtxPos[nVtxPoint].y;
}

//==========================================================================
// �˂��グ�X�V����
//==========================================================================
void CElevation::UpdateThrustWave(int nCntWave, int nVtxPoint)
{
	// ���_���W�擾
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// ���_�ƒ��_�̋���
	float fNowLength =
		sqrtf((m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x) * (m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x)
			+ (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z) * (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z));

	// ����
	float Wariai = fNowLength / m_aWave[nCntWave].sSetWave.fRatio;
	float WaveWariai = fNowLength / m_aWave[nCntWave].sSetWave.fRange.x;

	// �g�̍���
	float fWaveStrength;
	fWaveStrength = m_aWave[nCntWave].sSetWave.fStrength;

	if (WaveWariai >= 1.0f)
	{// �͈͊O�͈ړ��ʃ[��
		fWaveStrength = 0.0f;
	}

	// �g�̌���
	float fRot = D3DX_PI * Wariai;

	// �E�F�[�u������
	m_aWave[nCntWave].fWave[nVtxPoint] += (fWaveStrength - m_aWave[nCntWave].fWave[nVtxPoint]) * 0.2f;

	// �p�x�̐��K��
	RotNormalize(fRot);
	RotNormalize(m_aWave[nCntWave].fWave[nVtxPoint]);

	// �g�̍����ݒ�
	m_aWave[nCntWave].VtxPos[nVtxPoint].y = m_aWave[nCntWave].fWave[nVtxPoint];

	// �������Z�b�g
	m_VtxPos[nVtxPoint].y += m_aWave[nCntWave].VtxPos[nVtxPoint].y;
}

//==========================================================================
// ����グ�X�V����
//==========================================================================
void CElevation::UpdateRaiseField(int nCntWave, int nVtxPoint)
{
	// ���_���W�擾
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// ���_�J���[�擾
	D3DXCOLOR *pVtxCol = GetVtxCol();

	// ���_�ƒ��_�̋���
	float fNowLength =
		sqrtf((m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x) * (m_aWave[nCntWave].sSetWave.pos.x - pVtxPos[nVtxPoint].x)
			+ (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z) * (m_aWave[nCntWave].sSetWave.pos.z - pVtxPos[nVtxPoint].z));

	// ����
	float Wariai = fNowLength / m_aWave[nCntWave].sSetWave.fRatio;
	float WaveWariai = fNowLength / m_aWave[nCntWave].sSetWave.fRange.x;

	// �g�̍���
	float fWaveStrength = m_aWave[nCntWave].sSetWave.fStrength;

	if (WaveWariai >= 1.0f)
	{// ������1.0f�ȉ�����
		m_aWave[nCntWave].VtxPos[nVtxPoint].y = 0.0f;

		// �f�t�H���g�F�ɖ߂�
		if (pVtxCol[nVtxPoint] != mylib_const::FIELDCOLOR_01)
		{
			pVtxCol[nVtxPoint] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
		return;
	}

	// ���������߂�
	fWaveStrength = fWaveStrength - (fWaveStrength * WaveWariai);

	// �g�̍����ݒ�
	m_aWave[nCntWave].VtxPos[nVtxPoint].y = fWaveStrength;

	// �n�ʐF�ɐݒ�
	pVtxCol[nVtxPoint] = mylib_const::FIELDCOLOR_01;

	// �������Z�b�g
	m_VtxPos[nVtxPoint].y += m_aWave[nCntWave].VtxPos[nVtxPoint].y;
}

//==========================================================================
// �����蔻�菈��
//==========================================================================
void CElevation::Collision(int nCntWave)
{
	// ���Â炢����ꎞ�ۑ�
	D3DXVECTOR2 fRange = m_aWave[nCntWave].sSetWave.fRange;
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 move = m_aWave[nCntWave].sSetWave.move;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;

	// �擪��ۑ�
	CObject *pObj = CObject::GetTop(mylib_const::ENEMY_PRIORITY);

	if (m_aWave[nCntWave].sSetWave.playertype != TYPE_NATURAL)
	{// ���R�̔g�ȊO�S��

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
			switch (m_aWave[nCntWave].sSetWave.playertype)
			{
			case TYPE_PLAYER:

				if (pObj->GetType() != CObject::TYPE_ENEMY)
				{// �G����Ȃ�������

					if (pObj->GetType() != CObject::TYPE_BOSS)
					{// �{�X����Ȃ�������

						// ���̃I�u�W�F�N�g����
						pObj = pObjNext;
						continue;
					}
				}
				break;

			case TYPE_ENEMY:

				if (pObj->GetType() != CObject::TYPE_PLAYER)
				{// �v���C���[����Ȃ�������

					// ���̃I�u�W�F�N�g����
					pObj = pObjNext;
					continue;
				}
				break;

			case TYPE_NATURAL:

				return;
				break;

			case TYPE_EFFECT:
				return;
				break;

			case TYPE_SPAWN:
				return;
				break;
			}

			// �G�̈ʒu�擾
			D3DXVECTOR3 TargetPos = pObj->GetPosition();
			D3DXVECTOR3 TargetPosOld = pObj->GetOldPosition();

			// ����T�C�Y�擾
			float fRadius = pObj->GetObjectChara()->GetRadius();

			// �g�ɓ������ĂȂ���Ԃɐݒ肷��
			pObj->GetObjectChara()->SetDisableWaveHit();

			switch (m_aWave[nCntWave].sSetWave.type)
			{
			case WAVETYPE_RIPPLE:	// �g��
				break;

			case WAVETYPE_DIRECT:	// ���s
				CollisionDirectWave(nCntWave, pObj);
				break;

			case WAVETYPE_IMPACT:	// �Ռ��g
				CollisionImpactWave(nCntWave, pObj);
				break;
			}

			// ���̃I�u�W�F�N�g����
			pObj = pObjNext;
		}

		if (m_aWave[nCntWave].sSetWave.type == TYPE_EFFECT)
		{
			int n = 0;
			return;
		}

		// �I�������ɔg���m�̊m�F
		switch (m_aWave[nCntWave].sSetWave.type)
		{
		case WAVETYPE_RIPPLE:	// �g��
			break;

		case WAVETYPE_DIRECT:	// ���s
			for (int nCnt = 0; nCnt < mylib_const::MAX_WAVE; nCnt++)
			{
				if (m_aWave[nCnt].state != STATE_WAVE)
				{// �g���Ă��Ȃ�������
					continue;
				}

				if (m_aWave[nCnt].sSetWave.playertype == TYPE_EFFECT ||
					m_aWave[nCnt].sSetWave.playertype == TYPE_SPAWN)
				{// ����Ȃ��g�̏ꍇ
					continue;
				}

				if (nCnt == nCntWave)
				{// �����Ɠ�����������
					continue;
				}

				// �G�̏��擾
				D3DXVECTOR3 TargetPos = m_aWave[nCnt].sSetWave.pos;
				D3DXVECTOR3 TargetPosOld = m_aWave[nCnt].sSetWave.pos - m_aWave[nCnt].sSetWave.move;
				D3DXVECTOR3 TargetRot = m_aWave[nCnt].sSetWave.rot;
				D3DXVECTOR2 TargetSize = m_aWave[nCnt].sSetWave.fRange;

				// �Ίp���̌���,����
				float fLength = sqrtf(fRange.x * fRange.x + fRange.y * fRange.y);
				float fAngle = atan2f(fRange.x, fRange.y);

				// ���肷��l�p
				D3DXVECTOR3 LeftUp = D3DXVECTOR3(pos.x + sinf(rot.y - fAngle) * fLength, pos.y, pos.z + cosf(rot.y - fAngle) * fLength);
				D3DXVECTOR3 RightUp = D3DXVECTOR3(pos.x + sinf(rot.y + fAngle) * fLength, pos.y, pos.z + cosf(rot.y + fAngle) * fLength);
				D3DXVECTOR3 LeftDown = D3DXVECTOR3(pos.x + sinf(rot.y - D3DX_PI + fAngle) * fLength, pos.y, pos.z + cosf(rot.y - D3DX_PI + fAngle) * fLength);
				D3DXVECTOR3 RightDown = D3DXVECTOR3(pos.x + sinf(rot.y + D3DX_PI - fAngle) * fLength, pos.y, pos.z + cosf(rot.y + D3DX_PI - fAngle) * fLength);

				// ���̒��_���W
				D3DXVECTOR3 pos0 = LeftDown;

				// �E�̒��_���W
				D3DXVECTOR3 pos1 = RightDown;

				// ���E���̃x�N�g��
				D3DXVECTOR3 vecLine;
				vecLine.x = pos1.x - pos0.x;
				vecLine.z = pos1.z - pos0.z;

				// �v���C���[�̋��E���̃x�N�g��
				D3DXVECTOR3 vecLinePlayer;
				vecLinePlayer.x = TargetPos.x - TargetPosOld.x;
				vecLinePlayer.z = TargetPos.z - TargetPosOld.z;

				// �v���C���[�ƕǂ̃x�N�g��
				D3DXVECTOR3 vecToPosPlayer;
				vecToPosPlayer.x = pos1.x - TargetPosOld.x;
				vecToPosPlayer.z = pos1.z - TargetPosOld.z;

				// �����ۑ�
				float fStr = m_aWave[nCnt].sSetWave.fStrength;
				if (fStr > 0)
				{// �������t��������t�ɂ���
					fStr *= -1;
				}

				bool bHit = false;	// �g���m�̔���
				switch (m_aWave[nCnt].sSetWave.type)
				{
				case WAVETYPE_DIRECT:	// ����
					// ��`�Ƌ�`�̔���
					bHit = CollisionSquareSquare2D(pos, TargetPos, fRange, TargetSize, rot.y, TargetRot.y);
					break;

				case WAVETYPE_IMPACT:	// �~
					// �~�Ƌ�`�̔���
					bHit = CollisionCircleSquare2D(TargetPos, pos, rot, m_aWave[nCnt].sSetWave.fLength, fRange);
					break;

				case WAVETYPE_RAISE:	// ����グ
					bHit = CollisionCircleSquare2D(TargetPos, pos, rot, m_aWave[nCnt].sSetWave.fRange.x, fRange);
					break;
				}

				if (bHit == false || m_aWave[nCntWave].sSetWave.fStrength >= fStr)
				{// �������ĂȂ� or �����̍����������Ă�
					continue;
				}

				// ������������̎��
				if (m_aWave[nCntWave].sSetWave.playertype != m_aWave[nCnt].sSetWave.playertype &&
					(m_aWave[nCnt].nMoveLife > 0 || m_aWave[nCnt].sSetWave.type == WAVETYPE_RAISE))
				{// �����Ɠ��������Ȃ������� && �ړ���

					if (m_aWave[nCntWave].sSetWave.playertype == TYPE_ENEMY && m_aWave[nCnt].sSetWave.playertype == TYPE_NATURAL)
					{//	�G�Ǝ��R���͂͂���
						continue;
					}

					switch (m_aWave[nCnt].sSetWave.type)
					{
					case WAVETYPE_DIRECT:	// ����

						// �U��
						CManager::GetCamera()->SetShake(SHAKETIME_DIRECT, IMPACTSHAKE_DIRECT, 0.0f);

						// �q�b�g�X�g�b�v�L��
						if (m_aWave[nCntWave].sSetWave.playertype == TYPE_PLAYER && m_aWave[nCnt].sSetWave.playertype != TYPE_PLAYER)
						{
							m_bHitStop_Direct = true;
						}

						// �Ռ��g����
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
							D3DXVECTOR3(D3DX_PI * 0.75f, 0.0f, 0.0f),	// ����
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// �F
							140.0f,										// ��
							1.0f,										// ����
							30,											// ����
							30.0f,										// ���̈ړ���
							CImpactWave::TYPE_SMOKE,					// �e�N�X�`���^�C�v
							true										// ���Z�������邩
						);

						// �Ռ��g����
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
							D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// ����
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// �F
							140.0f,										// ��
							1.0f,										// ����
							30,											// ����
							30.0f,										// ���̈ړ���
							CImpactWave::TYPE_SMOKE,					// �e�N�X�`���^�C�v
							true										// ���Z�������邩
						);

						// �Ռ��g����
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
							D3DXVECTOR3(D3DX_PI * -0.75f, 0.0f, 0.0f),	// ����
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// �F
							140.0f,										// ��
							1.0f,										// ����
							30,											// ����
							30.0f,										// ���̈ړ���
							CImpactWave::TYPE_SMOKE,					// �e�N�X�`���^�C�v
							true										// ���Z�������邩
						);
						break;

					case WAVETYPE_IMPACT:	// �~
						m_aWave[nCntWave].sSetWave.move.x *= 0.6f;
						m_aWave[nCntWave].sSetWave.move.z *= 0.6f;

						// �U��
						CManager::GetCamera()->SetShake(SHAKETIME_IMPACT, IMPACTSHAKE_IMPACT, 0.0f);

						// �q�b�g�X�g�b�v�L��
						if (m_aWave[nCntWave].sSetWave.playertype == TYPE_PLAYER && m_aWave[nCnt].sSetWave.playertype != TYPE_PLAYER)
						{
							m_bHitStop_Impact = true;
						}
						//CManager::SetEnableHitStop();

						// �Ռ��g����
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
							D3DXVECTOR3(D3DX_PI * 0.75f, 0.0f, 0.0f),	// ����
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// �F
							140.0f,										// ��
							1.0f,										// ����
							30,											// ����
							30.0f,										// ���̈ړ���
							CImpactWave::TYPE_SMOKE,					// �e�N�X�`���^�C�v
							true										// ���Z�������邩
						);

						// �Ռ��g����
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
							D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// ����
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// �F
							140.0f,										// ��
							1.0f,										// ����
							30,											// ����
							30.0f,										// ���̈ړ���
							CImpactWave::TYPE_SMOKE,					// �e�N�X�`���^�C�v
							true										// ���Z�������邩
						);

						// �Ռ��g����
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
							D3DXVECTOR3(D3DX_PI * -0.75f, 0.0f, 0.0f),	// ����
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// �F
							140.0f,										// ��
							1.0f,										// ����
							30,											// ����
							30.0f,										// ���̈ړ���
							CImpactWave::TYPE_SMOKE,					// �e�N�X�`���^�C�v
							true										// ���Z�������邩
						);
						break;

					case WAVETYPE_RAISE:	// ����グ

						// �U��
						CManager::GetCamera()->SetShake(SHAKETIME_IMPACT, IMPACTSHAKE_IMPACT, 0.0f);

						// �q�b�g�X�g�b�v�L��
						m_bHitStop_Impact = true;
						//CManager::SetEnableHitStop();

						// �Ռ��g����
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
							D3DXVECTOR3(D3DX_PI * 0.75f, 0.0f, 0.0f),	// ����
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// �F
							140.0f,										// ��
							1.0f,										// ����
							30,											// ����
							30.0f,										// ���̈ړ���
							CImpactWave::TYPE_SMOKE,					// �e�N�X�`���^�C�v
							true										// ���Z�������邩
						);

						// �Ռ��g����
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
							D3DXVECTOR3(0.0f, 0.0f, 0.0f),	// ����
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// �F
							140.0f,										// ��
							1.0f,										// ����
							30,											// ����
							30.0f,										// ���̈ړ���
							CImpactWave::TYPE_SMOKE,					// �e�N�X�`���^�C�v
							true										// ���Z�������邩
						);

						// �Ռ��g����
						CImpactWave::Create
						(
							D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
							D3DXVECTOR3(D3DX_PI * -0.75f, 0.0f, 0.0f),	// ����
							D3DXCOLOR(0.7f, 0.7f, 1.0f, 0.4f),			// �F
							140.0f,										// ��
							1.0f,										// ����
							30,											// ����
							30.0f,										// ���̈ړ���
							CImpactWave::TYPE_SMOKE,					// �e�N�X�`���^�C�v
							true										// ���Z�������邩
						);
						break;
					}


					switch (m_aWave[nCnt].sSetWave.playertype)
					{// ������������̃v���C���[���
					case TYPE_PLAYER:
						// ����������𕽒R�ɂ���
						memset(&m_aWave[nCnt].VtxPos[0], 0, sizeof(D3DXVECTOR3) * mylib_const::MAX_VTX);
						m_aWave[nCnt].state = STATE_NONE;
						break;

					case TYPE_ENEMY:
						// ����������𕽒R�ɂ���
						memset(&m_aWave[nCnt].VtxPos[0], 0, sizeof(D3DXVECTOR3) * mylib_const::MAX_VTX);
						m_aWave[nCnt].state = STATE_NONE;
						break;

					case TYPE_NATURAL:
						if (m_aWave[nCntWave].sSetWave.playertype != TYPE_PLAYER)
						{//	�v���C���[�������g����
							continue;
						}

						// ����������𕽒R�ɂ���
						memset(&m_aWave[nCnt].VtxPos[0], 0, sizeof(D3DXVECTOR3) * mylib_const::MAX_VTX);
						m_aWave[nCnt].state = STATE_NONE;

						// �����ɓ���������̗v�f������
						m_aWave[nCntWave].sSetWave.move += m_aWave[nCnt].sSetWave.move;
						m_aWave[nCntWave].sSetWave.fRange.x = m_aWave[nCnt].sSetWave.fRange.x;

						if (m_aWave[nCnt].sSetWave.fStrengthOrigin > 0)
						{// �������t��������t�ɂ���
							m_aWave[nCnt].sSetWave.fStrengthOrigin *= -1;
						}

						// ���̍����ɓ���������̍����𑫂�
						m_aWave[nCntWave].sSetWave.fStrengthOrigin += m_aWave[nCnt].sSetWave.fStrengthOrigin;

						// ���̎����ɖ߂�
						m_aWave[nCntWave].nCntState = m_aWave[nCntWave].nOriginCntState;
						m_aWave[nCntWave].nMoveLife = m_aWave[nCntWave].nOriginMoveLife;
						break;
					}
				}
			}
			break;

		case WAVETYPE_IMPACT:
			break;

		}
	}
	else if (m_aWave[nCntWave].sSetWave.playertype == TYPE_NATURAL)
	{// ���R�̔g

	}
}

//==========================================================================
// �����g�̓����蔻�菈��
//==========================================================================
void CElevation::CollisionDirectWave(int nCntWave, CObject *pObj)
{
	// ���Â炢����ꎞ�ۑ�
	D3DXVECTOR2 fRange = m_aWave[nCntWave].sSetWave.fRange;
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 move = m_aWave[nCntWave].sSetWave.move;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;

	// �G�̈ʒu�擾
	D3DXVECTOR3 TargetPos = pObj->GetPosition();
	D3DXVECTOR3 TargetPosOld = pObj->GetOldPosition();
	D3DXVECTOR3 TargetRot = pObj->GetRotation();

	// ����T�C�Y�擾
	float fRadius = pObj->GetObjectChara()->GetRadius();

	if (TargetPos.y > m_aWave[nCntWave].fMaxWaveHeight)
	{// �g������ɋ�������
		return;
	}

	float fLength = sqrtf(fRange.x * fRange.x + fRange.y * fRange.y);	// �Ίp���̒���
	float fAngle = atan2f(fRange.x, fRange.y);							// �Ίp���̌���

	// ���肷��l�p
	D3DXVECTOR3 LeftUp = D3DXVECTOR3(pos.x + sinf(rot.y - fAngle) * fLength, pos.y, pos.z + cosf(rot.y - fAngle) * fLength);
	D3DXVECTOR3 RightUp = D3DXVECTOR3(pos.x + sinf(rot.y + fAngle) * fLength, pos.y, pos.z + cosf(rot.y + fAngle) * fLength);
	D3DXVECTOR3 LeftDown = D3DXVECTOR3(pos.x + sinf(rot.y - D3DX_PI + fAngle) * fLength, pos.y, pos.z + cosf(rot.y - D3DX_PI + fAngle) * fLength);
	D3DXVECTOR3 RightDown = D3DXVECTOR3(pos.x + sinf(rot.y + D3DX_PI - fAngle) * fLength, pos.y, pos.z + cosf(rot.y + D3DX_PI - fAngle) * fLength);

	// ���̒��_���W
	D3DXVECTOR3 pos0 = LeftDown;

	// �E�̒��_���W
	D3DXVECTOR3 pos1 = RightDown;

	// ���E���̃x�N�g��
	D3DXVECTOR3 vecLine;
	vecLine.x = pos1.x - pos0.x;
	vecLine.z = pos1.z - pos0.z;

	// �v���C���[�̋��E���̃x�N�g��
	D3DXVECTOR3 vecLinePlayer;
	vecLinePlayer.x = TargetPos.x - TargetPosOld.x;
	vecLinePlayer.z = TargetPos.z - TargetPosOld.z;

	// �v���C���[�ƕǂ̃x�N�g��
	D3DXVECTOR3 vecToPosPlayer;
	vecToPosPlayer.x = pos1.x - TargetPosOld.x;
	vecToPosPlayer.z = pos1.z - TargetPosOld.z;

	if (CollisionSquare(pos, fRange, rot.y, TargetPos) == false)
	{// �������ĂȂ��ꍇ������
		return;
	}

	// �g�ɓ������Ă����Ԃɐݒ肷��
	pObj->GetObjectChara()->SetEnableWaveHit();

	if (pObj->GetType() == TYPE_BOSS && m_aWave[nCntWave].nCntState > 60)
	{
		m_aWave[nCntWave].nCntState = 60;
	}

	if (pObj->GetType() == TYPE_BOSS && pObj->Hit(0) == true)
	{// �{�X && �m�b�N�o�b�N��
		return;
	}

	if (pObj->GetType() == TYPE_BOSS && (pObj->GetObjectChara()->GetState() == CBoss::STATE_ATTACK || pObj->GetObjectChara()->GetState() == CBoss::STATE_KNOCKBACK))
	{
		return;
	}

	if (pObj->GetType() == CObject::TYPE_ENEMY && pObj->GetObjectChara()->GetState() == CEnemy::STATE_BURY)
	{// �G�����܂��Ă���Ƃ�
		return;
	}

	if (pObj->GetType() == CObject::TYPE_ENEMY && pObj->GetObjectChara()->GetState() == CEnemy::STATE_KNOCKBACK)
	{// �G�����܂��Ă���Ƃ�
		return;
	}

	// �q�b�g����
	switch (m_aWave[nCntWave].sSetWave.playertype)
	{
	case TYPE_PLAYER:

		if (pObj->GetObjectChara()->GetState() != CEnemy::STATE_DMG &&
			pObj->GetObjectChara()->GetState() != CEnemy::STATE_KNOCKBACK &&
			pObj->GetObjectChara()->GetState() != CEnemy::STATE_DEAD &&
			pObj->GetObjectChara()->GetState() != CEnemy::STATE_SPAWN &&
			pObj->GetObjectChara()->GetState() != CEnemy::STATE_FADEOUT)
		{
			// �Ռ��g�ƓG�Ƃ̌���
			float fRot = atan2f((pos.x - TargetPos.x), (pos.z - TargetPos.z));

			m_aWave[nCntWave].sSetWave.fWave;
			pObj->SetMove(D3DXVECTOR3(
				sinf(D3DX_PI + fRot) * 20.0f,
				15.0f,
				cosf(D3DX_PI + fRot) * 20.0f));

			// �����𐳖ʂɂ���
			pObj->SetRotation(D3DXVECTOR3(TargetRot.x, D3DX_PI + rot.y, TargetRot.z));
		}

		if (pObj->Hit(mylib_const::DIRECTWAVE_DMG))
		{
			if (CManager::GetMode() == CScene::MODE_TUTORIAL)
			{// �`���[�g���A����������
				CTutorial::GetStep()->SetStep(CTutorialStep::STEP_DIRECTWAVE);
			}
			break;
		}
		break;

	case TYPE_ENEMY:

		if (pObj->GetObjectChara()->GetState() != CPlayer::STATE_KNOCKBACK &&
			pObj->GetObjectChara()->GetState() != CPlayer::STATE_DEAD &&
			pObj->GetObjectChara()->GetState() != CPlayer::STATE_INVINCIBLE)
		{
			// �ړ��̃x�N�g�������߂�
			float fMove = sqrtf(move.x * move.x + move.z * move.z);
			fMove *= 0.6f;

			pObj->SetMove(D3DXVECTOR3(
				sinf(D3DX_PI + rot.y) * fMove,
				15.0f,
				cosf(D3DX_PI + rot.y) * fMove));

			// �����𐳖ʂɂ���
			pObj->SetRotation(D3DXVECTOR3(TargetRot.x, D3DX_PI + rot.y, TargetRot.z));
		}

		if (pObj->Hit(mylib_const::DIRECTWAVE_DMG))
		{
			//my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
		}
		break;
	}

	// �ʐς̍ő�l
	float fMaxAreaPlayer = (vecLinePlayer.z * vecLine.x) - (vecLinePlayer.x * vecLine.z);
	if (fMaxAreaPlayer == 0.0f)
	{// �����ĂȂ���͔�����
		return;
	}

	switch (m_aWave[nCntWave].sSetWave.playertype)
	{
	case TYPE_PLAYER:
		if (pObj->GetObjectChara()->GetState() == CEnemy::STATE_DMG ||
			pObj->GetObjectChara()->GetState() == CEnemy::STATE_KNOCKBACK ||
			pObj->GetObjectChara()->GetState() == CEnemy::STATE_DEAD ||
			pObj->GetObjectChara()->GetState() != CEnemy::STATE_SPAWN ||
			pObj->GetObjectChara()->GetState() == CEnemy::STATE_FADEOUT)
		{
			int n = 0;
			return;
		}
		break;

	case TYPE_ENEMY:
		if (pObj->GetObjectChara()->GetState() == CPlayer::STATE_DMG ||
			pObj->GetObjectChara()->GetState() == CPlayer::STATE_KNOCKBACK ||
			pObj->GetObjectChara()->GetState() == CPlayer::STATE_DEAD ||
			pObj->GetObjectChara()->GetState() == CPlayer::STATE_INVINCIBLE)
		{
			return;
		}
		break;
	}

	// ����̖ʐ�
	float fNowAreaPlayer = (vecToPosPlayer.z * vecLine.x) - (vecToPosPlayer.x * vecLine.z);

	// ����
	float fRatePlayer = fNowAreaPlayer / fMaxAreaPlayer;

	// ��_����͂ݏo����
	D3DXVECTOR3 CollisionPointPlayer = D3DXVECTOR3(0.0f, TargetPos.y, 0.0f);
	CollisionPointPlayer.x = (TargetPos.x + (vecLinePlayer.x * (fRatePlayer - 1.0f)));
	CollisionPointPlayer.z = (TargetPos.z + (vecLinePlayer.z * (fRatePlayer - 1.0f)));

	// �@���x�N�g��(���E���x�N�g����X��Z���])
	D3DXVECTOR3 vecNor = D3DXVECTOR3(vecLine.z, 0.0f, -vecLine.x);

	// �x�N�g���̐��K��
	D3DXVec3Normalize(&vecNor, &vecNor);

	// �v���C���[�̋t�ړ���
	D3DXVECTOR3 PlayerInverceMove;
	PlayerInverceMove.z = vecLinePlayer.z * (fRatePlayer - 1.0f);
	PlayerInverceMove.x = vecLinePlayer.x * (fRatePlayer - 1.0f);

	// ����(�ǂ̖@���ƃv���C���[�̋t�ړ���)
	float fDot = (PlayerInverceMove.x * vecNor.x) + (PlayerInverceMove.z * vecNor.z);

	// �ǂ���ړ���
	D3DXVECTOR3 MoveWall = vecNor * fDot;

	// �Ԃ������_�ɕ␳
	TargetPos += MoveWall + (vecNor * 0.1f);

	// �ʒu�ݒ�
	pObj->SetPosition(TargetPos);

	if (pObj->GetType() != TYPE_BOSS && pObj->GetObjectChara()->GetState() != CEnemy::STATE_DEAD)
	{// �{�X && �m�b�N�o�b�N��
		pObj->SetMove(D3DXVECTOR3(0.0f, pObj->GetMove().y, 0.0f));
	}
}

//==========================================================================
// �~�`�g�̓����蔻�菈��
//==========================================================================
void CElevation::CollisionImpactWave(int nCntWave, CObject *pObj)
{
	// ���Â炢����ꎞ�ۑ�
	D3DXVECTOR2 fRange = m_aWave[nCntWave].sSetWave.fRange;
	D3DXVECTOR3 pos = m_aWave[nCntWave].sSetWave.pos;
	D3DXVECTOR3 move = m_aWave[nCntWave].sSetWave.move;
	D3DXVECTOR3 rot = m_aWave[nCntWave].sSetWave.rot;

	// �G�̈ʒu�擾
	D3DXVECTOR3 TargetPos = pObj->GetPosition();
	D3DXVECTOR3 TargetPosOld = pObj->GetOldPosition();
	D3DXVECTOR3 TargetRot = pObj->GetRotation();

	// ����T�C�Y�擾
	float fRadius = pObj->GetObjectChara()->GetRadius();

	if (TargetPos.y > m_aWave[nCntWave].fMaxWaveHeight)
	{// �g������ɋ�������
		return;
	}

	// �ő�~�̔���
	bool bHit = CircleRange(pos, TargetPos, m_aWave[nCntWave].sSetWave.fLength - m_aWave[nCntWave].sSetWave.fRatio * 2.0f, fRadius);

	if (bHit == false)
	{// �������Ă��Ȃ�������
		return;
	}

	if (pObj->GetType() == TYPE_BOSS && pObj->GetObjectChara()->GetState() == CEnemy::STATE_KNOCKBACK)
	{// �m�b�N�o�b�N����������
		return;
	}

	if (pObj->GetType() == TYPE_BOSS && pObj->GetObjectChara()->GetState() == CBoss::STATE_ATTACK)
	{
		return;
	}

	// �g�ɓ������Ă����Ԃɐݒ肷��
	pObj->GetObjectChara()->SetEnableWaveHit();

	// �G�Ƃ̋���
	float fLength =
		sqrtf((pos.x - TargetPos.x) * (pos.x - TargetPos.x)
			+ (pos.z - TargetPos.z) * (pos.z - TargetPos.z));

	// �Ռ��g�ƓG�Ƃ̌���
	float fRot = atan2f((pos.x - TargetPos.x), (pos.z - TargetPos.z));

	// �G�̈ʒu�������o��
	TargetPos.x = pos.x + sinf(D3DX_PI + fRot) * (m_aWave[nCntWave].sSetWave.fLength + m_aWave[nCntWave].sSetWave.fRatio + m_aWave[nCntWave].sSetWave.fRatio);
	TargetPos.z = pos.z + cosf(D3DX_PI + fRot) * (m_aWave[nCntWave].sSetWave.fLength + m_aWave[nCntWave].sSetWave.fRatio + m_aWave[nCntWave].sSetWave.fRatio);

	if (pObj->GetType() == CObject::TYPE_ENEMY && pObj->GetObjectChara()->GetState() == CEnemy::STATE_BURY)
	{// �G�����܂��Ă���Ƃ�

		// �ʒu�ݒ�
		pObj->SetPosition(D3DXVECTOR3(TargetPos.x, TargetPos.y + 10.0f, TargetPos.z));

		pObj->SetMove(D3DXVECTOR3(
			sinf(D3DX_PI + fRot) * 15.0f,
			20.0f,
			cosf(D3DX_PI + fRot) * 15.0f));

		D3DXVECTOR3 MoveRot = pObj->GetRotation();
		pObj->SetRotation(D3DXVECTOR3(MoveRot.x, fRot, MoveRot.z));
	}
	else
	{
		switch (m_aWave[nCntWave].sSetWave.playertype)
		{
		case TYPE_PLAYER:
			if (pObj->GetObjectChara()->GetState() != CEnemy::STATE_DMG &&
				pObj->GetObjectChara()->GetState() != CEnemy::STATE_KNOCKBACK &&
				pObj->GetObjectChara()->GetState() != CEnemy::STATE_DEAD &&
				pObj->GetObjectChara()->GetState() != CEnemy::STATE_SPAWN &&
				pObj->GetObjectChara()->GetState() != CEnemy::STATE_FADEOUT)
			{
				// �ʒu�ݒ�
				pObj->SetPosition(D3DXVECTOR3(TargetPos.x, TargetPos.y + 5.0f, TargetPos.z));

				// �����𐳖ʂɂ���
				pObj->SetRotation(D3DXVECTOR3(TargetRot.x, D3DX_PI + fRot, TargetRot.z));

				// �m�b�N�o�b�N�ړ���
				pObj->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * 10.0f,
					10.0f,
					cosf(D3DX_PI + fRot) * 10.0f));

				if (pObj->Hit(mylib_const::IMPACTWAVE_DMG))
				{
					if (CManager::GetMode() == CScene::MODE_TUTORIAL)
					{// �`���[�g���A����������
						CTutorial::GetStep()->SetStep(CTutorialStep::STEP_IMPACTWAVE);
					}
					return;
				}
			}
			break;

		case TYPE_ENEMY:
			if (pObj->GetObjectChara()->GetState() != CPlayer::STATE_DMG &&
				pObj->GetObjectChara()->GetState() != CPlayer::STATE_KNOCKBACK &&
				pObj->GetObjectChara()->GetState() != CPlayer::STATE_DEAD &&
				pObj->GetObjectChara()->GetState() != CPlayer::STATE_INVINCIBLE)
			{
				// �ʒu�ݒ�
				pObj->SetPosition(D3DXVECTOR3(TargetPos.x, TargetPos.y + 5.0f, TargetPos.z));

				// �����𐳖ʂɂ���
				pObj->SetRotation(D3DXVECTOR3(TargetRot.x, D3DX_PI + fRot, TargetRot.z));

				// �m�b�N�o�b�N�ړ���
				pObj->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * 4.0f,
					15.0f,
					cosf(D3DX_PI + fRot) * 4.0f));

				if (pObj->Hit(mylib_const::IMPACTWAVE_DMG))
				{
					return;
				}
			}
			break;

		default:
			break;
		}
	}

	//********************************************
	// �{�X�̏���
	//********************************************
	if (pObj->GetType() != TYPE_BOSS)
	{// �{�X�ȊO
		return;
	}

	// �{�X�̏���
	if (m_aWave[nCntWave].nCntState > 10)
	{
		m_aWave[nCntWave].nCntState = 10;
	}

	float fMove;
	switch (m_aWave[nCntWave].sSetWave.nLevel)
	{
	case CPowerGauge::LEVEL_1:
		fMove = 30.0f;
		break;

	case CPowerGauge::LEVEL_2:
		fMove = 50.0f;
		break;

	case CPowerGauge::LEVEL_3:
		fMove = 100.0f;
		break;

	case CPowerGauge::LEVEL_MAX:
		fMove = 150.0f;
		break;
	}
	move = D3DXVECTOR3(sinf(D3DX_PI + fRot) * fMove, pObj->GetMove().y, cosf(D3DX_PI + fRot) * fMove);

	// �ړ��ʐݒ�
	pObj->SetMove(move);

}

//==========================================================================
// �E�F�[�u���̔z�u����
//==========================================================================
void CElevation::SetWaveNaturalwave(int nStage, int nWave)
{
	// �G���_�f�[�^�擾
	CEnemyBase::BaseData *pEnemyBase = CGame::GetEnemyBase()->GetBaseData(nStage);

	// �g�z�u
	for (int nCntWave = 0; nCntWave < m_aSetInfo[nStage].aWaveInfo[nWave].nSetNum; nCntWave++)
	{
		// �z�u���
		CWaveManager::SetInfo info = m_aSetInfo[nStage].aWaveInfo[nWave].Info[nCntWave];

		// �g����
		switch (info.nType)
		{
		case 0:
			CGame::GetObject3DMesh()->SetWave
			(
				pEnemyBase[info.nBase].pos + info.pos,	// �ʒu
				info.rot,								// ����
				D3DXVECTOR3(
					0.0f,
					0.0f,
					0.0f),								// �ړ���
				D3DXVECTOR2(200.0f, 200.0f),			// ����͈�
				0.0f,									// �g�����̕p�x
				180.0f,									// �g�̍���
				0.0f,									// �g�̍L����
				60,										// ��������
				CElevation::WAVETYPE_RAISE,				// ���
				CElevation::TYPE_NATURAL,				// �v���C���[�̎��
				0										// ���x��
			);
			break;

		case 1:
			CGame::GetObject3DMesh()->SetWave
			(
				pEnemyBase[info.nBase].pos + info.pos,				// �ʒu
				info.rot,											// ����
				D3DXVECTOR3(
					sinf(D3DX_PI + GetRotation().y) * 10.0f,
					0.0f,
					cosf(D3DX_PI + GetRotation().y) * 10.0f),		// �ړ���
				D3DXVECTOR2(200.0f, 300.0f),						// ����͈�
				CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,		// �g�����̕p�x
				-300.0f,											// �g�̍���
				D3DX_PI * 0.0f,										// �g�łp�x
				150,												// ��������
				CElevation::WAVETYPE_DIRECT,						// ���
				CElevation::TYPE_NATURAL,							// �v���C���[�̎��
				CPowerGauge::LEVEL_1
			);
			break;
		}
	}
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CElevation::SetVtx(void)
{

	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	D3DXVECTOR3 *pVtxNor = GetVtxNor();
	D3DXVECTOR3 vec1, vec2, nor;
	D3DXVECTOR3 VtxRight, VtxLeft, VtxNow;
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();

	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{// �c�̕������J��Ԃ�

		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// ���̕������J��Ԃ�

			m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x = pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x;
			m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z = pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z;

			//// �������Z�b�g
			//m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y = 0.0f;
			//for (int nCntWave = 0; nCntWave < mylib_const::MAX_WAVE; nCntWave++)
			//{// �S�Ă̔g�̍���������
			//	m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += m_aWave[nCntWave].VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;
			//}

			// ���v�l��������
			pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))] = m_VtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))];
			pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y += m_aInfo.pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y;

			// ����̒��_
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nVerTexW = (m_aInfo.nWidthBlock + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1));
			int nRight = nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1)) + nVerTexW;

			if (nNowPoint >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{
				continue;
			}

			if (nRight >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{// ���_����������

				// ���_���W�̐ݒ�
				VtxRight = D3DXVECTOR3(
					(fWidthLen * nCntWidth) - ((fWidthLen * m_aInfo.nWidthBlock) * 0.5f),
					0.0f,
					-((fHeightLen * nCntHeight) - ((fHeightLen * m_aInfo.nHeightBlock) * 0.5f)));
			}
			else
			{
				VtxRight = pVtxPos[nRight];
			}

			if (nLeft >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{// ���_����������

				// ���_���W�̐ݒ�
				VtxLeft = D3DXVECTOR3(
					(fWidthLen * nCntWidth) - ((fWidthLen * m_aInfo.nWidthBlock) * 0.5f),
					0.0f,
					-((fHeightLen * nCntHeight) - ((fHeightLen * m_aInfo.nHeightBlock) * 0.5f)));
			}
			else
			{
				VtxLeft = pVtxPos[nLeft];
			}

			if (nNowPoint >= (m_aInfo.nWidthBlock + 1) * (m_aInfo.nHeightBlock + 1))
			{// ���_����������

				// ���_���W�̐ݒ�
				VtxNow = D3DXVECTOR3(
					(fWidthLen * nCntWidth) - ((fWidthLen * m_aInfo.nWidthBlock) * 0.5f),
					0.0f,
					-((fHeightLen * nCntHeight) - ((fHeightLen * m_aInfo.nHeightBlock) * 0.5f)));
			}
			else
			{
				VtxNow = pVtxPos[nNowPoint];
			}

			// �x�N�g�����v�Z
			vec1 = VtxRight - VtxNow;
			vec2 = VtxLeft - VtxNow;

			//// ���_�̌v�Z�����������Z
			//nCntCalVtx[nRight]++;
			//nCntCalVtx[nLeft]++;
			//nCntCalVtx[nNowPoint]++;

			//if (nRight == 0 || nLeft == 0)
			//{
			//	int n = 0;
			//}

			// �O�ς����߂�
			D3DXVec3Cross(&nor, &vec1, &vec2);

			// �O�ς̐��K�������Ė@���ɂ���
			D3DXVec3Normalize(&nor, &nor);

			//// �@�������Z����
			//vecCalVtx[nNowPoint] += nor;

			// �@��
			pVtxNor[nNowPoint] = nor;
		}
	}

	//for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock; nCntHeight++)
	//{// �c�̕������J��Ԃ�

	//	for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock; nCntWidth++)
	//	{// ���̕������J��Ԃ�

	//		// �@�������Z����
	//		vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].x /= (float)nCntCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))];
	//		vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y /= (float)nCntCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))];
	//		vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].z /= (float)nCntCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))];

	//		if (nCntCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))] == 6)
	//		{
	//			int n = 0;
	//		}

	//		// �O�ς̐��K�������Ė@���ɂ���
	//		D3DXVec3Normalize(&vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))], &vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))]);

	//		// �@��
	//		pVtxNor[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))] = vecCalVtx[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))];
	//	}
	//}

	// ���_���X�V
	CObject3DMesh::SetVtx();
}

//==========================================================================
// ���[�h����
//==========================================================================
HRESULT CElevation::Load(const char *pText)
{
	char aComment[mylib_const::MAX_STRING] = {};	//�R�����g�p

	FILE *pFile = NULL;	// �t�@�C���|�C���^��錾

	// �t�@�C�����J��
	pFile = fopen("data\\TEXT\\elevation_field.txt", "r");

	if (pFile == NULL)
	{// �t�@�C�����J���Ȃ������ꍇ
		fclose(pFile);
		return E_FAIL;
	}

	int nCntStage = 0;	// �X�e�[�W�̃J�E���g

	while (1)
	{// END_SCRIPT������܂ŌJ��Ԃ�

		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		if (strcmp(&aComment[0], "TEXTURE_FILENAME") == 0)
		{// TEXTYPE���������ޓǂݍ���

			fscanf(pFile, "%s", &aComment[0]);	// =�̕�
			fscanf(pFile, "%s", &aComment[0]);	// �e�N�X�`���t�@�C����

			// �t�@�C�����ۑ�
			m_aInfo.TextureFileName = aComment;
		}

		// ���b�V���t�B�[���h�̐ݒ�
		if (strcmp(&aComment[0], "FIELDSET") == 0)
		{// ���f���̓ǂݍ��݂��J�n

			while (strcmp(&aComment[0], "END_FIELDSET"))
			{// END_FIELDSET������܂ŌJ��Ԃ�

				fscanf(pFile, "%s", &aComment[0]);	// �m�F����

				if (strcmp(&aComment[0], "POS") == 0)
				{// POS��������ʒu�ǂݍ���

					fscanf(pFile, "%s", &aComment[0]);	// =�̕�
					fscanf(pFile, "%f", &m_aInfo.pos.x);		// X���W
					fscanf(pFile, "%f", &m_aInfo.pos.y);		// Y���W
					fscanf(pFile, "%f", &m_aInfo.pos.z);		// Z���W
				}

				if (strcmp(&aComment[0], "BLOCK") == 0)
				{// BLOCK������������ǂݍ���

					fscanf(pFile, "%s", &aComment[0]);			// =�̕�
					fscanf(pFile, "%d", &m_aInfo.nWidthBlock);	// ���̕�����
					fscanf(pFile, "%d", &m_aInfo.nHeightBlock);	// �c�̕�����
				}

				if (strcmp(&aComment[0], "SIZE") == 0)
				{// SIZE������������ǂݍ���

					fscanf(pFile, "%s", &aComment[0]);		// =�̕�
					fscanf(pFile, "%f", &m_aInfo.fWidthLength);	// ���̒���
					fscanf(pFile, "%f", &m_aInfo.fHeightLength);	// �c�̒���
				}

				if (strcmp(&aComment[0], "VTXPOS") == 0)
				{// VTXPOS�������璸�_�ǂݍ���

					// ���_��
					int nVtxNum = (m_aInfo.nHeightBlock + 1) * (m_aInfo.nWidthBlock + 1);
					int nCntVtx = 0;

					// ���_���Ń������m��
					if (nCntStage == 0)
					{
						m_aInfo.pVtxPos = DEBUG_NEW D3DXVECTOR3[nVtxNum];
					}
					m_aInfo.pStageVtxPos[nCntStage] = DEBUG_NEW D3DXVECTOR3[nVtxNum];

					// ���_�ǂݍ��ݗp
					std::string VtxPos;

					while (1)
					{// END_VTXPOS������܂ŌJ��Ԃ�

						fscanf(pFile, "%s", (char*)VtxPos.c_str());

						if (strcmp(VtxPos.c_str(), "END_VTXPOS") == 0)
						{// �r���I������Ă���

							for (int nCnt = nCntVtx; nCnt < nVtxNum; nCnt++)
							{
								// 0�N���A
								m_aInfo.pVtxPos[nCnt].y = 0.0f;	// ����
								m_aInfo.pStageVtxPos[nCntStage][nCnt].y = 0.0f;	// ���
							}
							break;
						}

						// float�ϊ�
						if (nCntStage == 0)
						{
							m_aInfo.pVtxPos[nCntVtx].y = 0.0f;	// ����
						}
						m_aInfo.pStageVtxPos[nCntStage][nCntVtx].y = std::strtof(VtxPos.c_str(), nullptr);	// ���
						nCntVtx++;
					}
					// �X�e�[�W�����Z
					nCntStage++;
				}

			}// END_FIELDSET�̂�����
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���

			break;
		}
	}

	// �t�@�C�������
	fclose(pFile);
	return S_OK;

}

//==========================================================================
// �Z�[�u����
//==========================================================================
void CElevation::Save(void)
{
	FILE *pFile = NULL;	// �t�@�C���|�C���^��錾

	// �t�@�C�����J��
	pFile = fopen("data\\TEXT\\elevation_field_save.txt", "w");

	if (pFile == NULL)
	{// �t�@�C�����J���Ȃ������ꍇ
		return;
	}

	// �t�@�C���ɏ����o��
	fprintf(pFile,
		"#------------------------------------------------------------------------------\n"
		"# �e�N�X�`���t�@�C����\n"
		"#------------------------------------------------------------------------------\n");

	// �e�N�X�`���t�@�C����
	fprintf(pFile, "TEXTURE_FILENAME = %s\t\t\n", m_aInfo.TextureFileName.c_str());
	
	// �t�B�[���h�̐ݒu
	fprintf(pFile,
		"\n"
		"#==============================================================================\n"
		"# �t�B�[���h�̐ݒu\n"
		"#==============================================================================\n");

	float m_fWidthLen = GetWidthLen();
	float m_fHeightLen = GetHeightLen();

	fprintf(pFile,
		"FIELDSET\n"
		"\tPOS = %.2f %.2f %.2f\n"
		"\tBLOCK = %d %d\n"
		"\tSIZE = %.2f %.2f\n"
		"\tVTXPOS\n", m_aInfo.pos.x, m_aInfo.pos.y, m_aInfo.pos.z,
		m_aInfo.nWidthBlock, m_aInfo.nHeightBlock,
		m_fWidthLen, m_fHeightLen);

	// ���_�擾
	D3DXVECTOR3 *pVtxPos = GetVtxPos();

	// ���_���̐ݒ�
	for (int nCntHeight = 0; nCntHeight < m_aInfo.nHeightBlock + 1; nCntHeight++)
	{//�c�̒��_�����J��Ԃ�

		fprintf(pFile, "\t");
		for (int nCntWidth = 0; nCntWidth < m_aInfo.nWidthBlock + 1; nCntWidth++)
		{// ���̒��_�����J��Ԃ�

			fprintf(pFile, "%.2f ", pVtxPos[nCntWidth + (nCntHeight * (m_aInfo.nWidthBlock + 1))].y);
		}
		fprintf(pFile, "\n");
	}
	fprintf(pFile, "\nEND_VTXPOS\n");

	fprintf(pFile, "\nEND_FIELDSET\n");
	fprintf(pFile, "\nEND_SCRIPT		# ���̍s�͐�Ώ����Ȃ����ƁI");

	//�t�@�C�������
	fclose(pFile);
}

//==========================================================================
// �G�f�B�b�g�̔���
//==========================================================================
bool CElevation::IsEdit(void)
{
	return m_bEdit;
}

//==========================================================================
// �N���̏��擾
//==========================================================================
CElevation *CElevation::GetElevation(void)
{
	return this;
}
