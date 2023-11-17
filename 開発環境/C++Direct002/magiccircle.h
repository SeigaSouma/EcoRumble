//=============================================================================
// 
//  魔法陣ヘッダー [magiccircle.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MAGICCIRCLE_H_
#define _MAGICCIRCLE_H_	// 二重インクルード防止

#include "main.h"
#include "object3D.h"

//==========================================================================
// クラス定義
//==========================================================================
// 弾クラス定義
class CMagicCircle : public CObject3D
{
public:

	CMagicCircle(int nPriority = 5);
	~CMagicCircle();

	static CMagicCircle *Create(void);
	static CMagicCircle *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, D3DXVECTOR3 size);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static int GetNumAll(void);

private:
	D3DXVECTOR3 m_sizeDest;		// 目標のサイズ
	static int m_nNumAll;		// 総数
	int m_nTexIdx;				// テクスチャのインデックス番号
};


#endif