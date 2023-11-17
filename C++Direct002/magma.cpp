//=============================================================================
// 
//  起伏処理 [magma.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "magma.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "debugproc.h"
#include "player.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\image.png"	// テクスチャ
#define MAX_MOVE	(8.0f)		// 移動量
#define MAX_RANGE	(200.0f)	// 範囲
#define MOVE_LEN	(1.0f)		// 幅移動量
#define MIN_HEIGHT	(-400.0f)	// 最低の高さ

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CMagma::CMagma(int nPriority) : CObject3DMesh(nPriority)
{
	m_nTexIdx = 0;			// テクスチャのインデックス番号
}

//==========================================================================
// デストラクタ
//==========================================================================
CMagma::~CMagma()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CMagma *CMagma::Create(D3DXVECTOR3 pos, float fWidthLen, int nBlock)
{
	// 生成用のオブジェクト
	CMagma *pObjMeshField = NULL;

	if (pObjMeshField == NULL)
	{// NULLだったら

		// メモリの確保
		pObjMeshField = DEBUG_NEW CMagma();

		if (pObjMeshField != NULL)
		{// メモリの確保が出来ていたら

			pObjMeshField->SetPosition(pos);				// 位置
			pObjMeshField->SetWidthBlock(nBlock);		// 幅部活
			pObjMeshField->SetHeightBlock(nBlock);	// 縦分割
			pObjMeshField->SetWidthLen(fWidthLen);		// 縦長さ
			pObjMeshField->SetHeightLen(fWidthLen);	// 横長さ

			// 初期化処理
			pObjMeshField->Init();
		}

		return pObjMeshField;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CMagma::Init(void)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 種類設定
	SetType(TYPE_MAGMA);

	// オブジェクト3Dメッシュの初期化処理
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_FIELD);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CMagma::Uninit(void)
{

	// 終了処理
	CObject3DMesh::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CMagma::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();


	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	int nHeightBlock = GetHeightBlock();
	int nWidthBlock = GetWidthBlock();
	float fWidthLen = GetWidthLen();

	static float fWave[1280];

	for (int nCntHeight = 0; nCntHeight < nHeightBlock; nCntHeight++)
	{// 縦の分割分繰り返す

		for (int nCntWidth = 0; nCntWidth < nWidthBlock; nCntWidth++)
		{// 横の分割分繰り返す

			// 中心点と頂点の長さ
			float fNowLength =
				sqrtf((pos.x - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].x) * (pos.x - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].x)
					+ (pos.z - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].z) * (pos.z - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].z));

			// 中心と頂点座標の向き
			float fRotDest = atan2f((pos.x - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].x), (pos.z - pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].z));


			// 割合
			float Wariai = fNowLength / (fWidthLen * 4.0f);

			// 波の向き
			float fRot = D3DX_PI * Wariai;

			// ウェーブさせる
			fWave[nCntWidth + (nCntHeight * (nWidthBlock + 1))] -= D3DX_PI * 0.001f;

			// 角度の正規化
			RotNormalize(fRot);
			RotNormalize(fWave[nCntWidth + (nCntHeight * (nWidthBlock + 1))]);

			// 波
			pVtxPos[nCntWidth + (nCntHeight * (nWidthBlock + 1))].y = 
				sinf(fRot + fWave[nCntWidth + (nCntHeight * (nWidthBlock + 1))]) * 20.0f;
		}
	}




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
			if (pObj->GetType() == CObject::TYPE_PLAYER ||
				pObj->GetType() == CObject::TYPE_ENEMY)
			{// 既定のタイプだったら

				D3DXVECTOR3 pos = pObj->GetPosition();

				// 高さ取得
				bool bHit = IsHit(pos);

				if (bHit == true)
				{// 当たっていたら

					// ヒット
					pObj->Hit(1);
				}
				
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
	}



	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CMagma::Draw(void)
{
	// 描画処理
	CObject3DMesh::Draw();
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CMagma::SetVtx(void)
{

	// 頂点情報更新
	CObject3DMesh::SetVtx();
}

//==========================================================================
// 地面に当たったか
//==========================================================================
bool CMagma::IsHit(D3DXVECTOR3& pos)
{
	bool bHit = false;

	// ベクトルと法線
	D3DXVECTOR3 vec1, vec2, nor;
	float fHeight = -99999.0f;
	D3DXVECTOR3 *pVtxPos = GetVtxPos();
	D3DXVECTOR3 posfield = GetPosition();

	// 長さ取得
	float fWidthLen = GetWidthLen();
	float fHeightLen = GetHeightLen();
	//+posfield.x
	// 最大の長さ
	float fMaxWidthLen = fWidthLen * GetWidthBlock();
	float fMaxHeightLen = -fWidthLen * GetHeightBlock();

	// 判定する頂点の計算
	int nCntWidth = 0;
	int nCntHeight = 0;
	int nWidthPoint = ((pos.x - posfield.x) - fMaxWidthLen * 0.5f) / fWidthLen + GetWidthBlock();
	int nHeightPoint = GetHeightBlock() - ((pos.z - posfield.z) - fMaxHeightLen * 0.5f) / fHeightLen;

	for (int nCntH = 0; nCntH < 2; nCntH++)
	{
		for (int nCntW = 0; nCntW < 2; nCntW++)
		{
			// 横頂点
			nCntWidth = nWidthPoint + (nCntW - 1);
			nCntHeight = nHeightPoint + (nCntH - 1);

			if (nCntWidth < 0 || GetWidthBlock() < nCntWidth)
			{// 範囲外で先頭
				continue;
			}

			if (nCntHeight < 0 || GetHeightBlock() < nCntHeight)
			{// 範囲外で先頭
				continue;
			}

			// 今回の頂点
			int nNowPoint = (nCntWidth + 1) + (nCntHeight * (GetWidthBlock() + 1));
			int nVerTexW = (GetWidthBlock() + 1) + 1;

			int nLeft = nCntWidth + (nCntHeight * (GetWidthBlock() + 1));
			int nRight = nCntWidth + (nCntHeight * (GetWidthBlock() + 1)) + nVerTexW;

			if (CollisionTriangle(pVtxPos[nNowPoint] + posfield, pVtxPos[nLeft] + posfield, pVtxPos[nRight] + posfield, pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nRight] - pVtxPos[nNowPoint];
				vec2 = pVtxPos[nLeft] - pVtxPos[nNowPoint];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint].x - posfield.x) * nor.x + (pos.z - pVtxPos[nNowPoint].z - posfield.z) * nor.z + (pVtxPos[nNowPoint].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					break;
				}
			}

			if (CollisionTriangle(pVtxPos[nNowPoint + GetWidthBlock()] + posfield, pVtxPos[nRight] + posfield, pVtxPos[nLeft] + posfield, pos, pos) == true)
			{// 三角に入っていたら

				// ベクトルを計算
				vec1 = pVtxPos[nLeft] - pVtxPos[nNowPoint + GetWidthBlock()];
				vec2 = pVtxPos[nRight] - pVtxPos[nNowPoint + GetWidthBlock()];

				// 外積を求める
				D3DXVec3Cross(&nor, &vec1, &vec2);

				// 外積の正規化をして法線にする
				D3DXVec3Normalize(&nor, &nor);

				if (nor.y != 0.0f)
				{// 法線が0.0fじゃなかったら

					// 高さを求める
					fHeight = ((pos.x - pVtxPos[nNowPoint + GetWidthBlock()].x - posfield.x) * nor.x + (pos.z - pVtxPos[nNowPoint + GetWidthBlock()].z - posfield.z) * nor.z + (pVtxPos[nNowPoint + GetWidthBlock()].y * -nor.y)) / -nor.y;
					fHeight += GetPosition().y;
					break;
				}
			}
		}
	}

	if (fHeight > pos.y)
	{// 前回の位置よりも下だったら(当たっていたら)
		bHit = true;
	}

	// 判定を返す
	return bHit;
}

//==========================================================================
// 起伏の情報取得
//==========================================================================
CMagma *CMagma::GetMagma(void)
{
	return this;
}
