//=============================================================================
// 
//  妄想のポイントヘッダー [delusion_point.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _DELUSION_POINT_H_
#define _DELUSION_POINT_H_	// 二重インクルード防止


#include "main.h"
#include "object2D.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CNumber;

//==========================================================================
// クラス定義
//==========================================================================
// 妄想のウィンドウクラス定義
class CDelusionPoint : public CObject2D
{
public:

	CDelusionPoint(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CDelusionPoint();

	static CDelusionPoint *Create(void);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetValue(int nNum);	// 値の設定

private:

	int m_nNum;					// 値
	CNumber *m_apNumber;		// 数字のオブジェクト
	int m_nTexIdx;				// テクスチャのインデックス番号
};



#endif