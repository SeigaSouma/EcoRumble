//=============================================================================
// 
//  テクスチャ処理 [texture.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "objectX.h"

// 読み込むテクスチャのヘッダー
#include "bg.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "explosion.h"
#include "2D_effect.h"
#include "3D_effect.h"
#include "particle.h"
#include "score.h"
#include "timer.h"
#include "block.h"
#include "item.h"
#include "field.h"
#include "hp_gauge.h"
#include "meshfield.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CTexture::m_nNumAll = 0;	// 総数

//==========================================================================
// コンストラクタ
//==========================================================================
CTexture::CTexture()
{
	for (int nCntData = 0; nCntData < mylib_const::MAX_OBJ; nCntData++)
	{
		m_pTexInfo[nCntData].nFileNameLen = 0;	// ファイル名の文字数
		m_pTexInfo[nCntData].pTexture = NULL;		// テクスチャのポインタ
		memset(&m_pTexInfo[nCntData].imageInfo, 0, sizeof(D3DXIMAGE_INFO));	// テクスチャ素材情報

		for (int nCntFile = 0; nCntFile < mylib_const::MAX_STRING; nCntFile++)
		{
			m_pTexInfo[nCntData].acFilename[nCntFile] = NULL;	// ファイル名
		}
	}
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CTexture::~CTexture()
{

}

//==========================================================================
// テクスチャの読み込み
//==========================================================================
HRESULT CTexture::LoadAll(void)
{
#if 0
	////**********************
	//// 2Dオブジェクト
	////**********************
	//// 背景
	//if (FAILED(CBG::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	//// 敵
	//if (FAILED(CEnemy::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	//// 弾
	//if (FAILED(CBullet::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	//// 爆発
	//if (FAILED(CExplosion::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	//// 2Dエフェクト
	//if (FAILED(CEffect2D::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	//// 3Dエフェクト
	//if (FAILED(CEffect3D::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	//// スコア
	//if (FAILED(CScore::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	//// タイマー
	//if (FAILED(CTimer::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	//// ブロック
	//if (FAILED(CBlock::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	//// アイテム
	//if (FAILED(CItem::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	//// テクスチャ読み込み
	//if (FAILED(CPlayer::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}


	////**********************
	//// 3Dオブジェクト
	////**********************
	//// フィールド
	//if (FAILED(CField::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}

	//// メッシュフィールド
	//if (FAILED(CMeshField::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}


	////**********************
	//// ビルボードオブジェクト
	////**********************
	//// 体力ゲージ
	//if (FAILED(CHP_Gauge::Load()))
	//{// 失敗していたら
	//	return E_FAIL;
	//}
#endif
	return S_OK;
}

//==========================================================================
// 既定テクスチャの読み込み
//==========================================================================
HRESULT CTexture::Load(void)
{
	return S_OK;
}

//==========================================================================
// 全てのテクスチャの破棄
//==========================================================================
void CTexture::Unload(void)
{
	for (int nCntData = 0; nCntData < mylib_const::MAX_OBJ; nCntData++)
	{
		// テクスチャの破棄
		if (m_pTexInfo[nCntData].pTexture != NULL)
		{
			m_pTexInfo[nCntData].pTexture->Release();
			m_pTexInfo[nCntData].pTexture = NULL;
		}
	}
}

//==========================================================================
// テクスチャの割り当て処理
//==========================================================================
int CTexture::Regist(const char *pFileName)
{
	// 最大数取得
	int nNumAll = GetNumAll() + 1;
	int nIdx = -1;	// 今回の番号保存
	int nNowLen = 0;	// 今回のファイル名長さ

	if (pFileName != NULL)
	{
		nNowLen = strlen(pFileName);
	}

	for (int nCntData = 0; nCntData < nNumAll; nCntData++)
	{
		if (pFileName != NULL)
		{// NULLじゃなかったら

			if (m_pTexInfo[nCntData].nFileNameLen == nNowLen)
			{// ファイル名の長さが同じだったら

				// 既にテクスチャが読み込まれてないかの最終確認
				if (strcmp(m_pTexInfo[nCntData].acFilename, pFileName) == 0)
				{// ファイル名が一致している

					// 番号割り当て
					nIdx = nCntData;

					return nIdx;
				}
			}
		}
	}

	if (pFileName != NULL)
	{// NULLじゃなかったら

		// テクスチャ読み込み
		HRESULT hr = LoadTex(pFileName);

		if (FAILED(hr))
		{// 失敗していたら
			return 0;
		}

		// 番号割り当て
		nIdx = nNumAll - 1;
		return nIdx;
	}

	return 0;
}

//==========================================================================
// テクスチャの読み込み処理
//==========================================================================
HRESULT CTexture::LoadTex(const char *pFileName)
{
	HRESULT hr;
	int nIdx = m_nNumAll;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevive = CManager::GetRenderer()->GetDevice();

	// テクスチャの読み込み
	hr = D3DXCreateTextureFromFile(pDevive,
		pFileName,
		&m_pTexInfo[nIdx].pTexture);

	if (hr == D3DXERR_INVALIDDATA)
	{// 失敗したとき
		return E_FAIL;
	}

	// テクスチャ素材情報
	D3DXGetImageInfoFromFile(pFileName, &m_pTexInfo[nIdx].imageInfo);

	// ファイル名と長さ保存
	strcpy(m_pTexInfo[nIdx].acFilename, pFileName);
	m_pTexInfo[nIdx].nFileNameLen = strlen(&m_pTexInfo[nIdx].acFilename[0]);

	// 総数加算
	m_nNumAll++;

	return S_OK;
}

//==========================================================================
// テクスチャのアドレス取得
//==========================================================================
LPDIRECT3DTEXTURE9 CTexture::GetAdress(int nIdx)
{
	return m_pTexInfo[nIdx].pTexture;
}

//==========================================================================
// テクスチャの総数取得
//==========================================================================
int CTexture::GetNumAll(void)
{
	return m_nNumAll;
}

//==========================================================================
// テクスチャ情報取得
//==========================================================================
CTexture::STexture CTexture::GetTextureInfo(const char *pFileName)
{
	// 最大数取得
	int nNumAll = GetNumAll() + 1;

	for (int nCntData = 0; nCntData < nNumAll; nCntData++)
	{
		if (pFileName != NULL)
		{// NULLじゃなかったら

			if (m_pTexInfo[nCntData].nFileNameLen == strlen(pFileName))
			{// ファイル名の長さが同じだったら

				// 既にテクスチャが読み込まれてないかの最終確認
				if (strcmp(m_pTexInfo[nCntData].acFilename, pFileName) == 0)
				{// ファイル名が一致している

					return m_pTexInfo[nCntData];
				}
			}
		}
	}

	return m_pTexInfo[nNumAll + 1];
}

//==========================================================================
// テクスチャ情報取得
//==========================================================================
CTexture::STexture CTexture::GetTextureInfo(int nIdxTex)
{
	return m_pTexInfo[nIdxTex];
}

//==========================================================================
// テクスチャ素材のサイズ取得
//==========================================================================
D3DXVECTOR2 CTexture::GetImageSize(int nIdx)
{
	return D3DXVECTOR2((float)m_pTexInfo[nIdx].imageInfo.Width, (float)m_pTexInfo[nIdx].imageInfo.Height);
}
