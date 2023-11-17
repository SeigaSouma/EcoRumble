//=============================================================================
// 
//  高速ラッシュ処理 [speed_rush.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "speed_rush.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "objectX.h"
#include "objectChara.h"
#include "3D_Effect.h"
#include "particle.h"
#include "ballast.h"
#include "model.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define OBJ_LIFE	(4)		// オブジェの寿命
#define RANGE		(100)	// ランダム生成の範囲
#define MOVE		(20.0f)	// 移動量
#define ATK_RANGE	(10.0f)	// 攻撃判定
#define OFFSET		(-40.0f)	// 当たり判定のオフセット

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CSpeedRush::m_pModelFile = "data\\MODEL\\rushhand.x";	// モデルファイル
int CSpeedRush::m_nIdxXFile = 0;			// Xファイルのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CSpeedRush::CSpeedRush(int nPriority) : CObject(nPriority)
{
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 原点
	m_rotOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 原向き
	m_nNumAll = 0;		// 総数
	m_nLife = 0;		// 寿命

	for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
	{
		m_nObjLife[nCntRush] = 0;	// 寿命
		m_pObjX[nCntRush] = NULL;	// オブジェクトXのポインタ
	}
}

//==========================================================================
// デストラクタ
//==========================================================================
CSpeedRush::~CSpeedRush()
{
	
}

