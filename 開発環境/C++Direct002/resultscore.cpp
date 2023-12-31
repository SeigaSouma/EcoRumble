//=============================================================================
// 
//  背景処理 [resultscore.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "resultscore.h"
#include "result.h"
#include "manager.h"
#include "resultmanager.h"
#include "texture.h"
#include "renderer.h"
#include "object2D.h"
#include "number.h"
#include "calculation.h"
#include "result_screen.h"
#include "rankingmanager.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define BASETEXT_TEXTURE	"data\\TEXTURE\\resultscoretext_02.png"	// テクスチャのファイル
#define TEXTURE				"data\\TEXTURE\\resultscoretext_00.png"	// テクスチャのファイル
#define NUMBERTEXTURE		"data\\TEXTURE\\number_blackclover_01.png"	// テクスチャのファイル
#define MAX_VALUE			(999999)			// 値の最大値
#define TEX_U				(0.1f)				// Uの分割
#define BASE_WIDTH			(40.0f * 0.75f)		// 横幅
#define BASE_HEIGHT			(50.0f * 0.75f)		// 縦幅
#define BASE_DIS_X			(50.0f * 0.75f)		// 間隔
#define WIDTH				(40.0f)				// 横幅
#define HEIGHT				(50.0f)				// 縦幅
#define DIS_X				(50.0f)				// 間隔
#define POSY_BASE			(330.0f)			// Y位置
#define POSY				(650.0f)			// Y位置
#define BASETEXT_POSX		(180.0f)			// X位置
#define TEXT_POSX			(450.0f)			// X位置
#define INT_BASESCORESET	(60)				// スコアの変動時間
#define INT_SCORESET		(120)				// スコアの変動時間
#define TEXT_MOVETIME		(20)				// 移動時間
#define INIT_POSX			(1600.0f)			// 初期位置

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CResultScore::CResultScore(int nPriority)
{
	// 値のクリア
	m_nBaseNum = 0;					// 元のスコア
	m_nBaseNumDest = 0;				// 元のスコアの目標値
	m_nToatalNum = 0;					// 値
	m_nToatalNumDest = 0;				// 値
	m_nTexIdx = 0;			// テクスチャのインデックス番号
	m_nTexIdxNumber = 0;	// 数字テクスチャのインデックス番号
	m_pBaseObj2D = NULL;		// 元のスコア2Dのオブジェクト
	m_pToatalObj2D = NULL;		// オブジェクト2Dのオブジェクト
	m_fBasePosDest_X = 0.0f;		// 元スコアの目標位置
	m_fToatalPosDest_X = 0.0f;		// 最終スコアの目標位置
	m_bArrivalBase = false;			// 元のスコアの到着判定
	m_bArrivalToatal = false;		// 最終スコアの到着判定
	m_bSetBase = false;				// 元のスコアの設定判定
	m_bSetToatal = false;			// 最終スコアの設定判定
	memset(&m_pToatalScore[0], NULL, sizeof(m_pToatalScore));	// 数字のオブジェクト
	memset(&m_pBaseScore[0], NULL, sizeof(m_pBaseScore));	// 元のスコア
}

