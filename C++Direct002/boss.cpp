//=============================================================================
// 
//  ボス処理 [boss.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "boss.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "bullet.h"
#include "explosion.h"
#include "particle.h"
#include "motion.h"
#include "model.h"
#include "elevation.h"
#include "enemymanager.h"
#include "camera.h"
#include "ballast.h"
#include "3D_Effect.h"
#include "hp_gauge.h"
#include "player.h"
#include "enemy.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(60.0f)							// 横幅
#define HEIGHT			(60.0f)							// 縦幅
#define MAX_LIFE		(30)							// 体力
#define PLAYER_SERCH	(400.0f)	// プレイヤー探索範囲
#define CHACE_DISTABCE	(50.0f)		// 追い掛ける時の間隔
#define JUMP			(15.0f)					// ジャンプ力初期値

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CBoss::CBoss(int nPriority) : CObjectChara(nPriority)
{
	// 値のクリア
	m_action = ACTION_TACKLE;	// 行動
	m_ImpalseMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 吹き飛ぶ移動量
	m_nCntAction = 0;			// 行動カウンター
	m_nCntHit = 0;				// ヒットカウンター
	m_nCntHitAfter = 0;			// ヒット後のカウント

	m_state = STATE_NONE;	// 状態
	m_Oldstate = m_state;	// 前回の状態
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// マテリアルの色
	m_nCntState = 0;		// 状態遷移カウンター
	m_nBeforCntState = 0;	// 前の状態遷移カウンター
	m_nLife = 0;			// 体力
	m_nTexIdx = 0;			// テクスチャのインデックス番号
	m_nIdxManager = 0;		// マネージャのインデックス番号
	m_bJump = false;		// ジャンプ中かどうか
	m_bMove = false;		// 移動中かどうか
	m_bATK = false;			// 攻撃中かどうか
	m_pHPGauge = NULL;		// HPゲージの情報
	m_pMotion = NULL;		// モーションの情報
}

