//=============================================================================
// 
//  プレイヤーヘッダー [enemy.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMY_H_
#define _ENEMY_H_	// 二重インクルード防止

#include "main.h"
#include "objectChara.h"
#include "motion.h"
#include "enemymanager.h"
#include "resultmanager.h"

// 前方宣言
class CHP_Gauge;
class CShadow;
class CEffect3D;

//==========================================================================
// クラス定義
//==========================================================================
// プレイヤークラス定義
class CEnemy : public CObjectChara
{
public:
	// 列挙型定義
	typedef enum
	{
		TYPE_POWER = 0,	// パワー
		TYPE_CROWD,		// 群体
		TYPE_JUMP,		// スピード
		TYPE_BOMB,		// ボム
		TYPE_SPEED,		// スピード
		TYPE_IMPACT,	// 衝撃
		TYPE_IMPACTCENTER,	// 真ん中の衝撃
		TYPE_MAX
	}TYPE;

	enum MOVETYPE
	{
		MOVETYPE_RAMDOM = 0,	// ランダム
		MOVETYPE_ROTATION,		// 回転
		MOVETYPE_FORMATION,		// 隊列
		MOVETYPE_PLAYERRUSH,	// プレイヤー突撃
		MOVETYPE_MAX
	};

	// 列挙型定義
	typedef enum
	{
		STATE_NONE = 0,		// なにもない
		STATE_SPAWN,		// スポーン
		STATE_DMG,			// ダメージ
		STATE_KNOCKBACK,	// ノックバック
		STATE_DEAD,			// 死
		STATE_FADEOUT,		// フェードアウト
		STATE_RETURNBASE,	// 拠点に戻る
		STATE_PLAYERSEARCH,	// プレイヤー探す
		STATE_PLAYERCHASE,	// プレイヤー追い掛け
		STATE_PARENTCHASE,	// プレイヤー追い掛け
		STATE_ATTACK,		// 攻撃
		STATE_TURRETATK,	// タレット攻撃
		STATE_TURRETWAIT,	// タレット待機
		STATE_BURY,			// 埋まっている
		STATE_OCCUR,		// 起き上がり
		STATE_IMPALSE,		// 衝撃で飛ぶ
		STATE_WAIT,			// 待機
		STATE_MAX
	}STATE;

	CEnemy(int nPriority = mylib_const::ENEMY_PRIORITY);
	virtual ~CEnemy();

	static CEnemy *Create(int nIdx, const char *pFileName, D3DXVECTOR3 pos, TYPE type = TYPE_POWER, int nCharacter = -1);

	// オーバーライドされた関数
	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;
	int GetState(void) override;
	void SetState(int state) override;		// 状態設定
	void SetState(int state, int nCntState) override;	// 状態設定
	int GetCharaType(void) override;	// 種類取得
	virtual bool Hit(const int nValue);

	HRESULT RoadText(const char *pFileName);
	virtual void Kill(void);
	void SetParent(CEnemy *pParent);		// 親のポインタ設定
	void SetFormationPos(D3DXVECTOR3 posFormation, D3DXVECTOR3 posParent);	// 隊列の位置
	void SetOriginRotation(D3DXVECTOR3 rot);	// 元の向き
	void SetMoveType(MOVETYPE movetype);	// 行動の種類設定
	void SetSpawnType(CEnemyManager::SPAWNTYPE type);	// 行動の種類設定
	CEnemy *GetEnemy(void);

protected:

	enum COLORTYPE
	{
		COLORTYPE_NORMAL = 0,	// ノーマル
		COLORTYPE_POISON= 7,	// 毒キノコ
		COLORTYPE_LUCKY,		// 緑の1UP
		COLORTYPE_CONFUSION,	// 混乱攻撃
		COLORTYPE_SWIFTFOOT,	// 脚はやい
		COLORTYPE_TUTORIAL,		// チュートリアル
		COLORTYPE_MAX
	};

	// モーションの判定
	struct SMotionFrag
	{
		bool bJump;			// ジャンプ中かどうか
		bool bATK;			// 攻撃中かどうか
		bool bKnockback;	// ノックバック中かどうか
		bool bMove;			// 移動中かどうか
	};

