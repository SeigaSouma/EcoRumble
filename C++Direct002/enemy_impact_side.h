//=============================================================================
// 
//  波起こし敵ヘッダー [enemy_impact_side.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYIMPACT_SIDE_H_
#define _ENEMYIMPACT_SIDE_H_	// 二重インクルード防止

#include "main.h"
#include "enemy.h"

// 前方宣言

//==========================================================================
// クラス定義
//==========================================================================
// 波起こし敵クラス定義
class CEnemyImpactSide : public CEnemy
{
public:

	CEnemyImpactSide(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyImpactSide();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

	CEnemyImpactSide *GetEnemy(void);
protected:

private:
	virtual void ProcessLanding(void) override;	// 着地時処理
	virtual void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo) override;	// 攻撃時処理
	virtual void ChaseMove(float fMove);	// 追い掛け移動
	void ChangeToAttackState(void);			// 攻撃状態移行処理
	virtual void UpdateStateByType(void);	// 種類別状態更新処理
	void MotionSet(void) override;		// モーションの設定
	
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

	// 状態更新系
};



#endif