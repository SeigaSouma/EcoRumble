//=============================================================================
// 
//  草ヘッダー [grassblades.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _GRASSBLADES_H_
#define _GRASSBLADES_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CMeshWall;

//==========================================================================
// マクロ定義
//==========================================================================
#define MAX_GRASSBLADES	(240)	// 草の最大数

//==========================================================================
// クラス定義
//==========================================================================
// 草クラス定義
class CGrassBlades : public CObject
{
public:
	CGrassBlades(int nPriority = 7);
	~CGrassBlades();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CGrassBlades *Create(D3DXVECTOR3 pos);

private:

	// 草情報の構造体定義
	struct SGrassInfo
	{
		D3DXVECTOR3 rotDest;		// 目標の向き
		D3DXVECTOR3 windDirection;	// 風の方向
		D3DXVECTOR3 windRot;		// 風の向き
		int nCntWind;				// 風のカウンター
		int nDirection;				// 向き
		bool bHit;					// 当たってるか
		CMeshWall *pMeshWall;		// メッシュウォールのポインタ
	};

	// メンバ関数
	void Falling(int nCntObj);
	void WindFalling(int nCntObj);

	// メンバ変数
	SGrassInfo m_sGrassInfo[MAX_GRASSBLADES];	// 草情報
	static int m_nTexIdx;						// テクスチャのインデックス番号
};


#endif