//==========================================================================
// デストラクタ
//==========================================================================
CResultScore::~CResultScore()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CResultScore *CResultScore::Create(void)
{
	// 生成用のオブジェクト
	CResultScore *pScore = NULL;

	if (pScore == NULL)
	{// NULLだったら

		// メモリの確保
		pScore = DEBUG_NEW CResultScore;

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
HRESULT CResultScore::Init(void)
{
	// 種類設定
	SetType(CObject::TYPE_SCORE);

	m_fBasePosDest_X = BASETEXT_POSX;	// 元スコアの目標位置
	m_fToatalPosDest_X = TEXT_POSX;		// 最終スコアの目標位置

	// 元のスコアの生成
	CreateBaseScore();

	// 最終スコアの生成
	CreateToatalScore();


	// 元のスコア取得
	m_nBaseNumDest = CManager::GetResultManager()->GetBaseScore();

	// 目標のスコア取得
	m_nToatalNumDest = CManager::GetResultManager()->GetToatalScore();
	CManager::GetRankingManager()->SetNowScore(m_nToatalNumDest);
	
	return S_OK;
}

//==========================================================================
// 元のスコアの生成
//==========================================================================
void CResultScore::CreateBaseScore(void)
{
	// 生成処理
	m_pBaseObj2D = CObject2D::Create(7);

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetTexture()->Regist(BASETEXT_TEXTURE);

	// テクスチャの割り当て
	m_pBaseObj2D->GetObject2D()->BindTexture(m_nTexIdx);

	// 各種変数の初期化
	m_pBaseObj2D->GetObject2D()->SetSize(CManager::GetTexture()->GetImageSize(m_nTexIdx) * 0.3f);	// サイズ
	m_pBaseObj2D->GetObject2D()->SetPosition(D3DXVECTOR3(INIT_POSX, POSY_BASE, 0.0f));	// 位置

	// 種類の設定
	m_pBaseObj2D->GetObject2D()->SetType(CObject::TYPE_SCORE);


	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		// 生成処理
		m_pBaseScore[nCntScore] = CNumber::Create();

		// 各種変数の初期化
		m_pBaseScore[nCntScore]->GetObject2D()->SetSize(D3DXVECTOR2(BASE_WIDTH, BASE_HEIGHT));	// サイズ
		m_pBaseScore[nCntScore]->GetObject2D()->SetPosition(D3DXVECTOR3(INIT_POSX + m_pBaseObj2D->GetSize().x + nCntScore * BASE_DIS_X, POSY_BASE, 0.0f));	// 位置

		// 種類の設定
		m_pBaseScore[nCntScore]->GetObject2D()->SetType(CObject::TYPE_SCORE);

		// テクスチャの割り当て
		m_nTexIdxNumber = CManager::GetTexture()->Regist(NUMBERTEXTURE);

		// テクスチャの割り当て
		m_pBaseScore[nCntScore]->GetObject2D()->BindTexture(m_nTexIdxNumber);
	}
}

//==========================================================================
// 最終スコアの生成
//==========================================================================
void CResultScore::CreateToatalScore(void)
{
	// 生成処理
	m_pToatalObj2D = CObject2D::Create(7);

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// テクスチャの割り当て
	m_pToatalObj2D->GetObject2D()->BindTexture(m_nTexIdx);

	// 各種変数の初期化
	m_pToatalObj2D->GetObject2D()->SetSize(CManager::GetTexture()->GetImageSize(m_nTexIdx) * 0.4f);	// サイズ
	m_pToatalObj2D->GetObject2D()->SetPosition(D3DXVECTOR3(INIT_POSX, POSY, 0.0f));	// 位置

	// 種類の設定
	m_pToatalObj2D->GetObject2D()->SetType(CObject::TYPE_SCORE);


	float fPosX = INIT_POSX + m_pToatalObj2D->GetSize().x + 50.0f;
	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		// 生成処理
		m_pToatalScore[nCntScore] = CNumber::Create();

		// 各種変数の初期化
		m_pToatalScore[nCntScore]->GetObject2D()->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// サイズ
		m_pToatalScore[nCntScore]->GetObject2D()->SetPosition(D3DXVECTOR3(fPosX + nCntScore * DIS_X, POSY, 0.0f));	// 位置

		// 種類の設定
		m_pToatalScore[nCntScore]->GetObject2D()->SetType(CObject::TYPE_SCORE);

		// テクスチャの割り当て
		m_nTexIdxNumber = CManager::GetTexture()->Regist(NUMBERTEXTURE);

		// テクスチャの割り当て
		m_pToatalScore[nCntScore]->GetObject2D()->BindTexture(m_nTexIdxNumber);
	}
}

