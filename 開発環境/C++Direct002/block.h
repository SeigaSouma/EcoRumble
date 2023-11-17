//=============================================================================
//
// ブロック処理 [block.h]
// Author : 相馬靜雅
//
//=============================================================================

#ifndef _BLOCK_H_		//このマクロ定義がされていなかったら
#define _BLOCK_H_		//二重インクルード防止のマクロを定義する

#include "main.h"
#include "object2D.h"

//==========================================================================
// 前方宣言
//==========================================================================

//==========================================================================
// クラス定義
//==========================================================================
// ブロッククラス定義
class CBlock : public CObject2D
{
public:

	CBlock(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CBlock();

	static HRESULT Load(void);
	static void Unload(void);
	static CBlock *Create(void);
	static CBlock *Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 size, const int nWidth = 1, const int nHeight = 1);

	// オーバーライドされた関数
	HRESULT Init(void);
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR2 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

private:
	int m_nWidth;	// 横分割数
	int m_nHeight;	// 縦分割数
	int m_nTexIdx;	// テクスチャのインデックス番号
	int m_nAlphaCnt;	// 透明度のカウント
};

#endif