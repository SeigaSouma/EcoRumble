//=============================================================================
// 
//  妄想のウィンドウヘッダー [delusion_window.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _DELUSION_WINDOW_H_
#define _DELUSION_WINDOW_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"


//==========================================================================
// クラス定義
//==========================================================================
// 妄想のウィンドウクラス定義
class CDelusionWindow : public CObject2D
{
public:

	CDelusionWindow(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CDelusionWindow();

	static CDelusionWindow *Create(void);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static int GetNumAll(void);

private:

	static int m_nNumAll;			// 妄想のウィンドウの総数
	int m_nTexIdx;					// テクスチャのインデックス番号
};


#endif