//=============================================================================
// 
//  マグマヘッダー [magma.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MAGMA_H_
#define _MAGMA_H_	// 二重インクルード防止

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// クラス定義
//==========================================================================
// マグマクラス定義
class CMagma : public CObject3DMesh
{
public:

	CMagma(int nPriority = 2);
	~CMagma();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	bool IsHit(D3DXVECTOR3& pos);	// 当たったか

	static CMagma *Create(D3DXVECTOR3 pos, float fWidthLen, int nBlock);
	CMagma *GetMagma(void);
protected:

private:

	int m_nTexIdx;	// テクスチャのインデックス番号
};



#endif