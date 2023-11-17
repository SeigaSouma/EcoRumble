//=============================================================================
// 
//  �}�l�[�W������ [manager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "debugproc.h"
#include "object2D.h"
#include "object3D.h"
#include "objectBillboard.h"
#include "objectX.h"
#include "texture.h"
#include "calculation.h"

#include "pause.h"
#include "fade.h"
#include "instantfade.h"
#include "light.h"
#include "camera.h"
#include "bg.h"
#include "edit.h"
#include "resultmanager.h"
#include "rankingmanager.h"


//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CRenderer *CManager::m_pRenderer = NULL;				// �����_���[�̃I�u�W�F�N�g
CInputKeyboard *CManager::m_pInputKeyboard = NULL;		// �L�[�{�[�h�̃I�u�W�F�N�g
CInputGamepad *CManager::m_pInputGamepad = NULL;		// �Q�[���p�b�h�̃I�u�W�F�N�g
CSound *CManager::m_pSound = NULL;						// �T�E���h�̃I�u�W�F�N�g
CInputMouse *CManager::m_pInputMouse = NULL;			// �}�E�X�̃I�u�W�F�N�g
CDebugProc *CManager::m_pDebugProc = NULL;				// �f�o�b�O�\���̃I�u�W�F�N�g
CBG *CManager::m_pBg = NULL;							// �w�i�̃I�u�W�F�N�g
CLight *CManager::m_pLight = NULL;						// ���C�g�̃I�u�W�F�N�g
CCamera *CManager::m_pCamera = NULL;					// �J�����̃I�u�W�F�N�g
CMap *CManager::m_pMap = NULL;							// �}�b�v�̃I�u�W�F�N�g
CTexture *CManager::m_pTexture = NULL;					// �e�N�X�`���̃I�u�W�F�N�g
CEdit *CManager::m_pEdit = NULL;						// �G�f�B�b�g�̃I�u�W�F�N�g
CScene *CManager::m_pScene = NULL;						// �V�[���̃I�u�W�F�N�g
CFade *CManager::m_pFade = NULL;						// �t�F�[�h�̃I�u�W�F�N�g
CInstantFade *CManager::m_pInstantFade = NULL;			// �J�ڂȂ��t�F�[�h�̃I�u�W�F�N�g
CPause *CManager::m_pPause = NULL;						// �|�[�Y�̃I�u�W�F�N�g
CResultManager *CManager::m_pResultManager = NULL;		// ���U���g�}�l�[�W���̃I�u�W�F�N�g
CRankingManager *CManager::m_pRankingManager = NULL;	// �����L���O�}�l�[�W���̃I�u�W�F�N�g

