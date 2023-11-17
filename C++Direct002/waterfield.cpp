//=============================================================================
// 
//  水のフィールド処理 [waterfield.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "waterfield.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "calculation.h"
#include "player.h"
#include "impactwave.h"
#include "particle.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CWaterField::CWaterField(int nPriority) : CMeshField(nPriority)
{
	m_fTexU = 0.0f;		// Uスクロール用
	m_fTexV = 0.0f;		// Vスクロール用
	memset(&m_nTimeInWater[0], 0, sizeof(m_nTimeInWater));	// 水に入ってる時間
}

//==========================================================================
// デストラクタ
//==========================================================================
CWaterField::~CWaterField()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CWaterField::Init(void)
{
	// メッシュフィールドの初期化処理
	CMeshField::Init();

	// 種類設定
	SetType(TYPE_WATERFIELD);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CWaterField::Uninit(void)
{
	// メッシュフィールドの初期化処理
	CMeshField::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CWaterField::Update(void)
{
	// 先頭を保存
	CObject *pObj = CObject::GetTop(mylib_const::ENEMY_PRIORITY);

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();
	int nCntObj = 0;
	while (pObj != NULL)
	{// NULLが来るまで無限ループ

		// 次のオブジェクトを一時保存
		CObject *pObjNext = pObj->GetNext();

		// 死亡の判定
		if (pObj->IsDeath() == true)
		{// 死亡フラグが立っていたら

			// 次のオブジェクトを代入
			pObj = pObjNext;
			continue;
		}

		if (pObj == CObject::GetObject())
		{// 自分と同じだったら

			// 次のオブジェクトを代入
			pObj = pObjNext;
			continue;
		}

		// 種類の取得
		if (pObj->GetType() != CObject::TYPE_ENEMY)
		{// 敵じゃなかったら

			if (pObj->GetType() != CObject::TYPE_PLAYER)
			{// プレイヤーじゃなかったら
				// 次のオブジェクトを代入
				pObj = pObjNext;
				continue;
			}
		}

		// 対象の情報取得
		D3DXVECTOR3 PlayerPos = pObj->GetPosition();
		D3DXVECTOR3 PlayerPosOld = pObj->GetOldPosition();
		D3DXVECTOR3 PlayerRot = pObj->GetRotation();

		if (PlayerPos.y <= pos.y)
		{// 水よりも下だったら

			if (PlayerPosOld.y > pos.y)
			{// 前回は上だったら

				if (pObj->GetType() == CObject::TYPE_PLAYER)
				{// プレイヤーだったら

					CPlayer *pPlayer = CManager::GetScene()->GetPlayer();
					if (pPlayer != NULL)
					{
						pPlayer->IsSwitchInWater(true);
					}

					// サウンド再生
					CManager::GetSound()->PlaySound(CSound::LABEL_SE_WATERIN);
				}

				CImpactWave::Create
				(
					D3DXVECTOR3(PlayerPos.x, pos.y + 1.0f, PlayerPos.z),	// 位置
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// 向き
					D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.2f),			// 色
					20.0f,										// 幅
					2.0f,										// 高さ
					40,											// 寿命
					4.2f,										// 幅の移動量
					CImpactWave::TYPE_BLACK2,					// テクスチャタイプ
					true										// 加算合成するか
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(PlayerPos.x, pos.y + 1.0f, PlayerPos.z),	// 位置
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// 向き
					D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.2f),			// 色
					20.0f,										// 幅
					2.0f,										// 高さ
					45,											// 寿命
					5.0f,										// 幅の移動量
					CImpactWave::TYPE_BLACK2,					// テクスチャタイプ
					true										// 加算合成するか
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(PlayerPos.x, pos.y + 1.0f, PlayerPos.z),	// 位置
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// 向き
					D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.2f),			// 色
					20.0f,										// 幅
					2.0f,										// 高さ
					50,											// 寿命
					6.0f,										// 幅の移動量
					CImpactWave::TYPE_BLACK2,					// テクスチャタイプ
					true										// 加算合成するか
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(
						PlayerPos.x + sinf(D3DX_PI + PlayerRot.y) * 50.0f,
						pos.y + 1.0f + 60.0f,
						PlayerPos.z + cosf(D3DX_PI + PlayerRot.y) * 50.0f),	// 位置
					D3DXVECTOR3(D3DX_PI, 0.0f, 0.0f),				// 向き
					D3DXCOLOR(0.8f, 0.8f, 1.0f, 1.0f),			// 色
					20.0f,										// 幅
					60.0f,										// 高さ
					30,											// 寿命
					2.0f,										// 幅の移動量
					CImpactWave::TYPE_SMOKE,					// テクスチャタイプ
					true										// 加算合成するか
				);
			}

			// 水に入ってる時間加算
			if (pObj->GetType() == CObject::TYPE_ENEMY)
			{// 敵だったら
				m_nTimeInWater[nCntObj] = (m_nTimeInWater[nCntObj] + 1) % 30;
			}
			else if (pObj->GetType() == CObject::TYPE_PLAYER)
			{// プレイヤーだったら
				m_nTimeInWater[nCntObj] = (m_nTimeInWater[nCntObj] + 1) % 25;
			}

			// 衝撃波生成
			if (m_nTimeInWater[nCntObj] == 0)
			{
				
				CImpactWave::Create
				(
					D3DXVECTOR3(
						PlayerPos.x + sinf(D3DX_PI + PlayerRot.y) * 50.0f,
						pos.y + 1.0f,
						PlayerPos.z + cosf(D3DX_PI + PlayerRot.y) * 50.0f),	// 位置
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// 向き
					D3DXCOLOR(0.6f, 0.6f, 1.0f, 0.2f),			// 色
					20.0f,										// 幅
					2.0f,										// 高さ
					40,											// 寿命
					4.2f,										// 幅の移動量
					CImpactWave::TYPE_BLACK2,					// テクスチャタイプ
					true										// 加算合成するか
				);

				CImpactWave::Create
				(
					D3DXVECTOR3(
						PlayerPos.x + sinf(D3DX_PI + PlayerRot.y) * 50.0f,
						pos.y + 1.0f + 30.0f,
						PlayerPos.z + cosf(D3DX_PI + PlayerRot.y) * 50.0f),	// 位置
					D3DXVECTOR3(D3DX_PI, 0.0f, 0.0f),				// 向き
					D3DXCOLOR(0.8f, 0.8f, 1.0f, 1.0f),			// 色
					10.0f,										// 幅
					30.0f,										// 高さ
					30,											// 寿命
					2.0f,										// 幅の移動量
					CImpactWave::TYPE_SMOKE,					// テクスチャタイプ
					true										// 加算合成するか
				);

				// 水パーティクル生成
				my_particle::Create(D3DXVECTOR3(
					PlayerPos.x + sinf(D3DX_PI + PlayerRot.y) * 50.0f,
					pos.y + 1.0f,
					PlayerPos.z + cosf(D3DX_PI + PlayerRot.y) * 50.0f), my_particle::TYPE_WATERIN);
			}
		}
		else
		{
			m_nTimeInWater[nCntObj] = 0;

			if (pObj->GetType() == CObject::TYPE_PLAYER)
			{// プレイヤーだったら
				// サウンド停止
				CManager::GetSound()->StopSound(CSound::LABEL_SE_WATERIN);

				CPlayer *pPlayer = CManager::GetScene()->GetPlayer();
				if (pPlayer != NULL)
				{
					pPlayer->IsSwitchInWater(false);
				}
			}
		}

		// 次のオブジェクトを代入
		pObj = pObjNext;
		nCntObj++;
	}



	// スクロール
	Scroll();

	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// スクロール処理
