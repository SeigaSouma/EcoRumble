//=============================================================================
// 
//  水のフィールドヘッダー [waterfield.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _WATERFIELD_H_
#define _WATERFIELD_H_	// 二重インクルード防止

#include "main.h"
#include "meshfield.h"

//==========================================================================
// クラス定義
//==========================================================================
// メッシュフィールドクラス定義
class CWaterField : public CMeshField
{
public:
	
	CWaterField(int nPriority = 5);
	~CWaterField();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	CWaterField *GetObject3DMesh(void);
protected:

private:

	// メンバ関数
	void Scroll(void);

	// メンバ変数
	float m_fTexU;	// Uスクロール用
	float m_fTexV;	// Vスクロール用
	int m_nTimeInWater[mylib_const::MAX_OBJ];	// 水に入ってる時間
};



#endif