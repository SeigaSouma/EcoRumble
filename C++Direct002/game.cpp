//=============================================================================
// 
//  ゲーム処理 [game.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "fade.h"
#include "camera.h"

#include "input.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "timer.h"
#include "map.h"
#include "elevation.h"
#include "magma.h"
#include "damage_gate.h"
#include "power_gauge.h"
#include "grassblades.h"
#include "stageswitch.h"
#include "sound.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CScore *CGame::m_pScore = NULL;					// スコアのオブジェクト
CTimer *CGame::m_pTimer = NULL;					// タイマーのオブジェクト
CPowerGauge *CGame::m_pPowerGauge = NULL;		// パワーゲージのオブジェクト

//==========================================================================
// コンストラクタ
//==========================================================================
CGame::CGame()
{
	// 値のクリア
	m_pPlayer = NULL;	// プレイヤーのオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CGame::~CGame()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CGame::Init(void)
{

	// 初期化処理
	if (FAILED(CScene::Init()))
	{// 失敗した場合
		return E_FAIL;
	}

	// BGM再生
	CManager::GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

	//**********************************
	// プレイヤー
	//**********************************
	// メモリ確保
	m_pPlayer = CPlayer::Create();
	m_pPlayer->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1000.0f));
	m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));

	// スコアの生成処理
	m_pScore = CScore::Create(D3DXVECTOR3(1000.0f, 50.0f, 0.0f));

	// タイマーの生成処理
	m_pTimer = CTimer::Create(D3DXVECTOR3(200.0f, 50.0f, 0.0f));

	CGrassBlades::Create(D3DXVECTOR3(-160.0f, 0.0f, -20.0f));
	CGrassBlades::Create(D3DXVECTOR3(-1220.0f, 0.0f, -1016.0f));
	CGrassBlades::Create(D3DXVECTOR3(-20.0f, 200.0f, 280.0f));
	CGrassBlades::Create(D3DXVECTOR3(200.0f, 200.0f, 580.0f));

	CManager::GetCamera()->Reset(CScene::MODE_GAME);

	// ステージ切り替え生成
	CStageSwitch::Create();

	// 成功
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CGame::Uninit(void)
{
	// プレイヤーの破棄
	if (m_pPlayer != NULL)
	{// メモリの確保が出来ていたら

		m_pPlayer->Kill();
		m_pPlayer = NULL;
	}

	// スコアの破棄
	if (m_pScore != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pScore->Uninit();

		// メモリの開放
		delete m_pScore;
		m_pScore = NULL;
	}

	// タイマーの破棄
	if (m_pTimer != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pTimer->Uninit();

		// メモリの開放
		delete m_pTimer;
		m_pTimer = NULL;
	}

	// 終了処理
	CScene::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CGame::Update(void)
{
	CManager::GetDebugProc()->Print(
		"現在のモード：【ゲーム】\n"
		"切り替え：【 F 】\n\n");

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

#if 1
	// スコアの更新処理
	m_pScore->Update();

	if (CManager::GetEdit() == NULL &&
		CScene::GetObject3DMesh()->IsEdit() == false &&
		GetWaveManager()->IsStageFrag() == true &&
		GetEnemyManager()->GetState() != CEnemyManager::STATE_COMBOANIM)
	{
		// タイマーの更新処理
		m_pTimer->Update();
	}
#endif
	// 敵マネージャの更新処理
	GetEnemyManager()->Update();

	// ウェーブ管理の更新処理
	if (GetWaveManager() != NULL)
	{
		GetWaveManager()->Update();
	}

#if _DEBUG
	if (/*pInputKeyboard->GetTrigger(DIK_P) || */pInputGamepad->GetTrigger(CInputGamepad::BUTTON_BACK, 0) == true)
	{
		for (int nCnt = 0; nCnt < 20; nCnt++)
		{
			Reset();
		}
	}

	if (pInputKeyboard->GetTrigger(DIK_F))
	{
		// モード設定
		CManager::GetFade()->SetFade(CScene::MODE_RESULT);
	}
#endif
}

//==========================================================================
// 描画処理
//==========================================================================
void CGame::Draw(void)
{

}

//==========================================================================
// プレイヤーの取得
//==========================================================================
CPlayer *CGame::GetPlayer(void)
{
	return m_pPlayer;
}

//==========================================================================
// プレイヤーの取得
//==========================================================================
CPlayer **CGame::GetPlayer(int n)
{
	return &m_pPlayer;
}

//==========================================================================
// スコアの取得
//==========================================================================
CScore *CGame::GetScore(void)
{
	return m_pScore;
}

//==========================================================================
// タイマーの取得
//==========================================================================
CTimer *CGame::GetTimer(void)
{
	return m_pTimer;
}

//==========================================================================
// パワーゲージの取得
//==========================================================================
CPowerGauge *CGame::GetPowerGauge(void)
{
	return m_pPowerGauge;
}

//==========================================================================
// リセット処理
//==========================================================================
void CGame::Reset(void)
{
	// 敵
	GetEnemyManager()->SetEnemy(
		Random(0, 4),
		D3DXVECTOR3(Random(-200, 200) * 10.0f, 0.0f, Random(-200, 200) * 10.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}