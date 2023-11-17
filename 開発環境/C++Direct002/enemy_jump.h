//=============================================================================
// 
//  �W�����v�G�w�b�_�[ [enemy_jump.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYJUMP_H_
#define _ENEMYJUMP_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "enemy.h"

// �O���錾

//==========================================================================
// �N���X��`
//==========================================================================
// �v���C���[�N���X��`
class CEnemyJump : public CEnemy
{
public:

	CEnemyJump(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyJump();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

	CEnemyJump *GetEnemy(void);
protected:

private:
	virtual void ProcessLanding(void) override;	// ���n������
	virtual void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo) override;	// �U��������
	virtual void ChaseMove(float fMove);	// �ǂ��|���ړ�
	void ChangeToAttackState(void);			// �U����Ԉڍs����
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

	int m_nCntWaitJump;		// �W�����v�̑ҋ@����
	D3DXVECTOR3 m_rotParts;	// �p�[�c��]�p
};



#endif