bool CManager::m_bWireframe = false;					// ���C���[�t���[��
bool CManager::m_bHitStop = false;						// �q�b�g�X�g�b�v�̔���
int CManager::m_nCntHitStop = 0;						// �q�b�g�X�g�b�v�̃J�E���^�[
CScene::MODE CManager::m_OldMode = CScene::MODE_NONE;	// �O��̃��[�h

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CManager::CManager()
{

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CManager::~CManager()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{

	HRESULT hr;
	m_bHitStop = false;		// �q�b�g�X�g�b�v�̔���
	m_nCntHitStop = 0;		// �q�b�g�X�g�b�v�̃J�E���^�[

	//**********************************
	// �L�[�{�[�h
	//**********************************
	if (m_pInputKeyboard != NULL)
	{// �m�ۂ���Ă�����
		return E_FAIL;
	}

	// �������m��
	m_pInputKeyboard = DEBUG_NEW CInputKeyboard;

	if (m_pInputKeyboard != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		hr = m_pInputKeyboard->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// ���������������s�����ꍇ

			return E_FAIL;
		}
	}
	
	//**********************************
	// �Q�[���p�b�h
	//**********************************
	if (m_pInputGamepad != NULL)
	{// �m�ۂ���Ă�����
		return E_FAIL;
	}

	// �������m��
	m_pInputGamepad = DEBUG_NEW CInputGamepad;

	if (m_pInputGamepad != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		hr = m_pInputGamepad->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// ���������������s�����ꍇ

			return E_FAIL;
		}
	}


	//**********************************
	// �}�E�X
	//**********************************
	if (m_pInputMouse != NULL)
	{// �m�ۂ���Ă�����
		return E_FAIL;
	}

	// �������m��
	m_pInputMouse = DEBUG_NEW CInputMouse;

	if (m_pInputMouse != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		hr = m_pInputMouse->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// ���������������s�����ꍇ

			return E_FAIL;
		}
	}


	//**********************************
	// �����_���[
	//**********************************
	if (m_pRenderer != NULL)
	{// �m�ۂ���Ă�����

		return E_FAIL;
	}

	// �������m��
	m_pRenderer = DEBUG_NEW CRenderer;

	if (m_pRenderer != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		hr = m_pRenderer->Init(hWnd, TRUE);
		if (FAILED(hr))
		{// ���������������s�����ꍇ

			return E_FAIL;
		}
	}


	//**********************************
	// �T�E���h
	//**********************************
	if (m_pSound != NULL)
	{// �m�ۂ���Ă�����
		return E_FAIL;
	}

	// �������m��
	m_pSound = DEBUG_NEW CSound;

	if (m_pSound != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		hr = m_pSound->Init(hWnd);
		if (FAILED(hr))
		{// ���������������s�����ꍇ

			return E_FAIL;
		}
	}


	//**********************************
	// �f�o�b�O�\��
	//**********************************
	if (m_pDebugProc != NULL)
	{// �m�ۂ���Ă�����
		return E_FAIL;
	}

	// �������m��
	m_pDebugProc = DEBUG_NEW CDebugProc;

	if (m_pDebugProc != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		hr = m_pDebugProc->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// ���������������s�����ꍇ

			return E_FAIL;
		}
	}


	//**********************************
	// ���C�g
	//**********************************
	if (m_pLight != NULL)
	{// �m�ۂ���Ă�����
		return E_FAIL;
	}

	// �������m��
	m_pLight = DEBUG_NEW CLight;

	if (m_pLight != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		hr = m_pLight->Init();
		if (FAILED(hr))
		{// ���������������s�����ꍇ

			return E_FAIL;
		}
	}


	//**********************************
	// �J����
	//**********************************
	if (m_pCamera != NULL)
	{// �m�ۂ���Ă�����
		return E_FAIL;
	}

	// �������m��
	m_pCamera = DEBUG_NEW CCamera;

	if (m_pCamera != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		hr = m_pCamera->Init();
		if (FAILED(hr))
		{// ���������������s�����ꍇ

			return E_FAIL;
		}
	}

	//**********************************
	// �S�Ẵe�N�X�`���ǂݍ���
	//**********************************
	if (m_pTexture != NULL)
	{// �m�ۂ���Ă�����
		return E_FAIL;
	}

	// �������m��
	m_pTexture = DEBUG_NEW CTexture;

	if (m_pTexture != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		hr = m_pTexture->Load();
		if (FAILED(hr))
		{// ���������������s�����ꍇ

			return E_FAIL;
		}

		// ����������
		hr = m_pTexture->LoadAll();

		if (FAILED(hr))
		{// ���������������s�����ꍇ

			return E_FAIL;
		}
	}

	//**********************************
	// �}�b�v
	//**********************************
	//map::Create();


	//**********************************
	// �I�u�W�F�N�g����
	//**********************************
	CObject2D *pObject2D = NULL;


#if 0
	// �u���b�N�̐�������
	pObject2D = CBlock::Create(D3DXVECTOR3(640.0f, 550.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f), 2, 1);
	pObject2D = CBlock::Create(D3DXVECTOR3(340.0f, 550.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f), 2, 1);
	pObject2D = CBlock::Create(D3DXVECTOR3(100.0f, 650.0f, 0.0f), D3DXVECTOR2(120.0f, 120.0f));

	pObject2D = CBlock::Create(D3DXVECTOR3(640.0f, 400.0f, 0.0f), D3DXVECTOR2(30.0f, 30.0f));
	pObject2D = CBlock::Create(D3DXVECTOR3(450.0f, 150.0f, 0.0f), D3DXVECTOR2(30.0f, 30.0f), 1, 8);
	pObject2D = CBlock::Create(D3DXVECTOR3(810.0f, 150.0f, 0.0f), D3DXVECTOR2(30.0f, 30), 1, 8);
	pObject2D = CBlock::Create(D3DXVECTOR3(480.0f, 150.0f, 0.0f), D3DXVECTOR2(30.0f, 30.0f), 11, 1);


	for (int nCnt = 0; nCnt < 30; nCnt++)
	{
		CBlock::Create(D3DXVECTOR3(nCnt * 50.0f, 700.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f));
	}

	// �����珇��
	for (int nCnt = 0; nCnt < 8; nCnt++)
	{
		CBlock::Create(D3DXVECTOR3(1000.0f, 550.0f - nCnt * 50.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f));
	}

	// �ォ�珇��
	for (int nCnt = 0; nCnt < 8; nCnt++)
	{
		CBlock::Create(D3DXVECTOR3(1200.0f, 200.0f + nCnt * 50.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f));
	}

	// �u���b�N�̐�������
	pObject2D = CItem::Create(CItem::TYPE_ITEM, D3DXVECTOR3(640.0f, 500.0f, 0.0f));
	pObject2D = CItem::Create(CItem::TYPE_ITEM, D3DXVECTOR3(340.0f, 500.0f, 0.0f));
	pObject2D = CItem::Create(CItem::TYPE_ITEM, D3DXVECTOR3(800.0f, 600.0f, 0.0f));
