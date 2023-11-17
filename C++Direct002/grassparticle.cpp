//=============================================================================
// 
//  草のパーティクル処理 [grassparticle.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "game.h"
#include "grassparticle.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "elevation.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define LIFE	(40)	// 寿命
#define WIDTHLEN	(2.5f * 1.2f)	// 横幅
#define HEIGHTLEN	(10.0f * 1.2f)	// 縦幅
#define TEXTURE	"grassblades_01"	// 寿命

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CGrassParticle::m_nTexIdx = 0;		// テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CGrassParticle::CGrassParticle(int nPriority) : CObject(nPriority)
{
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 原点
	m_nNumAll = 0;		// 総数
	memset(&m_nLife[0], 0, sizeof(m_nLife));	// 寿命
	memset(&m_nMaxLife[0], 0, sizeof(m_nMaxLife));	// 最大寿命
	memset(&m_pObjMesh[0], NULL, sizeof(m_pObjMesh));	// 3Dメッシュのポインタ
}

//==========================================================================
// デストラクタ
//==========================================================================
CGrassParticle::~CGrassParticle()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CGrassParticle *CGrassParticle::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 move, int nNum)
{
	// 生成用のオブジェクト
	CGrassParticle *pBallast = NULL;

	if (pBallast == NULL)
	{// NULLだったら

		// メモリの確保
		pBallast = DEBUG_NEW CGrassParticle;

		if (pBallast != NULL)
		{// メモリの確保が出来ていたら

			if (nNum >= MAX_GRASSPARTICLE)
			{
				nNum = MAX_GRASSPARTICLE;
			}

			// 総数設定
			pBallast->m_nNumAll = nNum;

			// 位置設定
			pBallast->m_posOrigin = pos;

			// 元の向き
			pBallast->m_rotOrigin = rot;;

			// 元の移動量設定
			pBallast->m_moveOrigin = move;

			// 初期化処理
			HRESULT hr = pBallast->Init();
			if (FAILED(hr))
			{
				return NULL;
			}
		}

		return pBallast;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CGrassParticle::Init(void)
{
	// 種類の設定
	SetType(TYPE_BALLAST);

	for (int nCntGrass = 0; nCntGrass < m_nNumAll; nCntGrass++)
	{
		// 移動量設定
		m_move[nCntGrass] = D3DXVECTOR3(
			sinf(Random(-30, 30) * 0.01f + m_rotOrigin.y) * m_moveOrigin.x,
			m_moveOrigin.y * (Random(800, 1000) * 0.001f),
			cosf(Random(-30, 30) * 0.01f + m_rotOrigin.y) * m_moveOrigin.x);

		// 回転移動量
		m_rotMove[nCntGrass] = D3DXVECTOR3(
			(Random(-400, 400) * 0.0001f) * D3DX_PI,
			(Random(-400, 400) * 0.0001f) * D3DX_PI,
			(Random(-400, 400) * 0.0001f) * D3DX_PI);

		// 生成処理
		m_pObjMesh[nCntGrass] = CObject3DMesh::Create
		(
			D3DXVECTOR3(
				m_posOrigin.x + sinf(Random(-314, 314) * 0.01f) * m_moveOrigin.x,
				m_posOrigin.y + sinf(Random(-314, 314) * 0.01f) * m_moveOrigin.y,
				m_posOrigin.z + cosf(Random(-314, 314) * 0.01f) * m_moveOrigin.x),
			D3DXVECTOR3(Random(-31, 31) * 0.1f, Random(-31, 31) * 0.1f, Random(-31, 31) * 0.1f),
			WIDTHLEN, HEIGHTLEN,
			1, 3,
			CObject3DMesh::TYPE_FIELD,
			TEXTURE
		);

		if (m_pObjMesh[nCntGrass] == NULL)
		{// 失敗したとき

			// 3Dメッシュの終了処理
			delete m_pObjMesh[nCntGrass];
			m_pObjMesh[nCntGrass] = NULL;
			return E_FAIL;
		}

		// 寿命設定
		m_nLife[nCntGrass] = LIFE + Random(0, 10);
		m_nMaxLife[nCntGrass] = m_nLife[nCntGrass];

		// 色設定
		D3DXCOLOR *pVtxCol = m_pObjMesh[nCntGrass]->GetVtxCol();
		D3DXVECTOR3 *pVtxPos = m_pObjMesh[nCntGrass]->GetVtxPos();
		int nWidth = m_pObjMesh[nCntGrass]->GetWidthBlock();
		int nHeight = m_pObjMesh[nCntGrass]->GetHeightBlock();

		for (int nCntHeight = 0; nCntHeight < nHeight + 1; nCntHeight++)
		{//縦の頂点数分繰り返す

			for (int nCntWidth = 0; nCntWidth < nWidth + 1; nCntWidth++)
			{// 横の頂点数分繰り返す

				// 今回の頂点
				int nNowVtx = nCntWidth + (nCntHeight * (nWidth + 1));
				float fLen = ((1.0f / (float)nHeight) * (WIDTHLEN * 0.5f)) * nCntHeight;
				int nAngle = -1;

				if (nCntWidth % 2 == 0)
				{
					nAngle *= -1;
				}

				// 頂点座標設定
				pVtxPos[nNowVtx] = D3DXVECTOR3
				(
					pVtxPos[nNowVtx].x + fLen * nAngle,
					pVtxPos[nNowVtx].y,
					pVtxPos[nNowVtx].z
				);
				int n = 0;
			}
		}

		for (int nCntVtx = 0; nCntVtx < m_pObjMesh[nCntGrass]->GetNumVertex(); nCntVtx++)
		{
			// 頂点カラー設定
			pVtxCol[nCntVtx] = D3DXCOLOR(0.2f, 0.8f, 0.2f, 1.0f);
		}
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CGrassParticle::Uninit(void)
{
	for (int nCntGrass = 0; nCntGrass < MAX_GRASSPARTICLE; nCntGrass++)
	{
		if (m_pObjMesh[nCntGrass] != NULL)
		{// NULLじゃなかったら

			// 3Dメッシュの終了処理
			m_pObjMesh[nCntGrass]->Uninit();
			m_pObjMesh[nCntGrass] = NULL;
		}
	}

	// 情報削除
	CObject::Release();
}

//==========================================================================
// 終了処理
//==========================================================================
void CGrassParticle::Release(int nIdx)
{
	if (m_pObjMesh[nIdx] != NULL)
	{// NULLじゃなかったら

		// 3Dメッシュの終了処理
		m_pObjMesh[nIdx]->Uninit();
		m_pObjMesh[nIdx] = NULL;
	}

}

//==========================================================================
// 更新処理
//==========================================================================
void CGrassParticle::Update(void)
{
	for (int nCntGrass = 0; nCntGrass < m_nNumAll; nCntGrass++)
	{
		if (m_pObjMesh[nCntGrass] == NULL)
		{// NULLだったら
			continue;
		}

		// 位置取得
		D3DXVECTOR3 pos = m_pObjMesh[nCntGrass]->GetPosition();

		// 向き取得
		D3DXVECTOR3 rot = m_pObjMesh[nCntGrass]->GetRotation();

		// 幅取得
		float fWidthLen = m_pObjMesh[nCntGrass]->GetWidthLen();
		float fHeightLen = m_pObjMesh[nCntGrass]->GetHeightLen();

		// 位置更新
		pos += m_move[nCntGrass];

		// 向き更新
		rot += m_rotMove[nCntGrass];

		// 角度の正規化
		RotNormalize(rot);

		// 不透明度更新
		fWidthLen = WIDTHLEN * ((float)m_nLife[nCntGrass] / (float)m_nMaxLife[nCntGrass]);
		fHeightLen = HEIGHTLEN * ((float)m_nLife[nCntGrass] / (float)m_nMaxLife[nCntGrass]);

		// 幅設定
		m_pObjMesh[nCntGrass]->SetWidthLen(fWidthLen);
		m_pObjMesh[nCntGrass]->SetHeightLen(fHeightLen);

		// 位置設定
		m_pObjMesh[nCntGrass]->SetPosition(pos);

		// 向き設定
		m_pObjMesh[nCntGrass]->SetRotation(rot);

		m_nLife[nCntGrass]--;
		if (m_nLife[nCntGrass] <= 0)
		{// 寿命が尽きたら

			// 終了処理
			Release(nCntGrass);
			continue;
		}

		// 元の更新処理
		m_pObjMesh[nCntGrass]->Update();

		D3DXVECTOR3 *pVtxPos = m_pObjMesh[nCntGrass]->GetVtxPos();
		int nWidth = m_pObjMesh[nCntGrass]->GetWidthBlock();
		int nHeight = m_pObjMesh[nCntGrass]->GetHeightBlock();

		// 頂点情報の設定
		for (int nCntHeight = 0; nCntHeight < nHeight + 1; nCntHeight++)
		{//縦の頂点数分繰り返す

			for (int nCntWidth = 0; nCntWidth < nWidth + 1; nCntWidth++)
			{// 横の頂点数分繰り返す
				
				// 今回の頂点
				int nNowVtx = nCntWidth + (nCntHeight * (nWidth + 1));

				float fLen = ((1.0f / (float)nHeight) * (WIDTHLEN * 0.5f)) * nCntHeight;
				int nAngle = -1;

				if (nCntWidth % 2 == 0)
				{
					nAngle *= -1;
				}

				// 頂点座標の設定
				pVtxPos[nNowVtx] = D3DXVECTOR3
				(
					(fWidthLen * nCntWidth) - ((fWidthLen * nWidth) * 0.5f) + (fLen * nAngle),
					(fHeightLen * nHeight) - ((fHeightLen * nCntHeight)),
					0.0f
				);
			}
		}

	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CGrassParticle::Draw(void)
{
	for (int nCntGrass = 0; nCntGrass < m_nNumAll; nCntGrass++)
	{
		if (m_pObjMesh[nCntGrass] == NULL)
		{// NULLの時
			continue;
		}

		// 描画処理
		m_pObjMesh[nCntGrass]->Draw();
	}

}
