//=============================================================================
// 
//  プレイヤー処理 [player.cpp]
//  Author : 相馬靜雅
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

// 派生先
#include "tutorialplayer.h"
#include "resultplayer.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define CHARAFILE		"data\\TEXT\\motion_set_player.txt"
#define JUMP			(20.0f * 1.5f)	// ジャンプ力初期値
#define MAX_LIFE		(20)			// 体力
#define TARGET_LEN		(400.0f)		// 目標までの距離
#define WALK_INT		(30)			// 歩行の時間
#define INVINCIBLE_INT	(2)				// 無敵の間隔
#define INVINCIBLE_TIME	(90)			// 無敵の時間
#define CONFUSIONTIME	(60 * 2)		// 混乱時間

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CPlayer::CPlayer(int nPriority) : CObjectChara(nPriority)
{
	// 値のクリア
	m_state = STATE_NONE;	// 状態
	m_Oldstate = m_state;	// 前回の状態
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// マテリアルの色
	m_rotConfusion = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 混乱の向き
	m_fMoveSlide = 0.0f;			// スライド移動量
	m_fMoveSlideDest = 0.0f;		// 目標のスライド移動量
	m_fMoveRatio = 0.0f;			// 移動の割合
	m_bJump = false;				// ジャンプ中かどうか
	m_bKnockback = false;			// ノックバック中かどうか
	m_bMove = false;				// 移動中かどうか
	m_bATK = false;					// 攻撃中かどうか
	m_bMWait = false;				// 魔法待機中かどうか
	m_bCharge = false;				// チャージ中かどうか
	m_bConfusion = false;			// 混乱中かどうか
	m_bInWater = false;				// 水の中にいる判定
	m_nCntState = 0;				// 状態遷移カウンター
	m_nCntWalk = 0;					// 歩行カウンター
	m_nCntConfusion = 0;			// 混乱カウンター
	m_nLife = 0;					// 体力
	m_nTexIdx = 0;					// テクスチャのインデックス番号
	m_nIdxXFile = 0;				// Xファイルのインデックス番号
	m_walkType = WALKTYPE_NORMAL;	// 移動種類
	m_pMotion = NULL;				// モーションの情報
	m_pShadow = NULL;				// 影の情報
	m_pMCircle = NULL;				// 魔法陣の情報
	m_pTargetP = NULL;				// 目標の地点
	m_pHPGauge = NULL;				// HPゲージの情報
	m_pDelusionWindow = NULL;		// 妄想ウィンドウ
	m_pConfusionEffect = NULL;		// 混乱エフェクトの情報

#if ACTION_MODE
	m_Shotmode = SHOTMODE_TRIGGER;	// 射撃の種類

#else
	m_Shotmode = SHOTMODE_NONE;	// 射撃の種類
#endif
}