//==========================================================================
// 終了処理
//==========================================================================
void CResultScore::Uninit(void)
{
	// 終了処理
	if (m_pBaseObj2D != NULL)
	{// メモリの確保がされていたら

		// 終了処理
		m_pBaseObj2D->Uninit();
		m_pBaseObj2D = NULL;
	}

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		// 終了処理
		if (m_pBaseScore[nCntScore] != NULL)
		{// メモリの確保がされていたら

			// 終了処理
			m_pBaseScore[nCntScore]->Uninit();
			delete m_pBaseScore[nCntScore];
			m_pBaseScore[nCntScore] = NULL;
		}
	}

	// 終了処理
	if (m_pToatalObj2D != NULL)
	{// メモリの確保がされていたら

		// 終了処理
		m_pToatalObj2D->Uninit();
		m_pToatalObj2D = NULL;
	}

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		// 終了処理
		if (m_pToatalScore[nCntScore] != NULL)
		{// メモリの確保がされていたら

			// 終了処理
			m_pToatalScore[nCntScore]->Uninit();
			delete m_pToatalScore[nCntScore];
			m_pToatalScore[nCntScore] = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CResultScore::Update(void)
{
	// 元のスコアの移動処理
	if (m_bSetBase == false)
	{
		MoveBaseScore();
	}

	// 最終スコアの移動処理
	if (m_bSetToatal == false)
	{
		MoveToatalScore();
	}

	// 元のスコアの設定処理
	SetBaseValue();

	// 最終スコアの設定処理
	SetToatalValue();
}

//==========================================================================
// 元スコアの移動処理
//==========================================================================
void CResultScore::MoveBaseScore(void)
{
	if (m_pBaseObj2D == NULL)
	{// 元のスコアが生成されていたら
		return;
	}

	// 位置取得
	D3DXVECTOR3 pos = m_pBaseObj2D->GetPosition();

	if (pos.x == INIT_POSX)
	{
		// サウンド再生
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_NUMBERMOVE);
	}

	// 移動
	pos.x += (m_fBasePosDest_X - INIT_POSX) / (float)TEXT_MOVETIME;
	if (pos.x <= m_fBasePosDest_X)
	{// 補正完了
		pos.x = m_fBasePosDest_X;
		m_bArrivalBase = true;		// 元のスコアの到着判定
	}

	// 位置設定
	m_pBaseObj2D->SetPosition(pos);

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pBaseScore[nCntScore] == NULL)
		{// NULLだったら
			continue;
		}

		// 位置取得
		D3DXVECTOR3 posNumber = m_pBaseScore[nCntScore]->GetObject2D()->GetPosition();

		// 位置更新
		m_pBaseScore[nCntScore]->GetObject2D()->SetPosition(
			D3DXVECTOR3(
				pos.x + m_pBaseObj2D->GetSize().x + nCntScore * BASE_DIS_X,
				posNumber.y,
				0.0f));
	}

	if (m_bArrivalBase == true)
	{// 到着していたら

		// 元のスコア取得
#if _DEBUG
		m_nBaseNumDest = 1000;
#else
		m_nBaseNumDest = CManager::GetResultManager()->GetBaseScore();
#endif

		if (m_nBaseNumDest > m_nBaseNum)
		{// 規定時間かけて補正
			m_nBaseNum += (int)((float)m_nBaseNumDest / (float)INT_BASESCORESET);

			// サウンド再生
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_NUMBER);
		}
		else
		{// 目標の値で固定
			m_nBaseNum = m_nBaseNumDest;
			m_bSetBase = true;				// 元のスコアの設定判定
		}
	}
}

//==========================================================================
// 最終スコアの移動処理
//==========================================================================
void CResultScore::MoveToatalScore(void)
{
	if (CResult::GetResultScreen()->IsSetNumber() == false || m_pToatalObj2D == NULL)
	{// 最終スコアが生成されていたら
		return;
	}

	// 位置取得
	D3DXVECTOR3 pos = m_pToatalObj2D->GetPosition();

	if (pos.x == INIT_POSX)
	{
		// サウンド再生
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_NUMBERMOVE);
	}

	// 移動
	pos.x += (m_fToatalPosDest_X - INIT_POSX) / (float)TEXT_MOVETIME;
	if (pos.x <= m_fToatalPosDest_X)
	{// 補正完了
		pos.x = m_fToatalPosDest_X;
		m_bArrivalToatal = true;	// 最終スコアの到着判定
	}

	// 位置設定
	m_pToatalObj2D->SetPosition(pos);

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pToatalScore[nCntScore] == NULL)
		{// NULLだったら
			continue;
		}

		// 位置取得
		D3DXVECTOR3 posNumber = m_pToatalScore[nCntScore]->GetObject2D()->GetPosition();

		// 位置更新
		m_pToatalScore[nCntScore]->GetObject2D()->SetPosition(
			D3DXVECTOR3(
				pos.x + m_pToatalObj2D->GetSize().x + nCntScore * DIS_X + 50.0f,
				posNumber.y,
				0.0f));
	}

	if (m_bArrivalToatal == true)
	{// 到着していたら

		// 目標のスコア取得
#if _DEBUG
		m_nToatalNumDest = 1000;
#else
		m_nToatalNumDest = CManager::GetResultManager()->GetToatalScore();
#endif
		CManager::GetRankingManager()->SetNowScore(m_nToatalNumDest);

		if (m_nToatalNumDest > m_nToatalNum)
		{// 規定時間かけて補正
			m_nToatalNum += (int)((float)m_nToatalNumDest / (float)INT_SCORESET);

			// サウンド再生
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_NUMBER);
		}
		else
		{// 目標の値で固定
			m_nToatalNum = m_nToatalNumDest;
			m_bSetToatal = true;			// 最終スコアの設定判定
			CResult::SetEnableArrival();
		}
	}
}

