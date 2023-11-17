//=============================================================================
// 
//  �g�N�����G�w�b�_�[ [enemy_impact_side.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYIMPACT_SIDE_H_
#define _ENEMYIMPACT_SIDE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "enemy.h"

// �O���錾

//==========================================================================
// �N���X��`
//==========================================================================
// �g�N�����G�N���X��`
class CEnemyImpactSide : public CEnemy
{
public:

	CEnemyImpactSide(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyImpactSide();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

	CEnemyImpactSide *GetEnemy(void);
protected:

private:
	virtual void ProcessLanding(void) override;	// ���n������
	virtual void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo) override;	// �U��������
	virtual void ChaseMove(float fMove);	// �ǂ��|���ړ�
	void ChangeToAttackState(void);			// �U����Ԉڍs����
	virtual void UpdateStateByType(void);	// ��ޕʏ�ԍX�V����
	void MotionSet(void) override;		// ���[�V�����̐ݒ�
	
	// ��ԍX�V�n
	void Spawn(void) override;		// �X�|�[��

	typedef enum
	{
		MOTION_DEF = 0,	// �j���[�g�������[�V����
		MOTION_WALK,	// �ړ����[�V����
		MOTION_ATK,		// �U��
		MOTION_KNOCKBACK,		// ���ꃂ�[�V����
		MOTION_MAX
	}MOTION;

	// ��ԍX�V�n
};



#endif