//==========================================================================
// デストラクタ
//==========================================================================
CPlayer::~CPlayer()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CPlayer *CPlayer::Create(void)
{
	// 生成用のオブジェクト
	CPlayer *pPlayer = NULL;

	if (pPlayer == NULL)
	{// NULLだったら

		// メモリの確保
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
		{// メモリの確保が出来ていたら

			// 初期化処理
			pPlayer->Init();
		}

		return pPlayer;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CPlayer::Init(void)
{
	// 種類の設定
	SetType(TYPE_PLAYER);

	m_state = STATE_NONE;	// 状態
	m_nCntState = 0;		// 状態遷移カウンター
	m_nLife = MAX_LIFE;		// 体力
	m_Shotmode = ATK_DEBUG_POWER;

	// キャラ作成
	HRESULT hr = SetCharacter(CHARAFILE);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// モーションの生成処理
	m_pMotion = CMotion::Create();

	// モーションのファイル読み込み
	m_pMotion->ReadText(CHARAFILE);

	// オブジェクトキャラクターの情報取得
	CObjectChara *pObjChar = GetObjectChara();

	// モーションの設定
	m_pMotion->SetModel(pObjChar->GetModel(), pObjChar->GetNumModel(), CObjectChara::GetObjectChara());

	// 体力ゲージ
	//m_pHPGauge = CHP_Gauge::Create(120.0f, m_nLife);

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 影の生成
	m_pShadow = CShadow::Create(pos, 50.0f);

	// ポーズのリセット
	m_pMotion->ResetPose(MOTION_DEF);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CPlayer::Uninit(void)
{
	if (m_pMotion != NULL)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// 魔法陣を消す
	if (m_pMCircle != NULL)
	{
		m_pMCircle->Uninit();
		m_pMCircle = NULL;
	}

	// HPゲージを消す
	if (m_pHPGauge != NULL)
	{
		//m_pHPGauge->Uninit();
		m_pHPGauge = NULL;
	}

	// 妄想ウィンドウを消す
	if (m_pDelusionWindow != NULL)
	{
		//m_pDelusionWindow->Uninit();
		m_pDelusionWindow = NULL;
	}

	// 影を消す
	if (m_pShadow != NULL)
	{
		//m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	// 目標の地点消す
	if (m_pTargetP != NULL)
	{
		m_pTargetP->Uninit();
		m_pTargetP = NULL;
	}

	// 混乱エフェクト消す
	if (m_pConfusionEffect != NULL)
	{
		m_pConfusionEffect = NULL;
	}

	// 終了処理
	CObjectChara::Uninit();

	// モード別終了処理
	UninitByMode();
}

//==========================================================================
// モード別終了処理
//==========================================================================
void  CPlayer::UninitByMode(void)
{
	CScene *pScene = CManager::GetScene();
	if (pScene != NULL)
	{
		// プレイヤー情報
		//CPlayer **ppPlayer = CGame::GetPlayer(0);
		CPlayer **ppPlayer = CManager::GetScene()->GetPlayer(0);

		// プレイヤーをNULL
		*ppPlayer = NULL;
	}
}

//==========================================================================
// 終了処理
//==========================================================================
void CPlayer::Kill(void)
{

	my_particle::Create(GetPosition(), my_particle::TYPE_OFFSETTING);

	// HPゲージを消す
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->Uninit();
		m_pHPGauge = NULL;
	}

	// 妄想ウィンドウを消す
	if (m_pDelusionWindow != NULL)
	{
		m_pDelusionWindow->Uninit();
		m_pDelusionWindow = NULL;
	}

	// 影を消す
	if (m_pShadow != NULL)
	{
		m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	// 混乱エフェクト消す
	if (m_pConfusionEffect != NULL)
	{
		m_pConfusionEffect->Uninit();
		m_pConfusionEffect = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CPlayer::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();
	if (pInputKeyboard->GetTrigger(DIK_F5) == true)
	{// F5でリセット
		SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1000.0f));
		SetMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}


	// エディット中は抜ける
	if (CGame::GetObject3DMesh()->IsEdit())
	{
		return;
	}

	// コンボ演出中は抜ける
	if (CGame::GetEnemyManager()->GetState() == CEnemyManager::STATE_COMBOANIM)
	{
		return;
	}


	// 過去の位置保存
	SetOldPosition(GetPosition());

	// 操作
	Controll();

	// モーションの設定処理
	MotionSet();

	// モーションの更新処理
	m_pMotion->Update();

	// 頂点情報設定
	SetVtx();

	// 攻撃処理
	Atack();

	// 状態更新
	UpdateState();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 影の位置更新
	if (m_pShadow != NULL)
	{
		m_pShadow->SetPosition(D3DXVECTOR3(pos.x, m_pShadow->GetPosition().y, pos.z));
	}

	// HPゲージの位置更新
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->UpdatePosition(GetPosition(), m_nLife);
	}

	// デバッグ表示
	CManager::GetDebugProc()->Print(
		"------------------[プレイヤーの操作]------------------\n"
		"移動：【X：%f, Y：%f, Z：%f】 【W / A / S / D】\n"
		"向き：【X：%f, Y：%f, Z：%f】 【Z / C】\n"
		"移動量：【X：%f, Y：%f, Z：%f】\n", pos.x, pos.y, pos.z, rot.x, rot.y, rot.y, move.x, move.y, move.z);
}

//==========================================================================
// 操作処理
//==========================================================================
void CPlayer::Controll(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	// カメラの情報取得
	CCamera *pCamera = CManager::GetCamera();

	// カメラの向き取得
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// 現在の種類取得
	int nType = m_pMotion->GetType();

	// 移動量取得
	float fMove = GetVelocity();

	// 加速の割合取得
	m_fMoveRatio = pInputGamepad->GetStickPositionRatioL(0).y;


#if 0
	// 煙
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
		// セットアップ位置設定
		pEffect->SetUp(D3DXVECTOR3(50.0f, 150.0f, 50.0f), CObject::GetObject());
	}
#endif

	static int nWalk = 0;
	if (m_bInWater == false && (nType == MOTION_WALK || nType == MOTION_SPEEDWALK || nType == MOTION_SLOWWALK))
	{// 歩行中

		if (m_fMoveSlideDest >= SLIDEMOVE * 0.35f)
		{// 加速中
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
			// 歩行音再生
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
	{// 移動可能モーションの時 && 混乱じゃないとき

#ifndef AUTO_MOVE
		// 移動中にする
		if (CManager::GetScene()->GetWaveManager()->IsStageFrag() == true)
		{
			m_bMove = true;
		}
#endif

#if AUTO_MOVE
		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//←キーが押された,左移動

			// 移動中にする
			m_bMove = true;

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//A+W,左上移動

				move.x += sinf(-D3DX_PI * 0.25f + Camerarot.y) * fMove;
				move.z += cosf(-D3DX_PI * 0.25f + Camerarot.y) * fMove;
				fRotDest = D3DX_PI * 0.75f/* + rot.y*/ + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//A+S,左下移動

				move.x += sinf(-D3DX_PI * 0.75f + Camerarot.y) * fMove;
				move.z += cosf(-D3DX_PI * 0.75f + Camerarot.y) * fMove;
				fRotDest = D3DX_PI * 0.25f /*+ rot.y */ + Camerarot.y;
			}
			else
			{//A,左移動

				move.x += sinf(-D3DX_PI * 0.5f + Camerarot.y) * fMove;
				move.z += cosf(-D3DX_PI * 0.5f + Camerarot.y) * fMove;
				fRotDest = D3DX_PI * 0.5f/* + rot.y*/ + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//Dキーが押された,右移動

			// 移動中にする
			m_bMove = true;

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//D+W,右上移動

				move.x += sinf(D3DX_PI * 0.25f + Camerarot.y) * fMove;
				move.z += cosf(D3DX_PI * 0.25f + Camerarot.y) * fMove;
				fRotDest = -D3DX_PI * 0.75f/* + rot.y*/ + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//D+S,右下移動

				move.x += sinf(D3DX_PI * 0.75f + Camerarot.y) * fMove;
				move.z += cosf(D3DX_PI * 0.75f + Camerarot.y) * fMove;
				fRotDest = -D3DX_PI * 0.25f/* + rot.y*/ + Camerarot.y;
			}
			else
			{//D,右移動

				move.x += sinf(D3DX_PI * 0.5f + Camerarot.y) * fMove;
				move.z += cosf(D3DX_PI * 0.5f + Camerarot.y) * fMove;
				fRotDest = -D3DX_PI * 0.5f /*+ rot.y*/ + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
		{//Wが押された、上移動

			// 移動中にする
			m_bMove = true;

			move.x += sinf(D3DX_PI * 0.0f + Camerarot.y) * fMove;
			move.z += cosf(D3DX_PI * 0.0f + Camerarot.y) * fMove;
			fRotDest = D3DX_PI * 1.0f/* + rot.y */ + Camerarot.y;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
		{//Sが押された、下移動

			// 移動中にする
			m_bMove = true;

			move.x += sinf(D3DX_PI * 1.0f + Camerarot.y) * fMove;
			move.z += cosf(D3DX_PI * 1.0f + Camerarot.y) * fMove;
			fRotDest = D3DX_PI * 0.0f /*+ rot.y */ + Camerarot.y;
		}
		else
		{
			// 移動中かどうか
			m_bMove = false;
		}
#else

		//*************************************************
		// 向きの変更
		//*************************************************
		// スティックの向き取得
		float fStickRot = pInputGamepad->GetStickRotR(0);

		if (fStickRot > D3DX_PI * 0.75f || fStickRot < -D3DX_PI * 0.75f)
		{// 真後ろは無し
			fStickRot = 0.0f;
		}

		fRotDest = D3DX_PI + fStickRot + Camerarot.y;

		// 目標の向き設定
		SetRotDest(fRotDest);

		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//←キーが押された,左移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//A+W,左上移動
				fRotDest = D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//A+S,左下移動
				fRotDest = D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//A,左移動
				fRotDest = D3DX_PI * 0.5f + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//Dキーが押された,右移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//D+W,右上移動
				fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//D+S,右下移動
				fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//D,右移動
				fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
			}
		}
		//else if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
		//{//Wが押された、上移動
		//	fRotDest = D3DX_PI * 1.0f + Camerarot.y;
		//}
		//else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
		//{//Sが押された、下移動
		//	fRotDest = D3DX_PI * 0.0f + Camerarot.y;
		//}

#endif

		// 加速度設定
		if (pInputKeyboard->GetPress(DIK_W) == true)
		{// 上移動

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
		{// 下移動

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
		{//SPACEが押された,ジャンプ

#if 0
			// 妄想ウィンドウ生成, 破棄
			if (m_pDelusionWindow == NULL)
			{
				m_pDelusionWindow = CDelusionWindow::Create();
				CDelusionMeter::Create();
			}
#endif
		}

		// チャージ
#if 0
		if (nType == MOTION_CHARGE)
		{// チャージの時

			if (m_pMotion->GetAllCount() <= 20)
			{// 始めたては強制移動制限
				m_bMove = false;	// 移動判定OFF
				move.x = 0.0f;
				move.z = 0.0f;
			}

			if (CGame::GetPowerGauge()->GetLevel() == CPowerGauge::LEVEL_MAX)
			{
				m_bCharge = false;	// チャージOFF
			}

			float fMove = 0.0f;		// 移動量
			float fMoveY = 0.0f;	// 移動量

			switch (CGame::GetPowerGauge()->GetLevel())
			{
			case CPowerGauge::LEVEL_1:

				fMove = (float)Random(0, 150) * 0.01f;	// 移動量
				fMoveY = (float)Random(200, 350) * 0.1f;	// 移動量

				// エフェクトの設定
				CEffect3D::Create(
					pos,
					D3DXVECTOR3(sinf((float)Random(-314, 314) * 0.01f) * fMove, sinf((float)Random(0, 314) * 0.01f) * fMoveY, cosf((float)Random(-314, 314) * 0.01f) * fMove),
					D3DXCOLOR(0.2f, 0.2f, 1.0f, 0.6f),
					60.0f,
					10,
					CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_SMOKE);
				break;

			case CPowerGauge::LEVEL_2:

				fMove = (float)Random(0, 150) * 0.01f;	// 移動量
				fMoveY = (float)Random(200, 350) * 0.1f;	// 移動量

				// エフェクトの設定
				CEffect3D::Create(
					pos,
					D3DXVECTOR3(sinf((float)Random(-314, 314) * 0.01f) * fMove, sinf((float)Random(0, 314) * 0.01f) * fMoveY, cosf((float)Random(-314, 314) * 0.01f) * fMove),
					D3DXCOLOR(0.2f, 1.0f, 0.2f, 0.4f),
					80.0f,
					15,
					CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_SMOKE);
				break;

			case CPowerGauge::LEVEL_3:

				fMove = (float)Random(0, 150) * 0.01f;	// 移動量
				fMoveY = (float)Random(200, 350) * 0.1f;	// 移動量

				// エフェクトの設定
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
	{// 移動可能モーションじゃない場合
#if 1
		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//←キーが押された,左移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//A+W,左上移動
				fRotDest = D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//A+S,左下移動
				fRotDest = D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//A,左移動
				fRotDest = D3DX_PI * 0.5f + Camerarot.y;
			}
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//Dキーが押された,右移動

			if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
			{//D+W,右上移動
				fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
			{//D+S,右下移動
				fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
			}
			else
			{//D,右移動
				fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
			}
		}
		//else if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
		//{//Wが押された、上移動
		//	fRotDest = D3DX_PI * 1.0f + Camerarot.y;
		//}
		//else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
		//{//Sが押された、下移動
		//	fRotDest = D3DX_PI * 0.0f + Camerarot.y;
		//}
#endif
	}

	if (/*m_state == STATE_CONFUSION ||*/ m_state == STATE_DMG || m_state == STATE_KNOCKBACK)
	{// 吹っ飛び状態のとき
		fRotDest = rot.y;
	}

	//*******************************
	// 左右で武器チェンジ
	//*******************************
#if 0
	if (pInputKeyboard->GetTrigger(DIK_LEFT) == true)
	{// ←が押された、モード変更

		// 魔法陣を消す
		if (m_pMCircle != NULL)
		{
			m_pMCircle->Uninit();
			m_pMCircle = NULL;
		}

		// 目標の地点消す
		if (m_pTargetP != NULL)
		{
			m_pTargetP->Uninit();
			m_pTargetP = NULL;
		}
		m_bMWait = false;		// 魔法待機OFF

		m_Shotmode = (m_Shotmode + 1) % ATK_DEBUG_MAX;

		// リセット
		for (int nCntParts = 16; nCntParts < GetObjectChara()->GetNumModel(); nCntParts++)
		{
			ChangeObject(nCntParts, -1);
		}

		// 武器変える
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
	{// →が押された、モード変更

		// 魔法陣を消す
		if (m_pMCircle != NULL)
		{
			m_pMCircle->Uninit();
			m_pMCircle = NULL;
		}

		// 目標の地点消す
		if (m_pTargetP != NULL)
		{
			m_pTargetP->Uninit();
			m_pTargetP = NULL;
		}
		m_bMWait = false;		// 魔法待機OFF

		m_Shotmode = (m_Shotmode + (ATK_DEBUG_MAX - 1)) % ATK_DEBUG_MAX;

		// リセット
		for (int nCntParts = 16; nCntParts < GetObjectChara()->GetNumModel(); nCntParts++)
		{
			ChangeObject(nCntParts, -1);
		}

		// 武器変える
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
		// デバッグ表記
		CManager::GetDebugProc()->Print("攻撃の種類変更：【パワー+3】 変更：【SPACE】\n", m_Shotmode);
		break;

	case ATK_DEBUG_POWER2_SPEED1:
		// デバッグ表記
		CManager::GetDebugProc()->Print("攻撃の種類変更：【パワー+2, スピード+1】 変更：【SPACE】\n", m_Shotmode);
		break;

	case ATK_DEBUG_POWER2_CROWD1:
		// デバッグ表記
		CManager::GetDebugProc()->Print("攻撃の種類変更：【パワー+2, 範囲+1】 変更：【SPACE】\n", m_Shotmode);
		break;

	case ATK_DEBUG_CROWD:
		// デバッグ表記
		CManager::GetDebugProc()->Print("攻撃の種類変更：【範囲+3】 変更：【SPACE】\n", m_Shotmode);
		break;

	case ATK_DEBUG_CROWD2_POWER1:
		// デバッグ表記
		CManager::GetDebugProc()->Print("攻撃の種類変更：【範囲+2, パワー+1】 変更：【SPACE】\n", m_Shotmode);
		break;

	case ATK_DEBUG_CROWD2_SPEED1:
		// デバッグ表記
		CManager::GetDebugProc()->Print("攻撃の種類変更：【範囲+2, スピード+1】 変更：【SPACE】\n", m_Shotmode);
		break;

	case ATK_DEBUG_SPEED:
		// デバッグ表記
		CManager::GetDebugProc()->Print("攻撃の種類変更：【スピード+3】 変更：【SPACE】\n", m_Shotmode);
		break;

	case ATK_DEBUG_SPEED2_POWER1:
		// デバッグ表記
		CManager::GetDebugProc()->Print("攻撃の種類変更：【スピード+2, パワー+1】 変更：【SPACE】\n", m_Shotmode);
		break;

	case ATK_DEBUG_SPEED2_CROWD1:
		// デバッグ表記
		CManager::GetDebugProc()->Print("攻撃の種類変更：【スピード+2, 範囲+1】 変更：【SPACE】\n", m_Shotmode);
		break;
	}
	CManager::GetDebugProc()->Print(
		"\n"
		"====================================================================\n"
		"地形破壊：【O or Y】\n"
		"====================================================================\n\n", m_Shotmode);

	// 魔法の攻撃
#if 0

	if ((pInputKeyboard->GetTrigger(DIK_RETURN) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true) &&
		m_bATK == false && nType != MOTION_DIRECTATK && nType != MOTION_SUPERATK)
	{// 攻撃

		// 攻撃タイプ変更
		m_Shotmode = ATK_DEBUG_POWER;
		m_bATK = true;		// 攻撃判定ON

		// 目標の距離設定
		pCamera->SetLenDest(pCamera->GetOriginDistance() + 200.0f);

		if (m_Shotmode == ATK_DEBUG_CROWD && nType != MOTION_MAGIC_ICE)
		{// 魔法の時

			// 目標の地点生成
			m_pTargetP = CTargetPoint::Create(
				D3DXVECTOR3(pos.x + sinf(D3DX_PI + GetRotation().y) * TARGET_LEN, pos.y, pos.z + cosf(D3DX_PI + GetRotation().y) * TARGET_LEN),
				400.0f,
				800.0f);

			m_bMWait = true;		// 魔法待機ON
			m_bMove = false;		// 移動OFF

			// 待機モーション
			m_pMotion->Set(MOTION_MAGIC_WAIT);
		}
		else if (m_Shotmode == ATK_DEBUG_CROWD2_POWER1 && nType != MOTION_MAGIC_EXPLOSION)
		{// 爆発魔法の時

			// 目標の地点生成
			m_pTargetP = CTargetPoint::Create(
				D3DXVECTOR3(pos.x + sinf(D3DX_PI + GetRotation().y) * TARGET_LEN, pos.y, pos.z + cosf(D3DX_PI + GetRotation().y) * TARGET_LEN),
				200.0f, 800.0f);

			m_bMWait = true;		// 魔法待機ON
			m_bMove = false;		// 移動OFF

			// 待機モーション
			m_pMotion->Set(MOTION_MAGIC_WAIT);
		}
		else if (m_Shotmode == ATK_DEBUG_POWER2_SPEED1 && nType != MOTION_CLOWWAIT)
		{// 爪の時

			m_bMWait = true;		// 魔法待機ON
			m_bMove = false;		// 移動OFF
			//m_bATK = true;		// 攻撃判定ON

			// 待機モーション
			//m_pMotion->Set(MOTION_CLOWWAIT);
		}
		else if (m_Shotmode == ATK_DEBUG_POWER)
		{
			//if (m_bCharge == false && CGame::GetPowerGauge()->GetLevel() != CPowerGauge::LEVEL_MAX)
			//{// チャージしてないときだけ
			//	m_bMove = false;	// 移動判定OFF
			//	m_bCharge = true;
			//}
			//else if (m_bCharge == false && CGame::GetPowerGauge()->GetLevel() == CPowerGauge::LEVEL_MAX)
			//{
			//	m_bATK = true;		// 攻撃判定ON
			//	m_bCharge = false;	// チャージOFF
			//}
			m_bATK = true;		// 攻撃判定ON
			m_bCharge = false;	// チャージOFF

			//if (m_bCharge == true && nType == MOTION_CHARGE)
			{// チャージ中の時

				m_bATK = true;		// 攻撃判定ON
				m_bCharge = false;	// チャージOFF
			}

		}
		else
		{
			// 攻撃判定ON
			m_bATK = true;
		}

		if (m_Shotmode == ATK_DEBUG_SPEED)
		{// ラッシュの時

			// 魔法陣生成
			m_pMCircle = CMagicCircle::Create(
				D3DXVECTOR3(pos.x, pos.y + 150.0f, pos.z),
				D3DXVECTOR3(-0.5f, rot.y, 0.0f),
				D3DXVECTOR3(100.0f, 100.0f, 0.0f));
		}
	}


	if (m_Shotmode == ATK_DEBUG_SPEED || m_Shotmode == ATK_DEBUG_CROWD || m_Shotmode == ATK_DEBUG_CROWD2_POWER1 || m_Shotmode == ATK_DEBUG_POWER2_SPEED1)
	{// ラッシュの時

		if (pInputKeyboard->GetPress(DIK_RETURN) == true)
		{// 攻撃

			if (m_Shotmode == ATK_DEBUG_SPEED)
			{// スピード系の時だけ

				// ラッシュ生成
				if (nType == MOTION_SPEED_RUSH_MOVE)
				{// 移動ラッシュ中
					CSpeedRush::Create(pos, rot, 1);
				}
				else
				{
					CSpeedRush::Create(pos, rot, 3);
				}

				// 攻撃判定ON
				m_bATK = true;
			}

			if (m_Shotmode == ATK_DEBUG_POWER2_SPEED1)
			{
				// 攻撃判定ON
				m_bATK = true;
			}
		}

		if (pInputKeyboard->GetRelease(DIK_RETURN) == true)
		{// Enterが離されたら

			// 魔法陣を消す
			if (m_pMCircle != NULL)
			{
				m_pMCircle->Uninit();
				m_pMCircle = NULL;
			}

			if (nType == MOTION_MAGIC_WAIT)
			{// 魔法の時

				// 攻撃判定ON
				m_bATK = true;

				// 待機判定OFF
				m_bMWait = false;
			}

			if (nType == MOTION_CLOWATK)
			{// 爪攻撃の時

				m_bATK = false;	// 攻撃判定OFF
			}
		}
	}

	// 魔法陣更新
	if (m_pMCircle != NULL)
	{// NULLじゃなかったら

		// 位置・向き取得
		D3DXVECTOR3 MagisSRot = m_pMCircle->GetRotation();
		m_pMCircle->SetRotation(D3DXVECTOR3(MagisSRot.x, rot.y, MagisSRot.z));
		m_pMCircle->SetPosition(D3DXVECTOR3(pos.x, pos.y + 150.0f, pos.z));
	}

	// 目標地点更新
	if (m_pTargetP != NULL)
	{
		// 向き取得
		D3DXVECTOR3 TargetRot = m_pTargetP->GetRotation();
		m_pTargetP->SetRotation(D3DXVECTOR3(TargetRot.x, rot.y, TargetRot.z));
	}

#else

#endif

	// 移動の割合で移動種類変える
	if (m_fMoveRatio >= 0.5f)
	{
		// 高速移動
		m_walkType = WALKTYPE_SPEED;
	}
	else if (m_fMoveRatio <= -0.75f)
	{
		// 遅い移動
		m_walkType = WALKTYPE_SLOW;
	}
	else
	{
		// 通常移動
		m_walkType = WALKTYPE_NORMAL;
	}

	// 移動量設定
#ifndef AUTO_MOVE
	fMove = fMove + ((fMove * 0.5f) * m_fMoveRatio);
#else
	fMove = fMove + ((fMove * 0.5f) * 1.0f);
	fMove *= 1.5f;
#endif

	//角度の正規化
	RotNormalize(fRotDest);

	//現在と目標の差分を求める
	float fRotDiff = fRotDest - rot.y;

	//角度の正規化
	RotNormalize(fRotDiff);

	//角度の補正をする
#ifndef AUTO_MOVE
	if (m_state != STATE_CONFUSION)
	{// 混乱状態のとき
		// 歩行音再生
		CManager::GetSound()->StopSound(CSound::LABEL_SE_CONFUSION);

		rot.y += fRotDiff * 0.014f;
	}
	
#else
	rot.y += fRotDiff * 0.1f;
#endif

	//角度の正規化
	RotNormalize(rot.y);

	// スライド
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

	// 重力処理
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		move.y -= mylib_const::GRAVITY;
	}

	// 位置更新
	pos += move;

	// 慣性補正
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);

	//**********************************
	// 当たり判定
	//**********************************
	Collision();

	if (m_state != STATE_ATTACK && m_state != STATE_CONFUSION && m_state != STATE_DMG && m_state != STATE_KNOCKBACK &&
		CManager::GetScene()->GetWaveManager()->IsStageFrag() == true)
	{
		if (pInputKeyboard->GetTrigger(DIK_O) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_Y, 0) == true)
		{//←キーが押された,左移動

			// 攻撃タイプ変更
			m_Shotmode = ATK_DIRECT;
			m_bATK = true;		// 攻撃判定ON
		}

		if ((pInputKeyboard->GetTrigger(DIK_RETURN) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true) &&
			m_bATK == false && nType != MOTION_DIRECTATK && nType != MOTION_SUPERATK)
		{// 攻撃

			// 攻撃タイプ変更
			m_Shotmode = ATK_DEBUG_POWER;
			m_bATK = true;		// 攻撃判定ON

			// 目標の距離設定
			pCamera->SetLenDest(pCamera->GetOriginDistance() + 200.0f);
		}
	}
	else if(CManager::GetScene()->GetWaveManager()->IsStageFrag() == false)
	{// ステージ切り替え中

		m_bATK = false;		// 攻撃判定OFF
		m_bMove = false;	// 移動判定OFF

		// ニュートラルモーション設定
		m_pMotion->Set(MOTION_DEF);
	}

#if _DEBUG
	if (pInputKeyboard->GetTrigger(DIK_I) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_X, 0) == true)
	{//←キーが押された,左移動

		// 波発生
		CGame::GetObject3DMesh()->SetWave
		(
			D3DXVECTOR3(
				pos.x + sinf(D3DX_PI * 0.75f + GetRotation().y) * 800.0f,
				pos.y,
				pos.z + cosf(D3DX_PI * 0.75f + GetRotation().y) * 800.0f),	// 位置
			GetRotation(),										// 向き
			D3DXVECTOR3(
				0.0f,
				0.0f,
				0.0f),											// 移動量
			D3DXVECTOR2(200.0f, 200.0f),						// 判定範囲
			0.0f,							// 波発生の頻度
			180.0f,											// 波の高さ
			0.0f,												// 波の広がり
			60,													// 発生時間
			CElevation::WAVETYPE_RAISE,						// 種類
			CElevation::TYPE_SPAWN,							// プレイヤーの種類
			0												// レベル
		);

		// 波発生
		CGame::GetObject3DMesh()->SetWave
		(
			D3DXVECTOR3(
				pos.x + sinf(-D3DX_PI * 0.75f + GetRotation().y) * 800.0f,
				pos.y,
				pos.z + cosf(-D3DX_PI * 0.75f + GetRotation().y) * 800.0f),	// 位置
			GetRotation(),									// 向き
			D3DXVECTOR3(
				sinf(D3DX_PI + GetRotation().y) * 10.0f,
				0.0f,
				cosf(D3DX_PI + GetRotation().y) * 10.0f),					// 移動量
			D3DXVECTOR2(200.0f, 300.0f),									// 判定範囲
			CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// 波発生の頻度
			-300.0f,														// 波の高さ
			D3DX_PI * 0.0f,													// 波打つ頻度
			150,															// 発生時間
			CElevation::WAVETYPE_DIRECT,										// 種類
			CElevation::TYPE_NATURAL,								// プレイヤーの種類
			CPowerGauge::LEVEL_1
		);
	}
#endif
}

//==========================================================================
// 武器チェンジ処理
//==========================================================================
void CPlayer::ChangeWeapon(const int *pPoint)
{
	// 手持ち武器リセット
	for (int nCntParts = 16; nCntParts < GetObjectChara()->GetNumModel(); nCntParts++)
	{
		ChangeObject(nCntParts, -1);
	}

	// 妄想絵ウィンドウを消す
	if (m_pDelusionWindow != NULL)
	{// 使われていたら
		m_pDelusionWindow->Uninit();
		m_pDelusionWindow = NULL;
	}

	// 魔法陣を消す
	if (m_pMCircle != NULL)
	{
		m_pMCircle->Uninit();
		m_pMCircle = NULL;
	}

	// 目標の地点消す
	if (m_pTargetP != NULL)
	{
		m_pTargetP->Uninit();
		m_pTargetP = NULL;
	}
	m_bMWait = false;		// 魔法待機OFF

	//*****************************
	// パワー系
	//*****************************
	if (pPoint[CDelusionMeter::TYPE_POWER] == 4)
	{// パワー3
		m_Shotmode = ATK_DEBUG_POWER;
		SetObject(16);
	}
	else if (pPoint[CDelusionMeter::TYPE_POWER] == 3 && pPoint[CDelusionMeter::TYPE_SPEED] == 2)
	{// パワー2, スピード1
		m_Shotmode = ATK_DEBUG_POWER2_SPEED1;
		SetObject(18);
		SetObject(19);
	}
	else if (pPoint[CDelusionMeter::TYPE_POWER] == 3 && pPoint[CDelusionMeter::TYPE_CROWD] == 2)
	{// パワー2, 群体1
		m_Shotmode = ATK_DEBUG_POWER2_CROWD1;
		SetObject(20);
	}
	//*****************************
	// スピード系
	//*****************************
	else if (pPoint[CDelusionMeter::TYPE_SPEED] == 4)
	{// スピード3
		m_Shotmode = ATK_DEBUG_SPEED;
	}
	else if (pPoint[CDelusionMeter::TYPE_SPEED] == 3 && pPoint[CDelusionMeter::TYPE_POWER] == 2)
	{// スピード2, パワー1
		m_Shotmode = ATK_DEBUG_SPEED2_POWER1;
	}
	else if (pPoint[CDelusionMeter::TYPE_SPEED] == 3 && pPoint[CDelusionMeter::TYPE_CROWD] == 2)
	{// スピード2, 群体1
		m_Shotmode = ATK_DEBUG_SPEED2_CROWD1;
	}
	//*****************************
	// 群体系
	//*****************************
	else if (pPoint[CDelusionMeter::TYPE_CROWD] == 4)
	{// 群体3
		m_Shotmode = ATK_DEBUG_CROWD;
		SetObject(17);
	}
	else if (pPoint[CDelusionMeter::TYPE_CROWD] == 3 && pPoint[CDelusionMeter::TYPE_POWER] == 2)
	{// 群体2, パワー1
		m_Shotmode = ATK_DEBUG_CROWD2_POWER1;
	}
	else if (pPoint[CDelusionMeter::TYPE_CROWD] == 3 && pPoint[CDelusionMeter::TYPE_SPEED] == 2)
	{// 群体2, スピード1
		m_Shotmode = ATK_DEBUG_CROWD2_SPEED1;
	}
	else
	{
		m_Shotmode = ATK_DEBUG_POWER1;
	}
}

//==========================================================================
// モーションの設定
//==========================================================================
void CPlayer::MotionSet(void)
{
	if (m_pMotion->IsFinish() == true)
	{// 終了していたら

		// 現在の種類取得
		int nType = m_pMotion->GetType();
		int nOldType = m_pMotion->GetOldType();

		if (m_bMove == true && m_bATK == false && m_bKnockback == false)
		{// 移動していたら

			m_bMove = false;	// 移動判定OFF
			m_bCharge = false;	// チャージ判定OFF

			// 移動モーション
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
		{// やられ中だったら

			// やられモーション
			m_pMotion->Set(MOTION_KNOCKBACK);
		}
		else if (m_state == STATE_CONFUSION)
		{// 混乱中だったら

			// やられモーション
			m_pMotion->Set(MOTION_CONFUSION);
		}
		else if (m_bATK == true)
		{// 攻撃していたら

			m_bATK = false;		// 攻撃判定OFF

			switch (m_Shotmode)
			{
			case ATK_DEBUG_POWER:

				m_pMotion->Set(MOTION_SUPERATK);
				break;

			case ATK_DIRECT:
				// 直行波攻撃
				m_pMotion->Set(MOTION_DIRECTATK);
				break;
			}
		}
		else
		{
			// ニュートラルモーション
			m_pMotion->Set(MOTION_DEF);
		}
	}
}

//==========================================================================
// 攻撃
//==========================================================================
void CPlayer::Atack(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// カメラの情報取得
	CCamera *pCamera = CManager::GetCamera();

	// カメラの向き取得
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// モーションの情報取得
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// 攻撃情報の総数取得
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	int nLevel = 0;
	//int nLevel = CGame::GetPowerGauge()->GetLevel();

	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (aInfo.AttackInfo[nCntAttack] == NULL)
		{// NULLだったら
			continue;
		}

		if (m_pMotion->GetAllCount() == aInfo.AttackInfo[nCntAttack]->nInpactCnt)
		{// 衝撃のカウントと同じとき

			D3DXMATRIX mtxTrans;	// 計算用マトリックス宣言

			if (GetObjectChara()->GetModel()[aInfo.AttackInfo[nCntAttack]->nCollisionNum] == NULL)
			{// NULLだったら
				return;
			}

			// 判定するパーツのマトリックス取得
			D3DXMATRIX mtxWepon = GetObjectChara()->GetModel()[aInfo.AttackInfo[nCntAttack]->nCollisionNum]->GetWorldMtx();

			// 位置を反映する
			D3DXMatrixTranslation(&mtxTrans, aInfo.AttackInfo[nCntAttack]->Offset.x, aInfo.AttackInfo[nCntAttack]->Offset.y, aInfo.AttackInfo[nCntAttack]->Offset.z);
			D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

			// 武器の位置
			D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

			// 種類別
			switch (m_pMotion->GetType())
			{
			case MOTION_SUPERATK:

				//// パーティクル生成
				//my_particle::Create(weponpos, my_particle::TYPE_SUPERATTACK);

				//// チャージカウントリセット
				////CGame::GetPowerGauge()->SetChargeCount(0);

				// 瓦礫生成
				CBallast::Create(weponpos, D3DXVECTOR3(2.0f, 8.0f, 2.0f), 16);

				// 衝撃波生成
				CImpactWave::Create
				(
					D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// 位置
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// 向き
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f),			// 色
					100.0f,										// 幅
					20.0f,										// 高さ
					20,											// 寿命
					28.0f,										// 幅の移動量
					CImpactWave::TYPE_BLACK2,					// テクスチャタイプ
					true										// 加算合成するか
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(pos.x, pos.y + 150.0f, pos.z),	// 位置
					D3DXVECTOR3(0.0f, 0.0f, D3DX_PI),				// 向き
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f),			// 色
					180.0f,										// 幅
					150.0f,										// 高さ
					14,											// 寿命
					4.0f,										// 幅の移動量
					CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
					false										// 加算合成するか
				);

				// 振動
				CManager::GetCamera()->SetShake(20, 10.0f, 0.0f);


				// 歩行音再生
				CManager::GetSound()->PlaySound(CSound::LABEL_SE_IMPACT01);

				// 波発生
				switch (nLevel)
				{
				case CPowerGauge::LEVEL_1:

					// 波発生
					CGame::GetObject3DMesh()->SetWave
					(
						pos,												// 位置
						GetRotation(),										// 向き
						D3DXVECTOR3(
							0.0f,
							0.0f,
							0.0f),											// 移動量
						D3DXVECTOR2(2000.0f, 2000.0f),						// 判定範囲
						CGame::GetObject3DMesh()->GetWidthLen() * 1.0f,		// 波発生の頻度
						-180.0f,											// 波の高さ
						12.5f,												// 波の広がり
						30,													// 発生時間
						CElevation::WAVETYPE_IMPACT,						// 種類
						CElevation::TYPE_PLAYER,							// プレイヤーの種類
						nLevel												// レベル
					);
					break;

				case CPowerGauge::LEVEL_2:

					// 波発生
					CGame::GetObject3DMesh()->SetWave
					(
						pos,												// 位置
						GetRotation(),										// 向き
						D3DXVECTOR3(
							0.0f,
							0.0f,
							0.0f),											// 移動量
						D3DXVECTOR2(2000.0f, 2000.0f),						// 判定範囲
						CGame::GetObject3DMesh()->GetWidthLen() * 1.0f,		// 波発生の頻度
						-180.0f,											// 波の高さ
						19.0f,												// 波の広がり
						30,													// 発生時間
						CElevation::WAVETYPE_IMPACT,						// 種類
						CElevation::TYPE_PLAYER,							// プレイヤーの種類
						nLevel												// レベル
					);
					break;

				case CPowerGauge::LEVEL_3:

					// 波発生
					CGame::GetObject3DMesh()->SetWave
					(
						pos,												// 位置
						GetRotation(),										// 向き
						D3DXVECTOR3(
							0.0f,
							0.0f,
							0.0f),											// 移動量
						D3DXVECTOR2(2000.0f, 2000.0f),						// 判定範囲
						CGame::GetObject3DMesh()->GetWidthLen() * 1.0f,		// 波発生の頻度
						-180.0f,											// 波の高さ
						30.0f,												// 波の広がり
						25,													// 発生時間
						CElevation::WAVETYPE_IMPACT,						// 種類
						CElevation::TYPE_PLAYER,							// プレイヤーの種類
						nLevel												// レベル
					);
					break;

				case CPowerGauge::LEVEL_MAX:
					// 波発生
					CGame::GetObject3DMesh()->SetWave
					(
						pos,												// 位置
						GetRotation(),										// 向き
						D3DXVECTOR3(
							0.0f,
							0.0f,
							0.0f),											// 移動量
						D3DXVECTOR2(2000.0f, 2000.0f),						// 判定範囲
						CGame::GetObject3DMesh()->GetWidthLen() * 1.5f,		// 波発生の頻度
						-200.0f,											// 波の高さ
						40.0f,												// 波の広がり
						30,													// 発生時間
						CElevation::WAVETYPE_IMPACT,						// 種類
						CElevation::TYPE_PLAYER,							// プレイヤーの種類
						nLevel												// レベル
					);
					break;
				}
				break;

			case MOTION_DIRECTATK:

				// 地面の判定
				CGame::GetObject3DMesh()->IsHit(weponpos);

				// 目標の距離設定
				pCamera->SetLenDest(pCamera->GetOriginDistance() + 100.0f);

				// 振動
				CManager::GetCamera()->SetShake(120, 8.0f, 0.0f);

				// 歩行音再生
				CManager::GetSound()->PlaySound(CSound::LABEL_SE_SWING);

				CImpactWave::Create
				(
					D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * 80.0f, pos.y + 50.0f, pos.z + cosf(D3DX_PI + rot.y) * 80.0f),	// 位置
					D3DXVECTOR3(D3DX_PI * 0.5f, rot.y, D3DX_PI),				// 向き
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.2f),			// 色
					80.0f,										// 幅
					80.0f,										// 高さ
					20,											// 寿命
					16.0f,										// 幅の移動量
					CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
					false										// 加算合成するか
				);

				// 波発生
				switch (nLevel)
				{
				case CPowerGauge::LEVEL_1:

					// 波発生
					CGame::GetObject3DMesh()->SetWave
					(
						D3DXVECTOR3(
							pos.x + sinf(D3DX_PI + rot.y) * 250.0f,
							pos.y,
							pos.z + cosf(D3DX_PI + rot.y) * 250.0f),	// 位置
						D3DXVECTOR3(
							0.0f,
							rot.y,
							0.0f),									// 向き
						D3DXVECTOR3(
							sinf(D3DX_PI + rot.y) * 25.0f,
							0.0f,
							cosf(D3DX_PI + rot.y) * 25.0f),					// 移動量
						D3DXVECTOR2(70.0f, 300.0f),									// 判定範囲
						CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// 波発生の頻度
						-300.0f,														// 波の高さ
						D3DX_PI * 0.0f,													// 波打つ頻度
						60,															// 発生時間
						CElevation::WAVETYPE_DIRECT,										// 種類
						CElevation::TYPE_PLAYER,								// プレイヤーの種類
						CPowerGauge::LEVEL_1
					);
					break;

				case CPowerGauge::LEVEL_2:

					// 波発生
					CGame::GetObject3DMesh()->SetWave
					(
						D3DXVECTOR3(
							pos.x + sinf(Camerarot.y) * 400.0f,
							pos.y,
							pos.z + cosf(Camerarot.y) * 400.0f),	// 位置
						D3DXVECTOR3(
							0.0f,
							D3DX_PI + Camerarot.y,
							0.0f),									// 向き
						D3DXVECTOR3(
							sinf(Camerarot.y) * 17.0f,
							0.0f,
							cosf(Camerarot.y) * 17.0f),					// 移動量
						D3DXVECTOR2(70.0f, 300.0f),									// 判定範囲
						CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// 波発生の頻度
						-375.0f,														// 波の高さ
						D3DX_PI * 0.0f,													// 波打つ頻度
						100,															// 発生時間
						CElevation::WAVETYPE_DIRECT,										// 種類
						CElevation::TYPE_PLAYER,								// プレイヤーの種類
						CPowerGauge::LEVEL_1
					);
					break;

				case CPowerGauge::LEVEL_3:

					// 波発生
					CGame::GetObject3DMesh()->SetWave
					(
						D3DXVECTOR3(
							pos.x + sinf(Camerarot.y) * 400.0f,
							pos.y,
							pos.z + cosf(Camerarot.y) * 400.0f),	// 位置
						D3DXVECTOR3(
							0.0f,
							D3DX_PI + Camerarot.y,
							0.0f),									// 向き
						D3DXVECTOR3(
							sinf(Camerarot.y) * 20.0f,
							0.0f,
							cosf(Camerarot.y) * 20.0f),					// 移動量
						D3DXVECTOR2(70.0f, 300.0f),									// 判定範囲
						CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// 波発生の頻度
						-450.0f,														// 波の高さ
						D3DX_PI * 0.0f,													// 波打つ頻度
						75,															// 発生時間
						CElevation::WAVETYPE_DIRECT,										// 種類
						CElevation::TYPE_PLAYER,								// プレイヤーの種類
						CPowerGauge::LEVEL_1
					);
					break;

				case CPowerGauge::LEVEL_MAX:

					// 波発生
					CGame::GetObject3DMesh()->SetWave
					(
						D3DXVECTOR3(
							pos.x + sinf(Camerarot.y) * 400.0f,
							pos.y,
							pos.z + cosf(Camerarot.y) * 400.0f),	// 位置
						D3DXVECTOR3(
							0.0f,
							D3DX_PI + Camerarot.y,
							0.0f),									// 向き
						D3DXVECTOR3(
							sinf(Camerarot.y) * 25.0f,
							0.0f,
							cosf(Camerarot.y) * 25.0f),				// 移動量
						D3DXVECTOR2(140.0f, 400.0f),									// 判定範囲
						CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// 波発生の頻度
						-500.0f,														// 波の高さ
						D3DX_PI * 0.0f,													// 波打つ頻度
						90,															// 発生時間
						CElevation::WAVETYPE_DIRECT,									// 種類
						CElevation::TYPE_PLAYER,										// プレイヤーの種類
						CPowerGauge::LEVEL_MAX											// レベル
					);
					break;
				}
				break;
			}
		}

		// モーションカウンター取得
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt && m_pMotion->GetAllCount() < aInfo.AttackInfo[nCntAttack]->nMaxCnt)
		{// 攻撃判定中

			// モデル情報取得
			CModel **pModel = GetObjectChara()->GetModel();
			D3DXMATRIX mtxTrans;	// 計算用マトリックス宣言

			// 武器のマトリックス取得
			D3DXMATRIX mtxWepon;
			D3DXMatrixIdentity(&mtxWepon);

			if (pModel[aInfo.AttackInfo[nCntAttack]->nCollisionNum] == NULL)
			{// NULLだったら
				return;
			}

			// 判定するパーツのマトリックス取得
			mtxWepon = pModel[aInfo.AttackInfo[nCntAttack]->nCollisionNum]->GetWorldMtx();

			// 位置を反映する
			D3DXMatrixTranslation(&mtxTrans, aInfo.AttackInfo[nCntAttack]->Offset.x, aInfo.AttackInfo[nCntAttack]->Offset.y, aInfo.AttackInfo[nCntAttack]->Offset.z);
			D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

			// 武器の位置
			D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

			CEffect3D::Create(weponpos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), aInfo.AttackInfo[nCntAttack]->fRangeSize * 0.5f, 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

#if TOPCUR

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

				// 敵の位置取得
				D3DXMATRIX mtxOrigin = pObj->GetObjectChara()->GetModel()[0]->GetWorldMtx();
				D3DXVECTOR3 TargetPos = D3DXVECTOR3(mtxOrigin._41, mtxOrigin._42, mtxOrigin._43);

				// 判定サイズ取得
				float fRadius = pObj->GetObjectChara()->GetRadius();

				if (SphereRange(weponpos, TargetPos, aInfo.AttackInfo[nCntAttack]->fRangeSize, fRadius))
				{// 球の判定

					if (pObj->Hit(aInfo.AttackInfo[nCntAttack]->nDamage) == true)
					{// 死んでたら

						my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
					}
				}

				// 次のオブジェクトを代入
				pObj = pObjNext;
			}
