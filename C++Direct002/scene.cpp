//=============================================================================
// 
//  シーン処理 [scene.cpp]
//  Author : 相馬靜雅
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

// 遷移先
#include "game.h"
#include "title.h"
#include "tutorial.h"
#include "result.h"
#include "ranking.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CXLoad *CScene::m_pXLoad = NULL;				// Xファイルのオブジェクト
CElevation *CScene::m_pObject3DMesh = NULL;		// オブジェクト3Dメッシュのオブジェクト
CElevation *CScene::m_pElevationOutside[CElevation::POSTYPE_MAX] = {};	// 外側起伏の地面のオブジェクト
CEnemyManager *CScene::m_pEnemyManager = NULL;	// 敵マネージャのオブジェクト
CEnemyBase *CScene::m_pEnemyBase = NULL;			// 敵拠点のオブジェクト

//==========================================================================
// コンストラクタ
//==========================================================================
CScene::CScene()
{
	// 変数のクリア
	m_mode = MODE_TITLE;
	m_pWaveManager = NULL;		// ウェーブ管理のオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CScene::~CScene()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CScene *CScene::Create(CScene::MODE mode)
{
	// 生成用のオブジェクト
	CScene *pScene = NULL;

	if (pScene == NULL)
	{// NULLだったら

		// 生成処理
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
		{// メモリの確保が出来ていたら

			// モードの設定
			pScene->m_mode = mode;
		}

		return pScene;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CScene::Init(void)
{
	HRESULT hr;

	//**********************************
	// Xファイル
	//**********************************
	if (m_pXLoad != NULL)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pXLoad = DEBUG_NEW CXLoad;

	if (m_pXLoad == NULL)
	{// メモリの確保が出来ていなかったら
		return E_FAIL;
	}

	//**********************************
	// 敵マネージャ
	//**********************************
	m_pEnemyManager = CEnemyManager::Create("data\\TEXT\\enemy_manager.txt");

	if (m_pEnemyManager == NULL)
	{// NULLだったら
		return E_FAIL;
	}

	//**********************************
	// 敵拠点
	//**********************************
	m_pEnemyBase = CEnemyBase::Create("data\\TEXT\\enemy_manager.txt");

	if (m_pEnemyBase == NULL)
	{// NULLだったら
		return E_FAIL;
	}

	//**********************************
	// ウェーブ管理
	//**********************************
	m_pWaveManager = CWaveManager::Create("data\\TEXT\\enemy_manager.txt");

	if (m_pWaveManager == NULL)
	{// NULLだったら
		return E_FAIL;
	}

	//**********************************
	// マップの生成
	//**********************************
	if (FAILED(map::Create()))
	{// 失敗した場合
		return E_FAIL;
	}

	//**********************************
	// 起伏の地面
	//**********************************
	m_pObject3DMesh = CElevation::Create("data\\TEXT\\elevation_field.txt", CObject::TYPE_ELEVATION);
	m_pElevationOutside[CElevation::POSTYPE_UP] = CElevation::Create("data\\TEXT\\elevation_outside_UP.txt", CObject::TYPE_ELEVATION_OUTSIDE);
	m_pElevationOutside[CElevation::POSTYPE_RIGHT] = CElevation::Create("data\\TEXT\\elevation_outside_Right.txt", CObject::TYPE_ELEVATION_OUTSIDE);
	m_pElevationOutside[CElevation::POSTYPE_DOWN] = CElevation::Create("data\\TEXT\\elevation_outside_Down.txt", CObject::TYPE_ELEVATION_OUTSIDE);
	m_pElevationOutside[CElevation::POSTYPE_LEFT] = CElevation::Create("data\\TEXT\\elevation_outside_Left.txt", CObject::TYPE_ELEVATION_OUTSIDE);

	// 初期化処理
	hr = m_pXLoad->Init();

	if (FAILED(hr))
	{// 初期化処理が失敗した場合
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CScene::Uninit(void)
{
	// Xファイルの破棄
	if (m_pXLoad != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pXLoad->Uninit();

		// メモリの開放
		delete m_pXLoad;
		m_pXLoad = NULL;
	}

	// ウェーブ管理
	if (m_pWaveManager != NULL)
	{
		m_pWaveManager->Uninit();
		delete m_pWaveManager;
		m_pWaveManager = NULL;
	}

	// 敵マネージャ
	if (m_pEnemyManager != NULL)
	{
		m_pEnemyManager->Uninit();
		delete m_pEnemyManager;
		m_pEnemyManager = NULL;
	}

	// 敵拠点
	if (m_pEnemyBase != NULL)
	{
		m_pEnemyBase->Uninit();
		delete m_pEnemyBase;
		m_pEnemyBase = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CScene::Update(void)
{

}

//==========================================================================
// 描画処理
//==========================================================================
void CScene::Draw(void)
{
	
}

//==========================================================================
// 現在のモード取得
//==========================================================================
CScene::MODE CScene::GetMode(void)
{
	return m_mode;
}

//==========================================================================
// Xファイルの取得
//==========================================================================
CXLoad *CScene::GetXLoad(void)
{
	return m_pXLoad;
}

//==========================================================================
// 3Dオブジェクトの取得
//==========================================================================
CElevation *CScene::GetObject3DMesh(void)
{
	return m_pObject3DMesh;
}

//==========================================================================
// 外側起伏の地面のオブジェクト
//==========================================================================
CElevation *CScene::GetElevationOutside(int nIdx)
{
	return m_pElevationOutside[nIdx];
}

//==========================================================================
// ウェーブ管理の取得
//==========================================================================
CWaveManager *CScene::GetWaveManager(void)
{
	return m_pWaveManager;
}

//==========================================================================
// 敵マネージャの取得
//==========================================================================
CEnemyManager *CScene::GetEnemyManager(void)
{
	return m_pEnemyManager;
}

//==========================================================================
// 敵拠点の取得
//==========================================================================
CEnemyBase *CScene::GetEnemyBase(void)
{
	return m_pEnemyBase;
}

//==========================================================================
// プレイヤーの取得
//==========================================================================
CPlayer *CScene::GetPlayer(void)
{
	return NULL;
}

//==========================================================================
// プレイヤーの取得
//==========================================================================
CPlayer **CScene::GetPlayer(int n)
{
	return NULL;
}