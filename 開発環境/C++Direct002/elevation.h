//=============================================================================
// 
//  �N���w�b�_�[ [elevation.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ELEVATION_H_
#define _ELEVATION_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object3DMesh.h"
#include "wavemanager.h"

// �O���錾
class CTargetPoint;

//==========================================================================
// �N���X��`
//==========================================================================
// �N���N���X��`
class CElevation : public CObject3DMesh
{
public:

	// �񋓌^��`
	typedef enum
	{
		DAMAGE_RATIO = 0,	// ����
		DAMAGE_EQUAL,		// ������
		DAMAGE_MAX
	}DAMAGE;

	// �񋓌^��`
	typedef enum
	{
		WAVETYPE_RIPPLE = 0,	// �g��
		WAVETYPE_DIRECT,		// ���s
		WAVETYPE_IMPACT,		// �Ռ��g
		WAVETYPE_THRUST,		// �˂��グ
		WAVETYPE_RAISE,			// ����グ
		WAVETYPE_MAX
	}WAVETYPE;

	enum TYPE
	{
		TYPE_PLAYER = 0,	// �v���C���[
		TYPE_ENEMY,			// �G
		TYPE_NATURAL,		// ���R
		TYPE_EFFECT,		// �G�t�F�N�g
		TYPE_SPAWN,			// �X�|�[��
		TYPE_MAX
	};

	enum POSTYPE
	{
		POSTYPE_UP = 0,
		POSTYPE_DOWN,
		POSTYPE_RIGHT,
		POSTYPE_LEFT,
		POSTYPE_MAX
	};

private:
	// �񋓌^��`
	typedef enum
	{
		STATE_NONE = 0,	// �Ȃɂ��Ȃ�
		STATE_SET,		// �ݒ�
		STATE_WAVE,		// �g�ł�
		STATE_MAX
	}STATE;

	// �񋓌^��`
	typedef enum
	{
		FADESTATE_INCREMENT = 0,
		FADESTATE_NONE,
		FADESTATE_DECREMENT,
		FADESTATE_MAX
	}FADESTATE;

	struct Info
	{
		std::string TextureFileName;	// �e�N�X�`���t�@�C����
		D3DXVECTOR3 pos;		// �ʒu
		float fWidthLength;		// ������
		float fHeightLength;	// �c����
		int nWidthBlock;		// ��������
		int nHeightBlock;		// �c������
		D3DXVECTOR3 *pVtxPos;	// ���_���W
		D3DXVECTOR3 *pStageVtxPos[mylib_const::MAX_STAGE];	// �X�e�[�W���̒��_���W
	};

	struct WaveInfo
	{
		D3DXVECTOR3 pos;		// �ʒu
		D3DXVECTOR3 posOrigin;	// �ʒu
		D3DXVECTOR3 rot;		// ����
		D3DXVECTOR3 move;		// �ړ���
		D3DXVECTOR2 fRange;		// �͈�
		float fRatio;			// �Ԋu
		float fStrength;		// ����
		float fStrengthOrigin;	// ���̋���
		float fWave;			// �g�̍���
		WAVETYPE type;			// ���
		TYPE playertype;		// �v���C���[�^�C�v
		float fLength;			// �g�̍���
		int nLevel;				// ���x��
	};

	struct SWave
	{
		WaveInfo sSetWave;							// �g���
		CWaveManager::SetInfo WaveManagerInfo;		// �E�F�[�u�}�l�[�W���̏��
		int nMyIdx;									// �����̃C���f�b�N�X�ԍ�
		int nParentIdx;								// �e�̃C���f�b�N�X�ԍ�
		int nNumEnemy;								// ���Ԗڂ̓G��
		int nWaitTime;								// �ҋ@����
		STATE state;								// ���
		int nMoveLife;								// �ړ�����
		int nOriginMoveLife;						// �ړ�����
		int nCntState;								// ��ԑJ�ڃJ�E���^�[
		int nOriginCntState;						// ��ԑJ�ڃJ�E���^�[
		float fMaxWaveHeight;						// �ő卂��
		float fFrameMaxWaveHeight;					// �t���[�����̍ő卂��
		float fWave[mylib_const::MAX_VTX];			// �g
		D3DXVECTOR3 VtxPos[mylib_const::MAX_VTX];	// ���_���W
		D3DXVECTOR3 WaveMove[mylib_const::MAX_VTX];	// �g�ړ���
	};

