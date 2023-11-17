//=============================================================================
// 
//  弾処理 [magiccircle.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "magiccircle.h"
#include "calculation.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\mss02-3.png"
#define WIDTH			(200.0f)						// 横幅
#define HEIGHT			(200.0f)						// 縦幅

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CMagicCircle::m_nNumAll = 0;						// 弾の総数

//==========================================================================
// コンストラクタ
//==========================================================================
CMagicCircle::CMagicCircle(int nPriority) : CObject3D(nPriority)
{
	m_nTexIdx = 0;	// テクスチャのインデックス番号
	m_sizeDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 目標のサイズ

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CMagicCircle::~CMagicCircle()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CMagicCircle *CMagicCircle::Create(void)
{
	// 生成用のオブジェクト
	CMagicCircle *pBullet = NULL;

	if (pBullet == NULL)
	{// NULLだったら

		// メモリの確保
		pBullet = DEBUG_NEW CMagicCircle;

		//if (pBullet->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pBullet;
		//	return NULL;
		//}

		if (pBullet != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pBullet->Init();
		}

		return pBullet;
	}

	return NULL;
}

//==========================================================================
// 生成処理(オーバーロード)
//==========================================================================
CMagicCircle *CMagicCircle::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, D3DXVECTOR3 size)
{
	// 生成用のオブジェクト
	CMagicCircle *pBullet = NULL;

	if (pBullet == NULL)
	{// NULLだったら

		// メモリの確保
		pBullet = DEBUG_NEW CMagicCircle;

		//if (pBullet->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pBullet;
		//	return NULL;
		//}

		if (pBullet != NULL)
		{// メモリの確保が出来ていたら

			// 位置設定
			pBullet->SetPosition(pos);

			// 向き設定
			pBullet->SetRotation(rot);

			// サイズ設定
			pBullet->m_sizeDest = size;

			// 初期化処理
			pBullet->Init();
		}

		return pBullet;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CMagicCircle::Init(void)
{
	HRESULT hr;

	// 種類の設定
	SetType(TYPE_MAGIC_CIRCLE);

	// 初期化処理
	hr = CObject3D::Init();

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	if (FAILED(hr))
	{// 失敗したとき

		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CMagicCircle::Uninit(void)
{
	// 終了処理
	CObject3D::Uninit();

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CMagicCircle::Update(void)
{
	// サイズ取得
	D3DXVECTOR3 size = GetSize();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 慣性補正
	InertiaCorrection(size.x, m_sizeDest.x, 0.2f);
	InertiaCorrection(size.y, m_sizeDest.y, 0.2f);
	InertiaCorrection(size.z, m_sizeDest.z, 0.2f);

	// 回転
	rot.z += D3DX_PI * 0.1f;

	// 向き設定
	SetRotation(rot);

	// サイズ設定
	SetSize(size);

	// 頂点情報設定
	CObject3D::SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CMagicCircle::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 背面のカリングなし
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 描画処理
	CObject3D::Draw();

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// カリングデフォルト
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//==========================================================================
// 総数取得
//==========================================================================
int CMagicCircle::GetNumAll(void)
{
	return m_nNumAll;
}
