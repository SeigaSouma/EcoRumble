//=============================================================================
// 
//  タイトル処理 [title.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "title_screen.h"
#include "wavemanager.h"
#include "stageswitch.h"
#include "enemymanager.h"
#include "enemy.h"
#include "sound.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CTitle::CTitle()
{
	// 値のクリア
	m_nSwitchWave = 0;		// ウェーブの遷移
	m_nCntSwitch = 0;		// 切り替えのカウンター
	m_bStageSwitch = false;		// ステージ切り替えのフラグ
}

//==========================================================================
// デストラクタ
//==========================================================================
CTitle::~CTitle()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTitle::Init(void)
{

	// BGM再生
	CManager::GetSound()->PlaySound(CSound::LABEL_BGM_TITLE);

	//int nMax = CScene::GetWaveManager()->GetMaxStage() - 1;
	// 最初のステージランダム
	m_nSwitchWave = Random(0, 2);

	// 初期化処理
	if (FAILED(CScene::Init()))
	{// 失敗した場合
		return E_FAIL;
	}

	// タイトル画面
	CTitleScreen::Create();

	// 敵配置
	CScene::GetEnemyManager()->SetSpawnWave(m_nSwitchWave, 0);
	m_bStageSwitch = false;		// ステージ切り替えのフラグ
	m_nCntSwitch = 0;		// 切り替えのカウンター

	// ステージ切り替え生成
	CStageSwitch::Create();

	// 成功
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTitle::Uninit(void)
{
	// 終了処理
	CScene::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTitle::Update(void)
{
	CManager::GetDebugProc()->Print(
		"現在のモード：【タイトル】\n"
		"切り替え：【 F 】\n\n");

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	// 切り替えのカウンター加算
	m_nCntSwitch++;

	if (pInputKeyboard->GetTrigger(DIK_RETURN) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true)
	{
		// モード設定
		CManager::GetFade()->SetFade(CScene::MODE_TUTORIAL);
	}

	if (m_nCntSwitch >= 60 * 40)
	{// 自動遷移

		// モード設定
		CManager::GetFade()->SetFade(CScene::MODE_RANKING);
	}

//#if _DEBUG
	if (pInputKeyboard->GetTrigger(DIK_R) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true)
	{

		// モード設定
		CManager::GetFade()->SetFade(CScene::MODE_RANKING);
	}
//#endif

	if (CScene::GetWaveManager()->IsStageFrag() == true)
	{// ステージ切り替えが終わっていたら

		if (m_bStageSwitch == true)
		{// 終わっていたら

			// 切り替え判定オフ
			m_bStageSwitch = false;

			// 終了判定にする
			CScene::GetWaveManager()->SetDisableStageEnd();

			// ステージ切り替え
			m_nSwitchWave = (m_nSwitchWave + 1) % CScene::GetWaveManager()->GetMaxStage();


			// 先頭を保存
			CObject *pObj = CObject::GetTop(mylib_const::ENEMY_PRIORITY);

			while (pObj != NULL)
			{// NULLが来るまで無限ループ

				// 次のオブジェクトを一時保存
				CObject *pObjNext = pObj->GetNext();

				// 死亡の判定
				if (pObj->IsDeath() == true)
				{// 死亡フラグが立っていたら

					// 次のオブジェクトを代入
					pObj = pObjNext;
					continue;
				}

				// 種類の取得
				if (pObj->GetType() != CObject::TYPE_ENEMY)
				{// 敵じゃなかったら

					// 次のオブジェクトを代入
					pObj = pObjNext;
					continue;
				}

				// 状態設定
				pObj->GetObjectChara()->SetState(CEnemy::STATE_FADEOUT);

				// 次のオブジェクトを代入
				pObj = pObjNext;
			}


			// 敵配置
			CScene::GetEnemyManager()->SetSpawnWave(m_nSwitchWave, 0);

			// ステージ切り替え生成
			CStageSwitch::Create();
		}
	}

}

//==========================================================================
// 描画処理
//==========================================================================
void CTitle::Draw(void)
{

}

//==========================================================================
// ステージの切り替え設定
//==========================================================================
void CTitle::SetStageSwitch(void)
{
	m_bStageSwitch = true;
}

//==========================================================================
// ウェーブ遷移取得
//==========================================================================
int CTitle::GetSwitchWave(void)
{
	return m_nSwitchWave;
}
