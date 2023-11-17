//=============================================================================
// 
//  爆発球処理 [explosion_sphere.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "explosion_sphere.h"
#include "game.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "input.h"
#include "objectchara.h"
#include "calculation.h"
#include "particle.h"
#include "debugproc.h"
#include "impactwave.h"
#include "elevation.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\image.png"	// テクスチャ
#define POS_MESHDOME	(0.0f)
#define LIFE			(90)
#define ALPHALIFE		(60)
#define IMPACT_DISTANCE	(100.0f)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CExplosionSphere::CExplosionSphere(int nPriority) : CObject3DMesh(nPriority)
{
	m_nTexIdx = 0;		// テクスチャのインデックス番号
	m_nLife = 0;		// 寿命
	m_fSizeDest = 0.0f;	// 目標のサイズ
}

//==========================================================================
// デストラクタ
//==========================================================================
CExplosionSphere::~CExplosionSphere()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CExplosionSphere *CExplosionSphere::Create(D3DXVECTOR3 pos, float fSizeDest)
{
	// 生成用のオブジェクト
	CExplosionSphere *pObjMeshField = NULL;

	if (pObjMeshField == NULL)
	{// NULLだったら

		// メモリの確保
		pObjMeshField = DEBUG_NEW CExplosionSphere();

		//if (pObjMeshField->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pObjMeshField;
		//	return NULL;
		//}

		if (pObjMeshField != NULL)
		{// メモリの確保が出来ていたら

			// テクスチャの割り当て
			pObjMeshField->m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

			// テクスチャの割り当て
			pObjMeshField->BindTexture(pObjMeshField->m_nTexIdx);

			// 位置・向き
			pObjMeshField->SetPosition(pos);
			pObjMeshField->SetWidthBlock(16);
			pObjMeshField->SetHeightBlock(16);
			pObjMeshField->SetWidthLen(POS_MESHDOME);
			pObjMeshField->SetHeightLen(POS_MESHDOME);
			pObjMeshField->m_fSizeDest = fSizeDest;

			// 初期化処理
			pObjMeshField->Init();
		}

		return pObjMeshField;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CExplosionSphere::Init(void)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 種類設定
	SetType(TYPE_EXPLOSIONSPHERE);

	// 寿命
	m_nLife = LIFE;

	// オブジェクト3Dメッシュの初期化処理
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_SPHERE);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CExplosionSphere::Uninit(void)
{
	////delete m_pVtxPos;
	//m_pVtxPos = NULL;

	// 終了処理
	CObject3DMesh::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CExplosionSphere::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 色取得
	D3DXCOLOR col = GetColor();

	// 横長さ取得
	float fWidthLen = GetWidthLen();

	// 縦長さ取得
	float fHeightLen = GetHeightLen();

	// 回転
	rot.y += D3DX_PI * 0.0025f;

	// 角度の正規化
	RotNormalize(rot.y);

	// 寿命を減らす
	m_nLife--;

	if (m_nLife <= LIFE)
	{// 規定値を下回ったら

		// 不透明度更新
		col.a = (float)m_nLife / (float)ALPHALIFE;
	}

	// 慣性補正
	if (m_nLife <= LIFE - 10)
	{
		InertiaCorrection(fWidthLen, m_fSizeDest, 0.2f);
		InertiaCorrection(fHeightLen, m_fSizeDest, 0.2f);
	}

	// 衝撃波生成
	if (m_nLife == LIFE - 6 ||
		m_nLife == LIFE - 12)
	{
		CImpactWave::Create
		(
			D3DXVECTOR3(pos.x, pos.y - (m_fSizeDest * 0.5f - 20.0f), pos.z),	// 位置
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// 向き
			D3DXCOLOR(1.0f, 0.4f, 0.1f, 0.4f),			// 色
			60.0f,										// 幅
			10.0f,										// 高さ
			30,											// 寿命
			20.0f,										// 幅の移動量
			CImpactWave::TYPE_BLACK2,					// テクスチャタイプ
			true
		);
	}

	// パーティクル生成
	if (m_nLife >= 40)
	{
		int nRand = (int)m_fSizeDest;
		// パーティクル
		my_particle::Create(
			D3DXVECTOR3(pos.x + Random(-nRand, nRand),
				pos.y + Random(-nRand, nRand),
				pos.z + Random(-nRand, nRand)), my_particle::TYPE_EXPLOSION_REMAIN);

		pos.y -= fHeightLen;
		my_particle::Create(pos, my_particle::TYPE_EXPLOSION_REMAIN);
	}

	// 長さ設定
	SetWidthLen(fWidthLen);
	SetHeightLen(fHeightLen);

	// 向き設定
	SetRotation(rot);

	// 色設定
	SetColor(col);

	if (m_nLife == 80)
	{
		// 当たり判定
		//CManager::GetObject3DMesh()->Damage(pos, m_fSizeDest * 2.5f, -150.0f, CElevation::DAMAGE_RATIO);

		// 早いやつ
		CGame::GetObject3DMesh()->SetWave
		(
			pos,												// 位置
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// 向き
			D3DXVECTOR3(0.0f, 0.0f, 0.0f),						// 移動量
			D3DXVECTOR2(m_fSizeDest * 6.0f, m_fSizeDest * 6.0f),									// 判定範囲
			CGame::GetObject3DMesh()->GetWidthLen() * 3.0f,	// 波発生の頻度
			80.0f,												// 波の高さ
			D3DX_PI * 0.05f,									// 波打つ頻度
			60 * 1,												// 発生時間
			CElevation::WAVETYPE_RIPPLE,						// 種類
			CElevation::TYPE_PLAYER,							// プレイヤーの種類
			1
		);


		//CManager::GetObject3DMesh()->Damage(pos, m_fSizeDest * 2.5f, -300.0f, CElevation::DAMAGE_RATIO);


		//CManager::GetObject3DMesh()->SetWave(pos, m_fSizeDest * 6.0f, CManager::GetObject3DMesh()->GetWidthLen() * 8.0f, 80.0f, D3DX_PI * 0.025f, 40);


		//CManager::GetObject3DMesh()->SetWave(pos, m_fSizeDest * 6.0f, CManager::GetObject3DMesh()->GetWidthLen() * 3.0f, 80.0f, D3DX_PI * 0.025f, 100);



		//CManager::GetObject3DMesh()->SetWave(pos, m_fSizeDest * 8.0f, CManager::GetObject3DMesh()->GetWidthLen() * 4.0f, -100.0f, D3DX_PI * 0.025f, 60);
		//CManager::GetObject3DMesh()->SetWave(pos, m_fSizeDest * 8.0f, CManager::GetObject3DMesh()->GetWidthLen() * 8.0f, -100.0f, D3DX_PI * 0.025f, 60);
		Collision();
	}


	// 頂点情報設定
	SetVtx();

	if (m_nLife <= 0)
	{// 寿命が尽きたら
		Uninit();
	}
}


//==========================================================================
// 当たり判定
//==========================================================================
void CExplosionSphere::Collision(void)
{
#if TOPCUR

	// 先頭を保存
	CObject *pObj = CObject::GetTop(mylib_const::ENEMY_PRIORITY);

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

		// 種類の取得
		if (pObj->GetType() != CObject::TYPE_ENEMY)
		{// 敵じゃなかったら

			// 次のオブジェクトを代入
			pObj = pObjNext;
			continue;
		}

		// 位置取得
		D3DXVECTOR3 pos = GetPosition();

		// 敵の位置取得
		D3DXVECTOR3 TargetPos = pObj->GetPosition();

		// 判定サイズ取得
		float fRadius = pObj->GetObjectChara()->GetRadius();

		if (SphereRange(pos, TargetPos, m_fSizeDest * 2.0f, fRadius))
		{
			if (pObj->Hit(3) == true)
			{// 死んでたら

				// パーティクル生成
				my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
			}
		}

		// 次のオブジェクトを代入
		pObj = pObjNext;
	}

#endif
}

//==========================================================================
// 描画処理
//==========================================================================
void CExplosionSphere::Draw(void)
{
	//  デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 描画処理
	CObject3DMesh::Draw();

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CExplosionSphere::SetVtx(void)
{
	// 頂点情報更新
	CObject3DMesh::SetVtx();
}
