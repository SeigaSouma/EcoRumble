//=============================================================================
// 
//  リザルトヘッダー [result.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _RESULT_H_
#define _RESULT_H_	// 二重インクルード防止

#include "main.h"
#include "manager.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CResultScreen;

//==========================================================================
// クラス定義
//==========================================================================
// リザルトクラス定義
class CResult : public CScene
{
public:

	CResult();
	~CResult();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	CPlayer *GetPlayer(void) override;
	CPlayer **GetPlayer(int n) override;
	static CResultScreen *GetResultScreen(void);
	static void SetEnableArrival(void);

private:
	CPlayer *m_pPlayer;		// プレイヤーのオブジェクト
	static bool m_bAllArrival;		// 全て到着した判定
	static CResultScreen *m_pResultScreen;
};



#endif