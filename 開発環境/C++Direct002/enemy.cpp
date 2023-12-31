//=============================================================================
// 
//  プレイヤー処理 [enemy.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "enemy.h"
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
#include "camera.h"
#include "ballast.h"
#include "3D_Effect.h"
#include "hp_gauge.h"
#include "shadow.h"
#include "sound.h"
#include "impactwave.h"
#include "score.h"

// 子クラス
#include "enemy_power.h"
#include "enemy_jump.h"
#include "enemy_crowd.h"
#include "enemy_impact_side.h"
#include "enemy_impact_center.h"


//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(60.0f)		// 横幅
#define HEIGHT			(60.0f)		// 縦幅
#define MAX_LIFE		(5)			// 体力
#define PLAYER_SERCH	(800.0f)	// プレイヤー探索範囲
#define CHACE_DISTABCE	(50.0f)		// 追い掛ける時の間隔
#define JUMP			(12.0f)		// ジャンプ力初期値
#define SURVIVALLIFE	(60 * 20)	// 生存時間
#define FADEOUT_TIME	(40)		// フェードアウト時間

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================


//==========================================================================
// コンストラクタ
//==========================================================================
CEnemy::CEnemy(int nPriority) : CObjectChara(nPriority)
{
	// 値のクリア
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 最初の位置
	memset(&m_sFormationInfo, NULL, sizeof(m_sFormationInfo));	// 隊列の情報
	m_SpawnType = CEnemyManager::SPAWNTYPE_NORMAL;	// 出現の種類
	m_posKnokBack = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ノックバックの位置
	m_rotOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 最初の向き
	m_type = TYPE_POWER;	// 種類
	m_state = STATE_NONE;	// 状態
	m_Oldstate = m_state;	// 前回の状態
	m_MoveType = MOVETYPE_RAMDOM;	// 行動の種類
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// マテリアルの色
	m_nCntState = 0;		// 状態遷移カウンター
	m_nTexIdx = 0;			// テクスチャのインデックス番号
	m_nNumChild = 0;		// この数
	m_nIdxManager = 0;		// マネージャのインデックス番号
	m_nSurvivalLife = 0;	// 生存時間
	m_nSurvivalLifeOrigin = 0;	// 生存時間
	m_bAddScore = false;	// スコア加算するかの判定
	m_AddType = CResultManager::ADDTYPE_NONE;	// スコアの加算量
	m_nBallastEmission = 0;	// 瓦礫の発生カウンター
	m_sMotionFrag.bJump = false;		// ジャンプ中かどうか
	m_sMotionFrag.bKnockback = false;	// ノックバック中かどうか
	m_sMotionFrag.bMove = false;		// 移動中かどうか
	m_sMotionFrag.bATK = false;			// 攻撃中かどうか
	m_pParent = NULL;					// 親のポインタ
	m_pHPGauge = NULL;					// HPゲージの情報
	m_pMotion = NULL;					// モーションの情報
	m_colorType = COLORTYPE_NORMAL;		// 色ごとの種類

	memset(&m_pChild[0], NULL, sizeof(m_pChild));	// 子のポインタ
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemy::~CEnemy()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CEnemy *CEnemy::Create(int nIdx, const char *pFileName, D3DXVECTOR3 pos, TYPE type, int nCharacter)
{
	// 生成用のオブジェクト
	CEnemy *pEnemy = NULL;

	if (pEnemy == NULL)
	{// NULLだったら

		// メモリの確保
		switch (type)
		{
		case TYPE_POWER:
			pEnemy = DEBUG_NEW CEnemyPower;
			break;

		case TYPE_CROWD:
			pEnemy = DEBUG_NEW CEnemyCrowd;
			break;

		case TYPE_JUMP:
			pEnemy = DEBUG_NEW CEnemyJump;
			break;

		case TYPE_BOMB:
			break;

		case TYPE_SPEED:
			break;

		case TYPE_IMPACT:
			pEnemy = DEBUG_NEW CEnemyImpactSide;
			break;

		case TYPE_IMPACTCENTER:
			pEnemy = DEBUG_NEW CEnemyImpactCenter;
			break;

		default:
			return NULL;
			break;
		}

		if (pEnemy != NULL)
		{// メモリの確保が出来ていたら

			if (nCharacter != -1)
			{
				pEnemy->m_colorType = (COLORTYPE)nCharacter;
			}

			// インデックス番号
			pEnemy->m_nIdxManager = nIdx;

			// 種類
			pEnemy->m_type = type;

			// 最初の位置設定
			pEnemy->m_posOrigin = pos;

			// 位置設定
			pEnemy->SetPosition(pos);

			// テキスト読み込み
			HRESULT hr = pEnemy->RoadText(pFileName);
			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 初期化処理
			pEnemy->Init();
		}

		return pEnemy;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemy::Init(void)
{
	// 各種変数の初期化
	m_state = STATE_SPAWN;	// 状態
	m_Oldstate = STATE_PLAYERCHASE;
	m_nCntState = 0;		// 状態遷移カウンター
	m_nSurvivalLife = SURVIVALLIFE;	// 生存時間
	m_posKnokBack = m_posOrigin;	// ノックバックの位置
	m_bAddScore = true;	// スコア加算するかの判定

	// 種類の設定
	SetType(TYPE_ENEMY);

	// 体力取得
	int nLife = GetLife();

	switch (m_type)
	{
	case TYPE_POWER:
		break;

	case TYPE_CROWD:
		break;

	case TYPE_JUMP:
		break;

	case TYPE_BOMB:
		// 体力ゲージ
		m_pHPGauge = CHP_Gauge::Create(80.0f, nLife);
		m_state = STATE_SPAWN;	// 出現状態

		// 移動量設定
		SetMove(D3DXVECTOR3(
			(float)Random(-31, 31) * 0.1f * 3.0f,
			20.0f + ((float)Random(-20, 20) * 0.1f),
			(float)Random(-31, 31) * 0.1f * 3.0f));
		break;

	case TYPE_SPEED:
		// 体力ゲージ
		m_pHPGauge = CHP_Gauge::Create(80.0f, nLife);
		m_state = STATE_PARENTCHASE;	// 親追い掛け状態
		break;

	case TYPE_IMPACT:
		break;

	case TYPE_IMPACTCENTER:
		m_nSurvivalLife = SURVIVALLIFE * 2;	// 生存時間
		break;
	}

	//if (m_pHPGauge == NULL)
	//{// NULLだったら
	//	m_pHPGauge = NULL;
	//}

	// 向き設定
	SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// 移動量設定
	SetMove(D3DXVECTOR3(0.0f, JUMP, 0.0f));

	// 影の生成
	m_pShadow = CShadow::Create(GetPosition(), GetRadius() * 0.5f);

	// 高さ取得
	bool bLand = false;
	m_posOrigin.y = CGame::GetObject3DMesh()->GetHeight(m_posOrigin, D3DXVECTOR3(0.0f, 0.0f, 0.0f), bLand, GetType());

	// 位置設定
	SetPosition(m_posOrigin);

	// ポーズのリセット
	m_pMotion->ResetPose(MOTION_DEF);

	// 生存時間
	m_nSurvivalLifeOrigin = m_nSurvivalLife;


	return S_OK;
}

//==========================================================================
// テキスト読み込み
//==========================================================================
HRESULT CEnemy::RoadText(const char *pFileName)
{
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
	m_pMotion->SetModel(pObjChar->GetModel(), pObjChar->GetNumModel(), pObjChar);

	return S_OK;
}

//==========================================================================
// 子登録
//==========================================================================
void CEnemy::RegistrChild(CEnemy *pChild)
{
	if (m_nNumChild >= mylib_const::MAX_ENEMY)
	{
		return;
	}

	if (m_pChild[m_nNumChild] == NULL)
	{
		m_pChild[m_nNumChild] = pChild;
		m_nNumChild++;	// 子の数加算
	}
	else
	{
		int n = 0;
	}
}

//==========================================================================
// 子のリセット
//==========================================================================
void CEnemy::ResetChild(CEnemy *pChild)
{
	int nMax = m_nNumChild;

	for (int nCntChild = 0; nCntChild < nMax; nCntChild++)
	{
		if (m_pChild[nCntChild] == NULL)
		{
			continue;
		}

		if (m_pChild[nCntChild] == pChild)
		{
			m_pChild[nCntChild] = NULL;
		}
	}
}


//==========================================================================
// 親のポインタ設定
//==========================================================================
void CEnemy::SetParent(CEnemy *pParent)
{
	if (this != nullptr)
	{
		// 子の割り当て
		if (pParent != NULL)
		{
			pParent->RegistrChild(this);
		}

		// 親のポインタ渡す
		m_pParent = pParent;
		
	}
	else
	{
		int n = 0;
	}
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemy::Uninit(void)
{
	int nIdx = m_nIdxManager;

	// 終了処理
	if (m_pMotion != NULL)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// 影を消す
	if (m_pShadow != NULL)
	{
		m_pShadow = NULL;
	}

	// 終了処理
	CObjectChara::Uninit();

	// マネージャの終了
	CEnemyManager::Release(nIdx);
}

//==========================================================================
// 死亡処理
//==========================================================================
void CEnemy::Kill(void)
{

	// スコア加算
	if (m_AddType != CResultManager::ADDTYPE_NONE && CManager::GetMode() == CScene::MODE_GAME)
	{
		CGame::GetScore()->Add(m_AddType);

		if (m_bAddScore == true)
		{// スコア加算する判定だったら
			CGame::GetScore()->Add(GetAddScoreValue());
		}
	}

	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_ENEMY; nCntEnemy++)
	{// 子の数分回す

		if (m_pChild[nCntEnemy] != NULL)
		{// 子がいる場合

			//m_pChild[nCntEnemy]->SetParent(NULL);

			// 自分の子の親をNULLにする
			m_pChild[nCntEnemy]->m_pParent = NULL;
		}
	}

	if (m_pParent != NULL)
	{// 自分に親がいる場合

		// 親の子をNULLにする
		m_pParent->ResetChild(this);
	}

	// HPゲージを消す
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->Uninit();
		m_pHPGauge = NULL;
	}

	// 影を消す
	if (m_pShadow != NULL)
	{
		m_pShadow->Uninit();
		m_pShadow = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemy::Update(void)
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
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

	// 過去の位置設定
	SetOldPosition(GetPosition());

	// 当たり判定
	Collision();

	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	

	// モーションの設定処理
	MotionSet();

	// 状態更新
	UpdateState();

	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 攻撃処理
	Atack();

	// モーションの更新
	m_pMotion->Update();

	// 種類別処理
	UpdateByType();

	// HPゲージの位置更新
	if (m_pHPGauge != NULL)
	{
		// 位置取得
		D3DXVECTOR3 pos = GetPosition();

		// 体力取得
		int nLife = GetLife();

		m_pHPGauge->UpdatePosition(pos, nLife);
	}

	// 影の位置更新
	if (m_pShadow != NULL)
	{
		// 位置取得
		D3DXVECTOR3 pos = GetPosition();
		m_pShadow->SetPosition(D3DXVECTOR3(pos.x, m_pShadow->GetPosition().y, pos.z));
	}

	for (int nCntEffect = 0; nCntEffect < mylib_const::MAX_OBJ; nCntEffect++)
	{
		CEffect3D *pEffect = GetEffectParent(nCntEffect);

		if (pEffect == NULL)
		{// NULLだったら
			continue;
		}

		// エフェクトの位置更新
		pEffect->UpdatePosition(GetPosition(), GetRotation());
	}


	if (GetPosition().y <= mylib_const::KILL_Y)
	{
		// 敵の終了処理
		Kill();
		Uninit();
		return;
	}

	// 生存時間減算
	m_nSurvivalLife--;

	//if (m_nSurvivalLife % 25 == 0)
	//{
	//	// 歩行音再生
	//	CManager::GetSound()->PlaySound(CSound::LABEL_SE_WALK);
	//}

	if (m_nSurvivalLife >= m_nSurvivalLifeOrigin - 30)
	{// 生存

		int nNowLife = m_nSurvivalLifeOrigin - m_nSurvivalLife;
		float fAlpha = 1.5f * (1.0f - (float)nNowLife / 30.0f);

		if (m_nSurvivalLife % 8 == 0)
		{
			// 位置取得
			D3DXVECTOR3 pos = GetPosition();
			float fRadius = GetRadius() * 0.5f;
			D3DXVECTOR3 spawnpos = D3DXVECTOR3(pos.x + (float)Random(0, 200) * 0.1f, m_posOrigin.y - 10.0f, pos.z + (float)Random(0, 200) * 0.1f);
			D3DXVECTOR3 spawnmove = D3DXVECTOR3((float)Random(-100, 100) * 0.01f, (float)Random(0, 90) * 0.1f + 2.0f, (float)Random(-100, 100) * 0.01f);

			// 煙
			CEffect3D::Create(
				spawnpos,
				spawnmove,
				D3DXCOLOR(0.5f, 0.5f, 0.2f, fAlpha),
				fRadius,
				60,
				CEffect3D::MOVEEFFECT_ADD,
				CEffect3D::TYPE_SMOKEBLACK,
				fRadius * 0.1f);


			// 出現情報
			spawnpos = D3DXVECTOR3(pos.x + (float)Random(0, 200) * 0.1f, m_posOrigin.y, pos.z + (float)Random(0, 200) * 0.1f);
			spawnmove = D3DXVECTOR3((float)Random(-100, 100) * 0.01f, (float)Random(0, 90) * 0.1f + 2.0f, (float)Random(-100, 100) * 0.01f);

			// 砂
			CEffect3D::Create(
				spawnpos,
				spawnmove,
				D3DXCOLOR(0.6f, 0.6f, 0.3f, fAlpha),
				fRadius * 0.8f,
				60,
				CEffect3D::MOVEEFFECT_ADD,
				CEffect3D::TYPE_SANDBLACK,
				(fRadius * 0.8f) * 0.0625f);
		}
	}

	if (CManager::GetMode() == CScene::MODE_GAME &&
		m_nSurvivalLife <= 0 && CManager::GetScene()->GetWaveManager()->GetNowStage() >= 0 &&
		m_state != STATE_FADEOUT && m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_KNOCKBACK)
	{// 生存時間が無くなったら

		m_nCntState = 0;
		m_state = STATE_FADEOUT;
		return;
	}

}

//==========================================================================
// 当たり判定
//==========================================================================
void CEnemy::Collision(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 重力処理
	if (m_type == TYPE_JUMP && m_state != STATE_SPAWN)
	{
		move.y -= mylib_const::GRAVITY * 0.3f;
	}
	else if (
		m_state != STATE_KNOCKBACK &&
		m_state != STATE_SPAWN &&
		m_state != STATE_IMPALSE &&
		m_state != STATE_DMG &&
		m_state != STATE_DEAD &&
		m_state != STATE_SPAWN &&
		m_state != STATE_FADEOUT)
	{
		move.y -= mylib_const::GRAVITY;
	}

	// 位置更新
	pos += move;

	// 慣性補正
	if (m_state != STATE_KNOCKBACK && m_state != STATE_SPAWN && m_state != STATE_IMPALSE && m_state != STATE_DMG && m_state != STATE_DEAD)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	if (m_state == STATE_DEAD)
	{
		move.x += (0.0f - move.x) * 0.01f;
		move.z += (0.0f - move.z) * 0.01f;
	}

	if (move.x >= 0.1f || move.x <= -0.1f ||
		move.z >= 0.1f || move.z <= -0.1f)
	{// 移動中
		m_sMotionFrag.bMove = true;
	}
	else
	{
		m_sMotionFrag.bMove = false;
	}


	// 着地したかどうか
	bool bLand = false;

	// 高さ取得
	float fHeight = CGame::GetObject3DMesh()->GetHeight(pos, move, bLand, GetType());


	if (fHeight > pos.y)
	{// 地面の方が自分より高かったら

		// 地面の高さに補正
		if (m_state != STATE_DEAD && m_state != STATE_FADEOUT)
		{
			pos.y = fHeight;
		}

		if (bLand == true)
		{// 着地してたら

			// 着地時処理
			ProcessLanding();

			// ジャンプ使用可能にする
			if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_DEAD)
			{
				move.y = 0.0f;
			}
			m_sMotionFrag.bJump = false;
		}
	}

	if (m_state != STATE_FADEOUT)
	{
		// 箱
		float fLen = CGame::GetObject3DMesh()->GetWidthLen();
		int nBlock = CGame::GetObject3DMesh()->GetWidthBlock();
		nBlock /= 2;
		bool bHit = false;
		if (pos.x + GetRadius() > fLen * nBlock) { pos.x = fLen * nBlock - GetRadius(); bHit = true; }
		if (pos.x - GetRadius() < -fLen * nBlock) { pos.x = -fLen * nBlock + GetRadius(); bHit = true; }
		if (pos.z + GetRadius() > fLen * nBlock) { pos.z = fLen * nBlock - GetRadius(); bHit = true; }
		if (pos.z - GetRadius() < -fLen * nBlock) { pos.z = -fLen * nBlock + GetRadius(); bHit = true; }

		if (bHit == true && m_pParent == NULL)
		{// 限界まで行ってて親だったら
			float fRotDest = GetRotDest();
			fRotDest += D3DX_PI * 0.5f;
			SetRotDest(fRotDest);
		}

		if (CManager::GetMode() == CScene::MODE_GAME && 
			bHit == true && m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_KNOCKBACK && m_state != STATE_FADEOUT)
		{
			// フェードアウト状態にする
			m_state = STATE_FADEOUT;
			m_nCntState = 0;
		}
	}

	if (pos.y <= -100.0f)
	{
		pos.y = -100.0f;
	}

	if (pos.y <= -800.0f)
	{
		pos.y = -800.0f;
	}



	if (m_state == STATE_IMPALSE || (m_type == TYPE_BOMB && IsWaveHit() == true))
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

			if (pObj == CObject::GetObject())
			{// 自分と同じだったら

				// 次のオブジェクトを代入
				pObj = pObjNext;
				continue;
			}

			// 種類の取得
			if (pObj->GetType() != CObject::TYPE_BOSS)
			{// 敵じゃなかったら

				// 次のオブジェクトを代入
				pObj = pObjNext;
				continue;
			}

			if (SphereRange(pos, pObj->GetPosition(), GetRadius(), pObj->GetObjectChara()->GetRadius()) == true)
			{// 球の判定内に入ったら

				if (Hit(50))
				{
					// パーティクル生成
					/*my_particle::Create(pos, my_particle::TYPE_MAGIC_EXPLOSION);
					my_particle::Create(D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z), my_particle::TYPE_EXPLOSION_START);*/
				}

				// 親の位置取得
				D3DXVECTOR3 TargetPos = pObj->GetPosition();

				// ヒット処理
				if (m_state == STATE_IMPALSE)
				{
					pObj->Hit(0, D3DXVECTOR3(sinf(D3DX_PI + rot.y) * 27.0f, 0.0f, cosf(D3DX_PI + rot.y) * 27.0f));
				}
				else
				{
					// 目標の角度を求める
					float fDest = atan2f((pos.x - TargetPos.x), (pos.z - TargetPos.z));

					pObj->Hit(0, D3DXVECTOR3(sinf(D3DX_PI + fDest) * 15.0f, 0.0f, cosf(D3DX_PI + fDest) * 15.0f));
				}
				break;
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
	}



	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// 着地時の処理
//==========================================================================
void CEnemy::ProcessLanding(void)
{
	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// ジャンプ使用可能にする
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_DEAD)
	{
		move.y = 0.0f;
	}
	m_sMotionFrag.bJump = false;

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// ヒット処理
//==========================================================================
bool CEnemy::Hit(const int nValue)
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
		if (m_type == TYPE_BOMB && m_state == STATE_BURY)
		{// ボムが埋まっているとき

			// 吹き飛び状態にする
			m_state = STATE_IMPALSE;

			// 遷移カウンター設定
			m_nCntState = 0;

			// ノックバックの位置更新
			m_posKnokBack = GetPosition();
		}
		else
		{
			m_state = STATE_KNOCKBACK;

			// 遷移カウンター設定
			m_nCntState = 30;

			D3DXVECTOR3 move = GetMove();
			move.y = 0.0f;
			move.y += 10.5f;
			SetMove(move);
		}
	}
	else if (nValue == 0 && m_state == STATE_KNOCKBACK)
	{
		return true;
	}
	else
	{
		if (m_state != STATE_DMG && m_state != STATE_KNOCKBACK && m_state != STATE_DEAD && m_state != STATE_SPAWN)
		{// なにもない状態の時

			if (nValue == mylib_const::DIRECTWAVE_DMG)
			{
				CManager::SetEnableHitStop(5);

				// 振動
				CManager::GetCamera()->SetShake(5, 10.0f, 0.0f);
			}
			else if (nValue == mylib_const::IMPACTWAVE_DMG)
			{
				if (m_type != TYPE_CROWD)
				{
					CManager::SetEnableHitStop(2);
				}
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

				if (m_type == TYPE_IMPACTCENTER || m_type == TYPE_IMPACT)
				{// 衝撃波の時だったら
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
				else
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
				}
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

				// 間隔内に倒した数取得
				if (CGame::GetEnemyManager()->GetNumIntervalKill())
				{

				}

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
	}

	// 死んでない
	return false;
}

//==========================================================================
// 種類別更新処理
//==========================================================================
void CEnemy::UpdateByType(void)
{

	// 現在のモーション種類取得
	int nType = m_pMotion->GetType();

	switch (m_type)
	{
	case TYPE_POWER:
		break;

	case TYPE_CROWD:
		break;

	case TYPE_JUMP:
		break;

	case TYPE_BOMB:
		break;

	case TYPE_SPEED:
		break;

	case TYPE_IMPACT:
		break;

	case TYPE_IMPACTCENTER:
		break;

	default:
		break;
	}
}

//==========================================================================
// 状態更新処理
//==========================================================================
void CEnemy::UpdateState(void)
{
	//return;
#if TOPCUR
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 pos11 = D3DXVECTOR3(GetObjectChara()->GetModel()[0]->GetWorldMtx()._41, GetObjectChara()->GetModel()[0]->GetWorldMtx()._42, GetObjectChara()->GetModel()[0]->GetWorldMtx()._43);

	//CEffect3D::Create(pos11, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), GetRadius(), 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);

	switch (m_state)
	{
	case STATE_NONE:

		// 状態遷移カウンター減算
		m_nCntState--;

		if (m_nCntState <= 0)
		{// 遷移カウンターが0になったら

			// プレイヤー探索状態にする
			m_state = STATE_PLAYERSEARCH;
			m_nCntState = 60 * 8;
		}

		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		break;

	case STATE_SPAWN:
		Spawn();
		break;

	case STATE_BURY:
		Bury();
		break;

	case STATE_OCCUR:
		Occur();
		break;

	case STATE_IMPALSE:
		Impalse();
		break;

	case STATE_DMG:
		Damage();
		break;

	case STATE_DEAD:
		Dead();
		break;

	case STATE_FADEOUT:
		FadeOut();
		break;

	case STATE_KNOCKBACK:
		KnockBack();
		break;

	case STATE_PLAYERCHASE:
		PlayerChase();
		break;

	case STATE_RETURNBASE:
		ReturnBase();
		break;

	case STATE_PLAYERSEARCH:
		PlayerSearch();
		break;

	case STATE_PARENTCHASE:
		ParentChase();
		break;

	case STATE_ATTACK:
		StateAttack();
		break;

	case STATE_TURRETATK:
		TurretAttack();
		break;

	case STATE_TURRETWAIT:
		TurretWait();
		break;

	case STATE_WAIT:
		StateWait();
		break;

	}
#endif
}

//==========================================================================
// 種類別状態更新処理
//==========================================================================
void CEnemy::UpdateStateByType(void)
{
	return;
}

//==========================================================================
// 出現
//==========================================================================
void CEnemy::Spawn(void)
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
		m_nCntState = 0;		// 遷移カウンター設定
		move.y = 0.0f;			// 移動量ゼロ
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// 埋まり
//==========================================================================
void CEnemy::Bury(void)
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
	m_nCntState--;

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら

		// 親追い掛け状態にする
		m_state = STATE_OCCUR;
		m_nCntState = 60;
		move.y = 30.0f;		// 移動量ゼロ
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
// 起き上がり
//==========================================================================
void CEnemy::Occur(void)
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
	m_nCntState--;

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら

		// 親追い掛け状態にする
		m_state = STATE_PLAYERCHASE;
		m_nCntState = 0;
	}

	float fRotDestX = 0.0f;
	float fRotDestZ = 0.0f;

	// 目標との差分
	float fRotDiffX = fRotDestX - rot.x;
	float fRotDiffZ = fRotDestZ - rot.z;

	// 角度の正規化
	RotNormalize(fRotDiffX);
	RotNormalize(fRotDiffZ);

	// 角度の補正をする
	rot.x += fRotDiffX * 0.25f;
	rot.z += fRotDiffZ * 0.25f;

	// 角度の正規化
	RotNormalize(rot);


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
// 衝撃
//==========================================================================
void CEnemy::Impalse(void)
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
	pos = (D3DXVECTOR3(0.0f, -0.4f, 0.0f) * (float)(m_nCntState * m_nCntState) + move * (float)m_nCntState) + m_posKnokBack;

	// 起伏との判定
	if (CGame::GetObject3DMesh()->IsHit(pos))
	{// 地面と当たっていたら
		if (Hit(50))
		{
			// パーティクル生成
			my_particle::Create(pos, my_particle::TYPE_MAGIC_EXPLOSION);
			my_particle::Create(D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z), my_particle::TYPE_EXPLOSION_START);
		}
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
// ダメージ
//==========================================================================
void CEnemy::Damage(void)
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

#if _DEBUG
	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
#endif

	// 状態遷移カウンター減算
	m_nCntState++;

	// 位置更新
#if 0
	pos = (D3DXVECTOR3(0.0f, -0.4f, 0.0f) * (float)(m_nCntState * m_nCntState) + move * (float)m_nCntState) + m_posKnokBack;
#else
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + move.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;
#endif

	// 起伏との判定
	if (CGame::GetObject3DMesh()->IsHit(pos) && m_nCntState >= 20)
	{// 地面と当たっていたら
		m_state = m_Oldstate;
		m_nCntState = 0;
		move.y = 0.0f;			// 移動量ゼロ
		
		// ノックバック判定消す
		m_sMotionFrag.bKnockback = false;
		m_pMotion->ToggleFinish(true);
	}

	if (m_nCntState >= 30)
	{// 遷移カウンターが20になったら

		// ノックバック状態にする
		m_state = STATE_KNOCKBACK;
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
// 死亡
//==========================================================================
void CEnemy::Dead(void)
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

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);
	m_mMatcol.a -= 1.0f / 80.0f;

	// 位置更新
	pos = (D3DXVECTOR3(0.0f, -0.15f, 0.0f) * (float)(m_nCntState * m_nCntState) + move * (float)m_nCntState) + m_posKnokBack;

	if (CGame::GetEnemyManager()->GetIntervalKill() <= 0 &&
		(m_nCntState >= 40 || (CGame::GetObject3DMesh()->IsHit(pos) && m_nCntState >= 20)))
	{// 遷移カウンターが0になったら or 地面に接触

		// パーティクル生成
		my_particle::Create(pos, my_particle::TYPE_ENEMY_FADE);
		//my_particle::Create(D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z), my_particle::TYPE_EXPLOSION_START);

		// 敵の終了処理
		Kill();
		Uninit();
		return;
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
// フェードアウト
//==========================================================================
void CEnemy::FadeOut(void)
{
	// 移動量取得
	float fMove = GetVelocity();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// フェードアウトモーション設定
	m_pMotion->Set(MOTION_FADEOUT);

	// フェードアウトのフレーム数
	int nAllFrame = m_pMotion->GetMaxAllCount(MOTION_FADEOUT);

	// モーションの情報取得
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// 攻撃情報の総数取得
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	// 高さ取得
	bool bLand = false;
	float fHeight = CGame::GetObject3DMesh()->GetHeight(pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), bLand, GetType());

	int nIntBallast = 0;

	switch (CManager::GetMode())
	{
	case CScene::MODE_TITLE:
		nIntBallast = 2;
		break;

	case CScene::MODE_GAME:
		nIntBallast = 1;
		break;

	default:
		break;
	}

	// 発生カウンター加算
	m_nBallastEmission = (m_nBallastEmission + 1) % nIntBallast;

	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (aInfo.AttackInfo[nCntAttack] == NULL)
		{// NULLだったら
			continue;
		}

		// モーションカウンター取得
		if (m_nBallastEmission == 0 && m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt)
		{// 攻撃判定中

			// 瓦礫生成
			if (m_type == TYPE_IMPACTCENTER)
			{// 衝撃波のやつ
				CBallast::Create(D3DXVECTOR3(pos.x, fHeight + 5.0f, pos.z), D3DXVECTOR3(2.0f, 10.0f, 2.0f), 8);
			}
			else
			{
				CBallast::Create(D3DXVECTOR3(pos.x, fHeight + 5.0f, pos.z), D3DXVECTOR3(1.0f, 6.0f, 1.0f), 1);
			}
		}
	}

	m_sMotionFrag.bMove = false;	// 移動判定OFF
	m_sMotionFrag.bATK = false;		// 攻撃判定OFF

	// 遷移カウンター加算
	m_nCntState++;

	if (m_nCntState >= nAllFrame)
	{// 遷移カウンターがモーションを超えたら

		// スコア加算の判定オフ
		m_bAddScore = false;

		// 敵の終了処理
		Kill();
		Uninit();
		return;
	}
}

//==========================================================================
// ノックバック
//==========================================================================
void CEnemy::KnockBack(void)
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
#if 0
	pos = (D3DXVECTOR3(0.0f, -0.4f, 0.0f) * (float)(m_nCntState * m_nCntState) + move * (float)m_nCntState) + m_posKnokBack;
#else
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + move.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;
#endif

	// 起伏との判定
	if (CGame::GetObject3DMesh()->IsHit(pos) && m_nCntState >= 20)
	{// 地面と当たっていたら
		m_state = m_Oldstate;
		m_nCntState = 0;
		move.y = 0.0f;	// 移動量ゼロ
		
		// ノックバック判定消す
		m_sMotionFrag.bKnockback = false;
		m_pMotion->ToggleFinish(true);
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
// プレイヤー探索
//==========================================================================
void CEnemy::PlayerSearch(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

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
	m_nCntState--;

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら
		m_nCntState = 0;
		m_state = STATE_RETURNBASE;
	}

	if (m_pParent != NULL)
	{// 親がいる場合

		// 親の移動量取得
		D3DXVECTOR3 moveParent = m_pParent->GetMove();

		// 親の位置取得
		D3DXVECTOR3 posParent = m_pParent->GetPosition();

		// 目標の角度を求める
		fRotDest = atan2f((pos.x - posParent.x), (pos.z - posParent.z));

		// 目標との差分
		fRotDiff = fRotDest - rot.y;

		//角度の正規化
		RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		RotNormalize(rot.y);

		if (CircleRange(pos, posParent, CHACE_DISTABCE, CHACE_DISTABCE) == true)
		{// 一定距離間に親が入ったら
			bLen = true;	// 長さ判定
		}

		// 向いてる方向にダッシュ
		if (bLen == false)
		{// 距離が保たれていたら

			// 追い掛け移動処理
			ChaseMove(fMove * 1.5f);
		}
	}
	else
	{// 自分自身が親の時

		static int nCnt = 0;

		float fRotDiff = 0.0f;	// 現在と目標の差分

		nCnt = (nCnt + 1) % 120;

		// 目標の角度を求める
		if (nCnt == 0)
		{
			fRotDest = Random(-31, 31) * 0.1f;
		}

		// 目標との差分
		fRotDiff = fRotDest - rot.y;

		//角度の正規化
		RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		RotNormalize(rot.y);

		// 追い掛け移動処理
		ChaseMove(fMove);

#if _DEBUG
		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 0.5f, 1.0f, 1.0f);
#endif
	}

	// プレイヤー追従の判定
	TriggerChasePlayer();


	// 位置設定
	SetPosition(pos);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// プレイヤー追従
//==========================================================================
void CEnemy::PlayerChase(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

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


	// 状態遷移カウンター減算
	m_nCntState--;

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら
		m_nCntState = 0;
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
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		RotNormalize(rot.y);

		// 向き設定
		SetRotation(rot);

#if 0
		// 先頭を保存
		pObj = CObject::GetTop(mylib_const::ENEMY_PRIORITY);

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

			if (pObj == CObject::GetObject())
			{// 自分と同じだったら

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

			if (CircleRange(pos, pObj->GetPosition(), 50.0f, 50.0f) == true && m_nCntState <= 0)
			{// 一定距離間に仲間が入ったら

				// 間隔をあける状態にする
				m_state = STATE_PLAYERCHASE;

				// 目標の角度を求める
				fRotDest = fRotDest + +Random(-50, 50) * 0.01f;
				break;
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
#endif

		// 攻撃状態移行処理
		ChangeToAttackState();

		// 向いてる方向にダッシュ
		if (bLen == false)
		{// 距離が保たれていたら

			// 追い掛け移動処理
			ChaseMove(fMove);
		}

		float fRadius = PLAYER_SERCH;

		if (m_type == TYPE_CROWD)
		{
			fRadius *= 0.5f;

			if (m_colorType == COLORTYPE_CONFUSION)
			{
				fRadius *= 0.5f;
			}
		}

		if (CircleRange(pos, pPlayer->GetPosition(), 200.0f, fRadius) == false &&
			m_SpawnType != CEnemyManager::SPAWNTYPE_CIRCLE &&
			m_SpawnType != CEnemyManager::SPAWNTYPE_CLOCK &&
			m_SpawnType != CEnemyManager::SPAWNTYPE_BOTHCLOCK)
		{// プレイヤーが視界から消えたら

			if (m_type == TYPE_CROWD)
			{
				m_state = STATE_PARENTCHASE;
			}
			else
			{
				m_state = STATE_PLAYERSEARCH;
			}
			m_nCntState = 60 * 10;
		}
	}


	// 位置設定
	SetPosition(pos);

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 親追従
//==========================================================================
void CEnemy::ParentChase(void)
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

	// かなり離れてるかの判定
	bool bLongDistance = true;

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	// オブジェクト情報
	CObject *pObj = NULL;


	// 状態遷移カウンター減算
	//m_nCntState++;

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら
		m_nCntState = 0;
	}

	if (m_pParent != NULL)
	{// 親がいる場合

		// 親の移動量取得
		D3DXVECTOR3 moveParent = m_pParent->GetMove();

		// 親の位置取得
		D3DXVECTOR3 posParent = m_pParent->GetPosition();

		// 親の向き取得
		D3DXVECTOR3 rotParent = m_pParent->GetRotation();

		// 目標の位置
		D3DXVECTOR3 posDest;

		switch (m_MoveType)
		{
		case CEnemy::MOVETYPE_RAMDOM:
			posDest = posParent;
			break;

		case CEnemy::MOVETYPE_ROTATION:
			posDest = posParent;
			break;

		case CEnemy::MOVETYPE_FORMATION:
			posDest = D3DXVECTOR3(
				posParent.x + sinf(D3DX_PI + m_sFormationInfo.fAngle + rotParent.y) * m_sFormationInfo.fLength,
				posParent.y,
				posParent.z + cosf(D3DX_PI + m_sFormationInfo.fAngle + rotParent.y) * m_sFormationInfo.fLength);
			break;

		case CEnemy::MOVETYPE_PLAYERRUSH:
			posDest = D3DXVECTOR3(
				posParent.x + sinf(D3DX_PI + m_sFormationInfo.fAngle + rotParent.y) * m_sFormationInfo.fLength,
				posParent.y,
				posParent.z + cosf(D3DX_PI + m_sFormationInfo.fAngle + rotParent.y) * m_sFormationInfo.fLength);
			break;
		}

		// 目標の角度を求める
		fRotDest = atan2f((pos.x - posDest.x), (pos.z - posDest.z));

		// 目標との差分
		fRotDiff = fRotDest - rot.y;

		//角度の正規化
		RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		RotNormalize(rot.y);

		// 向き設定
		SetRotation(rot);

		switch (m_MoveType)
		{
		case CEnemy::MOVETYPE_RAMDOM:

			if (CircleRange(pos, posDest, 25.0f, CHACE_DISTABCE) == true)
			{// 一定距離間に親が入ったら
				bLen = true;	// 長さ判定
			}

			// 向いてる方向にダッシュ
			if (bLen == false)
			{// 距離が保たれていたら

				// 追い掛け移動処理
				ChaseMove(fMove * 1.4f);
			}
			break;

		case CEnemy::MOVETYPE_ROTATION:

			// 移動量増加
			move.x += sinf(D3DX_PI + rot.y) * fMove;
			move.z += cosf(D3DX_PI + rot.y) * fMove;

			// 移動量設定
			SetMove(move);
			break;

		case CEnemy::MOVETYPE_FORMATION:
			if (CircleRange(pos, posDest, 25.0f, 25.0f) == true)
			{// 一定距離間に親が入ったら
				bLen = true;	// 長さ判定
			}

			// かなり離れてるかの判定
			bLongDistance = CircleRange(pos, posDest, 25.0f, 150.0f);

			// 向いてる方向にダッシュ
			if (bLen == false)
			{// 距離が保たれていたら

				if (bLongDistance == true)
				{// かなり離れてなかったら

					// 追い掛け移動処理
					ChaseMove(fMove * 1.4f);
				}
				else
				{
					// 追い掛け移動処理
					ChaseMove(fMove * 2.8f);
				}
			}
			break;

		case CEnemy::MOVETYPE_PLAYERRUSH:
			if (CircleRange(pos, posDest, 25.0f, CHACE_DISTABCE) == true)
			{// 一定距離間に親が入ったら
				bLen = true;	// 長さ判定
			}

			// かなり離れてるかの判定
			bLongDistance = CircleRange(pos, posDest, 25.0f, 200.0f);

			// 向いてる方向にダッシュ
			if (bLen == false)
			{// 距離が保たれていたら

				if (bLongDistance == true)
				{// かなり離れてなかったら

					// 追い掛け移動処理
					ChaseMove(fMove * 1.4f);
				}
				else
				{
					// 追い掛け移動処理
					ChaseMove(fMove * 2.4f);
				}
			}
			break;

		case CEnemy::MOVETYPE_MAX:
			break;

		default:
			break;
		}

	}
	else
	{// 自分自身が親の時

		float fRotDiff = 0.0f;	// 現在と目標の差分

		// 状態遷移カウンター更新
		m_nCntState = (m_nCntState + 1) % 120;

		switch (m_MoveType)
		{
		case CEnemy::MOVETYPE_RAMDOM:

			// 目標の角度を求める
			if (m_nCntState == 0)
			{
				fRotDest = Random(-31, 31) * 0.1f;
			}
			break;

		case CEnemy::MOVETYPE_ROTATION:

			// 目標の角度を回転
			fRotDest += D3DX_PI * 0.005f;

			// 角度の正規化
			RotNormalize(fRotDest);
			break;

		case CEnemy::MOVETYPE_MAX:
			break;

		default:
			break;
		}

		// 目標との差分
		fRotDiff = fRotDest - rot.y;

		//角度の正規化
		RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.025f;

		// 角度の正規化
		RotNormalize(rot.y);

		// 向き設定
		SetRotation(rot);

		// 追い掛け移動処理
		ChaseMove(fMove);

#if _DEBUG
		// 色設定
		m_mMatcol = D3DXCOLOR(1.0f, 0.5f, 1.0f, 1.0f);
#endif
	}

	if (m_colorType == COLORTYPE_POISON ||
		m_colorType == COLORTYPE_CONFUSION)
	{// 毒キノコは攻撃的

		// プレイヤー追従の判定
		TriggerChasePlayer();
	}

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 拠点に帰る
//==========================================================================
void CEnemy::ReturnBase(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

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


	// 目標の角度を求める
	fRotDest = atan2f((pos.x - m_posOrigin.x), (pos.z - m_posOrigin.z));

	// 目標との差分
	fRotDiff = fRotDest - rot.y;

	//角度の正規化
	RotNormalize(fRotDiff);

	//角度の補正をする
	rot.y += fRotDiff * 0.025f;

	// 角度の正規化
	RotNormalize(rot.y);

	if (CircleRange(pos, m_posOrigin, 25.0f, CHACE_DISTABCE) == true)
	{// 一定距離間に親が入ったら
		bLen = true;	// 長さ判定
		m_state = STATE_NONE;
		m_nCntState = 90;
	}

	// 向いてる方向にダッシュ
	if (bLen == false)
	{// 距離が保たれていたら

		// 追い掛け移動処理
		ChaseMove(fMove * 1.2f);
	}

	// プレイヤー追従の判定
	TriggerChasePlayer();

	// 位置設定
	SetPosition(pos);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 攻撃処理
//==========================================================================
void CEnemy::StateAttack(void)
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

	// オブジェクト情報
	CObject *pObj = NULL;


	// 現在の種類取得
	int nType = m_pMotion->GetType();

	if (m_pParent != NULL)
	{// 親がいる場合

		// 親も追い掛け状態にする
		if (m_pParent->m_state != STATE_ATTACK)
		{
			if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD)
			{
				m_pParent->m_state = STATE_PLAYERCHASE;
				m_pParent->m_nCntState = 60;
			}

			for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
			{// 子の数分回す
				if (m_pParent->m_pChild[nCntEnemy] == NULL)
				{
					continue;
				}

				if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_KNOCKBACK)
				{
					continue;
				}

				m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				m_pParent->m_pChild[nCntEnemy]->m_nCntState = 60;
			}
		}
	}
	else
	{// 自分が親な場合

		for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
		{// 子の数分回す

			if (m_pChild[nCntEnemy] == NULL)
			{// NULLだったら
				continue;
			}

			if (m_pChild[nCntEnemy]->m_state != STATE_ATTACK)
			{// 攻撃状態の時

				if (m_pChild[nCntEnemy]->m_state != STATE_DMG && m_pChild[nCntEnemy]->m_state != STATE_DEAD && m_pChild[nCntEnemy]->m_state != STATE_KNOCKBACK)
				{
					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
					m_pChild[nCntEnemy]->m_nCntState = 60;
				}
			}
		}
	}

	if (nType == MOTION_DEF && pPlayer != NULL &&
		(m_type != TYPE_JUMP || (m_type == TYPE_JUMP && m_sMotionFrag.bJump == false)))
	{// ニュートラルに戻れば

		if (CircleRange(pos, pPlayer->GetPosition(), 400.0f, PLAYER_SERCH) == false)
		{// プレイヤーと離れすぎていたら

			// 間隔をあける状態にする
			m_state = STATE_PLAYERSEARCH;
			m_nCntState = 60 * 8;
		}
		else
		{// まだ追える時

			m_state = STATE_PLAYERCHASE;

			if (m_pParent != NULL)
			{// 親がいる場合

				// 親も追い掛け状態にする
				if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD && m_pParent->m_state != STATE_KNOCKBACK)
				{
					m_pParent->m_state = STATE_PLAYERCHASE;
				}

				for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
				{// 子の数分回す

					if (m_pParent->m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_KNOCKBACK)
					{
						continue;
					}

					m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
			else
			{// 自分が親な場合

				for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
				{// 子の数分回す

					if (m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pChild[nCntEnemy]->m_state == STATE_DEAD || m_pChild[nCntEnemy]->m_state == STATE_KNOCKBACK)
					{
						continue;
					}

					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
		}
	}

	switch (m_type)
	{
	case TYPE_POWER:
		break;

	case TYPE_CROWD:
		break;

	case TYPE_JUMP:
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


	// 色設定
#if _DEBUG
	m_mMatcol = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
#endif
}

//==========================================================================
// 攻撃処理
//==========================================================================
void CEnemy::TurretAttack(void)
{
	// 現在の種類取得
	int nType = m_pMotion->GetType();

	if (nType == MOTION_DEF)
	{// ニュートラルに戻れば

		// 待機状態にする
		m_state = STATE_TURRETWAIT;

		switch (m_type)
		{
		case TYPE_IMPACT:
			m_nCntState = 60 * 4;
			break;

		case TYPE_IMPACTCENTER:
			m_nCntState = (int)(60 * 1.5f);
			break;
		}
	}

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{// NULLだったら
		return;
	}

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 目標の向き取得
	float fRotDest = GetRotDest();

	if (pPlayer != NULL)
	{// NULLじゃないとき

		// 親の位置取得
		D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

		// 目標の角度を求める
		fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

		// 目標との差分
		float fRotDiff = fRotDest - rot.y;

		//角度の正規化
		RotNormalize(fRotDiff);

		//角度の補正をする
		rot.y += fRotDiff * 0.035f;

		// 角度の正規化
		RotNormalize(rot.y);
	}

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// タレット待機処理
//==========================================================================
void CEnemy::TurretWait(void)
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

	// オブジェクト情報
	CObject *pObj = NULL;

	// 状態遷移カウンター減算
	m_nCntState--;

	if (m_nCntState <= 0)
	{// 遷移カウンターが0になったら
		m_state = STATE_TURRETATK;
		m_nCntState = 0;
		m_sMotionFrag.bATK = true;
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
		rot.y += fRotDiff * 0.035f;

		// 角度の正規化
		RotNormalize(rot.y);
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
// 種類別状態更新処理
//==========================================================================
void CEnemy::StateWait(void)
{
	return;
}

//==========================================================================
// プレイヤー追従ONにするトリガー
//==========================================================================
void CEnemy::TriggerChasePlayer(void)
{

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	if (pPlayer != NULL)
	{// NULLじゃないとき

		float fRadius = PLAYER_SERCH;

		if (m_type == TYPE_CROWD)
		{
			fRadius *= 0.5f;

			if (m_colorType == COLORTYPE_CONFUSION)
			{
				fRadius *= 0.5f;
			}
		}

		if (CircleRange(pos, pPlayer->GetPosition(), 200.0f, fRadius) == true)
		{// プレイヤーが範囲に入れば
			m_state = STATE_PLAYERCHASE;

			if (m_pParent != NULL)
			{// 親がいる場合

				// 親も追い掛け状態にする
				if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD && m_pParent->m_state != STATE_KNOCKBACK)
				{
					m_pParent->m_state = STATE_PLAYERCHASE;
				}

				for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
				{// 子の数分回す

					if (m_pParent->m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_KNOCKBACK)
					{
						continue;
					}

					// 自分の親の子もプレイヤー追い掛け状態
					m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
			else
			{// 自分が親な場合

				for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
				{// 子の数分回す

					if (m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pChild[nCntEnemy]->m_state == STATE_DEAD || m_pChild[nCntEnemy]->m_state == STATE_KNOCKBACK)
					{
						continue;
					}

					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
		}
	}
}

//==========================================================================
// 攻撃状態移行処理
//==========================================================================
void CEnemy::ChangeToAttackState(void)
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
// 追い掛け移動
//==========================================================================
void CEnemy::ChaseMove(float fMove)
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
// 攻撃
//==========================================================================
void CEnemy::Atack(void)
{

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

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

		// 攻撃情報保存
		CMotion::AttackInfo atkInfo = *aInfo.AttackInfo[nCntAttack];

		if (m_pMotion->GetAllCount() == aInfo.AttackInfo[nCntAttack]->nInpactCnt)
		{// 衝撃のカウントと同じとき

			// 攻撃時処理
			AttackAction(atkInfo.nCollisionNum, atkInfo);
		}

		// モーションカウンター取得
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt && m_pMotion->GetAllCount() < aInfo.AttackInfo[nCntAttack]->nMaxCnt)
		{// 攻撃判定中

			switch (m_type)
			{
			case TYPE_JUMP:
				rot.y += D3DX_PI * 0.2f;
				break;

			default:
				break;
			}

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
#if _DEBUG
			CEffect3D::Create(weponpos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), aInfo.AttackInfo[nCntAttack]->fRangeSize * 0.5f, 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
#endif
			// 判定サイズ取得
			float fRadius = pPlayer->GetObjectChara()->GetRadius();

			if (aInfo.AttackInfo[nCntAttack]->fRangeSize == 0.0f)
			{
				continue;
			}

			if (SphereRange(weponpos, TargetPos, aInfo.AttackInfo[nCntAttack]->fRangeSize, fRadius))
			{// 球の判定

				int playerState = pPlayer->GetState();
				if (playerState != CPlayer::STATE_DMG &&
					playerState != CPlayer::STATE_KNOCKBACK &&
					playerState != CPlayer::STATE_DEAD &&
					playerState != CPlayer::STATE_INVINCIBLE)
				{
					if (m_colorType == COLORTYPE_CONFUSION && playerState != CPlayer::STATE_CONFUSION)
					{// 混乱だったら

						// 混乱状態に設定する
						pPlayer->EnableConfusion();
					}
					else if(playerState != CPlayer::STATE_CONFUSION)
					{
						D3DXVECTOR3 PlayerRot = pPlayer->GetRotation();

						// ターゲットと敵との向き
						float fRot = atan2f((pos.x - TargetPos.x), (pos.z - TargetPos.z));

						// 向きを正面にする
						fRot = D3DX_PI + fRot;

						// 角度の正規化
						RotNormalize(fRot);

						pPlayer->SetRotation(D3DXVECTOR3(PlayerRot.x, fRot, PlayerRot.z));
						fRot = pPlayer->GetRotation().y;

						// 吹き飛ばし
						pPlayer->SetMove(D3DXVECTOR3(
							sinf(fRot) * 4.0f,
							12.0f,
							cosf(fRot) * 4.0f));
					}
				}

				if (playerState != CPlayer::STATE_CONFUSION && pPlayer->Hit(aInfo.AttackInfo[nCntAttack]->nDamage) == true)
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

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// 攻撃時処理
//==========================================================================
void CEnemy::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
{
	return;
}

//==========================================================================
// 描画処理
//==========================================================================
void CEnemy::Draw(void)
{
#if _DEBUG
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
#else
	//CObjectChara::Draw();

	if (m_mMatcol != D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a))
	{
		// オブジェクトキャラの描画
		CObjectChara::Draw(m_mMatcol);
	}
	else
	{
		// オブジェクトキャラの描画
		CObjectChara::Draw();
	}
#endif
}

//==========================================================================
// 状態取得
//==========================================================================
int CEnemy::GetState(void)
{
	return m_state;
}

//==========================================================================
// 隊列の位置
//==========================================================================
void CEnemy::SetFormationPos(D3DXVECTOR3 posFormation, D3DXVECTOR3 posParent)
{
	// 隊列の位置代入
	m_sFormationInfo.pos = posFormation;

	// 目標の向きを求める
	m_sFormationInfo.fAngle = atan2f((posParent.x - posFormation.x), (posParent.z - posFormation.z));

	// 強さを求める
	m_sFormationInfo.fLength =
		sqrtf((posParent.x - posFormation.x) * (posParent.x - posFormation.x)
			+ (posParent.z - posFormation.z) * (posParent.z - posFormation.z));
}

//==========================================================================
// 元の向き
//==========================================================================
void CEnemy::SetOriginRotation(D3DXVECTOR3 rot)
{
	m_rotOrigin = rot;
}

//==========================================================================
// 状態設定
//==========================================================================
void CEnemy::SetState(int state)
{
	m_state = (STATE)state;
}

//==========================================================================
// 状態設定
//==========================================================================
void CEnemy::SetState(int state, int nCntState)
{
	m_state = (STATE)state;
	m_nCntState = nCntState;
}

//==========================================================================
// 種類取得
//==========================================================================
int CEnemy::GetCharaType(void)
{
	return m_type;
}

//==========================================================================
// 行動の種類設定
//==========================================================================
void CEnemy::SetMoveType(MOVETYPE movetype)
{
	m_MoveType = movetype;
}

//==========================================================================
// 出現の種類設定
//==========================================================================
void CEnemy::SetSpawnType(CEnemyManager::SPAWNTYPE type)
{
	// 出現の種類設定
	m_SpawnType = type;

	if (m_SpawnType == CEnemyManager::SPAWNTYPE_CIRCLE ||
		m_SpawnType == CEnemyManager::SPAWNTYPE_CLOCK ||
		m_SpawnType == CEnemyManager::SPAWNTYPE_BOTHCLOCK)
	{// 円 or 時計回りだったら
		m_Oldstate = STATE_PLAYERCHASE;
	}
}

//==========================================================================
// 敵の情報取得
//==========================================================================
CEnemy *CEnemy::GetEnemy(void)
{
	// 自分自身のポインタを取得
	return this;
}