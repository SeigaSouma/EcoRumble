//=============================================================================
// 
//  パワーゲージ処理 [power_gauge.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "power_gauge.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "particle.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH	(40.0f * 9.0f)	// 横幅
#define HEIGHT	(5.0f * 9.0f)	// 縦幅

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CPowerGauge::m_apTextureFile[VTX_MAX] =					// テクスチャのファイル
{
	NULL,
	NULL,
	NULL,
	"data\\TEXTURE\\hypnosis_fram.png",
};

//==========================================================================
// コンストラクタ
//==========================================================================
CPowerGauge::CPowerGauge(int nPriority) : CObject(nPriority)
{
	// テクスチャデータの配列分繰り返す
	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		m_nTexIdx[nCntGauge] = 0;		// テクスチャのインデックス番号
		m_sPowerGauge[nCntGauge] = {};		// オブジェクト2Dのポインタ
	}
	m_Level = LEVEL_1;	// チャージ段階
	m_nCntWaitTime = 0;	// 待ち時間
	m_nCntCharge = 0;	// チャージカウント
	m_nMaxCharge = 0;	// 最大チャージカウント
	m_bFever = false;	// FEVERかの判定
}

//==========================================================================
// デストラクタ
//==========================================================================
CPowerGauge::~CPowerGauge()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CPowerGauge *CPowerGauge::Create(void)
{
	// 生成用のオブジェクト
	CPowerGauge *pGauge = NULL;

	if (pGauge == NULL)
	{// NULLだったら

		// メモリの確保
		pGauge = DEBUG_NEW CPowerGauge;

		if (pGauge != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pGauge->Init();
		}

		return pGauge;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CPowerGauge::Init(void)
{

	// 各種変数の初期化
	m_nCntCharge = 0;
	m_nMaxCharge = 180 * 7;

	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		// 生成処理
		m_sPowerGauge[nCntGauge].pObj2D = CObject2D::Create(7);

		m_sPowerGauge[nCntGauge].pObj2D->SetPosition(D3DXVECTOR3(640.0f, 650.0f, 0.0f));	// 位置
		m_sPowerGauge[nCntGauge].pObj2D->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));				// サイズ

		// 各種変数の初期化
		m_sPowerGauge[nCntGauge].fMaxWidth = m_sPowerGauge[nCntGauge].pObj2D->GetSize().x;	// 幅の最大値
		m_sPowerGauge[nCntGauge].fMaxHeight = m_sPowerGauge[nCntGauge].pObj2D->GetSize().y;	// 高さの最大値

		if (nCntGauge == VTX_GAUGE || nCntGauge == VTX_RED)
		{// ゲージ部分は初期0
			m_sPowerGauge[nCntGauge].pObj2D->SetSize(D3DXVECTOR2(0.0f, HEIGHT));			// サイズ
		}

		// 頂点カラーの設定
		switch (nCntGauge)
		{
		case VTX_BLACK:		// 下地

			// 色設定
			m_sPowerGauge[nCntGauge].pObj2D->SetColor(D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f));
			break;

			case VTX_RED:		// 変動するゲージ

			// 色設定
			m_sPowerGauge[nCntGauge].pObj2D->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
			break;

		case VTX_GAUGE:		// 変動するゲージ

			// 色設定
			m_sPowerGauge[nCntGauge].pObj2D->SetColor(D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
			break;

		case VTX_FRAM:		// フレーム

			// 色設定
			m_sPowerGauge[nCntGauge].pObj2D->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			break;
		}

		// テクスチャの割り当て
		m_nTexIdx[nCntGauge] = CManager::GetTexture()->Regist(m_apTextureFile[nCntGauge]);

		// テクスチャの割り当て
		m_sPowerGauge[nCntGauge].pObj2D->BindTexture(m_nTexIdx[nCntGauge]);
	}

	// 種類設定
	SetType(CObject::TYPE_ITEM);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CPowerGauge::Uninit(void)
{
	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		// 終了処理
		m_sPowerGauge[nCntGauge].pObj2D->Uninit();
		m_sPowerGauge[nCntGauge].pObj2D = NULL;
	}

	// 開放処理
	Release();

}

//==========================================================================
// 更新処理
//==========================================================================
void CPowerGauge::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		if (nCntGauge == VTX_GAUGE)
		{// ゲージ部分だけ　

			// 減少処理
			GaugeDecrement(nCntGauge);
			ChangeColor(nCntGauge);
		}
	}

	UpdateChargeCount();

	// 現在の段階を求める
	if ((float)m_nCntCharge / (float)m_nMaxCharge >= (1.0f / (float)LEVEL_MAX) * LEVEL_1 &&
		(float)m_nCntCharge / (float)m_nMaxCharge < (1.0f / (float)LEVEL_MAX) * LEVEL_2)
	{
		m_Level = LEVEL_1;
	}
	else if (
		(float)m_nCntCharge / (float)m_nMaxCharge >= (1.0f / (float)LEVEL_MAX) * LEVEL_2 &&
		(float)m_nCntCharge / (float)m_nMaxCharge < (1.0f / (float)LEVEL_MAX) * LEVEL_3)
	{
		m_Level = LEVEL_2;
	}
	else if (
		(float)m_nCntCharge / (float)m_nMaxCharge >= (1.0f / (float)LEVEL_MAX) * LEVEL_3 &&
		(float)m_nCntCharge / (float)m_nMaxCharge < (1.0f / (float)LEVEL_MAX) * LEVEL_MAX)
	{
		m_Level = LEVEL_3;
	}
	
	if ((float)m_nCntCharge / (float)m_nMaxCharge == 1.0f || m_bFever == true)
	{
		m_Level = LEVEL_MAX;
		m_bFever = true;
	}

	// 頂点情報設定
	SetVtx();
}

