//=============================================================================
// 
//  高速ラッシュヘッダー [speed_rush.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SPEEDRUSH_H_
#define _SPEEDRUSH_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObjectX;

//==========================================================================
// マクロ定義
//==========================================================================
#define MAX_SPEEDRUSH	(64)	// 高速ラッシュの数

//==========================================================================
// クラス定義
//==========================================================================
// 高速ラッシュクラス定義
class CSpeedRush : public CObject
{
public:
	CSpeedRush(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CSpeedRush();

	static HRESULT Load(void);
	static CSpeedRush *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nNum);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

	bool Set(void);	// 設定処理
	void Collision(void);	// 当たり判定

	D3DXVECTOR3 m_posOrigin;			// 原点
	D3DXVECTOR3 m_rotOrigin;			// 原向き
	int m_nNumAll;						// 総数
	int m_nLife;						// 寿命
	static const char *m_pModelFile;	// モデルファイル
	CObjectX *m_pObjX[MAX_SPEEDRUSH];	// オブジェクトXのポインタ
	int m_nObjLife[MAX_SPEEDRUSH];		// 寿命
	static int m_nIdxXFile;				// Xファイルのインデックス番号
};


#endif