//=============================================================================
// 
//  数字処理 [number.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "number.h"
#include "manager.h"
#include "renderer.h"
#include "object2D.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(640.0f)					// 横幅
#define HEIGHT			(360.0f)					// 縦幅
#define SCROLL_SPEED	(-0.005f)					// スクロール速度

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CNumber::CNumber(int nPriority)
{
	// 値のクリア
	m_aObject2D = NULL;		// オブジェクト2Dのオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CNumber::~CNumber()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CNumber *CNumber::Create(void)
{
	// 生成用のオブジェクト
	CNumber *pNumber = NULL;

	if (pNumber == NULL)
	{// NULLだったら

		// メモリの確保
		pNumber = DEBUG_NEW CNumber;

		if (pNumber != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pNumber->Init();
		}
		else
		{
			delete pNumber;
			pNumber = NULL;
		}

		return pNumber;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CNumber::Init(void)
{
	// 生成処理
	m_aObject2D = CObject2D::Create(7);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CNumber::Uninit(void)
{

	if (m_aObject2D != NULL)
	{// NULLじゃなかったら

		// 終了処理
		m_aObject2D = NULL;
	}
}

//==========================================================================
// 解放処理
//==========================================================================
void CNumber::Release(void)
{
	if (m_aObject2D != NULL)
	{// NULLじゃなかったら
		m_aObject2D->Uninit();
		m_aObject2D = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CNumber::Update(void)
{
	// 更新処理
	m_aObject2D->Update();
}

//==========================================================================
// 描画処理
//==========================================================================
void CNumber::Draw(void)
{
	// 描画処理
	m_aObject2D->Draw();
}

//==========================================================================
// オブジェクト2Dオブジェクトの取得
//==========================================================================
CObject2D *CNumber::GetObject2D(void)
{
	return m_aObject2D;
}