#endif
		}
	}

	CManager::GetDebugProc()->Print(
		"モーションカウンター：%d\n", m_pMotion->GetAllCount());
}

//==========================================================================
// 当たり判定
//==========================================================================
void CPlayer::Collision(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 着地したかどうか
	bool bLand = false;
	float fHeight = 0.0f;

	// 高さ取得
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		fHeight = CManager::GetScene()->GetObject3DMesh()->GetHeight(pos, move, bLand, GetType());
	}
	else
	{
		fHeight = pos.y;
	}

	//if (fHeight > pos.y)
	{// 地面の方が自分より高かったら

		// 地面の高さに補正
		pos.y = fHeight;

		if (bLand == true)
		{// 着地してたら

			// ジャンプ使用可能にする
			m_bJump = false;
			move.y = 0.0f;
		}
	}

	// 位置取得
	D3DXVECTOR3 posOld = GetPosition();

	// 箱
	float fLen = CGame::GetObject3DMesh()->GetWidthLen();
	int nBlock = CGame::GetObject3DMesh()->GetWidthBlock();
	nBlock /= 2;
	if (pos.x + GetRadius() >= fLen * nBlock) { pos.x = fLen * nBlock - GetRadius(); }
	if (pos.x - GetRadius() <= -fLen * nBlock) { pos.x = -fLen * nBlock + GetRadius(); }
	if (pos.z + GetRadius() >= fLen * nBlock) { pos.z = fLen * nBlock - GetRadius(); }
	if (pos.z - GetRadius() <= -fLen * nBlock) { pos.z = -fLen * nBlock + GetRadius(); }
	
	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// ヒット処理