#endif

	//**********************************
	// �J�ڂȂ��t�F�[�h
	//**********************************
	m_pInstantFade = CInstantFade::Create();

	if (m_pInstantFade == NULL)
	{// ���s���Ă�����
		return E_FAIL;
	}

	//**********************************
	// �|�[�Y
	//**********************************
	m_pPause = CPause::Create();

	if (m_pPause == NULL)
	{// ���s���Ă�����
		return E_FAIL;
	}
	//SetMode(CScene::MODE_GAME);

	//**********************************
	// ���U���g�}�l�[�W��
	//**********************************
	m_pResultManager = CResultManager::Create();

	if (m_pResultManager == NULL)
	{// ���s���Ă�����
		return E_FAIL;
	}

	//**********************************
	// �����L���O�}�l�[�W��
	//**********************************
	m_pRankingManager = CRankingManager::Create();

	if (m_pRankingManager == NULL)
	{// ���s���Ă�����
		return E_FAIL;
	}

	//**********************************
	// �t�F�[�h
	//**********************************
	m_pFade = CFade::Create();

	if (m_pFade == NULL)
	{// ���s���Ă�����
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// ���̃��[�h�ݒ�
//==========================================================================
void CManager::SetMode(CScene::MODE mode)
{
	// �O��̃��[�h�ݒ�
	m_OldMode = GetMode();

	if (mode == CScene::MODE_GAME && m_pResultManager != NULL)
	{// ���̃��[�h���Q�[����������

		// �X�R�A��񃊃Z�b�g
		m_pResultManager->Reset();
		m_pRankingManager->Reset();
	}

	// BGM�X�g�b�v
	if (m_pSound != NULL && mode != CScene::MODE_RANKING)
	{
		m_pSound->StopSound();
	}

	// �������̊m�ۂ�����Ă�����
	if (m_pScene != NULL)
	{
		// �I������
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = NULL;
	}

	// �S�ẴI�u�W�F�N�g�j��
	CObject::ReleaseAll();

	// �|�[�Y�󋵓���ւ�
	if (m_pPause != NULL)
	{
		if (m_pPause->IsPause() == true)
		{// �|�[�Y����������
			m_pPause->SetPause();
		}
	}

	// �J�����̏�񃊃Z�b�g
	if (m_pCamera != NULL)
	{
		m_pCamera->Reset(mode);
	}

	// ��������
	m_pScene = CScene::Create(mode);

	// ����������
	if (m_pScene != NULL)
	{
		m_pScene->Init();
	}

	m_bHitStop = false;		// �q�b�g�X�g�b�v�̔���
	m_nCntHitStop = 0;		// �q�b�g�X�g�b�v�̃J�E���^�[
}

//==========================================================================
// ���݂̃��[�h�擾
//==========================================================================
CScene::MODE CManager::GetMode(void)
{
	// ���[�h�擾
	if (m_pScene != NULL)
	{
		return m_pScene->GetMode();
	}
	else
	{
		return CScene::MODE_NONE;
	}
}

//==========================================================================
// �I������
//==========================================================================
void CManager::Uninit(void)
{
	// �S�ẴI�u�W�F�N�g�j��
	CObject::ReleaseAll();

	// BGM�X�g�b�v
	m_pSound->StopSound();

	// �L�[�{�[�h�̔j��
	if (m_pInputKeyboard != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pInputKeyboard->Uninit();

		// �������̊J��
		delete m_pInputKeyboard;
		m_pInputKeyboard = NULL;
	}

	// �Q�[���p�b�h�̔j��
	if (m_pInputGamepad != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pInputGamepad->Uninit();

		// �������̊J��
		delete m_pInputGamepad;
		m_pInputGamepad = NULL;
	}

	// �}�E�X�̔j��
	if (m_pInputMouse != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pInputMouse->Uninit();

		// �������̊J��
		delete m_pInputMouse;
		m_pInputMouse = NULL;
	}

	// �����_���[�̔j��
	if (m_pRenderer != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pRenderer->Uninit();

		// �������̊J��
		delete m_pRenderer;
		m_pRenderer = NULL;
	}

	// ���C�g�̔j��
	if (m_pLight != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pLight->Uninit();

		// �������̊J��
		delete m_pLight;
		m_pLight = NULL;
	}

	// �J�����̔j��
	if (m_pCamera != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pCamera->Uninit();

		// �������̊J��
		delete m_pCamera;
		m_pCamera = NULL;
	}

	// �T�E���h�̔j��
	if (m_pSound != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pSound->Uninit();

		// �������̊J��
		delete m_pSound;
		m_pSound = NULL;
	}

	// �f�o�b�O�\���̔j��
	if (m_pDebugProc != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pDebugProc->Uninit();

		// �������̊J��
		delete m_pDebugProc;
		m_pDebugProc = NULL;
	}

	// �w�i�̔j��
	if (m_pBg != NULL)
	{// �������̊m�ۂ��o���Ă�����

		m_pBg = NULL;
	}

	// �}�b�v�̔j��
	if (m_pMap != NULL)
	{// �������̊m�ۂ��o���Ă�����

		m_pMap = NULL;
	}

	// �G�f�B�b�g�̔j��
	if (m_pEdit != NULL)
	{// �������̊m�ۂ��o���Ă�����

		m_pEdit = NULL;
	}

	//**********************************
	// �S�Ẵe�N�X�`���j��
	//**********************************
	if (m_pTexture != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pTexture->Unload();

		// �������̊J��
		delete m_pTexture;
		m_pTexture = NULL;
	}

	if (m_pScene != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = NULL;
	}

	// �t�F�[�h�̔j��
	if (m_pFade != NULL)
	{// �������̊m�ۂ�����Ă�����

		// �I������
		m_pFade->Uninit();
		delete m_pFade;
		m_pFade = NULL;
	}

	// �J�ڂȂ��t�F�[�h�̔j��
	if (m_pInstantFade != NULL)
	{// �������̊m�ۂ�����Ă�����

		// �I������
		m_pInstantFade->Uninit();
		delete m_pInstantFade;
		m_pInstantFade = NULL;
	}

	if (m_pPause != NULL)
	{// �������̊m�ۂ�����Ă�����

		// �I������
		m_pPause->Uninit();
		delete m_pPause;
		m_pPause = NULL;
	}

	if (m_pResultManager != NULL)
	{// �������̊m�ۂ�����Ă�����

		// �I������
		m_pResultManager->Uninit();
		delete m_pResultManager;
		m_pResultManager = NULL;
	}

	if (m_pRankingManager != NULL)
	{// �������̊m�ۂ�����Ă�����

		// �I������
		m_pRankingManager->Uninit();
		delete m_pRankingManager;
		m_pRankingManager = NULL;
	}

}

//==========================================================================
// �X�V����
//==========================================================================
void CManager::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();


	// �t�F�[�h�̍X�V����
	m_pFade->Update();

	// �J�ڂȂ��t�F�[�h�̍X�V����
	m_pInstantFade->Update();

	// �L�[�{�[�h�̍X�V����
	m_pInputKeyboard->Update();

	// �Q�[���p�b�h�̍X�V����
	m_pInputGamepad->Update();

	// �}�E�X�̍X�V����
	m_pInputMouse->Update();

	if ((pInputKeyboard->GetTrigger(DIK_P) == true || m_pInputGamepad->GetTrigger(CInputGamepad::BUTTON_START, 0) == true) && 
		m_pFade->GetState() == CFade::STATE_NONE &&
		GetMode() == CScene::MODE_GAME)
	{// �t�F�[�h������Ȃ��Ƃ�
		
	// �T�E���h�Đ�
	CManager::GetSound()->PlaySound(CSound::LABEL_SE_TUTORIALWINDOW);
		m_pPause->SetPause();
	}

	// �|�[�Y�̍X�V����
	if (m_pPause->IsPause() == true)
	{// �|�[�Y����������
		m_pPause->Update();

#if _DEBUG

		// �J�����̍X�V����
		m_pCamera->Update();
#endif

		return;
	}

	if (m_bHitStop == true)
	{// �q�b�g�X�g�b�v���͍X�V��~

		// �J�����̍X�V����
		m_pCamera->Update();

		// �q�b�g�X�g�b�v�J�E���^�[���Z
		m_nCntHitStop--;

		if (m_nCntHitStop <= 0)
		{// �J�E���^�[���Ȃ��Ȃ�����
			m_bHitStop = false;
		}
		return;
	}
	else
	{
		m_nCntHitStop = 20;
	}

#if _DEBUG
	if (pInputKeyboard->GetTrigger(DIK_F2) == true)
	{// F2�Ń��C���[�t���[���؂�ւ�
		m_bWireframe = m_bWireframe ? false : true;
	}

	if (pInputKeyboard->GetTrigger(DIK_F8) == true)
	{// F8�ŃG�f�B�b�g���[�h�ؑւ�

		if (m_pEdit == NULL)
		{// NULL��������

			// �G�f�B�b�g�̐�������
			m_pEdit = CEdit::Create();
		}
		else
		{
			// �I��������
			m_pEdit->Release();
			m_pEdit = NULL;
		}
	}
#endif

	// �����_���[�̍X�V����
	m_pRenderer->Update();

	// ���C�g�̍X�V����
	m_pLight->Update();

	// �J�����̍X�V����
	m_pCamera->Update();

	// �f�o�b�O�\���̍X�V����
	m_pDebugProc->Update();

	if (m_pScene != NULL)
	{// �������̊m�ۂ��o���Ă�����
		m_pScene->Update();
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CManager::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = m_pRenderer->GetDevice();

	if (m_bWireframe)
	{
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);	// ���߂郂�[�h
	}

	// �����_���[�̕`�揈��
	m_pRenderer->Draw();

	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);	// ���߂郂�[�h
}

