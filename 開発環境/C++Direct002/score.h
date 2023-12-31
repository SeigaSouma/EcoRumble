//=============================================================================
// 
//  スコアヘッダー [score.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SCORE_H_
#define _SCORE_H_	// 二重インクルード防止

#include "main.h"
#include "resultmanager.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CNumber;

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CScore
{
public:

	CScore(int nPriority = 6);
	~CScore();

	static HRESULT Load(void);
	static void Unload(void);
	static CScore *Create(void);
	static CScore *Create(D3DXVECTOR3 pos);

	// オーバーライドされた関数
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Add(int nValue);
	void Add(CResultManager::ADDTYPE AddType);

private:
	//==============================
	// マクロ定義
	//=============================
#define NUM_SCORE	(6)	// 桁数

	void SetValue(void);

	int m_nNum;								// 値
	CNumber *m_apNumber[NUM_SCORE];			// 数字のオブジェクト
	int m_nTexIdx;					// テクスチャのインデックス番号
};



#endif