//==========================================================================
// デストラクタ
//==========================================================================
CBoss::~CBoss()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CBoss *CBoss::Create(int nIdx, const char *pFileName, D3DXVECTOR3 pos)
{
	// 生成用のオブジェクト
	CBoss *pEnemy = NULL;

	if (pEnemy == NULL)
	{// NULLだったら

		// メモリの確保
		pEnemy = DEBUG_NEW CBoss;

		if (pEnemy != NULL)
		{// メモリの確保が出来ていたら

			// インデックス番号
			pEnemy->m_nIdxManager = nIdx;

			// 位置設定
			pEnemy->SetPosition(pos);

			// 初期化処理
			pEnemy->Init(pFileName);
		}

		return pEnemy;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBoss::Init(void)
{
	return E_FAIL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBoss::Init(const char *pFileName)
{

	// 各種変数の初期化
	m_state = STATE_NONE;	// 状態
	m_nCntState = 0;		// 状態遷移カウンター
	m_nLife = MAX_LIFE;		// 体力

	// 体力ゲージ
	m_pHPGauge = CHP_Gauge::Create(350.0f, m_nLife, 3.0f);
		
	if (m_pHPGauge == NULL)
	{// NULLだったら
		
		m_pHPGauge = NULL;
	}

	// 種類の設定
	SetType(TYPE_BOSS);

	// キャラ作成
	HRESULT hr = SetCharacter(pFileName);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// モーションの生成処理
	if (m_pMotion == NULL)
	{
		m_pMotion = CMotion::Create();
	}

	if (m_pMotion == NULL)
	{
		return E_FAIL;
	}

	// モーションのファイル読み込み
	m_pMotion->ReadText(pFileName);

	// オブジェクトキャラクターの情報取得
	CObjectChara *pObjChar = GetObjectChara();

	// モーションの設定
	m_pMotion->SetModel(pObjChar->GetModel(), pObjChar->GetNumModel(), CObjectChara::GetObjectChara());

	// 向き設定
	SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBoss::Uninit(void)
{
	int nIdx = m_nIdxManager;

	// 終了処理
	if (m_pMotion != NULL)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// 終了処理
	CObjectChara::Uninit();

	// マネージャの終了
	CEnemyManager::Release(nIdx);
}

//==========================================================================
// 死亡処理
//==========================================================================
void CBoss::Kill(void)
{
	// HPゲージを消す
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->Uninit();
		m_pHPGauge = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CBoss::Update(void)
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 過去の位置設定
	SetOldPosition(GetPosition());

	// 当たり判定
	Collision();

	// モーションの設定処理
	MotionSet();

	// 状態更新
	UpdateState();

	// 攻撃処理
	Atack();

	// モーションの更新
	m_pMotion->Update();

	// HPゲージの位置更新
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->UpdatePosition(GetPosition(), m_nLife);
	}

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();
	if (pInputKeyboard->GetPress(DIK_SPACE) == true)
	{
		// 攻撃状態にする
		m_state = STATE_ATTACK;
		m_nCntState = 60 * 10;
		m_action = (ACTION)(rand() % ACTION_MAX);
	}

	//if (GetPosition().y <= -800.0f)
	//{
	//	// 敵の終了処理
	//	Kill();
	//	Uninit();
	//}
}

//==========================================================================
// 当たり判定
//==========================================================================
void CBoss::Collision(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 重力処理
	move.y -= mylib_const::GRAVITY * 0.25f;

	// 位置更新
	pos += move;

	// 慣性補正
	move.x += (0.0f - move.x) * 0.25f;
	move.z += (0.0f - move.z) * 0.25f;

	if (move.x >= 0.1f || move.x <= -0.1f ||
		move.z >= 0.1f || move.z <= -0.1f)
	{// 移動中
		m_bMove = true;
	}
	else
	{
		m_bMove = false;
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);


	// 着地したかどうか
	bool bLand = false;

	// 高さ取得
	float fHeight = CGame::GetObject3DMesh()->GetHeight(pos, move, bLand, GetType());


	if (fHeight > pos.y)
	{// 地面の方が自分より高かったら

		// 地面の高さに設定
		pos.y = fHeight;

		if (bLand == true)
		{// 着地してたら

			if (m_bJump == true)
			{// ジャンプから着地の時

				switch (m_action)
				{
				case CBoss::ACTION_JUMP:
					
					// 行動カウンターリセット
					m_nCntAction = 0;

					// ボム敵スポーン
					CGame::GetEnemyManager()->SetEnemy(CEnemyManager::PATTERN_BOMB, D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

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
						CGame::GetObject3DMesh()->GetWidthLen() * 2.0f,		// 波発生の頻度
						-180.0f,											// 波の高さ
						9.375f,												// 波の広がり
						40,													// 発生時間
						CElevation::WAVETYPE_IMPACT,						// 種類
						CElevation::TYPE_ENEMY,								// プレイヤーの種類
						1
					);
					break;

				case CBoss::ACTION_SP_JUMP:

					// 状態遷移カウンター
					m_nCntState = 0;

					// ボム敵スポーン
					CGame::GetEnemyManager()->SetEnemy(CEnemyManager::PATTERN_BOMB, D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

					// ボム敵スポーン
					CGame::GetEnemyManager()->SetEnemy(CEnemyManager::PATTERN_BOMB, D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

					// ボム敵スポーン
					CGame::GetEnemyManager()->SetEnemy(CEnemyManager::PATTERN_BOMB, D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

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
						CGame::GetObject3DMesh()->GetWidthLen() * 3.0f,		// 波発生の頻度
						-200.0f,											// 波の高さ
						13.0f,												// 波の広がり
						90,													// 発生時間
						CElevation::WAVETYPE_IMPACT,						// 種類
						CElevation::TYPE_ENEMY,								// プレイヤーの種類
						3
					);
					break;
				}
			}

			// ジャンプ使用可能にする
			m_bJump = false;
			move.y = 0.0f;
		}
	}


	if (pos.y <= -800.0f)
	{
		pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	
	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);
}


//==========================================================================
// モーションの設定
//==========================================================================
void CBoss::MotionSet(void)
{
	if (m_pMotion->IsFinish() == true)
	{// 終了していたら

		// 現在の種類取得
		int nType = m_pMotion->GetType();

		if (m_bMove == true)
		{// 移動していたら

			m_bMove = false;	// 移動判定OFF

			// 移動モーション
			m_pMotion->Set(MOTION_WALK);
		}
		else if (m_bATK == true)
		{// 攻撃していたら

			m_bATK = false;		// 攻撃判定OFF

			// 攻撃モーション
			m_pMotion->Set(MOTION_ATK);
		}
		else
		{
			// ニュートラルモーション
			m_pMotion->Set(MOTION_DEF);
		}
	}
}

//==========================================================================
// ヒット処理
//==========================================================================
bool CBoss::Hit(const int nValue)
{

	if (nValue == 0 && m_state != STATE_KNOCKBACK && m_state != STATE_ATTACK && m_state != STATE_DMG)
	{// ノーダメージの場合

		// ヒット回数加算
		if (m_nCntHitAfter > 0)
		{
			m_nCntHit++;
		}

		// ヒットカウンターリセット
		m_nCntHitAfter = 120;

		// 過去の状態保存
		m_Oldstate = m_state;

		// ノックバック状態
		m_state = STATE_KNOCKBACK;

		// 吹き飛ぶ移動量
		m_ImpalseMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// 途中のカウンター保存
		m_nBeforCntState = m_nCntState;

		// 遷移カウンター設定
		m_nCntState = 40;

		D3DXVECTOR3 move = GetMove();
		move.y += 5.5f;
		SetMove(move);

	}
	else if (m_state == STATE_KNOCKBACK)
	{
		return true;
	}
	else if(nValue != 0)
	{
		if (m_state != STATE_DEAD && m_state != STATE_DMG)
		{// なにもない状態の時

			// 体力減らす
			m_nLife -= nValue;

			if (m_nLife <= 0)
			{// 体力がなくなったら

				// 死状態
				m_state = STATE_DEAD;

				// 敵の終了処理
				Kill();
				Uninit();

				// 死んだ
				return true;
			}

			// 補正
			ValueNormalize(m_nLife, MAX_LIFE, 0);

			// ダメージ状態にする
			m_state = STATE_DMG;

			// 遷移カウンター設定
			m_nCntState = 60 * 3;

			// 上に飛ばす
			D3DXVECTOR3 move = GetMove();
			move.y += 25.0f;
			SetMove(move);
		}
	}

	// 死んでない
	return false;
}


//==========================================================================
// ヒット処理
//==========================================================================
bool CBoss::Hit(const int nValue, D3DXVECTOR3 move)
{

	if (nValue == 0 && m_state != STATE_KNOCKBACK && m_state != STATE_ATTACK && m_state != STATE_DMG)
	{// ノーダメージの場合

		// ヒット回数加算
		if (m_nCntHitAfter > 0)
		{
			m_nCntHit++;
		}

		// ヒットカウンターリセット
		m_nCntHitAfter = 120;

		// 過去の状態保存
		m_Oldstate = m_state;

		// ノックバック状態
		m_state = STATE_KNOCKBACK;

		// 吹き飛ぶ移動量
		m_ImpalseMove = move;

		// 途中のカウンター保存
		m_nBeforCntState = m_nCntState;

		// 遷移カウンター設定
		m_nCntState = 40;

		D3DXVECTOR3 move = GetMove();
		move.y += 5.5f;
		SetMove(move);

	}
	else if (m_state == STATE_KNOCKBACK)
	{
		return true;
	}
	else
	{
		if (m_state != STATE_DEAD && m_state != STATE_DMG)
		{// なにもない状態の時

			// 体力減らす
			m_nLife -= nValue;

			if (m_nLife <= 0)
			{// 体力がなくなったら

				// 死状態
				m_state = STATE_DEAD;

				// 敵の終了処理
				Kill();
				Uninit();

				// 死んだ
				return true;
			}

			// 補正
			ValueNormalize(m_nLife, MAX_LIFE, 0);

			// ダメージ状態にする
			m_state = STATE_DMG;

			// 遷移カウンター設定
			m_nCntState = 60 * 3;

			// 上に飛ばす
			D3DXVECTOR3 move = GetMove();
			move.y += 25.0f;
			SetMove(move);
		}
	}

	// 死んでない
	return false;
}

//==========================================================================
// 状態更新処理
//==========================================================================
void CBoss::UpdateState(void)
{
#if TOPCUR
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 pos11 = D3DXVECTOR3(GetObjectChara()->GetModel()[0]->GetWorldMtx()._41, GetObjectChara()->GetModel()[0]->GetWorldMtx()._42, GetObjectChara()->GetModel()[0]->GetWorldMtx()._43);

	CEffect3D::Create(pos11, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), GetRadius(), 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

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

	// オブジェクト情報
	CObject *pObj = NULL;

	// 現在の種類取得
	int nType = m_pMotion->GetType();


	// ヒットカウンター減算
	m_nCntHitAfter--;

	// ヒット回数加算
	if (m_nCntHitAfter < 0)
	{
		m_nCntHitAfter = 0;
		m_nCntHit = 0;
	}

	if (m_nCntHit >= 2 && m_state != STATE_ATTACK && m_state != STATE_KNOCKBACK)
	{
		m_state = STATE_ATTACK;
	}

	switch (m_state)
	{
	case STATE_NONE:
		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		{
			static int nCnt = 0;

			float fRotDiff = 0.0f;	// 現在と目標の差分

			nCnt = (nCnt + 1) % 60;

			// 目標の角度を求める
			if (nCnt == 0)
			{
				fRotDest = Random(-31, 31) * 0.1f;
			}
		}

		// 目標との差分
		fRotDiff = fRotDest - rot.y;

		//角度の正規化
		RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		RotNormalize(rot.y);

		// 向いてる方向にダッシュ
		move.x += sinf(D3DX_PI + rot.y) * fMove;
		move.z += cosf(D3DX_PI + rot.y) * fMove;

		if (pPlayer != NULL)
		{// NULLじゃないとき

			if (CircleRange(pos, pPlayer->GetPosition(), 200.0f, PLAYER_SERCH) == true)
			{// プレイヤーが範囲に入れば
				m_state = STATE_PLAYERCHASE;
			}
		}

		break;

	case STATE_DMG:

		// 状態遷移カウンター減算
		m_nCntState--;

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		if (m_nCntState <= 0)
		{// 遷移カウンターが0になったら

			// 親追い掛け状態にする
			m_state = STATE_PLAYERCHASE;
			m_nCntState = 0;
			// ボム爆破
			BombExplosion();
		}

		// 目標の角度を求める
		fRotDest = atan2f((pos.x - 0.0f), (pos.z - 0.0f));

		// 目標との差分
		fRotDiff = fRotDest - rot.y;

		//角度の正規化
		RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.2f;

		// 角度の正規化
		RotNormalize(rot.y);

		if (CircleRange(pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), 100.0f, 100.0f) == false)
		{// プレイヤーが範囲に入れば

			// 向いてる方向にダッシュ
			move.x += sinf(D3DX_PI + rot.y) * 8.0f;
			move.z += cosf(D3DX_PI + rot.y) * 8.0f;
		}
		break;

	case STATE_KNOCKBACK:

		// 状態遷移カウンター減算
		m_nCntState--;

		// 色設定
		m_mMatcol = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

		if (m_nCntState <= 0)
		{// 遷移カウンターが0になったら

			m_nCntState = m_nBeforCntState;
			
			if (m_Oldstate != STATE_ATTACK)
			{
				// プレイヤー追い掛け状態にする
				m_state = STATE_PLAYERCHASE;
			}
			else
			{
				m_state = STATE_ATTACK;
				m_action = (ACTION)(rand() % ACTION_MAX);
			}
			break;
		}

		// 向いてる方向と逆方向に移動
		move.x += sinf(D3DX_PI + rot.y) * -fMove * 0.7f;
		move.z += cosf(D3DX_PI + rot.y) * -fMove * 0.7f;

		// 吹き飛ぶ移動量加算
		pos += m_ImpalseMove;

		break;

	case STATE_PLAYERCHASE:

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// 状態遷移カウンター減算
		m_nCntState--;

		if (m_nCntState <= 0)
		{// 遷移カウンターが0になったら
			m_nCntState = 0;

			// 攻撃状態にする
			m_state = STATE_ATTACK;
			m_nCntState = 60 * 10;
			m_action = (ACTION)(rand() % ACTION_MAX);
		}

		if (pPlayer != NULL)
		{// NULLじゃないとき

			// 親の位置取得
			D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

			// 目標の角度を求める
			fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

			// 目標との差分
			fRotDiff = fRotDest - rot.y;

			//角度の正規化
			RotNormalize(fRotDiff);

			//角度の補正をする
			rot.y += fRotDiff * 0.05f;

			// 角度の正規化
			RotNormalize(rot.y);

			if (m_nCntState == 60 * 2)
			{
				// ボム爆破
				BombExplosion();
			}

			// 向いてる方向にダッシュ
			move.x += sinf(D3DX_PI + rot.y) * fMove;
			move.z += cosf(D3DX_PI + rot.y) * fMove;
		}

		break;

	case STATE_ATTACK:

		// 色設定
		m_mMatcol = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);

		// 状態遷移カウンター減算
		m_nCntState--;

		if (m_nCntState <= 0)
		{// 遷移カウンターが0になったら
			m_nCntState = 60 * 15;
			m_state = STATE_PLAYERCHASE;
			break;
		}

		// 行動カウンター加算
		m_nCntAction++;

		if (m_nCntAction <= 0)
		{// 行動カウンターが0になったら
			m_nCntAction = 0;
		}

		if (m_bATK == false && nType != MOTION_ATK)
		{// 攻撃中じゃなければ
			m_bATK = true;
		}

		// 攻撃情報
		switch (m_action)
		{
		case CBoss::ACTION_TACKLE:

			if (m_nCntAction >= 0 && m_nCntAction < 60)
			{// 一定時間突撃

				// 向いてる方向にダッシュ
				move.x += sinf(D3DX_PI + rot.y) * fMove * 4.5f;
				move.z += cosf(D3DX_PI + rot.y) * fMove * 4.5f;

				if (m_nCntAction == 5)
				{
					// ボム敵スポーン
					CGame::GetEnemyManager()->SetEnemy(CEnemyManager::PATTERN_BOMB, D3DXVECTOR3(pos.x, pos.y + 20.0f, pos.z), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
				}

			}
			else if (m_nCntAction >= 60)
			{// プレイヤーの方見る

				// 親の位置取得
				D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

				// 目標の角度を求める
				fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

				// 目標との差分
				fRotDiff = fRotDest - rot.y;

				//角度の正規化
				RotNormalize(fRotDiff);

				//角度の補正をする
				rot.y += fRotDiff * 0.05f;

				// 角度の正規化
				RotNormalize(rot.y);
			}

			if (m_nCntAction >= 120)
			{// 規定値を超えたら

				// 行動カウンターリセット
				m_nCntAction = 0;
			}

			break;

		case CBoss::ACTION_JUMP:

			if (m_nCntAction >= 60 && m_bJump == false)
			{// ジャンプ中じゃないとき

				// ジャンプさせる
				m_bJump = true;
				move.y += JUMP;
			}

			if (pPlayer != NULL)
			{// NULLじゃないとき

				if (CircleRange(pos, pPlayer->GetPosition(), 50.0f, 100.0f) == false)
				{// プレイヤーが範囲に入れば

					// 親の位置取得
					D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

					// 目標の角度を求める
					fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

					// 目標との差分
					fRotDiff = fRotDest - rot.y;

					//角度の正規化
					RotNormalize(fRotDiff);

					//角度の補正をする
					rot.y += fRotDiff * 0.05f;

					// 角度の正規化
					RotNormalize(rot.y);
				}
			}

			// 向いてる方向にダッシュ
			if (m_bJump == true)
			{
				move.x += sinf(D3DX_PI + rot.y) * fMove * 3.0f;
				move.z += cosf(D3DX_PI + rot.y) * fMove * 3.0f;
			}
			break;

		case CBoss::ACTION_SP_JUMP:

			if (m_bJump == false)
			{// ジャンプ中じゃないとき

				// ジャンプさせる
				m_bJump = true;
				move.y += JUMP * 2.0f;
			}

			break;
		}
		break;

	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);
#endif
}


//==========================================================================
// 攻撃
//==========================================================================
void CBoss::Atack(void)
{

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// モーションの情報取得
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// 攻撃情報の総数取得
	int nNumAttackInfo = aInfo.nNumAttackInfo;

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
			case MOTION_ATK:

				// パーティクル生成
				my_particle::Create(weponpos, my_particle::TYPE_SUPERATTACK);

				// 瓦礫生成
				CBallast::Create(weponpos, DEFAULTMOVE_BALLAST, 5);

				// 振動
				CManager::GetCamera()->SetShake(20, 5.0f, 0.0f);
				break;
			}
		}

		// モーションカウンター取得
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt && m_pMotion->GetAllCount() < aInfo.AttackInfo[nCntAttack]->nMaxCnt)
		{// 攻撃判定中

			// プレイヤー情報
			CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

			if (pPlayer == NULL)
			{// NULLだったら
				return;
			}

			// 敵の位置取得
			D3DXVECTOR3 TargetPos = pPlayer->GetPosition();

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

			// 判定サイズ取得
			float fRadius = pPlayer->GetObjectChara()->GetRadius();

			if (SphereRange(weponpos, TargetPos, aInfo.AttackInfo[nCntAttack]->fRangeSize, fRadius))
			{// 球の判定

				if (pPlayer->Hit(aInfo.AttackInfo[nCntAttack]->nDamage) == true)
				{// 死んでたら

					my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);

					// プレイヤー情報
					CPlayer **ppPlayer = CManager::GetScene()->GetPlayer(0);

					// プレイヤーをNULL
					*ppPlayer = NULL;
					int m = 0;
				}
			}
		}
	}
}

//==========================================================================
// ボム爆破
//==========================================================================
void CBoss::BombExplosion(void)
{
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

		if (pObj->GetObjectChara()->GetCharaType() == CEnemy::TYPE_BOMB)
		{// ボム
			pObj->GetObjectChara()->SetState(CEnemy::STATE_DEAD, 120);
		}

		// 次のオブジェクトを代入
		pObj = pObjNext;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CBoss::Draw(void)
{
	if (m_mMatcol != D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))
	{
		// オブジェクトキャラの描画
		CObjectChara::Draw(m_mMatcol);
	}
	else
	{
		// オブジェクトキャラの描画
		CObjectChara::Draw();
	}
}

//==========================================================================
// 状態取得
//==========================================================================
int CBoss::GetState(void)
{
	return m_state;
}

//==========================================================================
// 種類取得
//==========================================================================
int CBoss::GetCharaType(void)
{
	return 0;
}

//==========================================================================
// 敵の情報取得
//==========================================================================
CBoss *CBoss::GetBoss(void)
{
	// 自分自身のポインタを取得
	return this;
}