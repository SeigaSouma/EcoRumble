//=============================================================================
// 
//  プレイヤーヘッダー [player.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _PLAYER_H_
#define _PLAYER_H_	// 二重インクルード防止

#include "main.h"
#include "objectChara.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define SLIDEMOVE		(2.05f)			// 坂の移動量

// 前方宣言
class CMotion;
class CShadow;
class CMagicCircle;
class CTargetPoint;
class CHP_Gauge;
class CDelusionWindow;
class CPowerGauge;
class CConfusionEffect;

//==========================================================================
// クラス定義
//==========================================================================
// プレイヤークラス定義
class CPlayer : public CObjectChara
{
public:

	// 列挙型定義
#if ACTION_MODE
	typedef enum
	{
		SHOTMODE_TRIGGER = 0,	// トリガー
		SHOTMODE_RELEASE,		// リリース
		SHOTMODE_PRESS,			// プレス
		SHOTMODE_REPEAT,		// リピート
		SHOTMODE_MAX
	}SHOTMODE;

#else
	typedef enum
	{
		SHOTMODE_NONE = 0,	// なし
		SHOTMODE_REPEAT,	// リピート
		SHOTMODE_MAX
	}SHOTMODE;

	typedef enum
	{
		ATK_DEBUG_POWER = 0,		// パワー
		ATK_DEBUG_POWER2_SPEED1,	// パワー2, スピード1
		ATK_DEBUG_POWER2_CROWD1,	// パワー2, 群体1
		ATK_DEBUG_POWER1,			// パワー系全て1
		ATK_DEBUG_CROWD,			// 群体
		ATK_DEBUG_CROWD2_POWER1,	// 群体2, パワー1
		ATK_DEBUG_CROWD2_SPEED1,	// 群体2, スピード1
		ATK_DEBUG_CROWD1,			// 群体系全て1
		ATK_DEBUG_SPEED,			// スピード
		ATK_DEBUG_SPEED2_POWER1,	// スピード2, パワー1
		ATK_DEBUG_SPEED2_CROWD1,	// スピード2, 群体1
		ATK_DEBUG_SPEED1,			// スピード系全て1
		ATK_DIRECT,					// 直行波攻撃
		ATK_DEBUG_MAX
	}ATK_DEBUG;

	// 状態定義
	typedef enum
	{
		STATE_NONE = 0,		// なにもない
		STATE_INVINCIBLE,	// 無敵
		STATE_DMG,			// ダメージ
		STATE_KNOCKBACK,	// ノックバック
		STATE_DEAD,			// 死
		STATE_CONFUSION,	// 混乱
		STATE_ATTACK,		// 攻撃処理
		STATE_MAX
	}STATE;
#endif

	CPlayer(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CPlayer();

	static CPlayer *Create();

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	void Uninit(void);
	virtual void Update(void);
	void Draw(void);
	bool Hit(const int nValue);	// ヒット処理
	int GetState(void) override;

	virtual void UninitByMode(void);
	void EnableConfusion(void);	// 混乱を有効にする
	bool IsConfuison(void) { return m_bConfusion; }
	void IsSwitchInWater(bool bFrag);	// 水の中にいる判定
	void ChangeWeapon(const int *pPoint);	// 武器チェンジ処理
	void Kill(void);	// 死亡処理

protected:
	// 列挙型定義
	typedef enum
	{
		MOTION_DEF = 0,			// ニュートラルモーション
		MOTION_WALK,			// 移動モーション
		MOTION_SUPERATK,		// 超攻撃
		MOTION_DIRECTATK,		// 直行波攻撃
		MOTION_KNOCKBACK,		// やられモーション
		MOTION_CONFUSION,		// 混乱
		MOTION_SPEEDWALK,		// 高速移動モーション
		MOTION_SLOWWALK,		// 高速移動モーション
		MOTION_MAX
	}MOTION;

	// 移動タイプ
	enum WALKTYPE
	{
		WALKTYPE_NORMAL = 0,	// 通常
		WALKTYPE_SPEED,			// 早い
		WALKTYPE_SLOW,			// 遅い
		WALKTYPE_MAX
	};

	void Collision(void);	// 当たり判定

	bool m_bJump;				// ジャンプ中かどうか
	bool m_bKnockback;			// ノックバック中かどうか
	bool m_bMove;				// 移動中かどうか
	bool m_bATK;				// 攻撃中かどうか
	bool m_bMWait;				// 魔法待機中かどうか
	bool m_bCharge;				// チャージ中かどうか
	bool m_bConfusion;			// 混乱中かどうか
	bool m_bInWater;			// 水の中にいる判定
	float m_fMoveRatio;			// 移動の割合
	float m_fMoveSlide;			// スライド移動量
	float m_fMoveSlideDest;		// 目標のスライド移動量
	int m_Shotmode;			// 射撃の種類
	int m_nCntWalk;				// 歩行カウンター
	STATE m_state;			// 状態
	WALKTYPE m_walkType;	// 移動種類
	CMotion *m_pMotion;		// モーションの情報
private:

	// メンバ関数
	void UpdateState(void);	// 状態更新処理
	void KnockBack(void);	// ノックバック
	void Damage(void);		// ダメージ
	void Invincible(void);	// 無敵
	virtual void Controll(void);	// 操作
	void MotionSet(void);	// モーションの設定
	void Atack(void);		// 攻撃
	void Confusion(void);	// 混乱

	STATE m_Oldstate;			// 前回の状態
	D3DXCOLOR m_mMatcol;		// マテリアルの色
	D3DXVECTOR3 m_posKnokBack;	// ノックバックの位置
	D3DXVECTOR3 m_rotConfusion;	// 混乱の向き
	int m_nCntState;			// 状態遷移カウンター
	int m_nCntConfusion;		// 混乱カウンター
	int m_nLife;				// 体力
	int m_nTexIdx;				// テクスチャのインデックス番号
	int m_nIdxXFile;			// Xファイルのインデックス番号
	CShadow *m_pShadow;			// 影の情報
	CMagicCircle *m_pMCircle;	// 魔法陣の情報
	CTargetPoint *m_pTargetP;	// 目標の地点
	CHP_Gauge *m_pHPGauge;		// HPゲージの情報
	CDelusionWindow *m_pDelusionWindow;	// 妄想ウィンドウ
	CConfusionEffect *m_pConfusionEffect;	// 混乱エフェクトの情報
};



#endif