//==========================================================================
// �q�b�g�X�g�b�v�̐ݒ�
//==========================================================================
void CManager::SetEnableHitStop(int nCntHitStop)
{
	m_bHitStop = true;
	m_nCntHitStop = nCntHitStop;
}

//==========================================================================
// �����_���[�̎擾
//==========================================================================
CRenderer *CManager::GetRenderer(void)
{
	return m_pRenderer;
}

//==========================================================================
// �L�[�{�[�h�̎擾
//==========================================================================
CInputKeyboard *CManager::GetInputKeyboard(void)
{
	return m_pInputKeyboard;
}

//==========================================================================
// �Q�[���p�b�h�̎擾
//==========================================================================
CInputGamepad *CManager::GetInputGamepad(void)
{
	return m_pInputGamepad;
}

//==========================================================================
// �}�E�X�̎擾
//==========================================================================
CInputMouse *CManager::GetInputMouse(void)
{
	return m_pInputMouse;
}

//==========================================================================
// �T�E���h�̎擾
//==========================================================================
CSound *CManager::GetSound(void)
{
	return m_pSound;
}

//==========================================================================
// �f�o�b�O�\���̎擾
//==========================================================================
CDebugProc *CManager::GetDebugProc(void)
{
	return m_pDebugProc;
}