//==========================================================================
bool CPlayer::Hit(const int nValue)
{
	if (m_state != STATE_DEAD && m_state != STATE_DMG && m_state != STATE_KNOCKBACK && m_state != STATE_INVINCIBLE && m_state != STATE_CONFUSION)
	{// ダメージ受付状態の時

		// 体力減らす
		//m_nLife -= nValue;

		if (m_nLife <= 0)
		{// 体力がなくなったら

			// 死状態
			m_state = STATE_DEAD;

			// 死亡処理
			Kill();

			// 終了処理
			Uninit();

			// 死んだ
			return true;
		}

		// 補正
		ValueNormalize(m_nLife, MAX_LIFE, 0);

		// 過去の状態保存
		m_Oldstate = m_state;

		// ダメージ状態にする
		m_state = STATE_DMG;

		// 遷移カウンター設定
		m_nCntState = 0;

		// ノックバックの位置更新
		D3DXVECTOR3 pos = GetPosition();
		D3DXVECTOR3 rot = GetRotation();
		m_posKnokBack = pos;

		// ノックバック判定にする
		m_bKnockback = true;

		// やられモーション
		m_pMotion->Set(MOTION_KNOCKBACK);

		// 衝撃波生成
		CImpactWave::Create
		(
			D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// 位置
			D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// 向き
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f),			// 色
			80.0f,										// 幅
			80.0f,										// 高さ
			20,											// 寿命
			10.0f,										// 幅の移動量
			CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
			false										// 加算合成するか
		);

		if (nValue == mylib_const::DIRECTWAVE_DMG)
		{
			CManager::SetEnableHitStop(12);

			// 振動
			CManager::GetCamera()->SetShake(12, 25.0f, 0.0f);

			// サウンド再生
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_PLAYERDMG_BIG);
		}
		else if (nValue == mylib_const::IMPACTWAVE_DMG)
		{
			//CManager::SetEnableHitStop(8);

			// 振動
			CManager::GetCamera()->SetShake(8, 15.0f, 0.0f);

			// サウンド再生
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_PLAYERDMG);
		}
		else
		{
			CManager::SetEnableHitStop(12);

			// 振動
			//CManager::GetCamera()->SetShake(8, 20.0f, 0.0f);
			CManager::GetCamera()->SetShake(12, 25.0f, 0.0f);

			// サウンド再生
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_PLAYERDMG);
		}
	}

	// 死んでない
	return false;
}

