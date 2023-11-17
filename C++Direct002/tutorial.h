//=============================================================================
// 
//  チュートリアルヘッダー [tutorial.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_	// 二重インクルード防止

#include "main.h"
#include "manager.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CTutorialStep;
class CTutorialText;

//==========================================================================
// クラス定義
//==========================================================================
// チュートリアルクラス定義
class CTutorial : public CScene
{
public:

	CTutorial();
	~CTutorial();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	CPlayer *GetPlayer(void) override;
	CPlayer **GetPlayer(int n) override;

	static bool IsMovingPlayer(void);		// プレイヤーが動いてる判定取得
	static void SetEnableMovingPlayer(void);		// プレイヤーが動いてる判定
	static void SetDesableMovingPlayer(void);		// プレイヤーが動いてる判定OFF
	static CTutorialStep *GetStep(void);	// ステップ取得
private:

	CPlayer *m_pPlayer;		// プレイヤーのオブジェクト
	static bool m_bMovingPlayer;	// プレイヤーが動いてる判定
	static CTutorialStep *m_pStep;	// ステップ
};



#endif