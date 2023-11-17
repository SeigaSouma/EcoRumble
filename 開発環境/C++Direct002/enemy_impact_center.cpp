//=============================================================================
// 
//  波起こし敵処理 [enemy_impact_center.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "enemy_impact_center.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "bullet.h"
#include "explosion.h"
#include "particle.h"
#include "collision.h"
#include "motion.h"
#include "model.h"
#include "elevation.h"
#include "player.h"
#include "enemymanager.h"
#include "camera.h"
#include "ballast.h"
#include "3D_Effect.h"
#include "hp_gauge.h"
#include "shadow.h"
#include "sound.h"
#include "impactwave.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(60.0f)							// 横幅
#define HEIGHT			(60.0f)							// 縦幅
#define MAX_LIFE		(5)								// 体力
#define PLAYER_SERCH	(800.0f)	// プレイヤー探索範囲
#define CHACE_DISTABCE	(50.0f)		// 追い掛ける時の間隔
#define JUMP			(18.0f)					// ジャンプ力初期値

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyImpactCenter::CEnemyImpactCenter(int nPriority) : CEnemy(nPriority)
{
	// 値のクリア
	m_bInvicible = false;	// 無敵状態かの判定
	m_nCntInvicible = 0;	// 無敵カウンター

}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyImpactCenter::~CEnemyImpactCenter()
{

}


