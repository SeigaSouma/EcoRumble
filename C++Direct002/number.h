//=============================================================================
// 
//  数字ヘッダー [number.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _NUMBER_H_
#define _NUMBER_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObject2D;

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CNumber
{
public:

	CNumber(int nPriority = 6);
	~CNumber();

	static CNumber *Create(void);

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Release(void);	// 開放処理
	CObject2D *GetObject2D(void);

private:
	CObject2D *m_aObject2D;	// オブジェクト2Dのオブジェクト
};

#endif