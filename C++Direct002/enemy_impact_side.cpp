//=============================================================================
// 
//  波起こし敵処理 [enemy_impact_side.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "enemy_impact_side.h"
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
CEnemyImpactSide::CEnemyImpactSide(int nPriority) : CEnemy(nPriority)
{
	// 値のクリア
	
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyImpactSide::~CEnemyImpactSide()
{

}


//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyImpactSide::Init(void)
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

	if (m_pHPGauge == NULL)
	{// NULLだったら
		m_pHPGauge = NULL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyImpactSide::Uninit(void)
{
	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
// 死亡処理
//==========================================================================
void CEnemyImpactSide::Kill(void)
{
	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyImpactSide::Update(void)
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
}

//==========================================================================
// 着地時の処理
//==========================================================================
void CEnemyImpactSide::ProcessLanding(void)
{
	// 着地時の処理
	CEnemy::ProcessLanding();
}

//==========================================================================
// 攻撃状態移行処理
//==========================================================================
void CEnemyImpactSide::ChangeToAttackState(void)
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
void CEnemyImpactSide::UpdateStateByType(void)
{
	return;
}

//==========================================================================
// 出現
//==========================================================================
void CEnemyImpactSide::Spawn(void)
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
// 追い掛け移動
//==========================================================================
void CEnemyImpactSide::ChaseMove(float fMove)
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
void CEnemyImpactSide::MotionSet(void)
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
void CEnemyImpactSide::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
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
void CEnemyImpactSide::Draw(void)
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// 敵の情報取得
//==========================================================================
CEnemyImpactSide *CEnemyImpactSide::GetEnemy(void)
{
	// 自分自身のポインタを取得
	return this;
}