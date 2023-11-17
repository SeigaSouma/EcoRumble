//=============================================================================
// 
//  背景処理 [score.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "score.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "object2D.h"
#include "number.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\number_05.png"	// テクスチャのファイル
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
CScore::CScore(int nPriority)
{
	// 値のクリア
	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		m_apNumber[nCntScore] = NULL;
	}
	m_nNum = 0;		// 値
	m_nTexIdx = 0;		// テクスチャのインデックス番号
}

//==========================================================================
// デストラクタ
//==========================================================================
CScore::~CScore()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CScore *CScore::Create(void)
{
	// 生成用のオブジェクト
	CScore *pScore = NULL;

	if (pScore == NULL)
	{// NULLだったら

		// メモリの確保
		pScore = DEBUG_NEW CScore;

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
// 生成処理
//==========================================================================
CScore *CScore::Create(D3DXVECTOR3 pos)
{
	// 生成用のオブジェクト
	CScore *pScore = NULL;

	if (pScore == NULL)
	{// NULLだったら

		// メモリの確保
		pScore = DEBUG_NEW CScore;

		if (pScore != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pScore->Init(pos);
		}

		return pScore;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CScore::Init(void)
{
	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		// 生成処理
		m_apNumber[nCntScore] = CNumber::Create();

		// 各種変数の初期化
		m_apNumber[nCntScore]->GetObject2D()->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// サイズ
		m_apNumber[nCntScore]->GetObject2D()->SetPosition(D3DXVECTOR3(640.0f, 360.0f, 0.0f));	// 位置

		// 種類の設定
		m_apNumber[nCntScore]->GetObject2D()->SetType(CObject::TYPE_SCORE);

		// テクスチャの割り当て
		m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

		// テクスチャの割り当て
		m_apNumber[nCntScore]->GetObject2D()->BindTexture(m_nTexIdx);

	}
	return S_OK;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CScore::Init(D3DXVECTOR3 pos)
{

	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		// 生成処理
		m_apNumber[nCntScore] = CNumber::Create();

		// 各種変数の初期化
		m_apNumber[nCntScore]->GetObject2D()->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// サイズ
		m_apNumber[nCntScore]->GetObject2D()->SetPosition(D3DXVECTOR3(pos.x + DIS_X * nCntScore, pos.y, pos.z));	// 位置

		// 種類の設定
		m_apNumber[nCntScore]->GetObject2D()->SetType(CObject::TYPE_SCORE);

		// テクスチャの割り当て
		m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

		// テクスチャの割り当て
		m_apNumber[nCntScore]->GetObject2D()->BindTexture(m_nTexIdx);

	}
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CScore::Uninit(void)
{
	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		// 終了処理
		if (m_apNumber[nCntScore] != NULL)
		{// メモリの確保がされていたら

			// 終了処理
			m_apNumber[nCntScore]->Uninit();
			delete m_apNumber[nCntScore];
			m_apNumber[nCntScore] = NULL;
		}
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CScore::Update(void)
{
	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		if (m_apNumber[nCntScore] != NULL)
		{// NULLじゃなかったら
			m_apNumber[nCntScore]->Update();
		}
	}
	// 値の設定処理
	SetValue();
}

//==========================================================================
// 値の設定処理
//==========================================================================
void CScore::SetValue(void)
{
	int aTexU[NUM_SCORE];
	int nDigit = 1;		// aTexU計算用

	// 計算用割り出し
	for (int nCntDigit = 0; nCntDigit < NUM_SCORE; nCntDigit++)
	{
		nDigit *= 10;
	}

	// テクスチャ座標に代入する
	for (int nCntTex = 0; nCntTex < NUM_SCORE; nCntTex++)
	{// 桁数分設定

		aTexU[nCntTex] = m_nNum % nDigit / (nDigit / 10);
		nDigit /= 10;
	}

	for (int nCntScore = 0; nCntScore < NUM_SCORE; nCntScore++)
	{
		if (m_apNumber[nCntScore] != NULL)
		{// NULLじゃなかったら

			D3DXVECTOR2 *pTex = m_apNumber[nCntScore]->GetObject2D()->GetTex();

			// テクスチャ座標の設定
			pTex[0] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 1.0f);

			// 頂点設定
			m_apNumber[nCntScore]->GetObject2D()->SetVtx();
		}
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CScore::Draw(void)
{

}

//==========================================================================
// 加算処理
//==========================================================================
void CScore::Add(int nValue)
{
	// 値加算
	m_nNum += nValue;

	// スコアの加算
	CManager::GetResultManager()->AddScore(nValue);

	if (m_nNum >= MAX_VALUE)
	{// 最大値で固定
		m_nNum = MAX_VALUE;
	}

	// 値の設定処理
	SetValue();
}

//==========================================================================
// 加算処理
//==========================================================================
void CScore::Add(CResultManager::ADDTYPE AddType)
{
	// 値加算
	/*switch (AddType)
	{
	case CResultManager::ADDTYPE_NONE:
		break;

	case CResultManager::ADDTYPE_NORMAL:
		m_nNum += mylib_const::COMBOBONUS_1;
		break;

	case CResultManager::ADDTYPE_BLUE:
		m_nNum += mylib_const::COMBOBONUS_2;
		break;

	case CResultManager::ADDTYPE_GOLD:
		m_nNum += mylib_const::COMBOBONUS_3;
		break;

	default:
		break;
	}*/

	// スコアの加算
	CManager::GetResultManager()->AddScoreNum(AddType);

	if (m_nNum >= MAX_VALUE)
	{// 最大値で固定
		m_nNum = MAX_VALUE;
	}

	// 値の設定処理
	SetValue();
}
