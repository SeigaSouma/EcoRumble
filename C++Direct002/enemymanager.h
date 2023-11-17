//=============================================================================
// 
//  敵のマネージャヘッダー [enemymanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYMANAGER_H_
#define _ENEMYMANAGER_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"
#include "wavemanager.h"

// 前方宣言
class CEnemy;
class CBoss;

//==========================================================================
// クラス定義
//==========================================================================
// 敵のマネージャクラス定義
class CEnemyManager
{
protected:

	// 構造体定義
	struct EnemyData
	{
		int nType;				// キャラクター種類
		int nParent;			// 親のインデックス
		int nWaitTime;			// 待機時間
		D3DXVECTOR3 pos;		// 位置
	};

	struct Pattern
	{
		int nNumEnemy;	// 敵の数
		EnemyData EnemyData[mylib_const::MAX_PATTEN_ENEMY];
	};

public:

	// 列挙型定義
	enum SPAWNTYPE
	{
		SPAWNTYPE_NORMAL = 0,	// 通常
		SPAWNTYPE_ORDER,		// 順番
		SPAWNTYPE_CIRCLE,		// 円状
		SPAWNTYPE_CLOCK,		// 時計回り
		SPAWNTYPE_BOTHCLOCK,	// 両回り
		SPAWNTYPE_MAX
	};

	// 列挙型定義
	enum PATTERN
	{
		PATTERN_POWERONE = 0,
		PATTERN_CROWD,
		PATTERN_CROWD2,
		PATTERN_POWER_CROWD,
		PATTERN_JUMP,
		PATTERN_BOSS,
		PATTERN_BOMB,
		PATTERN_MAX
	};

	// 列挙型定義
	enum STATE
	{
		STATE_NONE = 0,		// 何もない状態
		STATE_COMBOANIM,	// コンボ時のアニメーション
		STATE_MAX
	};

	CEnemyManager();
	~CEnemyManager();

	HRESULT Init(void);
	void Uninit(void);
	virtual void Update(void);

	static CEnemyManager *Create(const std::string pTextFile);
	HRESULT ReadText(const std::string pTextFile);	// 外部ファイル読み込み処理
	void SetEnemy(int nPattern, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 rotBase, int movetype = 0);	// 敵配置
	void SetEnemy(int nIdx, int nPattern, int nCntEnemy, int nParent, CWaveManager::SetInfo setinfo, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int movetype = 0);	// 敵配置
	void SetBoss(int nPattern, D3DXVECTOR3 pos, D3DXVECTOR3 rot);	// ボス配置
	static int GetPatternNum(void);
	static void Release(int nIdx);		// 破棄
	static void ReleaseBoss(int nIdx);	// 破棄
	void SetWaveEnemy(int nStage, int nWave);		// ウェーブ毎の配置処理
	void SetSpawnWave(int nStage, int nWave);		// スポーンの起伏設定
	void SetEnemyInfo(CWaveManager::SetInfo *info, int nStage, int nWave, int nSpawnNum);	// 情報登録
	STATE GetState(void) { return m_state; }	// 状態取得

	void SetKillFlag(void);		// 倒したフラグを設定
	int GetIntervalKill(void);	// 倒した間隔取得
	int GetNumIntervalKill(void);	// 間隔内に倒した数取得
	int GetNumAll(void);	// 敵の総数取得

protected:

	void UpdateComboAnimation(void);	// コンボ時の演出

	static int m_nNumAll;		// 敵の総数
	static CEnemy *m_pEnemy[mylib_const::MAX_OBJ];						// 敵へのポインタ
	static Pattern m_aPattern[mylib_const::MAX_PATTEN_ENEMY];			// 配置の種類
	static CWaveManager::SetStageInfo m_aSetEnemyInfo[mylib_const::MAX_WAVE_ENEMY];	// 敵の配置情報
	static std::string sMotionFileName[mylib_const::MAX_PATTEN_ENEMY];	// モーションファイル名
	static int m_nParent[mylib_const::MAX_OBJ];		// 敵の親ポインタ
	static int m_nNumTotalKill;	// まとめて倒した数
	static int m_nIntervalKill;	// 倒したカウントのインターバル
	static bool m_bHitStop;		// ヒットストップの判定
	static STATE m_state;		// 状態

private:

	void SetSpawnWaveNormal(int nStage, int nWave, int nCntPattern, int *pParent);		// 通常湧き
	void SetSpawnWaveOrder(int nStage, int nWave, int nCntPattern, int *pParent);		// 順番湧き
	void SetSpawnWaveCircle(int nStage, int nWave, int nCntPattern, int *pParent);		// 円一気湧き
	void SetSpawnWaveClock(int nStage, int nWave, int nCntPattern, int *pParent);		// 時計回り湧き
	void SetSpawnWaveBothClock(int nStage, int nWave, int nCntPattern, int *pParent);	// 両回り湧き
	void SetSlashImpact(D3DXVECTOR3 pos);	// スラッシュ衝撃波設定

	static int m_nCntSpawn;		// 出現カウント
	static int m_nPatternNum;	// 出現パターン数
	static int m_nNumChara;		// 敵の種類の総数
	static CBoss *m_pBoss[mylib_const::MAX_OBJ];						// ボスへのポインタ
	static bool m_bLoadPattern;	// パターン読み込み判定
};



#endif