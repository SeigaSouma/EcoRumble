//=============================================================================
// 
//  パワーゲージヘッダー [power_gauge.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _POWERGAUGE_H_
#define _POWERGAUGE_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// アイテムクラス定義
class CPowerGauge : public CObject
{
public:

	// 列挙型定義
	typedef enum
	{
		LEVEL_1 = 0,	// 1段階目
		LEVEL_2,		// 2
		LEVEL_3,		// 3
		LEVEL_MAX		// 最大
	}LEVEL;

	CPowerGauge(int nPriority = 7);
	~CPowerGauge();

	static CPowerGauge *Create(void);

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetVtx(void);

	void UpdateChargeCount(void);		// チャージカウント更新処理
	void SetChargeCount(int nValue);	// チャージカウント設定処理
	int GetLevel(void);			// チャージ段階取得
	bool IsFever(void) { return m_bFever; }			// FEVERかの判定
private:

	// 列挙型定義
	typedef enum
	{
		VTX_BLACK = 0,	// 黒ゲージ
		VTX_RED,		// 赤ゲージ
		VTX_GAUGE,		// ピンクゲージ
		VTX_FRAM,		// 枠
		VTX_MAX
	}VTX;

	// 構造体定義
	struct SPowerGauge
	{
		CObject2D *pObj2D;	// 2Dオブジェクトのオブジェクト
		float fMaxWidth;		// 幅の最大値
		float fWidthDest;		// 幅の差分
		float fMaxHeight;		// 高さの最大値
	};

	void GaugeDecrement(int nCntGauge);		// 減少処理
	void ChangeColor(int nCntGauge);		// 色更新

	int m_Level;								// チャージ段階
	int m_nCntWaitTime;							// 待ち時間
	int m_nCntCharge;							// チャージカウント
	int m_nMaxCharge;							// 最大チャージカウント
	bool m_bFever;								// FEVERか
	SPowerGauge m_sPowerGauge[VTX_MAX];
	int m_nTexIdx[VTX_MAX];			// テクスチャのインデックス番号
	static const char *m_apTextureFile[VTX_MAX];			// テクスチャのファイル
};


#endif