	// �X�e�[�W�؂�ւ����̏��
	struct SFadeWave
	{
		float *pRot;		// �g�̌���
		float fLength;		// ���a�̒���
		FADESTATE state;	// ���
	};

public:

	CElevation(int nPriority = 1);
	~CElevation();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init(void);
	virtual HRESULT Init(const char *pText);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void SetVtx(void);

	void Release(void);
	bool IsHit(D3DXVECTOR3& pos);	// �����������̔���
	float Slide(D3DXVECTOR3& pos);	// �X���C�h�̔���
	void Damage(D3DXVECTOR3 pos, float fRange, float fStrength, DAMAGE damage);	// �_���[�W
	void SetWave(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 move, D3DXVECTOR2 fRange, float fRatio, float fStrength, float fWave, int nCntState, WAVETYPE type, TYPE playertype, int nLevel);	// �g�̐ݒ�
	void SetSpawnWave(int nIdx, int nParentID, int nNumEnemy, int nWaitTime, CWaveManager::SetInfo info, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 Range, float fStrength, int nCntState);	// �X�|�[���g�̐ݒ�
	bool IsEdit(void);	// �G�f�B�b�g�̔���
	void Save(void);	// �Z�[�u
	HRESULT Load(const char *pText);	// ���[�h
	void SetWaveNaturalwave(int nStage, int nWave);		// �E�F�[�u���̔z�u����
	void SetNaturalWaveInfo(CWaveManager::SetInfo *info, int nStage, int nWave, int nSpawnNum);	// ���o�^
	virtual void StageSwitch(int nNowStage, int nNextStage, int nNowLife, int nMaxLife);

	static CElevation *Create(const char *pText, CObject::TYPE type);
	CElevation *GetElevation(void);
	float GetHeight(D3DXVECTOR3& pos, D3DXVECTOR3& move, bool &bLand, CObject::TYPE type);	// �����擾
protected:

	Info m_aInfo;	// �N���̏��
	SFadeWave m_sFadeInfo;							// �t�F�[�h���̏��
	float m_fBrushStrength;							// �u���V����
	float m_fBrushRange;							// �͈�
	bool m_bEdit;									// �G�f�B�b�gON/OFF
	CTargetPoint *m_pTargetP;						// �ڕW�̒n�_
	D3DXVECTOR3 m_VtxPos[mylib_const::MAX_VTX];		// ���_���W

private:

	void UpdateState(void);				// ��ԍX�V����
	void UPVtxField(D3DXVECTOR3 pos);	// ���_�グ����(�f�o�b�O)
	void Wave(int nCntWave);			// �g�ł��X�V����
	void UpdateRippleWave(int nCntWave, int nVtxPoint);	// �g��X�V����
	void UpdateDirectWave(int nCntWave, int nVtxPoint);	// ���s�g�X�V����
	void UpdateImpactWave(int nCntWave, int nVtxPoint);	// �Ռ��g�X�V����
	void UpdateThrustWave(int nCntWave, int nVtxPoint);	// �˂��グ�X�V����
	void UpdateRaiseField(int nCntWave, int nVtxPoint);	// ����グ�X�V����
	void Collision(int nCntWave);		// �����蔻�菈��
	void CollisionDirectWave(int nCntWave, CObject *pObj);		// �����g�̓����蔻�菈��
	void CollisionImpactWave(int nCntWave, CObject *pObj);		// �~�`�g�̓����蔻�菈��
	void SetEnemy(int nCntWave);	// �G�̐ݒ�

	SWave m_aWave[mylib_const::MAX_WAVE];			// �g�̏��
	int m_nTexIdx;									// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nSpawnByFrame;							// �t���[�����̃X�|�[���J�E���g
	bool m_bSpawnByFrame;							// �t���[�����̃X�|�[������
	bool m_bHitStop_Direct;							// �����g�̃q�b�g�X�g�b�v����
	bool m_bHitStop_Impact;							// �~�`�g�̃q�b�g�X�g�b�v����
	static CWaveManager::SetStageInfo m_aSetInfo[mylib_const::MAX_WAVE_ENEMY];	// �G�̔z�u���
	static bool m_bLoadInfo;						// ���ǂݍ��ݔ���
	static bool m_bSetSpawnCamera;					// �X�|�[�����̃J�����ݒ蔻��
	static int m_nCntSetSpawn;						// �X�|�[�����̃J�����J�E���g
};



#endif