	// 隊列の構造体定義
	struct SFormationInfo
	{
		D3DXVECTOR3 pos;	// 隊列の位置
		float fAngle;		// 向き
		float fLength;		// 長さ
	};

	// マクロ定義
#define SPAWN_GRAVITY	(-0.25f)

	virtual void ProcessLanding(void);	// 着地時処理
	virtual void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo);	// 攻撃時処理
	virtual void ChaseMove(float fMove);	// 追い掛け移動

	// 状態更新系
	virtual void Spawn(void);				// スポーン
	virtual void Bury(void);				// 埋まり
	virtual void Occur(void);				// 起き上がり
	virtual void Impalse(void);				// 衝撃
	virtual void Damage(void);				// ダメージ
	virtual void Dead(void);				// 死亡
	virtual void FadeOut(void);				// フェードアウト
	virtual void KnockBack(void);			// ノックバック
	virtual void PlayerSearch(void);		// プレイヤー探索
	virtual void PlayerChase(void);			// プレイヤー追従
	virtual void ParentChase(void);			// 親追従
	virtual void ReturnBase(void);			// 拠点に帰る
	virtual void StateAttack(void);			// 攻撃処理
	virtual void TurretAttack(void);		// タレット攻撃処理
	virtual void TurretWait(void);			// タレット待機処理
	virtual void TriggerChasePlayer(void);	// プレイヤー追従ONにするトリガー
	virtual void ChangeToAttackState(void);	// 攻撃状態移行処理
	virtual void StateWait(void);			// 待機処理

	STATE m_state;							// 状態
	STATE m_Oldstate;						// 前回の状態
	int m_nCntState;						// 状態遷移カウンター
	D3DXVECTOR3 m_posOrigin;				// 最初の位置
	D3DXVECTOR3 m_posKnokBack;	// ノックバックの位置
	CEnemyManager::SPAWNTYPE m_SpawnType;	// 出現の種類
	SMotionFrag m_sMotionFrag;				// モーションのフラグ
	CHP_Gauge *m_pHPGauge;					// HPゲージの情報
	CMotion *m_pMotion;						// モーションの情報
	COLORTYPE m_colorType;					// 色ごとの種類
	CResultManager::ADDTYPE m_AddType;		// スコアの加算種類
	CEnemy *m_pParent;		// 親のポインタ
	D3DXCOLOR m_mMatcol;	// マテリアルの色
private:

	typedef enum
	{
		MOTION_DEF = 0,		// ニュートラルモーション
		MOTION_WALK,		// 移動モーション
		MOTION_ATK,			// 攻撃
		MOTION_KNOCKBACK,	// やられモーション
		MOTION_FADEOUT,		// 土帰還
		MOTION_MAX
	}MOTION;
	
	void UpdateState(void);					// 状態更新処理
	virtual void UpdateStateByType(void);	// 種類別状態更新処理
	void UpdateByType(void);				// 種類別更新処理
	void Collision(void);					// 当たり判定
	virtual void MotionSet(void) = 0;		// モーションの設定
	void RegistrChild(CEnemy *pChild);
	void ResetChild(CEnemy *pChild);
	void Atack(void);		// 攻撃

	TYPE m_type;			// 種類
	MOVETYPE m_MoveType;	// 行動の種類
	SFormationInfo m_sFormationInfo;	// 隊列の情報
	D3DXVECTOR3 m_rotOrigin;	// 最初の向き
	int m_nTexIdx;			// テクスチャのインデックス番号
	int m_nNumChild;		// 子の数
	int m_nIdxManager;		// マネージャのインデックス番号
	int m_nSurvivalLife;	// 生存時間
	int m_nSurvivalLifeOrigin;	// 生存時間
	bool m_bAddScore;		// スコア加算するかの判定
	int m_nBallastEmission;	// 瓦礫の発生カウンター
	CEnemy *m_pChild[mylib_const::MAX_ENEMY];	// 子のポインタ
	CShadow *m_pShadow;			// 影の情報
};



#endif