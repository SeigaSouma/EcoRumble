//=============================================================================
// 
//  爆発球ヘッダー [explosion_sphere.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EXPLOSION_SPHERE_H_
#define _EXPLOSION_SPHERE_H_	// 二重インクルード防止

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// クラス定義
//==========================================================================
// 起伏クラス定義
class CExplosionSphere : public CObject3DMesh
{
public:
	CExplosionSphere(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CExplosionSphere();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static CExplosionSphere *Create(D3DXVECTOR3 pos, float fSizeDest);

private:
	void Collision(void);	// 当たり判定

	int m_nTexIdx;	// テクスチャのインデックス番号
	int m_nLife;	// 寿命
	float m_fSizeDest;	// 目標のサイズ
};



#endif