//==========================================================================
// 状態更新処理
//==========================================================================
void CPlayer::UpdateState(void)
{
	switch (m_state)
	{
	case STATE_NONE:
		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		break;

	case STATE_INVINCIBLE:
		Invincible();
		break;

	case STATE_DMG:
		Damage();
		break;

	case STATE_CONFUSION:
		Confusion();	// 混乱
		break;

	case STATE_KNOCKBACK:
		KnockBack();
		break;
	}
}

//==========================================================================
// 無敵
//==========================================================================
void CPlayer::Invincible(void)
{
	// 状態遷移カウンター減算
	m_nCntState--;

	if (m_nCntState % INVINCIBLE_INT == 0)
	{// 規定間隔
		
		// 色設定
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
	{// 遷移カウンターが0になったら

		// なにもない状態にする
		m_state = STATE_NONE;
		m_nCntState = 0;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

}

//==========================================================================
// ダメージ
//==========================================================================
void CPlayer::Damage(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 状態遷移カウンター減算
	m_nCntState++;

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	// 位置更新
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + move.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;

	// 起伏との判定
	if (CGame::GetObject3DMesh()->IsHit(pos) && m_nCntState >= 20)
	{// 地面と当たっていたら
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		move.y = 0.0f;			// 移動量ゼロ
		
		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// ノックバック判定消す
		m_bKnockback = false;
		m_pMotion->ToggleFinish(true);

		//if (m_bConfusion == true)
		//{// 混乱中だったら
		//	

		//	// 混乱エフェクト設定
		//	if (m_pConfusionEffect == NULL)
		//	{
		//		m_pConfusionEffect = CConfusionEffect::Create(D3DXVECTOR3(pos.x, pos.y + 100.0f, pos.z), 60.0f, m_nCntConfusion);
		//	}

		//	// 混乱設定
		//	m_state = STATE_CONFUSION;
		//	m_pMotion->Set(MOTION_CONFUSION);
		//}
	}
	else if (m_nCntState >= 30)
	{// 遷移カウンターが30になったら

		// ノックバック状態にする
		m_state = STATE_KNOCKBACK;
	}


	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// ノックバック
//==========================================================================
void CPlayer::KnockBack(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// 移動量取得
	float fMove = GetVelocity();

	// 現在と目標の差分
	float fRotDiff = 0.0f;

	// 距離の判定
	bool bLen = false;

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();


	// 状態遷移カウンター減算
	m_nCntState++;

	// 位置更新
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + move.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;

	// 起伏との判定
	if (CGame::GetObject3DMesh()->IsHit(pos) && m_nCntState >= 20)
	{// 地面と当たっていたら
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		move.y = 0.0f;	// 移動量ゼロ
		
		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		
		// ノックバック判定消す
		m_bKnockback = false;
		m_pMotion->ToggleFinish(true);

		//if (m_bConfusion == true)
		//{// 混乱中だったら

		//	// 混乱エフェクト設定
		//	if (m_pConfusionEffect == NULL)
		//	{
		//		m_pConfusionEffect = CConfusionEffect::Create(D3DXVECTOR3(pos.x, pos.y + 100.0f, pos.z), 60.0f, m_nCntConfusion);
		//	}

		//	// 混乱設定
		//	m_state = STATE_CONFUSION;
		//	m_pMotion->Set(MOTION_CONFUSION);
		//}
	}


	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 混乱
//==========================================================================
void CPlayer::Confusion(void)
{

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 最大フレーム取得
	int nAllFrame = m_pMotion->GetMaxAllCount();
	nAllFrame *= 2;

	//float fDiff = m_rotConfusion.y - rot.y;
	rot.y += m_rotConfusion.y / (float)(nAllFrame);

	// 角度の正規化
	RotNormalize(rot.y);

	// 位置更新
	pos.x += sinf(rot.y) * 4.0f;
	pos.z += cosf(rot.y) * 4.0f;

	// カウンター減算
	m_nCntConfusion--;

	if (m_nCntConfusion <= 0)
	{
		m_nCntConfusion = 0;
		m_bConfusion = false;

		// 無敵状態にする
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;

		// 混乱エフェクト削除
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

	// 向き設定
	SetRotation(rot);

	// 位置設定
	SetPosition(pos);
}

//==========================================================================
// 描画処理
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
// 混乱を有効にする
//==========================================================================
void CPlayer::EnableConfusion(void)
{
	// 混乱カウンター設定
	m_nCntConfusion = m_pMotion->GetMaxAllCount(MOTION_CONFUSION) * 2;
	m_bConfusion = true;

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 混乱エフェクト設定
	/*if (m_pConfusionEffect == NULL)
	{
		m_pConfusionEffect = CConfusionEffect::Create(D3DXVECTOR3(pos.x, pos.y + 100.0f, pos.z), 60.0f, m_nCntConfusion);
	}*/

	// 混乱設定
	m_state = STATE_CONFUSION;
	m_pMotion->Set(MOTION_CONFUSION);


	// 向き取得
	D3DXVECTOR3 rot = GetRotation();
	//while (1)
	//{// 範囲外引くまで無限ループ

	//	// 向きランダム決め
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

	// 歩行音再生
	CManager::GetSound()->PlaySound(CSound::LABEL_SE_CONFUSION);

	m_rotConfusion.y = (rot.y - D3DX_PI * 2.0f) + ((float)Random(-30, 30) * 0.01f);	// 混乱の向き設定
}

//==========================================================================
// 水の中にいる判定
//==========================================================================
void CPlayer::IsSwitchInWater(bool bFrag)
{
	m_bInWater = bFrag;
}

//==========================================================================
// 状態取得
//==========================================================================
int CPlayer::GetState(void)
{
	return m_state;
}