//==========================================================================
// ロード処理
//==========================================================================
HRESULT CSpeedRush::Load(void)
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
CSpeedRush *CSpeedRush::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nNum)
{
	// 生成用のオブジェクト
	CSpeedRush *pBallast = NULL;

	if (pBallast == NULL)
	{// NULLだったら

		// メモリの確保
		pBallast = DEBUG_NEW CSpeedRush;

		//if (pBallast->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pBallast;
		//	return NULL;
		//}

		if (pBallast != NULL)
		{// メモリの確保が出来ていたら\

			// ブレ
			pos.y += 150.0f;

			// 位置設定
			pBallast->m_posOrigin = pos;

			// 向き設定
			rot.x = -0.5f;
			pBallast->m_rotOrigin = rot;

			// 寿命設定
			pBallast->m_nLife = nNum;

			// 初期化処理
			pBallast->Init();
		}

		return pBallast;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSpeedRush::Init(void)
{
	// 種類の設定
	SetType(TYPE_BALLAST);

	for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
	{
		m_nObjLife[nCntRush] = OBJ_LIFE;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSpeedRush::Uninit(void)
{
	for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
	{
		if (m_pObjX[nCntRush] != NULL)
		{// NULLじゃなかったら

			// オブジェクトXの終了処理
			m_pObjX[nCntRush]->Uninit();
			m_pObjX[nCntRush] = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSpeedRush::Update(void)
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

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

	for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
	{// それぞれの更新

		if (m_pObjX[nCntRush] == NULL)
		{// NULLだったら
			continue;
		}

		// 位置取得
		D3DXVECTOR3 pos = m_pObjX[nCntRush]->GetPosition();

		// 移動量取得
		D3DXVECTOR3 move = m_pObjX[nCntRush]->GetMove();

		// 向き取得
		D3DXVECTOR3 rot = m_pObjX[nCntRush]->GetRotation();

		// 位置更新
		pos += move;

		// 位置設定
		m_pObjX[nCntRush]->SetPosition(pos);

		// 移動量設定
		m_pObjX[nCntRush]->SetMove(move);

		// オブジェの寿命を減らす
		m_nObjLife[nCntRush]--;

		// 寿命が尽きたら消す
		if (m_nObjLife[nCntRush] < 0)
		{
			if (m_pObjX[nCntRush] != NULL)
			{// NULLじゃなかったら

				// オブジェクトXの終了処理
				m_pObjX[nCntRush]->Uninit();
				m_pObjX[nCntRush] = NULL;
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
bool CSpeedRush::Set(void)
{
	// 今回の番号保存
	int nIdx = m_nNumAll;

	if (nIdx >= MAX_SPEEDRUSH)
	{// 総数を超えていたら
		return false;
	}

	// 生成処理
	m_pObjX[nIdx] = CObjectX::Create(
		m_nIdxXFile,
		D3DXVECTOR3(
			m_posOrigin.x + sinf(Random(-31, 31) * 0.1f) * Random(-RANGE, RANGE),
			m_posOrigin.y + cosf(Random(-31, 31) * 0.1f) * Random(-RANGE, RANGE),
			m_posOrigin.z + cosf(Random(-31, 31) * 0.1f) * Random(-RANGE, RANGE)),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), false);

	if (m_pObjX[nIdx] == NULL)
	{// 失敗したとき

		delete m_pObjX[nIdx];
		m_pObjX[nIdx] = NULL;
		Uninit();
		return false;
	}

	// 移動量設定
	m_pObjX[nIdx]->SetMove(D3DXVECTOR3(sinf(D3DX_PI + m_rotOrigin.y) * MOVE, cosf(D3DX_PI + m_rotOrigin.x) * MOVE, cosf(D3DX_PI + m_rotOrigin.y) * MOVE));

	// 向き設定
	m_pObjX[nIdx]->SetRotation(D3DXVECTOR3(m_rotOrigin.x, m_rotOrigin.y, Random(-314, 314) * 0.01f));

	// 総数加算
	m_nNumAll++;
	return true;
}

//==========================================================================
// 当たり判定
//==========================================================================
void CSpeedRush::Collision(void)
{
#if TOPCUR
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObj = CObject::GetTop(nCntPriority);

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

			for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
			{// それぞれの更新

				if (m_pObjX[nCntRush] == NULL)
				{// 失敗したとき
					continue;
				}

				// 位置取得
				D3DXVECTOR3 pos = m_pObjX[nCntRush]->GetPosition();

				// 位置取得
				D3DXVECTOR3 rot = m_pObjX[nCntRush]->GetRotation();

				// 判定するパーツのマトリックス取得
				D3DXMATRIX mtxWepon, mtxTrans, mtxOrigin, mtxRot;	// 計算用マトリックス宣言
				D3DXMatrixIdentity(&mtxWepon);

				// 向きを反映する
				D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
				D3DXMatrixMultiply(&mtxWepon, &mtxWepon, &mtxRot);

				// 元の位置を反映する
				D3DXMatrixTranslation(&mtxOrigin, pos.x, pos.y, pos.z);
				D3DXMatrixMultiply(&mtxWepon, &mtxWepon, &mtxOrigin);

				// オフセットの位置を反映する
				D3DXMatrixTranslation(&mtxTrans, 0.0f, 0.0f, OFFSET);
				D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

				// 武器の位置
				D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

				// 判定サイズ取得
				float fRadius = pObj->GetObjectChara()->GetRadius();

				//CEffect3D::Create(weponpos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), ATK_RANGE, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

				// 敵の位置取得
				D3DXMATRIX mtxRange = pObj->GetObjectChara()->GetModel()[0]->GetWorldMtx();
				D3DXVECTOR3 TargetPos = D3DXVECTOR3(mtxRange._41, mtxRange._42, mtxRange._43);

				if (SphereRange(weponpos, TargetPos, ATK_RANGE, fRadius))
				{
					// オブジェクトXの終了処理
					m_pObjX[nCntRush]->Uninit();
					m_pObjX[nCntRush] = NULL;
					m_nNumAll--;

					if(pObj->Hit(1) == true)
					{// 死んでたら

						my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
					}
					break;
				}
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
	}
#endif
}

//==========================================================================
// 描画処理
//==========================================================================
void CSpeedRush::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCntRush = 0; nCntRush < MAX_SPEEDRUSH; nCntRush++)
	{
		if (m_pObjX[nCntRush] != NULL)
		{// NULLじゃなかったら

			// 描画処理
			m_pObjX[nCntRush]->Draw();
		}
	}

}