//==========================================================================
// �w�i�̎擾
//==========================================================================
CBG *CManager::GetBg(void)
{
	return m_pBg;
}

//==========================================================================
// ���C�g�̎擾
//==========================================================================
CLight *CManager::GetLight(void)
{
	return m_pLight;
}

//==========================================================================
// �J�����̎擾
//==========================================================================
CCamera *CManager::GetCamera(void)
{
	return m_pCamera;
}

//==========================================================================
// �e�N�X�`���̎擾
//==========================================================================
CTexture *CManager::GetTexture(void)
{
	return m_pTexture;
}

//==========================================================================
// �G�f�B�b�g�̎擾
//==========================================================================
CEdit *CManager::GetEdit(void)
{
	return m_pEdit;
}

//==========================================================================
// �t�F�[�h�̎擾
//==========================================================================
CFade *CManager::GetFade(void)
{
	return m_pFade;
}

//==========================================================================
// �J�ڂȂ��t�F�[�h�̎擾
//==========================================================================
CInstantFade *CManager::GetInstantFade(void)
{
	return m_pInstantFade;
}

//==========================================================================
// �|�[�Y�̎擾
//==========================================================================
CPause *CManager::GetPause(void)
{
	return m_pPause;
}

//==========================================================================
// ���U���g�}�l�[�W���擾
//==========================================================================
CResultManager *CManager::GetResultManager(void)
{
	return m_pResultManager;
}

//==========================================================================
// �����L���O�}�l�[�W���̃I�u�W�F�N�g
//==========================================================================
CRankingManager *CManager::GetRankingManager(void)
{
	return m_pRankingManager;
}

//==========================================================================
// �V�[���擾
//==========================================================================
CScene *CManager::GetScene(void)
{
	return m_pScene;
}

//--------------------------------------------------------------------------[ CScene�̏��� ]--------------------------------------------------------------------------//
