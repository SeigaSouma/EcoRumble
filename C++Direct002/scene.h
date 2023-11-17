//=============================================================================
// 
//  シーンヘッダー [scene.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SCENE_H_
#define _SCENE_H_	// 二重インクルード防止

#include "main.h"
#include "elevation.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CXLoad;
class CWaveManager;
class CEnemyManager;
class CEnemyBase;
class CPlayer;

//==========================================================================
// クラス定義
//==========================================================================
// シーンクラス定義
class CScene
{
public:
	// モードの列挙型定義
	enum MODE
	{
		MODE_NONE = 0,	// タイトル
		MODE_TITLE,		// タイトル
		MODE_TUTORIAL,	// チュートリアル
		MODE_GAME,		// ゲーム
		MODE_RESULT,	// リザルト
		MODE_RANKING,	// ランキング
		MODE_MAX
	};

	CScene();
	~CScene();

	// オーバーライドする関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	static CScene *Create(MODE mode);
	static CXLoad *GetXLoad(void);				// Xファイルのデータ取得
	static CElevation *GetObject3DMesh(void);	// オブジェクト3Dの取得
	static CElevation *GetElevationOutside(int nIdx);		// 外側起伏の地面のオブジェクト
	CWaveManager *GetWaveManager(void);
	static CEnemyManager *GetEnemyManager(void);
	static CEnemyBase *GetEnemyBase(void);
	MODE GetMode(void);		// 現在のモード取得

	virtual CPlayer *GetPlayer(void);
	virtual CPlayer **GetPlayer(int n);
private:

	MODE m_mode;				// 現在のモード
	CWaveManager *m_pWaveManager;		// ウェーブ管理のオブジェクト
	static CXLoad *m_pXLoad;	// Xファイルのオブジェクト
	static CElevation *m_pObject3DMesh;			// オブジェクト3Dメッシュのオブジェクト
	static CElevation *m_pElevationOutside[CElevation::POSTYPE_MAX];	// 外側起伏の地面のオブジェクト
	static CEnemyManager *m_pEnemyManager;		// 敵マネージャのオブジェクト
	static CEnemyBase *m_pEnemyBase;			// 敵拠点のオブジェクト
};



#endif