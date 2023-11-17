//=============================================================================
// 
//  メッシュシリンダーヘッダー [impactwave.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _IMPACTWAVE_H_
#define _IMPACTWAVE_H_	// 二重インクルード防止

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// クラス定義
//==========================================================================
// メッシュシリンダークラス定義
class CImpactWave : public CObject3DMesh
{
public:
	//列挙型定義
	typedef enum
	{
		TYPE_BLACK = 0,
		TYPE_BLACK2,
		TYPE_ORANGE,
		TYPE_ORANGE2,
		TYPE_BLUE,
		TYPE_GREEN,
		TYPE_PURPLE,
		TYPE_PURPLE2,
		TYPE_PURPLE3,
		TYPE_PINK,
		TYPE_SMOKE,
		TYPE_GIZA,
		TYPE_GIZA2,
		TYPE_GIZABLACK,
		TYPE_GIZAWHITE,
		TYPE_MAX
	}TYPE;

	CImpactWave(int nPriority = 6);
	~CImpactWave();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static CImpactWave *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fHeight, int nLife, float fMove, int nTexType, bool bAddBlend);
	CImpactWave *GetObject3DMesh(void);

private:
	int m_nTexIdx;	// テクスチャのインデックス番号
	int m_nLife;	// 寿命
	int m_nMaxLife;	// 最大寿命
	D3DXCOLOR m_colOrigin;	// 元の色
	float m_fOutWidth;		// 横幅(外)
	float m_fInWidth;		// 幅(内)
	float m_fRotWidth;		// 1分割数あたりの角度割合
	float m_fHeight;		// 高さ
	float m_fMove;			// 広がる速度
	bool m_bAddBlend;		// 加算合成の判定
	static const char *m_apFilename[TYPE_MAX];		//ファイル読み込み
};



#endif