//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyImpactCenter::Init(void)
{
	// 初期化処理
	CEnemy::Init();

	// 体力取得
	int nLife = GetLife();

	// 体力ゲージ
	m_pHPGauge = CHP_Gauge::Create(350.0f, nLife, 3.0f);
	m_state = STATE_SPAWN;
	m_Oldstate = STATE_TURRETWAIT;
	m_nCntState = 0;
	m_bInvicible = false;	// 無敵状態かの判定

	if (m_pHPGauge == NULL)
	{// NULLだったら
		m_pHPGauge = NULL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyImpactCenter::Uninit(void)
{
	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
// 死亡処理
//==========================================================================
void CEnemyImpactCenter::Kill(void)
{
	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyImpactCenter::Update(void)
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 更新処理
	CEnemy::Update();

	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	if (m_bInvicible == true)
	{
		// 無敵状態
		Invincible();
	}
}

//==========================================================================
// 着地時の処理
//==========================================================================
void CEnemyImpactCenter::ProcessLanding(void)
{
	// 着地時の処理
	CEnemy::ProcessLanding();
}

//==========================================================================
// 攻撃状態移行処理
//==========================================================================
void CEnemyImpactCenter::ChangeToAttackState(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{
		return;
	}

	// 親の位置取得
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	if (CircleRange(pos, posPlayer, 400.0f, pPlayer->GetRadius()) == true && m_sMotionFrag.bJump == false)
	{// 一定距離間にプレイヤーが入ったら

		// 攻撃状態にする
		m_state = STATE_ATTACK;
		m_sMotionFrag.bATK = true;
	}
}

//==========================================================================
// 種類別状態更新処理
//==========================================================================
void CEnemyImpactCenter::UpdateStateByType(void)
{
	return;
}

//==========================================================================
// ヒット処理
//==========================================================================
bool CEnemyImpactCenter::Hit(const int nValue)
{
	// 向き取得
	D3DXVECTOR3 rot = GetRotation();
	D3DXVECTOR3 pos = GetPosition();

	// 体力取得
	int nLife = GetLife();

	if (nValue == 0 && m_state != STATE_KNOCKBACK)
	{// ノーダメージの場合

		// 過去の状態保存
		m_Oldstate = m_state;

		// ノックバック状態
		m_state = STATE_KNOCKBACK;

		// 遷移カウンター設定
		m_nCntState = 30;

		D3DXVECTOR3 move = GetMove();
		move.y = 0.0f;
		move.y += 10.5f;
		SetMove(move);
	}
	else if (nValue == 0 && m_state == STATE_KNOCKBACK)
	{
		return true;
	}
	else
	{
		if (m_bInvicible == false &&
			m_state != STATE_DMG && m_state != STATE_KNOCKBACK && m_state != STATE_DEAD && m_state != STATE_SPAWN)
		{// なにもない状態の時

			if (nValue == mylib_const::DIRECTWAVE_DMG)
			{
				CManager::SetEnableHitStop(5);

				// 振動
				CManager::GetCamera()->SetShake(5, 10.0f, 0.0f);
			}
			else if (nValue == mylib_const::IMPACTWAVE_DMG)
			{
				// 振動
				CManager::GetCamera()->SetShake(5, 10.0f, 0.0f);
			}

			// 体力減らす
			nLife -= nValue;

			// 体力設定
			SetLife(nLife);

			if (nLife > 0)
			{// 体力がなくなってなかったら

				// ダメージ音再生
				CManager::GetSound()->PlaySound(CSound::LABEL_SE_DMG01);

				// 移動量
				SetMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

				CImpactWave::Create
				(
					D3DXVECTOR3(pos.x + sinf(D3DX_PI + rot.y) * 80.0f, pos.y + 80.0f, pos.z + cosf(D3DX_PI + rot.y) * 80.0f),	// 位置
					D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// 向き
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f),			// 色
					80.0f,										// 幅
					80.0f,										// 高さ
					40,											// 寿命
					9.0f,										// 幅の移動量
					CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
					false										// 加算合成するか
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(pos.x + sinf(rot.y) * 200.0f, pos.y + 80.0f, pos.z + cosf(rot.y) * 200.0f),	// 位置
					D3DXVECTOR3(D3DX_PI * 0.5f, rot.y, D3DX_PI),				// 向き
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f),			// 色
					40.0f,										// 幅
					200.0f,										// 高さ
					20,											// 寿命
					1.0f,										// 幅の移動量
					CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
					false										// 加算合成するか
				);
			}

			if (nLife <= 0)
			{// 体力がなくなったら

				// 間隔内に倒した数取得
				int nNumKill = CGame::GetEnemyManager()->GetNumIntervalKill();

				// 衝撃波生成
				if (nNumKill >= 0 && nNumKill < mylib_const::COMBONUM_1)
				{
					CImpactWave::Create
					(
						D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// 位置
						D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// 向き
						D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.2f),			// 色
						80.0f,										// 幅
						80.0f,										// 高さ
						20,											// 寿命
						9.0f,										// 幅の移動量
						CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
						false										// 加算合成するか
					);

					// ダメージ音再生
					CManager::GetSound()->PlaySound(CSound::LABEL_SE_ENEMYDMG_SMALL);

					// 加算スコア設定
					m_AddType = CResultManager::ADDTYPE_NORMAL;
				}
				else if (nNumKill >= mylib_const::COMBONUM_1 && nNumKill < mylib_const::COMBONUM_2)
				{
					CImpactWave::Create
					(
						D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// 位置
						D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// 向き
						D3DXCOLOR(0.2f, 0.2f, 1.0f, 0.5f),			// 色
						80.0f,										// 幅
						80.0f,										// 高さ
						30,											// 寿命
						13.0f,										// 幅の移動量
						CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
						true										// 加算合成するか
					);

					// ダメージ音再生
					CManager::GetSound()->PlaySound(CSound::LABEL_SE_ENEMYDMG_NORMAL);

					// 加算スコア設定
					m_AddType = CResultManager::ADDTYPE_BLUE;
				}
				else
				{
					CImpactWave::Create
					(
						D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// 位置
						D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// 向き
						D3DXCOLOR(1.0f, 0.6f, 0.3f, 0.6f),			// 色
						80.0f,										// 幅
						80.0f,										// 高さ
						40,											// 寿命
						13.0f,										// 幅の移動量
						CImpactWave::TYPE_GIZAWHITE,				// テクスチャタイプ
						true										// 加算合成するか
					);

					// ダメージ音再生
					CManager::GetSound()->PlaySound(CSound::LABEL_SE_ENEMYDMG_BIG);

					// 加算スコア設定
					m_AddType = CResultManager::ADDTYPE_GOLD;
				}

				// 死亡状態にする
				m_state = STATE_DEAD;

				// 遷移カウンター設定
				m_nCntState = 0;

				// ノックバックの位置更新
				m_posKnokBack = GetPosition();

				// ノックバック判定にする
				m_sMotionFrag.bKnockback = true;

				// やられモーション
				m_pMotion->Set(MOTION_KNOCKBACK);

				// 倒した判定設定
				CGame::GetEnemyManager()->SetKillFlag();

				// 死んだ
				return true;
			}

			// 補正
			ValueNormalize(nLife, GetLifeOrigin(), 0);

			// 過去の状態保存
			m_Oldstate = m_state;

			// ダメージ状態にする
			m_state = STATE_DMG;

			// 遷移カウンター設定
			m_nCntState = 0;

			// ノックバックの位置更新
			m_posKnokBack = GetPosition();

			// ノックバック判定にする
			m_sMotionFrag.bKnockback = true;

			// やられモーション
			m_pMotion->Set(MOTION_KNOCKBACK);

		}

		if (m_bInvicible == true)
		{
			// 移動量
			SetMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}
	}

	// 死んでない
	return false;
}

