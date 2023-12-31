//=============================================================================
// 
//  ゲームヘッダー [game.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _GAME_H_
#define _GAME_H_	// 二重インクルード防止

#include "main.h"
#include "scene.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CPlayer;
class CScore;
class CTimer;
class CPowerGauge;

//==========================================================================
// クラス定義
//==========================================================================
// ゲームクラス定義
class CGame : public CScene
{
public:

	CGame();
	~CGame();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Reset(void);
	CPlayer *GetPlayer(void);
	CPlayer **GetPlayer(int n);
	static CScore *GetScore(void);
	static CTimer *GetTimer(void);
	static CPowerGauge *GetPowerGauge(void);

private:
	
	CPlayer *m_pPlayer;					// プレイヤーのオブジェクト
	static CScore *m_pScore;					// スコアのオブジェクト
	static CTimer *m_pTimer;					// タイマーのオブジェクト
	static CPowerGauge *m_pPowerGauge;			// パワーゲージのオブジェクト
};



#endif