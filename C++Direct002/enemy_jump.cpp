//=============================================================================
// 
//  ジャンプ敵処理 [enemy_jump.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "enemy_jump.h"
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
CEnemyJump::CEnemyJump(int nPriority) : CEnemy(nPriority)
{
	// 値のクリア
	m_rotParts = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// パーツ回転用
	m_nCntWaitJump = 0;		// ジャンプの待機時間

}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyJump::~CEnemyJump()
{

}


//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyJump::Init(void)
{
	// 初期化処理
	CEnemy::Init();

	// 体力取得
	int nLife = GetLife();

	// 体力ゲージ
	m_pHPGauge = CHP_Gauge::Create(200.0f, nLife);

	if (m_pHPGauge == NULL)
	{// NULLだったら
		m_pHPGauge = NULL;
	}

	// ジャンプ待機時間
	m_nCntWaitJump = 30;

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyJump::Uninit(void)
{
	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
// 死亡処理
//==========================================================================
void CEnemyJump::Kill(void)
{
	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyJump::Update(void)
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

	// 現在のモーション種類取得
	int nType = m_pMotion->GetType();

	// ジャンプ待機時間減算
	m_nCntWaitJump--;
	if (m_nCntWaitJump <= 0)
	{
		m_nCntWaitJump = 0;
	}

	if (nType == MOTION_WALK)
	{// 移動モーション中

		// 頭パーツ
		CModel *pModel = GetObjectChara()->GetModel()[1];

		// 向き取得
		D3DXVECTOR3 rotParts = pModel->GetRotation();

		// 回転させる
		m_rotParts.y += D3DX_PI * 0.2f;

		// 角度の正規化
		RotNormalize(m_rotParts.y);

		// 向き設定
		pModel->SetRotation(D3DXVECTOR3(rotParts.x, m_rotParts.y, rotParts.z));

	}
	else
	{// 任意のモーション以外

		// 頭パーツ
		CModel *pModel = GetObjectChara()->GetModel()[1];

		// 向き取得
		D3DXVECTOR3 rotParts = pModel->GetRotation();

		// 元に戻す
		InertiaCorrection(m_rotParts.y, 0.0f, 0.25f);

		// 角度の正規化
		RotNormalize(m_rotParts.y);

		// 向き設定
		pModel->SetRotation(D3DXVECTOR3(rotParts.x, m_rotParts.y, rotParts.z));
	}

}

//==========================================================================
// 着地時の処理
//==========================================================================
void CEnemyJump::ProcessLanding(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 波発生
	if (m_state != STATE_NONE && m_state != STATE_DEAD  && m_sMotionFrag.bJump == true)
	{
		if (m_state == STATE_ATTACK)
		{
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
				-120.0f,											// 波の高さ
				10.0f,												// 波の広がり
				30,													// 発生時間
				CElevation::WAVETYPE_IMPACT,						// 種類
				CElevation::TYPE_ENEMY,							// プレイヤーの種類
				0													// レベル
			);
		}
		else
		{
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
				-60.0f,											// 波の高さ
				4.0f,												// 波の広がり
				20,													// 発生時間
				CElevation::WAVETYPE_IMPACT,						// 種類
				CElevation::TYPE_EFFECT,							// プレイヤーの種類
				0													// レベル
			);
		}

		if (m_state == STATE_ATTACK)
		{
			m_nCntWaitJump = 100;
		}
		else
		{
			m_nCntWaitJump = 30;
		}
	}

	// 着地時の処理
	CEnemy::ProcessLanding();
}

//==========================================================================
// 攻撃状態移行処理
//==========================================================================
void CEnemyJump::ChangeToAttackState(void)
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

		if (m_nCntWaitJump > 0)
		{// ジャンプ待機中は攻撃しない
			m_sMotionFrag.bATK = false;
		}
	}
}

//==========================================================================
// 出現
//==========================================================================
void CEnemyJump::Spawn(void)
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
		m_nCntState = 0;	// 遷移カウンター設定
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
void CEnemyJump::ChaseMove(float fMove)
{
	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	if (m_nCntWaitJump <= 0)
	{// ジャンプ待機時間が終わっていたら

		// 移動量加算
		move.x += sinf(D3DX_PI + rot.y) * fMove;
		move.z += cosf(D3DX_PI + rot.y) * fMove;
	}

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// モーションの設定
//==========================================================================
void CEnemyJump::MotionSet(void)
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
void CEnemyJump::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
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

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 種類別
	switch (m_pMotion->GetType())
	{
	case MOTION_WALK:
		// ジャンプさせる
		m_sMotionFrag.bJump = true;
		move.y = 0.0f;
		move.y += JUMP;

		// 移動量設定
		SetMove(move);
		break;

	case MOTION_ATK:

		if (m_sMotionFrag.bJump == false)
		{
			// ジャンプさせる
			m_sMotionFrag.bJump = true;
			move.y = 0.0f;
			move.y += JUMP;

			// 移動量設定
			SetMove(move);
		}
		break;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CEnemyJump::Draw(void)
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// 敵の情報取得
//==========================================================================
CEnemyJump *CEnemyJump::GetEnemy(void)
{
	// 自分自身のポインタを取得
	return this;
}