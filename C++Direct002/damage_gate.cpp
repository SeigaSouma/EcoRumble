//=============================================================================
// 
//  ダメージゲート処理 [damage_gate.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "damage_gate.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "calculation.h"
#include "objectX.h"
#include "objectchara.h"
#include "3D_Effect.h"
#include "particle.h"
#include "calculation.h"

#include "input.h"
#include "power_gauge.h"
//==========================================================================
// マクロ定義
//==========================================================================
#define MODELFILE	"data\\MODEL\\damage_gate.x"	// モデルファイル
#define LENGTH		(2500.0f)						// 原点からの距離
#define ROT_MOVE	(D3DX_PI * 0.001f)				// 回転移動量

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CDamageGate::m_nIdxXFile = 0;			// Xファイルのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CDamageGate::CDamageGate(int nPriority) : CObject(nPriority)
{
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 原点
	m_rotOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
	m_fLength = 0.0f;								// 原点からの距離
	memset(&m_pObjX[0], NULL, sizeof(m_pObjX));				// オブジェクトXのポインタ
}

//==========================================================================
// デストラクタ
//==========================================================================
CDamageGate::~CDamageGate()
{
	
}

//==========================================================================
// ロード処理
//==========================================================================
HRESULT CDamageGate::Load(void)
{
	// Xファイルのデータ取得
	CXLoad *pXLoad = CScene::GetXLoad();

	// Xファイルのロード
	m_nIdxXFile = pXLoad->XLoad(MODELFILE);

	if (m_nIdxXFile <= 0)
	{// 失敗していたら
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 生成処理
//==========================================================================
CDamageGate *CDamageGate::Create(void)
{
	// 生成用のオブジェクト
	CDamageGate *pBallast = NULL;

	if (pBallast == NULL)
	{// NULLだったら

		// メモリの確保
		pBallast = DEBUG_NEW CDamageGate;

		if (pBallast != NULL)
		{// メモリの確保が出来ていたら

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
HRESULT CDamageGate::Init(void)
{
	// 種類の設定
	SetType(TYPE_MAGIC_ICE);

	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 原点
	m_fLength = LENGTH;								// 原点からの距離

	float fRot = (D3DX_PI * 2.0f) / (float)MAX_DAMAGEGATE;

	for (int nCntGate = 0; nCntGate < MAX_DAMAGEGATE; nCntGate++)
	{// それぞれの更新

		// 生成処理
		m_pObjX[nCntGate] = CObjectX::Create(
			m_nIdxXFile,
			D3DXVECTOR3(m_posOrigin.x + sinf(fRot * nCntGate) * m_fLength, m_posOrigin.y, m_posOrigin.z + cosf(fRot * nCntGate) * m_fLength),
			D3DXVECTOR3(0.0f, 0.0f, 0.0f), false);

		if (m_pObjX[nCntGate] == NULL)
		{// 失敗したとき

			delete m_pObjX[nCntGate];
			m_pObjX[nCntGate] = NULL;
			return false;
		}
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CDamageGate::Uninit(void)
{
	for (int nCntGate = 0; nCntGate < MAX_DAMAGEGATE; nCntGate++)
	{
		if (m_pObjX[nCntGate] != NULL)
		{// NULLじゃなかったら

			// オブジェクトXの終了処理
			m_pObjX[nCntGate]->Uninit();
			m_pObjX[nCntGate] = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CDamageGate::Update(void)
{
	// 1つあたりの角度
	float fRot = (D3DX_PI * 2.0f) / (float)MAX_DAMAGEGATE;

	// 回転させる
	m_rotOrigin.y += ROT_MOVE;

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	if (pInputKeyboard->GetPress(DIK_UP) == true)
	{// 攻撃
		m_fLength += 5.0f;
	}
	if (pInputKeyboard->GetPress(DIK_DOWN) == true)
	{// 攻撃
		m_fLength -= 5.0f;
	}


	// 値の正規化処理
	ValueNormalize(m_fLength, LENGTH, 0.0f);


	for (int nCntGate = 0; nCntGate < MAX_DAMAGEGATE; nCntGate++)
	{// それぞれの更新

		if (m_pObjX[nCntGate] == NULL)
		{// NULLだったら
			continue;
		}

		// 位置取得
		D3DXVECTOR3 pos = m_pObjX[nCntGate]->GetPosition();

		// 移動量取得
		D3DXVECTOR3 move = m_pObjX[nCntGate]->GetMove();

		// 向き取得
		D3DXVECTOR3 rot = m_pObjX[nCntGate]->GetRotation();

		// 位置更新
		pos = D3DXVECTOR3(m_posOrigin.x + sinf(m_rotOrigin.y + fRot * nCntGate) * m_fLength, m_posOrigin.y, m_posOrigin.z + cosf(m_rotOrigin.y + fRot * nCntGate) * m_fLength);

		// 向き更新
		rot.y = fRot * nCntGate + m_rotOrigin.y;

		// 向き設定
		m_pObjX[nCntGate]->SetRotation(rot);

		// 位置設定
		m_pObjX[nCntGate]->SetPosition(pos);

		// 当たり判定
		Collision(nCntGate);
	}

}

//==========================================================================
// 当たり判定
//==========================================================================
void CDamageGate::Collision(int nCntGate)
{
	// 位置取得
	D3DXVECTOR3 pos = m_pObjX[nCntGate]->GetPosition();

	// 向き取得
	D3DXVECTOR3 rot = m_pObjX[nCntGate]->GetRotation();

	// 最大値取得
	D3DXVECTOR3 vtxMax = m_pObjX[nCntGate]->GetVtxMax();

	float fLength = sqrtf(vtxMax.x * vtxMax.x + vtxMax.z * vtxMax.z);	// 対角線の長さ
	float fAngle = atan2f(vtxMax.x, vtxMax.z);							// 対角線の向き

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

			if (pObj->GetType() != CObject::TYPE_BOSS)
			{// 敵じゃなかったら

				// 次のオブジェクトを代入
				pObj = pObjNext;
				continue;
			}
		}

		// 判定する四角
		float fLen = fLength + pObj->GetObjectChara()->GetRadius();
		D3DXVECTOR3 LeftUp = D3DXVECTOR3(pos.x + sinf(rot.y - fAngle) * fLen, pos.y, pos.z + cosf(rot.y - fAngle) * fLen);
		D3DXVECTOR3 RightUp = D3DXVECTOR3(pos.x + sinf(rot.y + fAngle) * fLen, pos.y, pos.z + cosf(rot.y + fAngle) * fLen);
		D3DXVECTOR3 LeftDown = D3DXVECTOR3(pos.x + sinf(rot.y - D3DX_PI + fAngle) * fLen, pos.y, pos.z + cosf(rot.y - D3DX_PI + fAngle) * fLen);
		D3DXVECTOR3 RightDown = D3DXVECTOR3(pos.x + sinf(rot.y + D3DX_PI - fAngle) * fLen, pos.y, pos.z + cosf(rot.y + D3DX_PI - fAngle) * fLen);

		// 敵の位置取得
		D3DXVECTOR3 TargetPos = pObj->GetPosition();

		// 判定サイズ取得
		float fRadius = pObj->GetObjectChara()->GetRadius();

		// 四角の判定
		bool bHit = CollisionSquare(LeftUp, RightUp, LeftDown, RightDown, TargetPos);

		if (bHit == false)
		{// 当たっていたら

			// 次のオブジェクトを代入
			pObj = pObjNext;
			continue;
		}

		if (pObj->Hit(10) == true)
		{// 死んでたら

			// パーティクル生成
			my_particle::Create(TargetPos, my_particle::TYPE_MAGIC_EXPLOSION);
			my_particle::Create(D3DXVECTOR3(TargetPos.x, TargetPos.y + 50.0f, TargetPos.z), my_particle::TYPE_EXPLOSION_START);
			CGame::GetPowerGauge()->SetChargeCount(5 * 6);
		}

		// 次のオブジェクトを代入
		pObj = pObjNext;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CDamageGate::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCntGate = 0; nCntGate < MAX_DAMAGEGATE; nCntGate++)
	{
		if (m_pObjX[nCntGate] != NULL)
		{// NULLじゃなかったら

			// 描画処理
			m_pObjX[nCntGate]->Draw();
		}
	}

}
