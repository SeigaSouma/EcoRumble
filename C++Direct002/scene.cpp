//=============================================================================
// 
//  �V�[������ [scene.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "scene.h"
#include "renderer.h"
#include "calculation.h"
#include "Xload.h"
#include "map.h"
#include "fade.h"
#include "elevation.h"
#include "enemymanager.h"
#include "wavemanager.h"
#include "enemybase.h"

// �J�ڐ�
#include "game.h"
#include "title.h"
#include "tutorial.h"
#include "result.h"
#include "ranking.h"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CXLoad *CScene::m_pXLoad = NULL;				// X�t�@�C���̃I�u�W�F�N�g
CElevation *CScene::m_pObject3DMesh = NULL;		// �I�u�W�F�N�g3D���b�V���̃I�u�W�F�N�g
CElevation *CScene::m_pElevationOutside[CElevation::POSTYPE_MAX] = {};	// �O���N���̒n�ʂ̃I�u�W�F�N�g
CEnemyManager *CScene::m_pEnemyManager = NULL;	// �G�}�l�[�W���̃I�u�W�F�N�g
CEnemyBase *CScene::m_pEnemyBase = NULL;			// �G���_�̃I�u�W�F�N�g

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CScene::CScene()
{
	// �ϐ��̃N���A
	m_mode = MODE_TITLE;
	m_pWaveManager = NULL;		// �E�F�[�u�Ǘ��̃I�u�W�F�N�g
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CScene::~CScene()
{

}

//==========================================================================
// ��������
//==========================================================================
CScene *CScene::Create(CScene::MODE mode)
{
	// �����p�̃I�u�W�F�N�g
	CScene *pScene = NULL;

	if (pScene == NULL)
	{// NULL��������

		// ��������
		switch (mode)
		{
		case CScene::MODE_TITLE:
			pScene = DEBUG_NEW CTitle;
			break;

		case CScene::MODE_TUTORIAL:
			pScene = DEBUG_NEW CTutorial;
			break;

		case CScene::MODE_GAME:
			pScene = DEBUG_NEW CGame;
			break;

		case CScene::MODE_RESULT:
			pScene = DEBUG_NEW CResult;
			break;

		case CScene::MODE_RANKING:
			pScene = DEBUG_NEW CRanking;
			break;
		}

		if (pScene != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ���[�h�̐ݒ�
			pScene->m_mode = mode;
		}

		return pScene;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CScene::Init(void)
{
	HRESULT hr;

	//**********************************
	// X�t�@�C��
	//**********************************
	if (m_pXLoad != NULL)
	{// �m�ۂ���Ă�����
		return E_FAIL;
	}

	// �������m��
	m_pXLoad = DEBUG_NEW CXLoad;

	if (m_pXLoad == NULL)
	{// �������̊m�ۂ��o���Ă��Ȃ�������
		return E_FAIL;
	}

	//**********************************
	// �G�}�l�[�W��
	//**********************************
	m_pEnemyManager = CEnemyManager::Create("data\\TEXT\\enemy_manager.txt");

	if (m_pEnemyManager == NULL)
	{// NULL��������
		return E_FAIL;
	}

	//**********************************
	// �G���_
	//**********************************
	m_pEnemyBase = CEnemyBase::Create("data\\TEXT\\enemy_manager.txt");

	if (m_pEnemyBase == NULL)
	{// NULL��������
		return E_FAIL;
	}

	//**********************************
	// �E�F�[�u�Ǘ�
	//**********************************
	m_pWaveManager = CWaveManager::Create("data\\TEXT\\enemy_manager.txt");

	if (m_pWaveManager == NULL)
	{// NULL��������
		return E_FAIL;
	}

	//**********************************
	// �}�b�v�̐���
	//**********************************
	if (FAILED(map::Create()))
	{// ���s�����ꍇ
		return E_FAIL;
	}

	//**********************************
	// �N���̒n��
	//**********************************
	m_pObject3DMesh = CElevation::Create("data\\TEXT\\elevation_field.txt", CObject::TYPE_ELEVATION);
	m_pElevationOutside[CElevation::POSTYPE_UP] = CElevation::Create("data\\TEXT\\elevation_outside_UP.txt", CObject::TYPE_ELEVATION_OUTSIDE);
	m_pElevationOutside[CElevation::POSTYPE_RIGHT] = CElevation::Create("data\\TEXT\\elevation_outside_Right.txt", CObject::TYPE_ELEVATION_OUTSIDE);
	m_pElevationOutside[CElevation::POSTYPE_DOWN] = CElevation::Create("data\\TEXT\\elevation_outside_Down.txt", CObject::TYPE_ELEVATION_OUTSIDE);
	m_pElevationOutside[CElevation::POSTYPE_LEFT] = CElevation::Create("data\\TEXT\\elevation_outside_Left.txt", CObject::TYPE_ELEVATION_OUTSIDE);

	// ����������
	hr = m_pXLoad->Init();

	if (FAILED(hr))
	{// ���������������s�����ꍇ
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CScene::Uninit(void)
{
	// X�t�@�C���̔j��
	if (m_pXLoad != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pXLoad->Uninit();

		// �������̊J��
		delete m_pXLoad;
		m_pXLoad = NULL;
	}

	// �E�F�[�u�Ǘ�
	if (m_pWaveManager != NULL)
	{
		m_pWaveManager->Uninit();
		delete m_pWaveManager;
		m_pWaveManager = NULL;
	}

	// �G�}�l�[�W��
	if (m_pEnemyManager != NULL)
	{
		m_pEnemyManager->Uninit();
		delete m_pEnemyManager;
		m_pEnemyManager = NULL;
	}

	// �G���_
	if (m_pEnemyBase != NULL)
	{
		m_pEnemyBase->Uninit();
		delete m_pEnemyBase;
		m_pEnemyBase = NULL;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CScene::Update(void)
{

}

//==========================================================================
// �`�揈��
//==========================================================================
void CScene::Draw(void)
{
	
}

//==========================================================================
// ���݂̃��[�h�擾
//==========================================================================
CScene::MODE CScene::GetMode(void)
{
	return m_mode;
}

//==========================================================================
// X�t�@�C���̎擾
//==========================================================================
CXLoad *CScene::GetXLoad(void)
{
	return m_pXLoad;
}

//==========================================================================
// 3D�I�u�W�F�N�g�̎擾
//==========================================================================
CElevation *CScene::GetObject3DMesh(void)
{
	return m_pObject3DMesh;
}

//==========================================================================
// �O���N���̒n�ʂ̃I�u�W�F�N�g
//==========================================================================
CElevation *CScene::GetElevationOutside(int nIdx)
{
	return m_pElevationOutside[nIdx];
}

//==========================================================================
// �E�F�[�u�Ǘ��̎擾
//==========================================================================
CWaveManager *CScene::GetWaveManager(void)
{
	return m_pWaveManager;
}

//==========================================================================
// �G�}�l�[�W���̎擾
//==========================================================================
CEnemyManager *CScene::GetEnemyManager(void)
{
	return m_pEnemyManager;
}

//==========================================================================
// �G���_�̎擾
//==========================================================================
CEnemyBase *CScene::GetEnemyBase(void)
{
	return m_pEnemyBase;
}

//==========================================================================
// �v���C���[�̎擾
//==========================================================================
CPlayer *CScene::GetPlayer(void)
{
	return NULL;
}

//==========================================================================
// �v���C���[�̎擾
//==========================================================================
CPlayer **CScene::GetPlayer(int n)
{
	return NULL;
}