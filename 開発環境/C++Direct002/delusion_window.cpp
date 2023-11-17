//=============================================================================
// 
//  妄想のウィンドウ処理 [delusion_window.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "delusion_window.h"
#include "input.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH	(400.0f)		// 横幅
#define HEIGHT	(200.0f)		// 縦幅
#define TEXTURE	"data\\TEXTURE\\bullet_01.png"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CDelusionWindow::m_nNumAll = 0;		// 妄想のウィンドウの総数

//==========================================================================
// コンストラクタ
//==========================================================================
CDelusionWindow::CDelusionWindow(int nPriority) : CObject2D(nPriority)
{
	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CDelusionWindow::~CDelusionWindow()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CDelusionWindow *CDelusionWindow::Create(void)
{
	// 生成用のオブジェクト
	CDelusionWindow *pItem = NULL;

	if (pItem == NULL)
	{// NULLだったら

		// メモリの確保
		pItem = DEBUG_NEW CDelusionWindow;

		if (pItem != NULL)
		{// メモリの確保が出来ていたら

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
HRESULT CDelusionWindow::Init(void)
{
	HRESULT hr;

	// 各種変数の初期化
 	SetSize(D3DXVECTOR2(0.0f, 0.0f));				// サイズ
	SetPosition(D3DXVECTOR3(640.0f, 250.0f, 0.0f));	// 位置
	SetColor(D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.8f));

	// 種類の設定
	SetType(CObject::TYPE_DELUSION_WINDOW);

	// 初期化処理
	hr = CObject2D::Init();

	if (FAILED(hr))
	{// 失敗したとき

		return E_FAIL;
	}

	// テクスチャの読み込み
	m_nTexIdx = CManager::GetTexture()->Regist(NULL);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CDelusionWindow::Uninit(void)
{

	// 終了処理
	CObject2D::Uninit();

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CDelusionWindow::Update(void)
{

	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// サイズ取得
	D3DXVECTOR2 size = GetSize();

	// 慣性補正
	InertiaCorrection(size.x, WIDTH, 0.25f);
	InertiaCorrection(size.y, HEIGHT, 0.25f);

	// サイズ設定
	SetSize(size);

	// 頂点情報設定
	SetVtx();

	// デバッグ表示
	CManager::GetDebugProc()->Print(
		"\n"
		"------------------[具現化の操作]------------------\n"
		" 要素切り替え：【←, →】\n"
		" 上昇 or 減少：【↑, ↓】\n"
		" 上：[パワー系], 右：[群体系], 左：[スピード系]\n"
		"\n");
}

//==========================================================================
// 描画処理
//==========================================================================
void CDelusionWindow::Draw(void)
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
void CDelusionWindow::SetVtx(void)
{
	// 頂点設定
	CObject2D::SetVtx();
}

//==========================================================================
// 総数取得
//==========================================================================
int CDelusionWindow::GetNumAll(void)
{
	return m_nNumAll;
}
