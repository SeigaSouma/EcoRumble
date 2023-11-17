//=============================================================================
// 
// ブロック処理 [explosion.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "main.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "block.h"
#include "collision.h"
#include "calculation.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CBlock::CBlock(int nPriority) : CObject2D(nPriority)
{
	m_nWidth = 0;		// 横分割数
	m_nHeight = 0;		// 縦分割数
	m_nTexIdx = 0;		// テクスチャのインデックス番号
	m_nAlphaCnt = 0;	// 透明度のカウント
}

//==========================================================================
// デストラクタ
//==========================================================================
CBlock::~CBlock()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CBlock *CBlock::Create(void)
{
	// 生成用のオブジェクト
	CBlock *pBlock = NULL;

	if (pBlock == NULL)
	{// NULLだったら

		// メモリの確保
		pBlock = DEBUG_NEW CBlock;

		//if (pBlock->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pBlock;
		//	return NULL;
		//}

		if (pBlock != NULL)
		{// メモリの確保が出来ていたら

			pBlock->m_nWidth = 1;	// 横分割数
			pBlock->m_nHeight = 1;	// 縦分割数

			// 初期化処理
			pBlock->Init();
		}

		return pBlock;
	}

	return NULL;
}

//==========================================================================
// 生成処理(オーバーロード)
//==========================================================================
CBlock *CBlock::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 size, const int nWidth, const int nHeight)
{
	// 生成用のオブジェクト
	CBlock *pBlock = NULL;

	if (pBlock == NULL)
	{// NULLだったら

		// メモリの確保
		pBlock = DEBUG_NEW CBlock;

		//if (pBlock->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pBlock;
		//	return NULL;
		//}

		if (pBlock != NULL)
		{// メモリの確保が出来ていたら

			pBlock->m_nWidth = nWidth;		// 横分割数
			pBlock->m_nHeight = nHeight;	// 縦分割数

			// 初期化処理
			pBlock->Init(pos, size);
		}

		return pBlock;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBlock::Init(void)
{
	HRESULT hr;

	// 種類の設定
	SetType(TYPE_BLOCK);

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetTexture()->Regist("data\\TEXTURE\\block000.jpg");

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 初期化処理
	hr = CObject2D::Init();

	if (FAILED(hr))
	{// 失敗したとき

		return E_FAIL;
	}

	// 頂点情報へのポインタ
	VERTEX_2D *pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f + 1.0f * m_nHeight);
	pVtx[3].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 1.0f + 1.0f * m_nHeight);

	// 頂点バッファをアンロックロック
	GetVtxBuff()->Unlock();

	return S_OK;
}

//==========================================================================
// 初期化処理(オーバーロード)
//==========================================================================
HRESULT CBlock::Init(const D3DXVECTOR3 pos, const D3DXVECTOR2 size)
{
	HRESULT hr;

	// 種類の設定
	SetType(TYPE_BLOCK);

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetTexture()->Regist("data\\TEXTURE\\block000.jpg");

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 位置設定
	SetPosition(pos);

	D3DXVECTOR2 sizeBlock = size;

	// 分割数分広げる
	sizeBlock.x *= m_nWidth;
	sizeBlock.y *= m_nHeight;

	// サイズ設定
	SetSize(sizeBlock);

	// 初期化処理
	hr = CObject2D::Init();

	if (FAILED(hr))
	{// 失敗したとき

		return E_FAIL;
	}

	// 頂点情報へのポインタ
	VERTEX_2D *pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f + 1.0f * m_nHeight);
	pVtx[3].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 1.0f + 1.0f * m_nHeight);

	// 頂点バッファをアンロックロック
	GetVtxBuff()->Unlock();

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBlock::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CBlock::Update(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// サイズ取得
	D3DXVECTOR2 size = GetSize();

	// 過去の位置設定
	SetOldPosition(pos);

	// 色の取得
	D3DXCOLOR col = GetColor();

	// 二次曲線補正
	CuadricCurveComp(col.a, 100, 0.0f, 1.0f, m_nAlphaCnt);

	// 色設定
	SetColor(col);

	// 頂点座標更新
	SetVtx();

	// ブロックとの当たり判定
	//if (collision::Collision(pos.x, pos.x + size.x, pos.y, pos.y + size.y, CObject::TYPE_BULLET) == true)
	//{// 当たっていたら

	//	int nnn = 0;
	//}
}

//==========================================================================
// 描画処理
//==========================================================================
void CBlock::Draw(void)
{
	CObject2D::Draw();
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CBlock::SetVtx(void)
{
	// 頂点設定
	CObject2D::SetVtx();

	// 頂点情報へのポインタ
	VERTEX_2D *pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 位置取得
	D3DXVECTOR3 posBlock = GetPosition();

	// サイズ取得
	D3DXVECTOR2 sizeBlock = GetSize();

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(posBlock.x, posBlock.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(posBlock.x + sizeBlock.x, posBlock.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(posBlock.x, posBlock.y + sizeBlock.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(posBlock.x + sizeBlock.x, posBlock.y + sizeBlock.y, 0.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f + 1.0f * m_nHeight);
	pVtx[3].tex = D3DXVECTOR2(1.0f + 1.0f * m_nWidth, 1.0f + 1.0f * m_nHeight);

	// 頂点バッファをアンロックロック
	GetVtxBuff()->Unlock();
}