//==========================================================================
// 元のスコアの設定処理
//==========================================================================
void CResultScore::SetBaseValue(void)
{
	int aTexU[RESULTSCORE_DIGIT];
	int nDigit = 1;		// aTexU計算用

	// 計算用割り出し
	for (int nCntDigit = 0; nCntDigit < RESULTSCORE_DIGIT; nCntDigit++)
	{
		nDigit *= 10;
	}

	// テクスチャ座標に代入する
	for (int nCntTex = 0; nCntTex < RESULTSCORE_DIGIT; nCntTex++)
	{// 桁数分設定

		aTexU[nCntTex] = m_nBaseNum % nDigit / (nDigit / 10);
		nDigit /= 10;
	}

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pBaseScore[nCntScore] != NULL)
		{// NULLじゃなかったら

			D3DXVECTOR2 *pTex = m_pBaseScore[nCntScore]->GetObject2D()->GetTex();

			// テクスチャ座標の設定
			pTex[0] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 1.0f);

			// 頂点設定
			m_pBaseScore[nCntScore]->GetObject2D()->SetVtx();
		}
	}
}

//==========================================================================
// 最終スコアの設定処理
//==========================================================================
void CResultScore::SetToatalValue(void)
{
	int aTexU[RESULTSCORE_DIGIT];
	int nDigit = 1;		// aTexU計算用

	// 計算用割り出し
	for (int nCntDigit = 0; nCntDigit < RESULTSCORE_DIGIT; nCntDigit++)
	{
		nDigit *= 10;
	}

	// テクスチャ座標に代入する
	for (int nCntTex = 0; nCntTex < RESULTSCORE_DIGIT; nCntTex++)
	{// 桁数分設定

		aTexU[nCntTex] = m_nToatalNum % nDigit / (nDigit / 10);
		nDigit /= 10;
	}

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pToatalScore[nCntScore] != NULL)
		{// NULLじゃなかったら

			D3DXVECTOR2 *pTex = m_pToatalScore[nCntScore]->GetObject2D()->GetTex();

			// テクスチャ座標の設定
			pTex[0] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU[nCntScore] * TEX_U + TEX_U, 1.0f);

			// 頂点設定
			m_pToatalScore[nCntScore]->GetObject2D()->SetVtx();
		}
	}
}

//==========================================================================
// 全ての到着設定
//==========================================================================
void CResultScore::SetAllArrival(void)
{
	m_bArrivalBase = true;			// 元のスコアの到着判定
	m_bArrivalToatal = true;		// 最終スコアの到着判定
	m_bSetBase = true;				// 元のスコアの設定判定
	m_bSetToatal = true;			// 最終スコアの設定判定

	// 位置取得
	D3DXVECTOR3 pos = m_pBaseObj2D->GetPosition();
	
	pos.x = m_fBasePosDest_X;

	// 位置設定
	m_pBaseObj2D->SetPosition(pos);

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pBaseScore[nCntScore] == NULL)
		{// NULLだったら
			continue;
		}

		// 位置取得
		D3DXVECTOR3 posNumber = m_pBaseScore[nCntScore]->GetObject2D()->GetPosition();

		// 位置更新
		m_pBaseScore[nCntScore]->GetObject2D()->SetPosition(
			D3DXVECTOR3(
				pos.x + m_pBaseObj2D->GetSize().x + nCntScore * BASE_DIS_X,
				posNumber.y,
				0.0f));
	}

	// 目標の値に設定
	m_nBaseNum = m_nBaseNumDest;
	m_nToatalNum = m_nToatalNumDest;


	// 位置取得
	pos = m_pToatalObj2D->GetPosition();

	// 移動
	pos.x = m_fToatalPosDest_X;

	// 位置設定
	m_pToatalObj2D->SetPosition(pos);

	for (int nCntScore = 0; nCntScore < RESULTSCORE_DIGIT; nCntScore++)
	{
		if (m_pToatalScore[nCntScore] == NULL)
		{// NULLだったら
			continue;
		}

		// 位置取得
		D3DXVECTOR3 posNumber = m_pToatalScore[nCntScore]->GetObject2D()->GetPosition();

		// 位置更新
		m_pToatalScore[nCntScore]->GetObject2D()->SetPosition(
			D3DXVECTOR3(
				pos.x + m_pToatalObj2D->GetSize().x + nCntScore * DIS_X + 50.0f,
				posNumber.y,
				0.0f));
	}

}

//==========================================================================
// 描画処理
//==========================================================================
void CResultScore::Draw(void)
{

}

//==========================================================================
// 値の設定処理
//==========================================================================
void CResultScore::SetValue(int nScore)
{
	m_nToatalNumDest = nScore;
}
