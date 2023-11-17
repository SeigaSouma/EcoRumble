//=============================================================================
// 
//  チュートリアル処理 [tutorial.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "tutorial.h"
#include "input.h"
#include "fade.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "enemymanager.h"
#include "player.h"
#include "tutorialstep.h"
#include "tutorialtext.h"
#include "sound.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CTutorialStep *CTutorial::m_pStep = NULL;	// ステップ
bool CTutorial::m_bMovingPlayer = false;	// プレイヤーが動いてる判定

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorial::CTutorial()
{
	// 値のクリア
	m_pPlayer = NULL;	// プレイヤーのオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorial::~CTutorial()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTutorial::Init(void)
{

	// 各種変数の初期化


	// 初期化処理
	if (FAILED(CScene::Init()))
	{// 失敗した場合
		return E_FAIL;
	}

	// BGM再生
	CManager::GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

	//**********************************
	// ステップ
	//**********************************
	m_pStep = CTutorialStep::Create();

	//**********************************
	// テキスト
	//**********************************
	CTutorialText::Create();

	//**********************************
	// プレイヤー
	//**********************************
	m_pPlayer = CPlayer::Create();
	m_pPlayer->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1000.0f));
	m_pPlayer->SetRotation(D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));

	// 成功
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTutorial::Uninit(void)
{
	// プレイヤーの破棄
	if (m_pPlayer != NULL)
	{// メモリの確保が出来ていたら

		m_pPlayer->Kill();
		m_pPlayer = NULL;
	}

	// ステップの破棄
	if (m_pStep != NULL)
	{// メモリの確保が出来ていたら

		m_pStep->Uninit();
		delete m_pStep;
		m_pStep = NULL;
	}

	// 終了処理
	CScene::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorial::Update(void)
{
	CManager::GetDebugProc()->Print(
		"現在のモード：【チュートリアル】\n"
		"切り替え：【 F 】\n\n");

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	if (/*m_pStep->GetNowStep() >= CTutorialStep::STEP_MAX - 1 &&*/
		(pInputKeyboard->GetTrigger(DIK_BACKSPACE) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_START, 0) == true))
	{
		// モード設定
		CManager::GetFade()->SetFade(CScene::MODE_GAME);
	}

	// 敵マネージャの更新処理
	GetEnemyManager()->Update();

	// ステップの更新処理
	if (m_pStep != NULL)
	{// NULLじゃなかったら
		m_pStep->Update();
	}

}

//==========================================================================
// 描画処理
//==========================================================================
void CTutorial::Draw(void)
{

}

//==========================================================================
// プレイヤーが動いてる判定
//==========================================================================
void CTutorial::SetEnableMovingPlayer(void)
{
	m_bMovingPlayer = true;
}

//==========================================================================
// プレイヤーが動いてる判定OFF
//==========================================================================
void CTutorial::SetDesableMovingPlayer(void)
{
	m_bMovingPlayer = false;
}

//==========================================================================
// プレイヤーが動いてる判定取得
//==========================================================================
bool CTutorial::IsMovingPlayer(void)
{
	return m_bMovingPlayer;
}

//==========================================================================
// プレイヤーの取得
//==========================================================================
CPlayer *CTutorial::GetPlayer(void)
{
	return m_pPlayer;
}

//==========================================================================
// プレイヤーの取得
//==========================================================================
CPlayer **CTutorial::GetPlayer(int n)
{
	return &m_pPlayer;
}

//==========================================================================
// ステップ取得
//==========================================================================
CTutorialStep *CTutorial::GetStep(void)
{
	return m_pStep;
}
