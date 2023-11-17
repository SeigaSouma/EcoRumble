//=============================================================================
// 
//  起伏ヘッダー [elevation.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ELEVATION_H_
#define _ELEVATION_H_	// 二重インクルード防止

#include "main.h"
#include "object3DMesh.h"
#include "wavemanager.h"

// 前方宣言
class CTargetPoint;

//==========================================================================
// クラス定義
//==========================================================================
// 起伏クラス定義
class CElevation : public CObject3DMesh
{
public:

	// 列挙型定義
	typedef enum
	{
		DAMAGE_RATIO = 0,	// 割合
		DAMAGE_EQUAL,		// 等しい
		DAMAGE_MAX
	}DAMAGE;

	// 列挙型定義
	typedef enum
	{
		WAVETYPE_RIPPLE = 0,	// 波紋
		WAVETYPE_DIRECT,		// 直行
		WAVETYPE_IMPACT,		// 衝撃波
		WAVETYPE_THRUST,		// 突き上げ
		WAVETYPE_RAISE,			// 盛り上げ
		WAVETYPE_MAX
	}WAVETYPE;

	enum TYPE
	{
		TYPE_PLAYER = 0,	// プレイヤー
		TYPE_ENEMY,			// 敵
		TYPE_NATURAL,		// 自然
		TYPE_EFFECT,		// エフェクト
		TYPE_SPAWN,			// スポーン
		TYPE_MAX
	};

	enum POSTYPE
	{
		POSTYPE_UP = 0,
		POSTYPE_DOWN,
		POSTYPE_RIGHT,
		POSTYPE_LEFT,
		POSTYPE_MAX
	};

private:
	// 列挙型定義
	typedef enum
	{
		STATE_NONE = 0,	// なにもなし
		STATE_SET,		// 設定
		STATE_WAVE,		// 波打ち
		STATE_MAX
	}STATE;

	// 列挙型定義
	typedef enum
	{
		FADESTATE_INCREMENT = 0,
		FADESTATE_NONE,
		FADESTATE_DECREMENT,
		FADESTATE_MAX
	}FADESTATE;

	struct Info
	{
		std::string TextureFileName;	// テクスチャファイル名
		D3DXVECTOR3 pos;		// 位置
		float fWidthLength;		// 横長さ
		float fHeightLength;	// 縦長さ
		int nWidthBlock;		// 横分割数
		int nHeightBlock;		// 縦分割数
		D3DXVECTOR3 *pVtxPos;	// 頂点座標
		D3DXVECTOR3 *pStageVtxPos[mylib_const::MAX_STAGE];	// ステージ毎の頂点座標
	};

	struct WaveInfo
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 posOrigin;	// 位置
		D3DXVECTOR3 rot;		// 向き
		D3DXVECTOR3 move;		// 移動量
		D3DXVECTOR2 fRange;		// 範囲
		float fRatio;			// 間隔
		float fStrength;		// 強さ
		float fStrengthOrigin;	// 元の強さ
		float fWave;			// 波の高さ
		WAVETYPE type;			// 種類
		TYPE playertype;		// プレイヤータイプ
		float fLength;			// 波の高さ
		int nLevel;				// レベル
	};

	struct SWave
	{
		WaveInfo sSetWave;							// 波情報
		CWaveManager::SetInfo WaveManagerInfo;		// ウェーブマネージャの情報
		int nMyIdx;									// 自分のインデックス番号
		int nParentIdx;								// 親のインデックス番号
		int nNumEnemy;								// 何番目の敵か
		int nWaitTime;								// 待機時間
		STATE state;								// 状態
		int nMoveLife;								// 移動寿命
		int nOriginMoveLife;						// 移動寿命
		int nCntState;								// 状態遷移カウンター
		int nOriginCntState;						// 状態遷移カウンター
		float fMaxWaveHeight;						// 最大高さ
		float fFrameMaxWaveHeight;					// フレーム毎の最大高さ
		float fWave[mylib_const::MAX_VTX];			// 波
		D3DXVECTOR3 VtxPos[mylib_const::MAX_VTX];	// 頂点座標
		D3DXVECTOR3 WaveMove[mylib_const::MAX_VTX];	// 波移動量
	};

	// ステージ切り替え時の情報
	struct SFadeWave
	{
		float *pRot;		// 波の向き
		float fLength;		// 半径の長さ
		FADESTATE state;	// 状態
	};

