//=============================================================================
// 
//  混乱エフェクト処理 [confusion_effect.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "confusion_effect.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "calculation.h"
#include "objectBillboard.h"
#include "texture.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define LENGTH		(2500.0f)			// 原点からの距離
#define ROT_MOVE	(D3DX_PI * 0.01f)	// 回転移動量
#define TEXTURE		"data\\TEXTURE\\confusion_01.png"
#define WIDTH		(40.0f)
#define HEIGHT		(20.0f)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CConfusionEffect::m_nTexIdx = 0;	// テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CConfusionEffect::CConfusionEffect(int nPriority) : CObject(nPriority)
{
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 原点
	m_rotOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
	m_fDistance = 0.0f;								// 原点からの距離
	m_nLife = 0;									// 体力
	memset(&m_pObjBillboard[0], NULL, sizeof(m_pObjBillboard));				// ビルボードのポインタ
}

//==========================================================================
// デストラクタ
//==========================================================================
CConfusionEffect::~CConfusionEffect()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CConfusionEffect *CConfusionEffect::Create(D3DXVECTOR3 pos, float fDistance, int nLife)
{
	// 生成用のオブジェクト
	CConfusionEffect *pBallast = NULL;

	if (pBallast == NULL)
	{// NULLだったら

		// メモリの確保
		pBallast = DEBUG_NEW CConfusionEffect;

		if (pBallast != NULL)
		{// メモリの確保が出来ていたら

			// 引数代入
			pBallast->m_fDistance = fDistance;	// 間隔
			pBallast->m_posOrigin = pos;		// 原点
			pBallast->SetPosition(pos);			// 原点
			pBallast->m_nLife = nLife;			// 体力

			// 初期化処理
			pBallast->Init();
		}
		else
		{
			return NULL;
		}

		return pBallast;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CConfusionEffect::Init(void)
{

	// テクスチャの割り当て
	if (m_nTexIdx == 0)
	{
		m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);
	}

	// 種類の設定
	SetType(TYPE_CONFUSIONEFFECT);

	float fRot = (D3DX_PI * 2.0f) / (float)MAX_CONFUSION;

	for (int nCntGate = 0; nCntGate < MAX_CONFUSION; nCntGate++)
	{// それぞれの更新

		// 生成処理
		m_pObjBillboard[nCntGate] = CObjectBillboard::Create(
			D3DXVECTOR3(
				m_posOrigin.x + sinf(fRot * nCntGate) * m_fDistance,
				m_posOrigin.y,
				m_posOrigin.z + cosf(fRot * nCntGate) * m_fDistance),
			D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		if (m_pObjBillboard[nCntGate] == NULL)
		{// 失敗したとき

			delete m_pObjBillboard[nCntGate];
			m_pObjBillboard[nCntGate] = NULL;
			return false;
		}

		// テクスチャの割り当て
		m_pObjBillboard[nCntGate]->BindTexture(m_nTexIdx);

		// サイズ設定
		m_pObjBillboard[nCntGate]->SetSize(D3DXVECTOR2(WIDTH, HEIGHT));
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CConfusionEffect::Uninit(void)
{
	for (int nCntGate = 0; nCntGate < MAX_CONFUSION; nCntGate++)
	{
		if (m_pObjBillboard[nCntGate] != NULL)
		{// NULLじゃなかったら

			// ビルボードの終了処理
			m_pObjBillboard[nCntGate]->Uninit();
			m_pObjBillboard[nCntGate] = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CConfusionEffect::Update(void)
{
	// 1つあたりの角度
	float fRot = (D3DX_PI * 2.0f) / (float)MAX_CONFUSION;

	// 回転させる
	m_rotOrigin.y += ROT_MOVE;
	RotNormalize(m_rotOrigin.y);

	//// 体力
	//m_nLife--;

	//if (m_nLife <= 0)
	//{// 寿命なくなったら
	//	Uninit();
	//	return;
	//}

	// 元の位置
	D3DXVECTOR3 posParent = GetPosition();

	for (int nCntGate = 0; nCntGate < MAX_CONFUSION; nCntGate++)
	{// それぞれの更新

		if (m_pObjBillboard[nCntGate] == NULL)
		{// NULLだったら
			continue;
		}

		// 位置取得
		D3DXVECTOR3 pos = m_pObjBillboard[nCntGate]->GetPosition();

		// 位置更新
		pos = D3DXVECTOR3(
			posParent.x + sinf(m_rotOrigin.y + fRot * nCntGate) * m_fDistance,
			posParent.y,
			posParent.z + cosf(m_rotOrigin.y + fRot * nCntGate) * m_fDistance);

		// 位置設定
		m_pObjBillboard[nCntGate]->SetPosition(pos);

		// 頂点座標更新
		m_pObjBillboard[nCntGate]->SetVtx();
	}

}

//==========================================================================
// 位置更新
//==========================================================================
void CConfusionEffect::UpdatePosition(D3DXVECTOR3 pos)
{
	// 元の位置
	D3DXVECTOR3 posParent = GetPosition();

	// 位置更新
	posParent.x = pos.x;
	posParent.z = pos.z;
	posParent.y = m_posOrigin.y + pos.y;

	// 位置設定
	SetPosition(posParent);
}

//==========================================================================
// 描画処理
//==========================================================================
void CConfusionEffect::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	for (int nCntGate = 0; nCntGate < MAX_CONFUSION; nCntGate++)
	{
		if (m_pObjBillboard[nCntGate] == NULL)
		{// NULLだったら
			continue;
		}

		// 描画処理
		m_pObjBillboard[nCntGate]->Draw();
	}

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

}
