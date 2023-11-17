//=============================================================================
// 
//  �v���C���[�w�b�_�[ [player.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _PLAYER_H_
#define _PLAYER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "objectChara.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define SLIDEMOVE		(2.05f)			// ��̈ړ���

// �O���錾
class CMotion;
class CShadow;
class CMagicCircle;
class CTargetPoint;
class CHP_Gauge;
class CDelusionWindow;
class CPowerGauge;
class CConfusionEffect;

//==========================================================================
// �N���X��`
//==========================================================================
// �v���C���[�N���X��`
class CPlayer : public CObjectChara
{
public:

	// �񋓌^��`
#if ACTION_MODE
	typedef enum
	{
		SHOTMODE_TRIGGER = 0,	// �g���K�[
		SHOTMODE_RELEASE,		// �����[�X
		SHOTMODE_PRESS,			// �v���X
		SHOTMODE_REPEAT,		// ���s�[�g
		SHOTMODE_MAX
	}SHOTMODE;

#else
	typedef enum
	{
		SHOTMODE_NONE = 0,	// �Ȃ�
		SHOTMODE_REPEAT,	// ���s�[�g
		SHOTMODE_MAX
	}SHOTMODE;

	typedef enum
	{
		ATK_DEBUG_POWER = 0,		// �p���[
		ATK_DEBUG_POWER2_SPEED1,	// �p���[2, �X�s�[�h1
		ATK_DEBUG_POWER2_CROWD1,	// �p���[2, �Q��1
		ATK_DEBUG_POWER1,			// �p���[�n�S��1
		ATK_DEBUG_CROWD,			// �Q��
		ATK_DEBUG_CROWD2_POWER1,	// �Q��2, �p���[1
		ATK_DEBUG_CROWD2_SPEED1,	// �Q��2, �X�s�[�h1
		ATK_DEBUG_CROWD1,			// �Q�̌n�S��1
		ATK_DEBUG_SPEED,			// �X�s�[�h
		ATK_DEBUG_SPEED2_POWER1,	// �X�s�[�h2, �p���[1
		ATK_DEBUG_SPEED2_CROWD1,	// �X�s�[�h2, �Q��1
		ATK_DEBUG_SPEED1,			// �X�s�[�h�n�S��1
		ATK_DIRECT,					// ���s�g�U��
		ATK_DEBUG_MAX
	}ATK_DEBUG;

	// ��Ԓ�`
	typedef enum
	{
		STATE_NONE = 0,		// �Ȃɂ��Ȃ�
		STATE_INVINCIBLE,	// ���G
		STATE_DMG,			// �_���[�W
		STATE_KNOCKBACK,	// �m�b�N�o�b�N
		STATE_DEAD,			// ��
		STATE_CONFUSION,	// ����
		STATE_ATTACK,		// �U������
		STATE_MAX
	}STATE;
#endif

	CPlayer(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CPlayer();

	static CPlayer *Create();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init(void);
	void Uninit(void);
	virtual void Update(void);
	void Draw(void);
	bool Hit(const int nValue);	// �q�b�g����
	int GetState(void) override;

	virtual void UninitByMode(void);
	void EnableConfusion(void);	// ������L���ɂ���
	bool IsConfuison(void) { return m_bConfusion; }
	void IsSwitchInWater(bool bFrag);	// ���̒��ɂ��锻��
	void ChangeWeapon(const int *pPoint);	// ����`�F���W����
	void Kill(void);	// ���S����

protected:
	// �񋓌^��`
	typedef enum
	{
		MOTION_DEF = 0,			// �j���[�g�������[�V����
		MOTION_WALK,			// �ړ����[�V����
		MOTION_SUPERATK,		// ���U��
		MOTION_DIRECTATK,		// ���s�g�U��
		MOTION_KNOCKBACK,		// ���ꃂ�[�V����
		MOTION_CONFUSION,		// ����
		MOTION_SPEEDWALK,		// �����ړ����[�V����
		MOTION_SLOWWALK,		// �����ړ����[�V����
		MOTION_MAX
	}MOTION;

	// �ړ��^�C�v
	enum WALKTYPE
	{
		WALKTYPE_NORMAL = 0,	// �ʏ�
		WALKTYPE_SPEED,			// ����
		WALKTYPE_SLOW,			// �x��
		WALKTYPE_MAX
	};

	void Collision(void);	// �����蔻��

	bool m_bJump;				// �W�����v�����ǂ���
	bool m_bKnockback;			// �m�b�N�o�b�N�����ǂ���
	bool m_bMove;				// �ړ������ǂ���
	bool m_bATK;				// �U�������ǂ���
	bool m_bMWait;				// ���@�ҋ@�����ǂ���
	bool m_bCharge;				// �`���[�W�����ǂ���
	bool m_bConfusion;			// ���������ǂ���
	bool m_bInWater;			// ���̒��ɂ��锻��
	float m_fMoveRatio;			// �ړ��̊���
	float m_fMoveSlide;			// �X���C�h�ړ���
	float m_fMoveSlideDest;		// �ڕW�̃X���C�h�ړ���
	int m_Shotmode;			// �ˌ��̎��
	int m_nCntWalk;				// ���s�J�E���^�[
	STATE m_state;			// ���
	WALKTYPE m_walkType;	// �ړ����
	CMotion *m_pMotion;		// ���[�V�����̏��
private:

	// �����o�֐�
	void UpdateState(void);	// ��ԍX�V����
	void KnockBack(void);	// �m�b�N�o�b�N
	void Damage(void);		// �_���[�W
	void Invincible(void);	// ���G
	virtual void Controll(void);	// ����
	void MotionSet(void);	// ���[�V�����̐ݒ�
	void Atack(void);		// �U��
	void Confusion(void);	// ����

	STATE m_Oldstate;			// �O��̏��
	D3DXCOLOR m_mMatcol;		// �}�e���A���̐F
	D3DXVECTOR3 m_posKnokBack;	// �m�b�N�o�b�N�̈ʒu
	D3DXVECTOR3 m_rotConfusion;	// �����̌���
	int m_nCntState;			// ��ԑJ�ڃJ�E���^�[
	int m_nCntConfusion;		// �����J�E���^�[
	int m_nLife;				// �̗�
	int m_nTexIdx;				// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nIdxXFile;			// X�t�@�C���̃C���f�b�N�X�ԍ�
	CShadow *m_pShadow;			// �e�̏��
	CMagicCircle *m_pMCircle;	// ���@�w�̏��
	CTargetPoint *m_pTargetP;	// �ڕW�̒n�_
	CHP_Gauge *m_pHPGauge;		// HP�Q�[�W�̏��
	CDelusionWindow *m_pDelusionWindow;	// �ϑz�E�B���h�E
	CConfusionEffect *m_pConfusionEffect;	// �����G�t�F�N�g�̏��
};



#endif