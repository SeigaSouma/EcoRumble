//=============================================================================
// 
//  妄想のメーターヘッダー [delusion_meter.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _DELUSION_METER_H_
#define _DELUSION_METER_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"

class CDelusionPoint;

//==========================================================================
// クラス定義
//==========================================================================
// 妄想のメータークラス定義
class CDelusionMeter : public CObject2D
{
public:

	// 列挙型定義
	typedef enum
	{
		TYPE_SPEED = 0,	// スピード
		TYPE_POWER,		// パワー
		TYPE_CROWD,		// 群体
		TYPE_MAX
	}TYPE;

	CDelusionMeter(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CDelusionMeter();

	static CDelusionMeter *Create(void);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static int GetNumAll(void);

private:

	int m_nChangeIdx;			// 変更するインデックス番号
	int m_nRemainPoint;			// 残りポイント
	int m_nPoint[3];			// 要素ポイント
	float m_fLength[3];			// 長さ
	float m_fDestLength[3];		// 目標の長さ
	static int m_nNumAll;		// 妄想のメーターの総数
	int m_nTexIdx;				// テクスチャのインデックス番号
	CDelusionPoint *m_pDelusionPoint;	// 妄想ポイント
	CObject2D *m_pMeterMain;			// 本来のメーター
	CObject2D *m_pMeter;			// 本来のメーター
};


#endif