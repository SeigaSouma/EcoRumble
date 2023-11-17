//=============================================================================
// 
//  草処理 [grassblades.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "grassblades.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "calculation.h"
#include "meshwall.h"
#include "elevation.h"
#include "player.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define RANGELEN		(300.0f)		// 範囲内の長さ
//#define RANGELEN		(40.0f)		// 範囲内の長さ
#define RANGE			(30.0f)
#define VECWIND			(0.25f)		// 風の強さ
#define INT_WINDCHANGE	(240)		// 風の切り替え間隔
#define GRASSTEXTURE	"data\\TEXTURE\\grassblades_01.png"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CGrassBlades::m_nTexIdx = 0;		// テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CGrassBlades::CGrassBlades(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	memset(&m_sGrassInfo[0], NULL, sizeof(m_sGrassInfo));	// 草情報
}

//==========================================================================
// デストラクタ
//==========================================================================
CGrassBlades::~CGrassBlades()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CGrassBlades *CGrassBlades::Create(D3DXVECTOR3 pos)
{
	// 生成用のオブジェクト
	CGrassBlades *pGrass = NULL;

	if (pGrass == NULL)
	{// NULLだったら

		// メモリの確保
		pGrass = DEBUG_NEW CGrassBlades;

		if (pGrass != NULL)
		{// メモリの確保が出来ていたら

			// 位置設定
			pGrass->SetPosition(pos);

			// 初期化処理
			pGrass->Init();
		}
		else
		{
			return NULL;
		}

		return pGrass;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CGrassBlades::Init(void)
{
	// 種類の設定
	SetType(TYPE_GRASSBLADES);

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	for (int nCntObj = 0; nCntObj < MAX_GRASSBLADES; nCntObj++)
	{
		// メッシュウォールの生成
		float fHeightLen = (float)Random(-90, 90) * 0.1f + 18.0f;
		fHeightLen *= 3.0f;
		m_sGrassInfo[nCntObj].pMeshWall = CMeshWall::Create(
			D3DXVECTOR3(
				pos.x + sinf(Random(-31, 31) * 0.1f) * Random(-RANGELEN, RANGELEN),
				pos.y,
				pos.z + cosf(Random(-31, 31) * 0.1f) * Random(-RANGELEN, RANGELEN)),
			D3DXVECTOR3(0.0f, Random(-31, 31) * 0.1f, 0.0f),
			//D3DXVECTOR3(0.0f, 0.0f, 0.0f),
			40.0f, fHeightLen,
			1, 1, 
			(CMeshWall::TYPE_GRASS), GRASSTEXTURE);

		if (m_sGrassInfo[nCntObj].pMeshWall == NULL)
		{// 失敗していたら
			Uninit();
			return E_FAIL;
		}

		// 草の情報取得
		D3DXVECTOR3 posGrass = m_sGrassInfo[nCntObj].pMeshWall->GetPosition();

		// 高さ取得
		bool bLand = false;
		posGrass.y = CGame::GetObject3DMesh()->GetHeight(posGrass, D3DXVECTOR3(0.0f, 0.0f, 0.0f), bLand, TYPE_GRASSBLADES);

		// 位置設定
		m_sGrassInfo[nCntObj].pMeshWall->SetPosition(posGrass);

		// 風の向きを決める
		m_sGrassInfo[nCntObj].windDirection.x = Random(-40, 40) * 0.01f;
		m_sGrassInfo[nCntObj].windDirection.z = Random(-40, 40) * 0.01f;

		// 頂点情報取得
		D3DXVECTOR3 WallRot = m_sGrassInfo[nCntObj].pMeshWall->GetRotation();
		D3DXVECTOR2 *pVtxTex = m_sGrassInfo[nCntObj].pMeshWall->GetVtxTex();
		D3DXVECTOR3 *pVtxPos = m_sGrassInfo[nCntObj].pMeshWall->GetVtxPos();
		int nHeight = m_sGrassInfo[nCntObj].pMeshWall->GetHeightBlock();
		int nWidth = m_sGrassInfo[nCntObj].pMeshWall->GetWidthBlock();

		for (int nCntHeight = 0; nCntHeight < nHeight + 1; nCntHeight++)
		{//縦の頂点数分繰り返す

			for (int nCntWidth = 0; nCntWidth < nWidth + 1; nCntWidth++)
			{// 横の頂点数分繰り返す

				// 今回の頂点
				int nNowVtx = nCntWidth + (nCntHeight * (nWidth + 1));

				// テクスチャ座標設定
				pVtxTex[nNowVtx] = D3DXVECTOR2
				(
					((float)nCntWidth / (1.0f / (float)(nWidth + 1))) * (1.0f / (float)(nWidth + 1)),
					(1.0f / (float)(nHeight) * nCntHeight)
				);
			}
		}

	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CGrassBlades::Uninit(void)
{
	for (int nCntObj = 0; nCntObj < MAX_GRASSBLADES; nCntObj++)
	{
		if (m_sGrassInfo[nCntObj].pMeshWall != NULL)
		{// NULLじゃなかったら

			// オブジェクトXの終了処理
			//m_sGrassInfo[nCntObj].pMeshWall->Uninit();
			m_sGrassInfo[nCntObj].pMeshWall = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CGrassBlades::Update(void)
{
	bool bStageEnd = false;
	if (CManager::GetScene()->GetWaveManager()->IsStageFrag() == false)
	{// ステージ変更中
		bStageEnd = true;
	}

	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{// NULLだったら
		return;
	}

	// プレイヤーの位置取得
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	// 原点取得
	D3DXVECTOR3 pos = GetPosition();

	// 範囲の判定
	bool bRange = false;

	// 円の判定
	bRange = CircleRange(posPlayer, pos, pPlayer->GetRadius(), RANGELEN);

	for (int nCntObj = 0; nCntObj < MAX_GRASSBLADES; nCntObj++)
	{
		// 草の情報取得
		D3DXVECTOR3 posGrass = m_sGrassInfo[nCntObj].pMeshWall->GetPosition();
		D3DXVECTOR3 rotGrass = m_sGrassInfo[nCntObj].pMeshWall->GetRotation();

		// 高さ取得
		if (bStageEnd == false)
		{
			bool bLand = false;
			posGrass.y = CGame::GetObject3DMesh()->GetHeight(posGrass, D3DXVECTOR3(0.0f, 0.0f, 0.0f), bLand, TYPE_GRASSBLADES);
		}

		// 草の位置設定
		m_sGrassInfo[nCntObj].pMeshWall->SetPosition(posGrass);

		if (bRange == true)
		{// プレイヤーが範囲内の場合

			// 倒れる処理
			Falling(nCntObj);
		}
		else
		{
			// 目標の向き設定
			m_sGrassInfo[nCntObj].rotDest.x = m_sGrassInfo[nCntObj].windRot.x;
			m_sGrassInfo[nCntObj].rotDest.z = m_sGrassInfo[nCntObj].windRot.z;
			m_sGrassInfo[nCntObj].bHit = false;
		}

		// 風で倒れる処理
		WindFalling(nCntObj);

		// 差分を求める
		float fDiffX = m_sGrassInfo[nCntObj].rotDest.x - rotGrass.x;
		float fDiffZ = m_sGrassInfo[nCntObj].rotDest.z - rotGrass.z;

		// 向きの正規化
		RotNormalize(fDiffX);
		RotNormalize(fDiffZ);

		// 向きを補正
		rotGrass.x += fDiffX * 0.05f;
		rotGrass.z += fDiffZ * 0.05f;

		// 向きの正規化
		RotNormalize(rotGrass);

		// 向き設定
		m_sGrassInfo[nCntObj].pMeshWall->SetRotation(rotGrass);
	}
}

//==========================================================================
// 倒れる処理
//==========================================================================
void  CGrassBlades::Falling(int nCntObj)
{
	// プレイヤー情報
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{// NULLだったら
		return;
	}

	// プレイヤーの情報取得
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();
	D3DXVECTOR3 rotPlayer = pPlayer->GetRotation();
	float fPlayerRadius = pPlayer->GetRadius();

	// 草の情報取得
	D3DXVECTOR3 pos = m_sGrassInfo[nCntObj].pMeshWall->GetPosition();
	D3DXVECTOR3 rot = m_sGrassInfo[nCntObj].pMeshWall->GetRotation();

	if (CircleRange(posPlayer, pos, fPlayerRadius, RANGE) == false)
	{// 判定に入ってなかったら
		// 目標の向き設定
		m_sGrassInfo[nCntObj].rotDest.x = m_sGrassInfo[nCntObj].windRot.x;
		m_sGrassInfo[nCntObj].rotDest.z = m_sGrassInfo[nCntObj].windRot.z;
		m_sGrassInfo[nCntObj].bHit = false;
		return;
	}

	// ベクトルを求める
	D3DXVECTOR3 impactDirection = posPlayer - pos;
	//D3DXVec3Normalize(&impactDirection, &impactDirection);

	// プレイヤーと草の距離
	float fNowLength =
		sqrtf((pos.x - posPlayer.x) * (pos.x - posPlayer.x)
			+ (pos.z - posPlayer.z) * (pos.z - posPlayer.z));

	if (fNowLength < 0)
	{
		int n = 0;
	}

	if (fNowLength >= RANGE + fPlayerRadius)
	{// 範囲外
		// 目標の向き設定
		m_sGrassInfo[nCntObj].rotDest.x = m_sGrassInfo[nCntObj].windRot.x;
		m_sGrassInfo[nCntObj].rotDest.z = m_sGrassInfo[nCntObj].windRot.z;
		m_sGrassInfo[nCntObj].bHit = false;
		return;
	}

	// 目標の角度を求める
	//float fMaxAngle = atan2f(impactDirection.x, impactDirection.z);
	float fMaxAngle = atan2f((posPlayer.x - pos.x), (posPlayer.z - pos.z));

	// 距離の割合から回転角度を補正
	float fAngle = ((RANGE + fPlayerRadius) - fNowLength) / (RANGE + fPlayerRadius);
	fAngle = fMaxAngle * fAngle;

	// 目標の向き設定
	m_sGrassInfo[nCntObj].rotDest.x = sinf(D3DX_PI/* * -0.5f*/ + fAngle);
	m_sGrassInfo[nCntObj].rotDest.z = cosf(D3DX_PI * -0.5f + fAngle);
	/*m_sGrassInfo[nCntObj].rotDest.x = cosf(fAngle) * sinf(rotPlayer.y);
	m_sGrassInfo[nCntObj].rotDest.z = cosf(fAngle) * cosf(rotPlayer.y);*/
	/*m_sGrassInfo[nCntObj].rotDest.x = fAngle;
	m_sGrassInfo[nCntObj].rotDest.z = fAngle;*/
	m_sGrassInfo[nCntObj].bHit = true;
	RotNormalize(m_sGrassInfo[nCntObj].windRot);

}

//==========================================================================
// 風で揺れる処理
//==========================================================================
void CGrassBlades::WindFalling(int nCntObj)
{
	// 風のカウンター加算
	switch (m_sGrassInfo[nCntObj].nDirection)
	{
	case 0:
		m_sGrassInfo[nCntObj].nCntWind++;

		if (m_sGrassInfo[nCntObj].nCntWind == INT_WINDCHANGE)
		{// 規定カウントに達したら
			m_sGrassInfo[nCntObj].nDirection = m_sGrassInfo[nCntObj].nDirection ? 0 : 1;
		}
		break;

	case 1:
		m_sGrassInfo[nCntObj].nCntWind--;

		if (m_sGrassInfo[nCntObj].nCntWind == -INT_WINDCHANGE)
		{// 規定カウントに達したら
			m_sGrassInfo[nCntObj].nDirection = m_sGrassInfo[nCntObj].nDirection ? 0 : 1;
		}
		break;

	default:
		break;
	}

	// 風で揺らす
	m_sGrassInfo[nCntObj].windRot.x = (m_sGrassInfo[nCntObj].windDirection.x / (float)INT_WINDCHANGE) * m_sGrassInfo[nCntObj].nCntWind;
	m_sGrassInfo[nCntObj].windRot.z = (m_sGrassInfo[nCntObj].windDirection.z / (float)INT_WINDCHANGE) * m_sGrassInfo[nCntObj].nCntWind;

	// 角度の正規化
	RotNormalize(m_sGrassInfo[nCntObj].windRot);
}

//==========================================================================
// 描画処理
//==========================================================================
void CGrassBlades::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 背面のカリングなし
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	for (int nCntObj = 0; nCntObj < MAX_GRASSBLADES; nCntObj++)
	{
		if (m_sGrassInfo[nCntObj].pMeshWall != NULL)
		{// NULLじゃなかったら

			// 描画処理
			m_sGrassInfo[nCntObj].pMeshWall->Draw();
		}
	}

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// カリングのデフォルト
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
