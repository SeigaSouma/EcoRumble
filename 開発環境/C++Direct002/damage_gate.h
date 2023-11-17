//=============================================================================
// 
//  ダメージゲートヘッダー [damage_gate.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _DAMAGE_GATE_H_
#define _DAMAGE_GATE_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObjectX;

//==========================================================================
// マクロ定義
//==========================================================================
#define MAX_DAMAGEGATE	(6)	// 氷魔法の数

//==========================================================================
// クラス定義
//==========================================================================
// 氷魔法クラス定義
class CDamageGate : public CObject
{
public:
	CDamageGate(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CDamageGate();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static HRESULT Load(void);
	static CDamageGate *Create(void);
private:

	void Collision(int nCntGate);	// 当たり判定

	D3DXVECTOR3 m_posOrigin;			// 原点
	D3DXVECTOR3 m_rotOrigin;			// 向き
	float m_fLength;					// 原点からの距離
	CObjectX *m_pObjX[MAX_DAMAGEGATE];	// オブジェクトXのポインタ
	static int m_nIdxXFile;				// Xファイルのインデックス番号
};


#endif