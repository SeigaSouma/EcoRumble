//=============================================================================
// 
//  ウェーブ管理ヘッダー [wavemanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _WAVEMANAGER_H_
#define _WAVEMANAGER_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"

// 前方宣言
class CEnemy;
class CBoss;

//==========================================================================
// クラス定義
//==========================================================================
// ウェーブ管理クラス定義
class CWaveManager
{
public:

	// 設置情報
	struct SetInfo
	{
		int nType;			// 種類
		int nBase;			// 何番目の位置か
		int nMoveType;		// 行動の種類
		int nSpawnType;		// 出現の種類
		int nSpawnNum;		// スポーンする数
		int nChara;			// キャラの種類
		D3DXVECTOR3 pos;	// 位置
		D3DXVECTOR3 rot;	// 向き
	};

	// 出現情報
	struct SetWaveInfo
	{
		SetInfo Info[mylib_const::MAX_OBJ];	// 設置情報
		int nSetNum;	// 設置する数
	};

	// ステージ情報
	struct SetStageInfo
	{
		SetWaveInfo aWaveInfo[mylib_const::MAX_WAVE_ENEMY];	// ウェーブの情報
	};

	CWaveManager();
	~CWaveManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	static CWaveManager *Create(const std::string pTextFile);
	HRESULT ReadText(const std::string pTextFile);	// 外部ファイル読み込み処理
	void SetEnableWaveEnd(void);	// 終了のフラグON
	void SetDisableWaveEnd(void);	// 終了のフラグOFF
	void SetEnableStageEnd(void);	// 終了のフラグON
	void SetDisableStageEnd(void);	// 終了のフラグOFF
	bool IsWaveFrag(void);		// 終了のフラグ取得
	bool IsStageFrag(void);		// ステージ終了のフラグ取得
	int GetNowWave(void);				// 現在のウェーブ取得
	int GetMaxWave(void);				// 現在のステージのウェーブ数取得
	int GetNowStage(void);				// 現在のステージ取得
	int GetMaxStage(void);				// ステージ数取得
	int GetRespawnThreshold(void);		// リスポーンの規定値取得

private:

	int m_nNowWave;		// 現在のウェーブ
	int m_nNowStage;	// 現在のステージ
	bool m_bEndWave;	// ウェーブ終了のフラグ
	bool m_bEndStage;	// ステージ終了のフラグ
	int m_nNumWave[mylib_const::MAX_STAGE];	// ウェーブの総数
	int m_nRespawnThreshold[mylib_const::MAX_STAGE][mylib_const::MAX_WAVE_ENEMY];	// ウェーブ毎の敵リスポーン規定値
	int m_nNumStage;	// ステージの総数
};



#endif