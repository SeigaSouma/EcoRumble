//=============================================================================
// 
//  背景処理 [delusion_point.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "delusion_point.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "object2D.h"
#include "number.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\number_05.png"	// テクスチャのファイル
#define TEXT_TEXTURE	"data\\TEXTURE\\remainpoint_01.png"	// テクスチャのファイル
#define MAX_VALUE		(999999)						// 値の最大値
#define TEX_U			(0.1f)							// Uの分割
#define WIDTH			(40.0f)							// 横幅
#define HEIGHT			(50.0f)							// 縦幅
#define DIS_X			(50.0f)							// 間隔
#define SCROLL_SPEED	(-0.005f)						// スクロール速度

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CDelusionPoint::CDelusionPoint(int nPriority)
{
	// 値のクリア
	m_apNumber = NULL;	// 数字のオブジェクト
	m_nNum = 0;			// 値
	m_nTexIdx = 0;		// テクスチャのインデックス番号
}

//==========================================================================
// デストラクタ
//==========================================================================
CDelusionPoint::~CDelusionPoint()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CDelusionPoint *CDelusionPoint::Create(void)
{
	// 生成用のオブジェクト
	CDelusionPoint *pScore = NULL;

	if (pScore == NULL)
	{// NULLだったら

		// メモリの確保
		pScore = DEBUG_NEW CDelusionPoint;

		if (pScore != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pScore->Init();
		}

		return pScore;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CDelusionPoint::Init(void)
{
	//*********************************
	// テキスト
	//*********************************
	// 各種変数の初期化
	SetSize(D3DXVECTOR2(100.0f, 40.0f));			// サイズ
	SetPosition(D3DXVECTOR3(580.0f, 400.0f, 0.0f));	// 位置
	SetType(CObject::TYPE_DELUSION_POINT);			// 種類の設定

	// 初期化処理
	HRESULT hr = CObject2D::Init();

	if (FAILED(hr))
	{// 失敗したとき

		return E_FAIL;
	}

	// テクスチャの読み込み
	m_nTexIdx = CManager::GetTexture()->Regist(TEXT_TEXTURE);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);


	//*********************************
	// 数字
	//*********************************
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 生成処理
	m_apNumber = CNumber::Create();

	// 各種変数の初期化
	m_apNumber->GetObject2D()->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// サイズ
	m_apNumber->GetObject2D()->SetPosition(D3DXVECTOR3(pos.x + 120.0f, pos.y, pos.z));					// 位置

	// 種類の設定
	m_apNumber->GetObject2D()->SetType(CObject::TYPE_SCORE);

	// 数字のテクスチャ読み込み
	int nTexNumberIdx = CManager::GetTexture()->Regist(TEXTURE);

	// テクスチャの割り当て
	m_apNumber->GetObject2D()->BindTexture(nTexNumberIdx);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CDelusionPoint::Uninit(void)
{
	// 終了処理
	if (m_apNumber != NULL)
	{// メモリの確保がされていたら

		// 終了処理
		m_apNumber->GetObject2D()->Uninit();
		m_apNumber->Uninit();
		delete m_apNumber;
		m_apNumber = NULL;
	}

	// 開放処理
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CDelusionPoint::Update(void)
{
	//SetVtx();

	// 値の設定処理
	//SetValue(m_nNum);
}

//==========================================================================
// 値の設定処理
//==========================================================================
void CDelusionPoint::SetValue(int nNum)
{
	int aTexU;
	int nDigit = 10;		// aTexU計算用
	m_nNum = nNum;

	// テクスチャ座標に代入する
	aTexU = nNum % nDigit / (nDigit / 10);

	if (m_apNumber != NULL)
	{// NULLじゃなかったら

		// テクスチャ座標取得
		D3DXVECTOR2 *pTex = m_apNumber->GetObject2D()->GetTex();

		// テクスチャ座標の設定
		pTex[0] = D3DXVECTOR2(aTexU * TEX_U, 0.0f);
		pTex[1] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 0.0f);
		pTex[2] = D3DXVECTOR2(aTexU * TEX_U, 1.0f);
		pTex[3] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 1.0f);

		// テクスチャ座標の設定
		SetTex(pTex);

		// 頂点設定
		m_apNumber->GetObject2D()->SetVtx();
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CDelusionPoint::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}