//==========================================================================
// 出現
//==========================================================================
void CEnemyImpactCenter::Spawn(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 状態遷移カウンター減算
	m_nCntState++;

	// 位置更新
	pos = (D3DXVECTOR3(0.0f, SPAWN_GRAVITY, 0.0f) * (float)(m_nCntState * m_nCntState) + move * (float)m_nCntState) + m_posOrigin;

	// 起伏との判定
	if (CGame::GetObject3DMesh()->IsHit(pos))
	{// 地面と当たっていたら
		m_state = m_Oldstate;	// 過去の情報保存
		m_nCntState = 60 * 1;	// 遷移カウンター設定
		move.y = 0.0f;			// 移動量ゼロ
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// ダメージ
//==========================================================================
void CEnemyImpactCenter::Damage(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	// 現在と目標の差分
	float fRotDiff = 0.0f;

	// 距離の判定
	bool bLen = false;

	// 状態遷移カウンター減算
	m_nCntState++;

	// 最大フレーム取得
	int nAllFrame = m_pMotion->GetMaxAllCount();

	// マテリアルの色設定
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, m_mMatcol.a);

	if (m_nCntState == 60)
	{
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);
		m_bInvicible = true;	// 無敵状態かの判定
		m_nCntInvicible = 80;	// 無敵カウンター
	}

	// 起伏との判定
	if (m_nCntState >= nAllFrame)
	{// フレームを超えていたら

		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);
		m_state = STATE_TURRETATK;
		m_nCntState = 0;
		move.y = 0.0f;			// 移動量ゼロ

		// ノックバック判定消す
		m_sMotionFrag.bKnockback = false;

		// 攻撃モーション
		m_pMotion->Set(MOTION_ATK);
	}

	// 地面の高さ取得
	bool bLand = false;
	pos.y = CManager::GetScene()->GetObject3DMesh()->GetHeight(pos, move, bLand, GetType());

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 無敵
//==========================================================================
void CEnemyImpactCenter::Invincible(void)
{
	// 無敵カウンター減算
	m_nCntInvicible--;

	if (m_nCntInvicible % 4 == 0)
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

	if (m_nCntInvicible <= 0)
	{// 遷移カウンターが0になったら

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		m_bInvicible = false;	// 無敵状態かの判定
	}

}

//==========================================================================
// 追い掛け移動
//==========================================================================
void CEnemyImpactCenter::ChaseMove(float fMove)
{
	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 移動量加算
	move.x += sinf(D3DX_PI + rot.y) * fMove;
	move.z += cosf(D3DX_PI + rot.y) * fMove;

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// モーションの設定
//==========================================================================
void CEnemyImpactCenter::MotionSet(void)
{
	if (m_pMotion->IsFinish() == true)
	{// 終了していたら

		// 現在の種類取得
		int nType = m_pMotion->GetType();

		if (m_sMotionFrag.bMove == true && m_sMotionFrag.bKnockback == false)
		{// 移動していたら

			m_sMotionFrag.bMove = false;	// 移動判定OFF

			// 移動モーション
			m_pMotion->Set(MOTION_WALK);
		}
		else if (m_sMotionFrag.bKnockback == true)
		{// やられ中だったら

			// やられモーション
			m_pMotion->Set(MOTION_KNOCKBACK);
		}
		else if (m_sMotionFrag.bATK == true)
		{// 攻撃していたら

			m_sMotionFrag.bATK = false;		// 攻撃判定OFF

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
// 攻撃時処理
//==========================================================================
void CEnemyImpactCenter::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
{
	D3DXMATRIX mtxTrans;	// 計算用マトリックス宣言

	// 攻撃時処理
	CEnemy::AttackAction(nModelNum, ATKInfo);

	if (GetObjectChara()->GetModel()[nModelNum] == NULL)
	{// NULLだったら
		return;
	}

	// 判定するパーツのマトリックス取得
	D3DXMATRIX mtxWepon = GetObjectChara()->GetModel()[nModelNum]->GetWorldMtx();

	// 位置を反映する
	D3DXMatrixTranslation(&mtxTrans, ATKInfo.Offset.x, ATKInfo.Offset.y, ATKInfo.Offset.z);
	D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

	// 武器の位置
	D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 種類別
	switch (m_pMotion->GetType())
	{
	case MOTION_WALK:
		break;

	case MOTION_ATK:
		// 振動
		CManager::GetCamera()->SetShake(20, 5.0f, 0.0f);

		// 波発生
		CGame::GetObject3DMesh()->SetWave
		(
			D3DXVECTOR3(
				weponpos.x + sinf(D3DX_PI + rot.y) * 250.0f,
				weponpos.y,
				weponpos.z + cosf(D3DX_PI + rot.y) * 250.0f),	// 位置
			D3DXVECTOR3(
				0.0f,
				rot.y,
				0.0f),									// 向き
			D3DXVECTOR3(
				sinf(D3DX_PI + rot.y) * 20.0f,
				0.0f,
				cosf(D3DX_PI + rot.y) * 20.0f),					// 移動量
			D3DXVECTOR2(70.0f, 300.0f),									// 判定範囲
			CGame::GetObject3DMesh()->GetWidthLen() * 5.0f,				// 波発生の頻度
			-300.0f,														// 波の高さ
			D3DX_PI * 0.0f,													// 波打つ頻度
			60,															// 発生時間
			CElevation::WAVETYPE_DIRECT,										// 種類
			CElevation::TYPE_ENEMY,								// プレイヤーの種類
			0
		);
		break;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CEnemyImpactCenter::Draw(void)
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// 敵の情報取得
//==========================================================================
CEnemyImpactCenter *CEnemyImpactCenter::GetEnemy(void)
{
	// 自分自身のポインタを取得
	return this;
}