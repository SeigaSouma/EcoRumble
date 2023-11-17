//=============================================================================
// 
//  �v���C���[�w�b�_�[ [enemy.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMY_H_
#define _ENEMY_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "objectChara.h"
#include "motion.h"
#include "enemymanager.h"
#include "resultmanager.h"

// �O���錾
class CHP_Gauge;
class CShadow;
class CEffect3D;

//==========================================================================
// �N���X��`
//==========================================================================
// �v���C���[�N���X��`
class CEnemy : public CObjectChara
{
public:
	// �񋓌^��`
	typedef enum
	{
		TYPE_POWER = 0,	// �p���[
		TYPE_CROWD,		// �Q��
		TYPE_JUMP,		// �X�s�[�h
		TYPE_BOMB,		// �{��
		TYPE_SPEED,		// �X�s�[�h
		TYPE_IMPACT,	// �Ռ�
		TYPE_IMPACTCENTER,	// �^�񒆂̏Ռ�
		TYPE_MAX
	}TYPE;

	enum MOVETYPE
	{
		MOVETYPE_RAMDOM = 0,	// �����_��
		MOVETYPE_ROTATION,		// ��]
		MOVETYPE_FORMATION,		// ����
		MOVETYPE_PLAYERRUSH,	// �v���C���[�ˌ�
		MOVETYPE_MAX
	};

	// �񋓌^��`
	typedef enum
	{
		STATE_NONE = 0,		// �Ȃɂ��Ȃ�
		STATE_SPAWN,		// �X�|�[��
		STATE_DMG,			// �_���[�W
		STATE_KNOCKBACK,	// �m�b�N�o�b�N
		STATE_DEAD,			// ��
		STATE_FADEOUT,		// �t�F�[�h�A�E�g
		STATE_RETURNBASE,	// ���_�ɖ߂�
		STATE_PLAYERSEARCH,	// �v���C���[�T��
		STATE_PLAYERCHASE,	// �v���C���[�ǂ��|��
		STATE_PARENTCHASE,	// �v���C���[�ǂ��|��
		STATE_ATTACK,		// �U��
		STATE_TURRETATK,	// �^���b�g�U��
		STATE_TURRETWAIT,	// �^���b�g�ҋ@
		STATE_BURY,			// ���܂��Ă���
		STATE_OCCUR,		// �N���オ��
		STATE_IMPALSE,		// �Ռ��Ŕ��
		STATE_WAIT,			// �ҋ@
		STATE_MAX
	}STATE;

	CEnemy(int nPriority = mylib_const::ENEMY_PRIORITY);
	virtual ~CEnemy();

	static CEnemy *Create(int nIdx, const char *pFileName, D3DXVECTOR3 pos, TYPE type = TYPE_POWER, int nCharacter = -1);

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;
	int GetState(void) override;
	void SetState(int state) override;		// ��Ԑݒ�
	void SetState(int state, int nCntState) override;	// ��Ԑݒ�
	int GetCharaType(void) override;	// ��ގ擾
	virtual bool Hit(const int nValue);

	HRESULT RoadText(const char *pFileName);
	virtual void Kill(void);
	void SetParent(CEnemy *pParent);		// �e�̃|�C���^�ݒ�
	void SetFormationPos(D3DXVECTOR3 posFormation, D3DXVECTOR3 posParent);	// ����̈ʒu
	void SetOriginRotation(D3DXVECTOR3 rot);	// ���̌���
	void SetMoveType(MOVETYPE movetype);	// �s���̎�ސݒ�
	void SetSpawnType(CEnemyManager::SPAWNTYPE type);	// �s���̎�ސݒ�
	CEnemy *GetEnemy(void);

protected:

	enum COLORTYPE
	{
		COLORTYPE_NORMAL = 0,	// �m�[�}��
		COLORTYPE_POISON= 7,	// �ŃL�m�R
		COLORTYPE_LUCKY,		// �΂�1UP
		COLORTYPE_CONFUSION,	// �����U��
		COLORTYPE_SWIFTFOOT,	// �r�͂₢
		COLORTYPE_TUTORIAL,		// �`���[�g���A��
		COLORTYPE_MAX
	};

	// ���[�V�����̔���
	struct SMotionFrag
	{
		bool bJump;			// �W�����v�����ǂ���
		bool bATK;			// �U�������ǂ���
		bool bKnockback;	// �m�b�N�o�b�N�����ǂ���
		bool bMove;			// �ړ������ǂ���
	};

