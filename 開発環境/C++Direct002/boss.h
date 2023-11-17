//=============================================================================
// 
//  �{�X�w�b�_�[ [boss.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _BOSS_H_
#define _BOSS_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "objectChara.h"

// �O���錾
class CMotion;
class CHP_Gauge;

//==========================================================================
// �N���X��`
//==========================================================================
// �v���C���[�N���X��`
class CBoss : public CObjectChara
{
public:

	// �񋓌^��`
	typedef enum
	{
		STATE_NONE = 0,	// �Ȃɂ��Ȃ�
		STATE_DMG,		// �_���[�W
		STATE_KNOCKBACK,		// �m�b�N�o�b�N
		STATE_DEAD,		// ��
		STATE_PLAYERCHASE,	// �v���C���[�ǂ��|��
		STATE_ATTACK,		// �U������
		STATE_MAX
	}STATE;

	CBoss(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CBoss();

	static CBoss *Create(int nIdx, const char *pFileName, D3DXVECTOR3 pos);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	HRESULT Init(const char *pFileName);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	bool Hit(const int nValue) override;
	bool Hit(const int nValue, D3DXVECTOR3 move) override;

	void Kill(void);
	CBoss *GetBoss(void);
	static int GetNumAll(void) { return m_nNumAll; }	// �����擾

	int GetState(void) override;
	int GetCharaType(void) override;	// ��ގ擾

protected:

private:

	typedef enum
	{
		MOTION_DEF = 0,	// �j���[�g�������[�V����
		MOTION_WALK,	// �ړ����[�V����
		MOTION_ATK,		// �U��
		MOTION_MAX
	}MOTION;

	enum ACTION
	{
		ACTION_TACKLE = 0,	// �^�b�N��
		ACTION_JUMP,		// �W�����v
		ACTION_SP_JUMP,		// ���W�����v
		ACTION_MAX
	};
	
	void UpdateState(void);	// ��ԍX�V����
	void Collision(void);	// �����蔻��
	void MotionSet(void);	// ���[�V�����̐ݒ�
	void Atack(void);		// �U��
	void BombExplosion(void);	// �{�����j

	ACTION m_action;		// �s��
	int m_nCntAction;		// �s���J�E���^�[
	int m_nCntHit;			// �q�b�g�J�E���^�[
	int m_nCntHitAfter;		// �q�b�g��̃J�E���g

	STATE m_state;			// ���
	STATE m_Oldstate;		// �O��̏��
	D3DXVECTOR3 m_ImpalseMove;	// ������Ԉړ���
	int m_nCntState;		// ��ԑJ�ڃJ�E���^�[
	int m_nBeforCntState;	// �O�̏�ԑJ�ڃJ�E���^�[
	int m_nLife;			// �̗�
	int m_nTexIdx;			// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nIdxManager;		// �}�l�[�W���̃C���f�b�N�X�ԍ�
	bool m_bJump;			// �W�����v�����ǂ���
	bool m_bMove;			// �ړ������ǂ���
	bool m_bATK;			// �U�������ǂ���
	D3DXCOLOR m_mMatcol;	// �}�e���A���̐F
	CMotion *m_pMotion;		// ���[�V�����̏��
	CHP_Gauge *m_pHPGauge;		// HP�Q�[�W�̏��
	static int m_nNumAll;		// �G�̑���
};



#endif