//==================================================================================
// 減少処理
//==================================================================================
void CPowerGauge::GaugeDecrement(int nCntGauge)
{
	// サイズ取得
	D3DXVECTOR2 size = m_sPowerGauge[nCntGauge].pObj2D->GetSize();

	// 目標のサイズにする
	size.x += (m_sPowerGauge[nCntGauge].fWidthDest - size.x) * 0.25f;

	// サイズ設定
	m_sPowerGauge[nCntGauge].pObj2D->SetSize(size);

}

//==================================================================================
// チャージカウント更新処理
//==================================================================================
void CPowerGauge::UpdateChargeCount(void)
{
	if (m_sPowerGauge[VTX_GAUGE].pObj2D == NULL)
	{// NULLだったら
		return;
	}

	m_nCntWaitTime--;
	if (m_nCntWaitTime <= 0 || m_bFever == true)
	{
		// 現在の体力設定
		m_nCntCharge--;

		if (m_bFever == true && m_nCntCharge <= 0)
		{
			m_bFever = false;
		}
	}

	// 値の正規化処理
	ValueNormalize(m_nCntCharge, m_nMaxCharge, 0);

	// 目標の幅設定
	m_sPowerGauge[VTX_GAUGE].fWidthDest =
		m_sPowerGauge[VTX_GAUGE].fMaxWidth * ((float)m_nCntCharge / (float)m_nMaxCharge);
}

//==================================================================================
// チャージカウント設定処理
//==================================================================================
void CPowerGauge::SetChargeCount(int nValue)
{
	if (m_sPowerGauge[VTX_GAUGE].pObj2D == NULL)
	{// NULLだったら
		return;
	}

	m_nCntWaitTime = 100;

	if (m_bFever == false)
	{
		// 現在の体力設定
		m_nCntCharge += nValue;
	}
	else
	{
		m_nCntCharge += 0;
	}

	// 値の正規化処理
	ValueNormalize(m_nCntCharge, m_nMaxCharge, 0);

	//目標の幅設定
	m_sPowerGauge[VTX_GAUGE].fWidthDest =
		m_sPowerGauge[VTX_GAUGE].fMaxWidth * ((float)m_nCntCharge / (float)m_nMaxCharge);
}


//==================================================================================
// 色更新
//==================================================================================
void CPowerGauge::ChangeColor(int nCntGauge)
{
	// サイズ取得
	D3DXVECTOR2 size = m_sPowerGauge[nCntGauge].pObj2D->GetSize();

	// 色取得
	D3DXCOLOR col = m_sPowerGauge[nCntGauge].pObj2D->GetColor();

	switch (m_Level)
	{
	case CPowerGauge::LEVEL_1:
		col = D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f);
		break;

	case CPowerGauge::LEVEL_2:
		col = D3DXCOLOR(0.3f, 1.0f, 0.3f, 1.0f);
		break;

	case CPowerGauge::LEVEL_3:
		col = D3DXCOLOR(0.6f, 0.0f, 0.6f, 1.0f);
		break;

	case CPowerGauge::LEVEL_MAX:
		static int nn = 0, nCnt= 0;
		nn = (nn + 1) % 4;

		if (nn == 0)
		{
			nCnt = (nCnt + 1) % 2;
		}
		
		if(nCnt == 0)
		{
			col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			col = D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f);
		}
		break;
	}

	// 色設定
	m_sPowerGauge[nCntGauge].pObj2D->SetColor(col);

	// サイズ取得
	m_sPowerGauge[nCntGauge].pObj2D->SetSize(size);

}

//==========================================================================
// 描画処理
//==========================================================================
void CPowerGauge::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		// 描画処理
		m_sPowerGauge[nCntGauge].pObj2D->Draw();
	}

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CPowerGauge::SetVtx(void)
{
	for (int nCntGauge = 0; nCntGauge < VTX_MAX; nCntGauge++)
	{
		// 頂点設定
		m_sPowerGauge[nCntGauge].pObj2D->SetVtx();

		// 位置取得
		D3DXVECTOR3 pos = m_sPowerGauge[nCntGauge].pObj2D->GetPosition();

		// 頂点情報へのポインタ
		VERTEX_2D *pVtx;

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_sPowerGauge[nCntGauge].pObj2D->GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

		// サイズ取得
		D3DXVECTOR2 size = m_sPowerGauge[nCntGauge].pObj2D->GetSize();

		//頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(pos.x + -size.x - (m_sPowerGauge[nCntGauge].fMaxWidth - size.x),	pos.y + -size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + size.x - (m_sPowerGauge[nCntGauge].fMaxWidth - size.x),	pos.y + -size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x + -size.x - (m_sPowerGauge[nCntGauge].fMaxWidth - size.x),	pos.y + size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + size.x - (m_sPowerGauge[nCntGauge].fMaxWidth - size.x),	pos.y + size.y, 0.0f);

		// 頂点バッファをアンロックロック
		m_sPowerGauge[nCntGauge].pObj2D->GetVtxBuff()->Unlock();
	}
}

//==========================================================================
// チャージ段階取得
//==========================================================================
int CPowerGauge::GetLevel(void)
{
	return m_Level;
}
