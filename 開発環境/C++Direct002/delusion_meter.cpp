//=============================================================================
// 
//  妄想のメーター処理 [delusion_meter.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "delusion_meter.h"
#include "input.h"
#include "manager.h"
#include "game.h"
#include "texture.h"
#include "renderer.h"
#include "calculation.h"
#include "particle.h"
#include "player.h"
#include "delusion_point.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define VERTEX	(3)
#define LENGTH	(50.0f)		// 長さ
#define TEXTURE	"data\\TEXTURE\\block000.jpg"
#define TEXTURE_METER	"data\\TEXTURE\\bullet_01.png"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CDelusionMeter::m_nNumAll = 0;		// 妄想のメーターの総数

//==========================================================================
// コンストラクタ
//==========================================================================
CDelusionMeter::CDelusionMeter(int nPriority) : CObject2D(nPriority)
{
	memset(&m_fDestLength[0], 0, sizeof(m_fDestLength));	// 目標の長さ
	memset(&m_fLength[0], 0, sizeof(m_fLength));			// 長さ
	memset(&m_nPoint[0], 0, sizeof(m_nPoint));				// 要素ポイント
	m_nRemainPoint = 0;										// 残りポイント
	m_nChangeIdx = 0;										// 変更するインデックス番号
	m_pDelusionPoint = NULL;								// 妄想ポイント
	m_pMeterMain = NULL;									// 本来のメーター
	m_pMeter = NULL;			// 本来のメーター

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CDelusionMeter::~CDelusionMeter()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CDelusionMeter *CDelusionMeter::Create(void)
{
	// 生成用のオブジェクト
	CDelusionMeter *pItem = NULL;

	if (pItem == NULL)
	{// NULLだったら

		// メモリの確保
		pItem = DEBUG_NEW CDelusionMeter;

		if (pItem != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pItem->Init();
		}

		return pItem;
	}

	return NULL;
}
//
////==========================================================================
//// 初期化処理
////==========================================================================
//HRESULT CDelusionMeter::Init(void)
//{
//	HRESULT hr;
//
//	//*********************************
//	// メーター下地
//	//*********************************
//	m_pMeterMain = CObject2D::Create();
//
//	if (m_pMeterMain == NULL)
//	{// 失敗していたら
//		return E_FAIL;
//	}
//
//	// 種類の設定
//	//m_pMeterMain->SetType(CObject::TYPE_DELUSION_METER_MAIN);
//
//	// テクスチャの読み込み
//	int nMainTex = CManager::GetTexture()->Regist(TEXTURE_METER);
//
//	// テクスチャの割り当て
//	m_pMeterMain->BindTexture(nMainTex);
//
//	m_pMeterMain->SetSize(D3DXVECTOR2(sinf(D3DXToRadian(120)) * LENGTH * 4, sinf(D3DXToRadian(-120)) * (-LENGTH * 4)));	// サイズ
//	m_pMeterMain->SetPosition(D3DXVECTOR3(640.0f, 250.0f, 0.0f));	// 位置
//	m_pMeterMain->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));
//
//
//	//*********************************
//	// メーター部分
//	//*********************************
//	// 各種変数の初期化
// 	SetSize(D3DXVECTOR2(0.0f, 0.0f));				// サイズ
//	SetPosition(D3DXVECTOR3(640.0f, 250.0f, 0.0f));	// 位置
//	SetColor(D3DXCOLOR(1.0f, 0.6f, 0.6f, 0.8f));
//	m_nRemainPoint = 3;								// 残りポイント
//	m_nChangeIdx = TYPE_POWER;						// 変更する要素のインデックス番号
//
//	for (int nCntVtx = 0; nCntVtx < VERTEX; nCntVtx++)
//	{
//		// 目標の長さ
//		m_fDestLength[nCntVtx] = LENGTH;
//		m_nPoint[nCntVtx] = 1;	// 要素ポイント
//	}
//
//	// 種類の設定
//	SetType(CObject::TYPE_DELUSION_METER);
//
//	// 初期化処理
//	hr = CObject2D::Init(3);
//
//	if (FAILED(hr))
//	{// 失敗したとき
//
//		return E_FAIL;
//	}
//
//	// テクスチャの読み込み
//	m_nTexIdx = CManager::GetTexture()->Regist(NULL);
//
//	// テクスチャの割り当て
//	BindTexture(m_nTexIdx);
//
//	return S_OK;
//}


//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CDelusionMeter::Init(void)
{
	HRESULT hr;

	// 種類の設定
	SetType(CObject::TYPE_DELUSION_METER);

	//*********************************
	// メーター部分
	//*********************************
	m_pMeterMain = CObject2D::Create(mylib_const::DEF2D_PRIORITY, 3);

	if (m_pMeterMain == NULL)
	{// 失敗していたら
		return E_FAIL;
	}

	// テクスチャの読み込み
	int nMainTex = CManager::GetTexture()->Regist(NULL);

	// テクスチャの割り当て
	m_pMeterMain->BindTexture(nMainTex);

	// 各種変数の初期化
	m_pMeterMain->SetSize(D3DXVECTOR2(0.0f, 0.0f));				// サイズ
	m_pMeterMain->SetPosition(D3DXVECTOR3(640.0f, 250.0f, 0.0f));	// 位置
	m_pMeterMain->SetColor(D3DXCOLOR(1.0f, 0.6f, 0.6f, 0.8f));
	m_nRemainPoint = 3;								// 残りポイント
	m_nChangeIdx = TYPE_POWER;						// 変更する要素のインデックス番号

	for (int nCntVtx = 0; nCntVtx < VERTEX; nCntVtx++)
	{
		// 目標の長さ
		m_fDestLength[nCntVtx] = LENGTH;
		m_nPoint[nCntVtx] = 1;	// 要素ポイント
	}


	//*********************************
	// メーター下地
	//*********************************
	// 初期化処理
	m_pMeter = m_pMeter->Create(mylib_const::DEF2D_PRIORITY);

	// 各種変数の初期化
	m_pMeter->SetSize(D3DXVECTOR2(sinf(D3DXToRadian(120)) * LENGTH * 4, sinf(D3DXToRadian(-120)) * (-LENGTH * 4)));	// サイズ
	m_pMeter->SetPosition(D3DXVECTOR3(640.0f, 250.0f, 0.0f));	// 位置
	m_pMeter->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));

	//if (FAILED(hr))
	//{// 失敗したとき

	//	return E_FAIL;
	//}

	// テクスチャの読み込み
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// テクスチャの割り当て
	m_pMeter->BindTexture(m_nTexIdx);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CDelusionMeter::Uninit(void)
{
	// 妄想ポイント
	if (m_pDelusionPoint != NULL)
	{
		m_pDelusionPoint->Uninit();
		m_pDelusionPoint = NULL;
	}

	if (m_pMeterMain != NULL)
	{// 情報が入っていたら

		// 終了処理
		m_pMeterMain->Uninit();
		m_pMeterMain = NULL;
	}

	if (m_pMeter != NULL)
	{// 情報が入っていたら

		// 終了処理
		m_pMeter->Uninit();
		m_pMeter = NULL;
	}

	// 終了処理
	CObject2D::Uninit();

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CDelusionMeter::Update(void)
{
/*
	CObject *pObj = CObject::GetObject();
	CObject *pObjNext = GetNext();
	CObject *pObjMainNext = m_pMeterMain->GetNext();*/

	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 妄想ポイント
	if (m_pDelusionPoint == NULL)
	{
		// 妄想ポイントの生成
		m_pDelusionPoint = CDelusionPoint::Create();
	}

	// 慣性補正
	for (int nCntVtx = 0; nCntVtx < VERTEX; nCntVtx++)
	{
		// 目標の長さ
		m_fDestLength[nCntVtx] = LENGTH * m_nPoint[nCntVtx];

		// 慣性補正
		InertiaCorrection(m_fLength[nCntVtx], m_fDestLength[nCntVtx], 0.1f);
	}

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	if (pInputKeyboard->GetTrigger(DIK_SPACE) == true && m_nRemainPoint == 0)
	{// SPACEが押されたら, 削除

		// 武器チェンジ
		CManager::GetScene()->GetPlayer()->ChangeWeapon(&m_nPoint[0]);

		// 終了処理
		Uninit();
		return;
	}

	if (pInputKeyboard->GetTrigger(DIK_UP) == true && m_nRemainPoint > 0)
	{// UP

		// ポイント追加
		m_nPoint[m_nChangeIdx]++;

		// 残りポイント減らす
		m_nRemainPoint--;
	}

	if (pInputKeyboard->GetTrigger(DIK_DOWN) == true && m_nPoint[m_nChangeIdx] > 1)
	{// DOWN && 消せるだけポイント残ってる && ポイントが

		// ポイント減らす
		m_nPoint[m_nChangeIdx]--;

		// 残りポイント増やす
		m_nRemainPoint++;
	}

	if (pInputKeyboard->GetTrigger(DIK_RIGHT) == true)
	{// 変更
		m_nChangeIdx = (m_nChangeIdx + 1) % VERTEX;
	}
	if (pInputKeyboard->GetTrigger(DIK_LEFT) == true)
	{// 変更
		m_nChangeIdx = (m_nChangeIdx + (VERTEX - 1)) % VERTEX;
	}

	if (m_pDelusionPoint != NULL)
	{// 生成されていたら

		// 値の設定
		m_pDelusionPoint->SetValue(m_nRemainPoint);
	}

	// 頂点座標取得
	D3DXVECTOR3 *pVtxPos = m_pMeterMain->GetVtxPos();

	pVtxPos[0] = D3DXVECTOR3(sinf(D3DXToRadian(-120)) * m_fLength[0], cosf(D3DXToRadian(-120)) * -m_fLength[0], 0.0f);
	pVtxPos[1] = D3DXVECTOR3(sinf(D3DXToRadian(0)) * m_fLength[1], cosf(D3DXToRadian(0)) * -m_fLength[1], 0.0f);
	pVtxPos[2] = D3DXVECTOR3(sinf(D3DXToRadian(120)) * m_fLength[2], cosf(D3DXToRadian(120)) * -m_fLength[2], 0.0f);

	my_particle::Create(m_pMeterMain->GetPosition() + pVtxPos[m_nChangeIdx], my_particle::TYPE_CHECK2D);
	my_particle::Create(m_pMeterMain->GetPosition(), my_particle::TYPE_CHECK2D);

	// 頂点座標設定
	m_pMeterMain->SetVtxPos(&pVtxPos[0]);

	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CDelusionMeter::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// アルファテストを有効にする
	/*pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);*/

	//CObject2D::Draw();
	m_pMeter->Draw();
	m_pMeterMain->Draw(VERTEX);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CDelusionMeter::SetVtx(void)
{
	// 頂点設定
	m_pMeter->SetVtx();
	m_pMeterMain->SetVtx(VERTEX);
}

//==========================================================================
// 総数取得
//==========================================================================
int CDelusionMeter::GetNumAll(void)
{
	return m_nNumAll;
}