	// ����̍\���̒�`
	struct SFormationInfo
	{
		D3DXVECTOR3 pos;	// ����̈ʒu
		float fAngle;		// ����
		float fLength;		// ����
	};

	// �}�N����`
#define SPAWN_GRAVITY	(-0.25f)

	virtual void ProcessLanding(void);	// ���n������
	virtual void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo);	// �U��������
	virtual void ChaseMove(float fMove);	// �ǂ��|���ړ�

	// ��ԍX�V�n
	virtual void Spawn(void);				// �X�|�[��
	virtual void Bury(void);				// ���܂�
	virtual void Occur(void);				// �N���オ��
	virtual void Impalse(void);				// �Ռ�
	virtual void Damage(void);				// �_���[�W
	virtual void Dead(void);				// ���S
	virtual void FadeOut(void);				// �t�F�[�h�A�E�g
	virtual void KnockBack(void);			// �m�b�N�o�b�N
	virtual void PlayerSearch(void);		// �v���C���[�T��
	virtual void PlayerChase(void);			// �v���C���[�Ǐ]
	virtual void ParentChase(void);			// �e�Ǐ]
	virtual void ReturnBase(void);			// ���_�ɋA��
	virtual void StateAttack(void);			// �U������
	virtual void TurretAttack(void);		// �^���b�g�U������
	virtual void TurretWait(void);			// �^���b�g�ҋ@����
	virtual void TriggerChasePlayer(void);	// �v���C���[�Ǐ]ON�ɂ���g���K�[
	virtual void ChangeToAttackState(void);	// �U����Ԉڍs����
	virtual void StateWait(void);			// �ҋ@����

	STATE m_state;							// ���
	STATE m_Oldstate;						// �O��̏��
	int m_nCntState;						// ��ԑJ�ڃJ�E���^�[
	D3DXVECTOR3 m_posOrigin;				// �ŏ��̈ʒu
	D3DXVECTOR3 m_posKnokBack;	// �m�b�N�o�b�N�̈ʒu
	CEnemyManager::SPAWNTYPE m_SpawnType;	// �o���̎��
	SMotionFrag m_sMotionFrag;				// ���[�V�����̃t���O
	CHP_Gauge *m_pHPGauge;					// HP�Q�[�W�̏��
	CMotion *m_pMotion;						// ���[�V�����̏��
	COLORTYPE m_colorType;					// �F���Ƃ̎��
	CResultManager::ADDTYPE m_AddType;		// �X�R�A�̉��Z���
	CEnemy *m_pParent;		// �e�̃|�C���^
	D3DXCOLOR m_mMatcol;	// �}�e���A���̐F
private:

	typedef enum
	{
		MOTION_DEF = 0,		// �j���[�g�������[�V����
		MOTION_WALK,		// �ړ����[�V����
		MOTION_ATK,			// �U��
		MOTION_KNOCKBACK,	// ���ꃂ�[�V����
		MOTION_FADEOUT,		// �y�A��
		MOTION_MAX
	}MOTION;
	
	void UpdateState(void);					// ��ԍX�V����
	virtual void UpdateStateByType(void);	// ��ޕʏ�ԍX�V����
	void UpdateByType(void);				// ��ޕʍX�V����
	void Collision(void);					// �����蔻��
	virtual void MotionSet(void) = 0;		// ���[�V�����̐ݒ�
	void RegistrChild(CEnemy *pChild);
	void ResetChild(CEnemy *pChild);
	void Atack(void);		// �U��

	TYPE m_type;			// ���
	MOVETYPE m_MoveType;	// �s���̎��
	SFormationInfo m_sFormationInfo;	// ����̏��
	D3DXVECTOR3 m_rotOrigin;	// �ŏ��̌���
	int m_nTexIdx;			// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nNumChild;		// �q�̐�
	int m_nIdxManager;		// �}�l�[�W���̃C���f�b�N�X�ԍ�
	int m_nSurvivalLife;	// ��������
	int m_nSurvivalLifeOrigin;	// ��������
	bool m_bAddScore;		// �X�R�A���Z���邩�̔���
	int m_nBallastEmission;	// ���I�̔����J�E���^�[
	CEnemy *m_pChild[mylib_const::MAX_ENEMY];	// �q�̃|�C���^
	CShadow *m_pShadow;			// �e�̏��
};



#endif