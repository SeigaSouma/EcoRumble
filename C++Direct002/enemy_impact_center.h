//=============================================================================
// 
//  �g�N�����G�w�b�_�[ [enemy_impact_center.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYIMPACT_CENTER_H_
#define _ENEMYIMPACT_CENTER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "enemy.h"

// �O���錾

//==========================================================================
// �N���X��`
//==========================================================================
// �g�N�����G�N���X��`
class CEnemyImpactCenter : public CEnemy
{
public:

	CEnemyImpactCenter(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyImpactCenter();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

	CEnemyImpactCenter *GetEnemy(void);
protected:

private:

	// �I�[�o�[���C�h�֐�
	bool Hit(const int nValue) override;
	virtual void ProcessLanding(void) override;	// ���n������
	virtual void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo) override;	// �U��������
	virtual void ChaseMove(float fMove);	// �ǂ��|���ړ�
	void ChangeToAttackState(void);			// �U����Ԉڍs����
	virtual void UpdateStateByType(void);	// ��ޕʏ�ԍX�V����
	void MotionSet(void) override;		// ���[�V�����̐ݒ�

	void Damage(void) override;		// �_���[�W
	void Invincible(void);			// ���G

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

	// �����o�ϐ�
	int m_nCntInvicible;	// ���G�J�E���^�[
	bool m_bInvicible;		// ���G��Ԃ��̔���
};



#endif