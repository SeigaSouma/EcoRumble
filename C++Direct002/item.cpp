//=============================================================================
// 
//  アイテム処理 [bullet.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "item.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "explosion.h"
#include "sound.h"
#include "particle.h"
#include "score.h"
#include "collision.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH	(30.0f)		// 横幅
#define HEIGHT	(30.0f)		// 縦幅

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CItem::m_apTextureFile[] =					// テクスチャのファイル
{
	NULL,
	"data\\TEXTURE\\bullet_01.png",
};
int CItem::m_nNumAll = 0;		// アイテムの総数

//==========================================================================
// コンストラクタ
//==========================================================================
CItem::CItem(int nPriority) : CObject2D(nPriority)
{
	// テクスチャデータの配列分繰り返す
	for (int nCntTex = 0; nCntTex < sizeof(m_apTextureFile) / sizeof(*m_apTextureFile); nCntTex++)
	{
		m_nTexIdx[nCntTex] = 0;		// テクスチャのインデックス番号
	}

	// 値のクリア
	m_type = TYPE_NONE;

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// コンストラクタ
//==========================================================================
CItem::CItem(TYPE type, int nPriority) : CObject2D(nPriority)
{

	// 値のクリア
	m_type = type;

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CItem::~CItem()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CItem *CItem::Create(TYPE type)
{
	// 生成用のオブジェクト
	CItem *pItem = NULL;

	if (pItem == NULL)
	{// NULLだったら

		// メモリの確保
		pItem = DEBUG_NEW CItem;

		//if (pItem->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pItem;
		//	return NULL;
		//}

		if (pItem != NULL)
		{// メモリの確保が出来ていたら

			// 種類設定
			pItem->m_type = type;

			// 初期化処理
			pItem->Init();
		}

		return pItem;
	}

	return NULL;
}

//==========================================================================
// 生成処理(オーバーロード)
//==========================================================================
CItem *CItem::Create(TYPE type, const D3DXVECTOR3 pos)
{
	// 生成用のオブジェクト
	CItem *pItem = NULL;

	if (pItem == NULL)
	{// NULLだったら

		// メモリの確保
		pItem = DEBUG_NEW CItem;

		//if (pItem->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pItem;
		//	return NULL;
		//}

		if (pItem != NULL)
		{// メモリの確保が出来ていたら

			// 種類設定
			pItem->m_type = type;

			// 位置割り当て
			pItem->SetPosition(pos);

			// 初期化処理
			pItem->Init();
		}

		return pItem;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CItem::Init(void)
{
	HRESULT hr;

	// 各種変数の初期化
 	SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// サイズ

	// 種類の設定
	SetType(CObject::TYPE_ITEM);

	// 初期化処理
	hr = CObject2D::Init();

	// テクスチャデータの配列分繰り返す
	for (int nCntTex = 0; nCntTex < sizeof(m_apTextureFile) / sizeof(*m_apTextureFile); nCntTex++)
	{
		// テクスチャの割り当て
		m_nTexIdx[nCntTex] = CManager::GetTexture()->Regist(m_apTextureFile[nCntTex]);

		// テクスチャの割り当て
		BindTexture(m_nTexIdx[nCntTex]);
	}

	if (FAILED(hr))
	{// 失敗したとき

		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CItem::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CItem::Update(void)
{
	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CItem::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	CObject2D::Draw();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CItem::SetVtx(void)
{
	// 頂点設定
	CObject2D::SetVtx();
}

//==========================================================================
// 総数取得
//==========================================================================
int CItem::GetNumAll(void)
{
	return m_nNumAll;
}
