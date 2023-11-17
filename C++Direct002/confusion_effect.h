//=============================================================================
// 
//  混乱エフェクトヘッダー [confusion_effect.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _CONFUSION_EFFECT_H_
#define _CONFUSION_EFFECT_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObjectBillboard;

//==========================================================================
// マクロ定義
//==========================================================================
#define MAX_CONFUSION	(4)	// 混乱エフェクトの数

//==========================================================================
// クラス定義
//==========================================================================
// 混乱エフェクトクラス定義
class CConfusionEffect : public CObject
{
public:
	CConfusionEffect(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CConfusionEffect();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void UpdatePosition(D3DXVECTOR3 pos);
	static CConfusionEffect *Create(D3DXVECTOR3 pos, float fDistance, int nLife);
private:

	D3DXVECTOR3 m_posOrigin;			// 原点
	D3DXVECTOR3 m_rotOrigin;			// 向き
	float m_fDistance;					// 原点からの距離
	int m_nLife;						// 体力
	CObjectBillboard *m_pObjBillboard[MAX_CONFUSION];	// ビルボードのポインタ
	static int m_nTexIdx;				// テクスチャのインデックス番号
};


#endif