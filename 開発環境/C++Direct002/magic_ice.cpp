//=============================================================================
// 
//  氷魔法処理 [magic_ice.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "magic_ice.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "calculation.h"
#include "objectX.h"
#include "objectChara.h"
#include "3D_Effect.h"
#include "particle.h"
#include "ballast.h"
#include "camera.h"
#include "elevation.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define LIFE	(64)	// 寿命
#define RANGE	(400)	// ランダム生成の範囲
#define ATK_RANGE	(25.0f)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CMagicIce::m_pModelFile = "data\\MODEL\\magic_ice.x";	// モデルファイル
int CMagicIce::m_nIdxXFile = 0;			// Xファイルのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CMagicIce::CMagicIce(int nPriority) : CObject(nPriority)
{
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 原点
	m_nNumAll = 0;		// 総数
	m_nLife = 0;		// 寿命
	memset(&m_pObjX[0], NULL, sizeof(m_pObjX));				// オブジェクトXのポインタ
	memset(&m_nWait[0], NULL, sizeof(m_nWait));				// 待機時間
	memset(&m_nDirection[0], NULL, sizeof(m_nDirection));	// 向き
}

//==========================================================================
// デストラクタ
//==========================================================================
CMagicIce::~CMagicIce()
{
	
}

//==========================================================================
// ロード処理
//==========================================================================
HRESULT CMagicIce::Load(void)
{
	// Xファイルのデータ取得
	CXLoad *pXLoad = CScene::GetXLoad();

	// Xファイルのロード
	m_nIdxXFile = pXLoad->XLoad(m_pModelFile);

	if (m_nIdxXFile <= 0)
	{// 失敗していたら
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 生成処理
//==========================================================================
CMagicIce *CMagicIce::Create(D3DXVECTOR3 pos)
{
	// 生成用のオブジェクト
	CMagicIce *pBallast = NULL;

	if (pBallast == NULL)
	{// NULLだったら

		// メモリの確保
		pBallast = DEBUG_NEW CMagicIce;

		//if (pBallast->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pBallast;
		//	return NULL;
		//}

		if (pBallast != NULL)
		{// メモリの確保が出来ていたら

			// ブレ
			pos.y += 300.0f;

			// 位置設定
			pBallast->m_posOrigin = pos;

			// 初期化処理
			pBallast->Init();
		}
		else
		{
			return NULL;
		}

		return pBallast;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CMagicIce::Init(void)
{
	// 種類の設定
	SetType(TYPE_MAGIC_ICE);

	m_nLife = LIFE;	// 寿命
	m_nIdxBig = LIFE / 2 + (LIFE / 2);	// 巨大なやつのインデックス番号

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CMagicIce::Uninit(void)
{
	for (int nCntIce = 0; nCntIce < MAX_MAGICICE; nCntIce++)
	{
		if (m_pObjX[nCntIce] != NULL)
		{// NULLじゃなかったら

			// オブジェクトXの終了処理
			m_pObjX[nCntIce]->Uninit();
			m_pObjX[nCntIce] = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CMagicIce::Update(void)
{

	// 寿命減算
	m_nLife--;

	if (m_nLife >= 0)
	{// まだ寿命がある場合

		// 設定処理
		if (Set() == false)
		{// 失敗していたら
			return;
		}
		if (Set() == false)
		{// 失敗していたら
			return;
		}
	}

	if (m_nNumAll <= 0)
	{// 全て消えていたら
		Uninit();
		return;
	}

	for (int nCntIce = 0; nCntIce < MAX_MAGICICE; nCntIce++)
	{// それぞれの更新

		if (m_pObjX[nCntIce] == NULL)
		{// NULLだったら
			continue;
		}

		// 位置取得
		D3DXVECTOR3 pos = m_pObjX[nCntIce]->GetPosition();

		// 移動量取得
		D3DXVECTOR3 move = m_pObjX[nCntIce]->GetMove();

		// 向き取得
		D3DXVECTOR3 rot = m_pObjX[nCntIce]->GetRotation();

		// 攻撃の判定
		float fATKRange = ATK_RANGE;

		if (nCntIce == m_nIdxBig)
		{// 巨大なやつの時だけ
			fATKRange *= 10.0f;
		}

		// 回転
		rot.y += D3DX_PI * 0.03f * m_nDirection[nCntIce];

		// 向き設定
		m_pObjX[nCntIce]->SetRotation(rot);

		// 待ち時間減算
		m_nWait[nCntIce]--;

		if (m_nWait[nCntIce] <= 0)
		{// 0を下回ったら
			m_nWait[nCntIce] = 0;
		}
		else
		{
			continue;
		}

		move.y -= 0.1f;

		// 位置更新
		pos += move;

		// 位置設定
		m_pObjX[nCntIce]->SetPosition(pos);

		// 移動量設定
		m_pObjX[nCntIce]->SetMove(move);

		// エフェクト生成
		CEffect3D::Create(pos, D3DXVECTOR3(move.x, -move.y * 2.0f, move.z), D3DXCOLOR(0.2f, 0.2f, 1.0f, 1.0f), fATKRange, 6, CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_NORMAL);

		if (CGame::GetObject3DMesh()->IsHit(pos) == true)
		//if (pos.y <= -50.0f)
		{
			if (m_pObjX[nCntIce] != NULL)
			{// NULLじゃなかったら

				// 瓦礫生成
				CBallast::Create(D3DXVECTOR3(pos.x, pos.y + 5.0f, pos.z), DEFAULTMOVE_BALLAST, 8, 0.6f, CBallast::TYPE_ICE);

				if (nCntIce == m_nIdxBig)
				{// 巨大な氷

					// 振動
					CManager::GetCamera()->SetShake(20, 0.0f, 10.0f);
					//CManager::GetObject3DMesh()->Damage(pos, (ATK_RANGE * 2.0f) * 10.0f, -100.0f);
				}
				else
				{
					//CManager::GetObject3DMesh()->Damage(pos, ATK_RANGE * 2.0f, -50.0f);
				}

				// オブジェクトXの終了処理
				m_pObjX[nCntIce]->Uninit();
				m_pObjX[nCntIce] = NULL;
				m_nNumAll--;
			}
			else
			{
				int n = 0;
			}
		}
	}

	// 当たり判定
	Collision();

}

//==========================================================================
// 設定処理
//==========================================================================
bool CMagicIce::Set(void)
{
	// 今回の番号保存
	int nIdx = m_nNumAll;

	if (nIdx >= MAX_MAGICICE)
	{// 総数を超えていたら
		return false;
	}

	// 生成処理
	m_pObjX[nIdx] = CObjectX::Create(
		m_nIdxXFile,
		D3DXVECTOR3(m_posOrigin.x + sinf(Random(-31, 31) * 0.1f) * Random(-RANGE, RANGE), m_posOrigin.y + Random(-200, 200) * 0.1f, m_posOrigin.z + cosf(Random(-31, 31) * 0.1f) * Random(-RANGE, RANGE)),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), false);

	if (m_pObjX[nIdx] == NULL)
	{// 失敗したとき

		delete m_pObjX[nIdx];
		m_pObjX[nIdx] = NULL;
		return false;
	}

	// 待ち時間設定
	m_nWait[nIdx] = 40;

	// 移動量設定
	m_pObjX[nIdx]->SetMove(D3DXVECTOR3(0.0f, -7.0f + Random(-20, 20) * 0.1f, 0.0f));

	if (rand() % 2 == 0)
	{
		m_nDirection[nIdx] = 1;
	}
	else
	{
		m_nDirection[nIdx] = -1;
	}

	if (m_nIdxBig == nIdx)
	{// 半分の時

		// 位置設定
		m_pObjX[nIdx]->SetPosition(D3DXVECTOR3(m_posOrigin.x, m_posOrigin.y + 100.0f, m_posOrigin.z));

		// スケール設定
		m_pObjX[nIdx]->SetScale(D3DXVECTOR3(10.0f, 10.0f, 10.0f));

		// 移動量設定
		m_pObjX[nIdx]->SetMove(D3DXVECTOR3(0.0f, -6.0f, 0.0f));
	}

	// パーティクル生成
	my_particle::Create(m_pObjX[nIdx]->GetPosition(), my_particle::TYPE_ICE_SPAWN);

	// 総数加算
	m_nNumAll++;
	return true;
}

//==========================================================================
// 当たり判定
//==========================================================================
void CMagicIce::Collision(void)
{
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

		for (int nCntIce = 0; nCntIce < MAX_MAGICICE; nCntIce++)
		{// それぞれの更新

			if (m_pObjX[nCntIce] == NULL)
			{// 失敗したとき
				continue;
			}

			// 位置取得
			D3DXVECTOR3 pos = m_pObjX[nCntIce]->GetPosition();

			// 敵の位置取得
			D3DXVECTOR3 TargetPos = pObj->GetPosition();

			// 攻撃の判定
			float fATKRange = ATK_RANGE;

			if (nCntIce == m_nIdxBig)
			{// 巨大なやつの時だけ
				fATKRange *= 10.0f;
			}

			// 判定サイズ取得
			float fRadius = pObj->GetObjectChara()->GetRadius();

			if (SphereRange(pos, TargetPos, fATKRange, fRadius))
			{
				if (pObj->Hit(1) == true)
				{// 死んでたら

					my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
					break;
				}
			}
		}

		// 次のオブジェクトを代入
		pObj = pObjNext;
	}

#endif
}

//==========================================================================
// 描画処理
//==========================================================================
void CMagicIce::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCntIce = 0; nCntIce < MAX_MAGICICE; nCntIce++)
	{
		if (m_pObjX[nCntIce] != NULL)
		{// NULLじゃなかったら

			// 描画処理
			m_pObjX[nCntIce]->Draw();
		}
	}

}
