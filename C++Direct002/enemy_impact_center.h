//=============================================================================
// 
//  波起こし敵ヘッダー [enemy_impact_center.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYIMPACT_CENTER_H_
#define _ENEMYIMPACT_CENTER_H_	// 二重インクルード防止

#include "main.h"
#include "enemy.h"

// 前方宣言

//==========================================================================
// クラス定義
//==========================================================================
// 波起こし敵クラス定義
class CEnemyImpactCenter : public CEnemy
{
public:

	CEnemyImpactCenter(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyImpactCenter();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

	CEnemyImpactCenter *GetEnemy(void);
protected:

private:

	// オーバーライド関数
	bool Hit(const int nValue) override;
	virtual void ProcessLanding(void) override;	// 着地時処理
	virtual void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo) override;	// 攻撃時処理
	virtual void ChaseMove(float fMove);	// 追い掛け移動
	void ChangeToAttackState(void);			// 攻撃状態移行処理
	virtual void UpdateStateByType(void);	// 種類別状態更新処理
	void MotionSet(void) override;		// モーションの設定

	void Damage(void) override;		// ダメージ
	void Invincible(void);			// 無敵

	// 状態更新系
	void Spawn(void) override;		// スポーン

	typedef enum
	{
		MOTION_DEF = 0,	// ニュートラルモーション
		MOTION_WALK,	// 移動モーション
		MOTION_ATK,		// 攻撃
		MOTION_KNOCKBACK,		// やられモーション
		MOTION_MAX
	}MOTION;

	// メンバ変数
	int m_nCntInvicible;	// 無敵カウンター
	bool m_bInvicible;		// 無敵状態かの判定
};



#endif