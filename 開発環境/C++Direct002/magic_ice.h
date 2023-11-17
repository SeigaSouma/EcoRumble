//=============================================================================
// 
//  氷魔法ヘッダー [magic_ice.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MAGIC_ICE_H_
#define _MAGIC_ICE_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObjectX;

//==========================================================================
// マクロ定義
//==========================================================================
#define MAX_MAGICICE	(320)	// 氷魔法の数

//==========================================================================
// クラス定義
//==========================================================================
// 氷魔法クラス定義
class CMagicIce : public CObject
{
public:
	CMagicIce(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CMagicIce();

	static HRESULT Load(void);
	static CMagicIce *Create(D3DXVECTOR3 pos);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

	bool Set(void);	// 設定処理
	void Collision(void);	// 当たり判定

	D3DXVECTOR3 m_posOrigin;			// 原点
	int m_nNumAll;						// 総数
	int m_nIdxBig;						// 巨大なやつのインデックス
	int m_nLife;						// 寿命
	static const char *m_pModelFile;	// モデルファイル
	int m_nWait[MAX_MAGICICE];			// 待機時間
	int m_nDirection[MAX_MAGICICE];		// 向き
	CObjectX *m_pObjX[MAX_MAGICICE];	// オブジェクトXのポインタ
	static int m_nIdxXFile;				// Xファイルのインデックス番号
};


#endif