public:

	CElevation(int nPriority = 1);
	~CElevation();

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual HRESULT Init(const char *pText);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void SetVtx(void);

	void Release(void);
	bool IsHit(D3DXVECTOR3& pos);	// 当たったかの判定
	float Slide(D3DXVECTOR3& pos);	// スライドの判定
	void Damage(D3DXVECTOR3 pos, float fRange, float fStrength, DAMAGE damage);	// ダメージ
	void SetWave(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR2 fRange, float fRatio, float fStrength, float fWave, int nCntState, WAVETYPE type, TYPE playertype, int nLevel);	// 波の設定
	void SetSpawnWave(int nIdx, int nParentID, int nNumEnemy, int nWaitTime, CWaveManager::SetInfo info, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 Range, float fStrength, int nCntState);	// スポーン波の設定
	bool IsEdit(void);	// エディットの判定
	void Save(void);	// セーブ
	HRESULT Load(const char *pText);	// ロード
	void SetWaveNaturalwave(int nStage, int nWave);		// ウェーブ毎の配置処理
	void SetNaturalWaveInfo(CWaveManager::SetInfo *info, int nStage, int nWave, int nSpawnNum);	// 情報登録
	virtual void StageSwitch(int nNowStage, int nNextStage, int nNowLife, int nMaxLife);

	static CElevation *Create(const char *pText, CObject::TYPE type);
	CElevation *GetElevation(void);
	float GetHeight(D3DXVECTOR3& pos, D3DXVECTOR3& move, bool &bLand, CObject::TYPE type);	// 高さ取得
protected:

	Info m_aInfo;	// 起伏の情報
	SFadeWave m_sFadeInfo;							// フェード時の情報
	float m_fBrushStrength;							// ブラシ強さ
	float m_fBrushRange;							// 範囲
	bool m_bEdit;									// エディットON/OFF
	CTargetPoint *m_pTargetP;						// 目標の地点
	D3DXVECTOR3 m_VtxPos[mylib_const::MAX_VTX];		// 頂点座標

private:

	void UpdateState(void);				// 状態更新処理
	void UPVtxField(D3DXVECTOR3 pos);	// 頂点上げ下げ(デバッグ)
	void Wave(int nCntWave);			// 波打ち更新処理
	void UpdateRippleWave(int nCntWave, int nVtxPoint);	// 波紋更新処理
	void UpdateDirectWave(int nCntWave, int nVtxPoint);	// 直行波更新処理
	void UpdateImpactWave(int nCntWave, int nVtxPoint);	// 衝撃波更新処理
	void UpdateThrustWave(int nCntWave, int nVtxPoint);	// 突き上げ更新処理
	void UpdateRaiseField(int nCntWave, int nVtxPoint);	// 盛り上げ更新処理
	void Collision(int nCntWave);		// 当たり判定処理
	void CollisionDirectWave(int nCntWave, CObject *pObj);		// 直線波の当たり判定処理
	void CollisionImpactWave(int nCntWave, CObject *pObj);		// 円形波の当たり判定処理
	void SetEnemy(int nCntWave);	// 敵の設定

	SWave m_aWave[mylib_const::MAX_WAVE];			// 波の情報
	int m_nTexIdx;									// テクスチャのインデックス番号
	int m_nSpawnByFrame;							// フレーム毎のスポーンカウント
	bool m_bSpawnByFrame;							// フレーム毎のスポーン判定
	bool m_bHitStop_Direct;							// 直線波のヒットストップ判定
	bool m_bHitStop_Impact;							// 円形波のヒットストップ判定
	static CWaveManager::SetStageInfo m_aSetInfo[mylib_const::MAX_WAVE_ENEMY];	// 敵の配置情報
	static bool m_bLoadInfo;						// 情報読み込み判定
	static bool m_bSetSpawnCamera;					// スポーン時のカメラ設定判定
	static int m_nCntSetSpawn;						// スポーン時のカメラカウント
};



#endif