//==========================================================================
void CWaterField::Scroll(void)
{
	m_fTexU += 0.001f;		// Uスクロール用
	m_fTexV += 0.001f;		// Vスクロール用

	if (m_fTexU >= 1.0f)
	{
		m_fTexU = 0.0f;
	}
	else if (m_fTexU <= 0.0f)
	{
		m_fTexU = 1.0f;
	}

	if (m_fTexV >= 1.0f)
	{
		m_fTexV = 0.0f;
	}
	else if (m_fTexV <= 0.0f)
	{
		m_fTexV = 1.0f;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CWaterField::Draw(void)
{
	//  デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (CManager::IsWireframe() == true)
	{
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);	// ワイヤーフレームモード
	}

	// 描画処理
	CMeshField::Draw();

	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);	// 埋めるモード
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CWaterField::SetVtx(void)
{
	// 頂点情報更新
	CMeshField::SetVtx();

	VERTEX_3D *pVtx;	// 頂点情報へのポインタ
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	int nHBlock = GetHeightBlock();
	int nWBlock = GetWidthBlock();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < nHBlock + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < nWBlock + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			// 頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2
			(
				((float)nCntWidth / (1.0f / (float)(nWBlock + 1))) * (1.0f / (float)(nWBlock + 1)) + m_fTexU,
				((float)nCntHeight / (1.0f / (float)(nHBlock + 1))) * (1.0f / (float)(nHBlock + 1)) + m_fTexV
			);
			pVtx += 1;
		}
	}

	// 頂点バッファをアンロックする
	pVtxBuff->Unlock();
}

//==========================================================================
// オブジェクトの取得
//==========================================================================
CWaterField *CWaterField::GetObject3DMesh(void)
{
	return this;
}
