//=============================================================================
// 
//  草のパーティクルヘッダー [grassparticle.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _GRASSPARTICLE_H_
#define _GRASSPARTICLE_H_		// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObject3DMesh;

//==========================================================================
// マクロ定義
//==========================================================================
#define MAX_GRASSPARTICLE	(64)	// 瓦礫の数

//==========================================================================
// クラス定義
//==========================================================================
// 草のパーティクルクラス定義
class CGrassParticle : public CObject
{
public:

	CGrassParticle(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CGrassParticle();

	static CGrassParticle *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 move, int nNum);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Release(int nIdx);
private:

	D3DXVECTOR3 m_posOrigin;		// 原点
	D3DXVECTOR2 m_moveOrigin;		// 元の移動量
	D3DXVECTOR3 m_rotOrigin;		// 元の向き
	D3DXVECTOR3 m_move[MAX_GRASSPARTICLE];		// 移動量
	D3DXVECTOR3 m_rotMove[MAX_GRASSPARTICLE];		// 回転移動量
	TYPE m_type;					// 種類
	int m_nNumAll;					// 総数
	int m_nLife[MAX_GRASSPARTICLE];					// 寿命
	int m_nMaxLife[MAX_GRASSPARTICLE];					// 最大寿命
	CObject3DMesh *m_pObjMesh[MAX_GRASSPARTICLE];	// 3Dメッシュのポインタ
	static int m_nTexIdx;						// テクスチャのインデックス番号
};


#endif