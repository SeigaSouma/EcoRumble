//=============================================================================
// 
//  ボスヘッダー [boss.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BOSS_H_
#define _BOSS_H_	// 二重インクルード防止

#include "main.h"
#include "objectChara.h"

// 前方宣言
class CMotion;
class CHP_Gauge;

//==========================================================================
// クラス定義
//==========================================================================
// プレイヤークラス定義
class CBoss : public CObjectChara
{
public:

	// 列挙型定義
	typedef enum
	{
		STATE_NONE = 0,	// なにもない
		STATE_DMG,		// ダメージ
		STATE_KNOCKBACK,		// ノックバック
		STATE_DEAD,		// 死
		STATE_PLAYERCHASE,	// プレイヤー追い掛け
		STATE_ATTACK,		// 攻撃処理
		STATE_MAX
	}STATE;

	CBoss(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CBoss();

	static CBoss *Create(int nIdx, const char *pFileName, D3DXVECTOR3 pos);

	// オーバーライドされた関数
	HRESULT Init(void) override;
	HRESULT Init(const char *pFileName);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	bool Hit(const int nValue) override;
	bool Hit(const int nValue, D3DXVECTOR3 move) override;

	void Kill(void);
	CBoss *GetBoss(void);
	static int GetNumAll(void) { return m_nNumAll; }	// 総数取得

	int GetState(void) override;
	int GetCharaType(void) override;	// 種類取得

protected:

private:

	typedef enum
	{
		MOTION_DEF = 0,	// ニュートラルモーション
		MOTION_WALK,	// 移動モーション
		MOTION_ATK,		// 攻撃
		MOTION_MAX
	}MOTION;

	enum ACTION
	{
		ACTION_TACKLE = 0,	// タックル
		ACTION_JUMP,		// ジャンプ
		ACTION_SP_JUMP,		// 超ジャンプ
		ACTION_MAX
	};
	
	void UpdateState(void);	// 状態更新処理
	void Collision(void);	// 当たり判定
	void MotionSet(void);	// モーションの設定
	void Atack(void);		// 攻撃
	void BombExplosion(void);	// ボム爆破

	ACTION m_action;		// 行動
	int m_nCntAction;		// 行動カウンター
	int m_nCntHit;			// ヒットカウンター
	int m_nCntHitAfter;		// ヒット後のカウント

	STATE m_state;			// 状態
	STATE m_Oldstate;		// 前回の状態
	D3DXVECTOR3 m_ImpalseMove;	// 吹き飛ぶ移動量
	int m_nCntState;		// 状態遷移カウンター
	int m_nBeforCntState;	// 前の状態遷移カウンター
	int m_nLife;			// 体力
	int m_nTexIdx;			// テクスチャのインデックス番号
	int m_nIdxManager;		// マネージャのインデックス番号
	bool m_bJump;			// ジャンプ中かどうか
	bool m_bMove;			// 移動中かどうか
	bool m_bATK;			// 攻撃中かどうか
	D3DXCOLOR m_mMatcol;	// マテリアルの色
	CMotion *m_pMotion;		// モーションの情報
	CHP_Gauge *m_pHPGauge;		// HPゲージの情報
	static int m_nNumAll;		// 敵の総数
};



#endif