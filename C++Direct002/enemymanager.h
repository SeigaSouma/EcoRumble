//=============================================================================
// 
//  �G�̃}�l�[�W���w�b�_�[ [enemymanager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYMANAGER_H_
#define _ENEMYMANAGER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "constans.h"
#include "wavemanager.h"

// �O���錾
class CEnemy;
class CBoss;

//==========================================================================
// �N���X��`
//==========================================================================
// �G�̃}�l�[�W���N���X��`
class CEnemyManager
{
protected:

	// �\���̒�`
	struct EnemyData
	{
		int nType;				// �L�����N�^�[���
		int nParent;			// �e�̃C���f�b�N�X
		int nWaitTime;			// �ҋ@����
		D3DXVECTOR3 pos;		// �ʒu
	};

	struct Pattern
	{
		int nNumEnemy;	// �G�̐�
		EnemyData EnemyData[mylib_const::MAX_PATTEN_ENEMY];
	};

public:

	// �񋓌^��`
	enum SPAWNTYPE
	{
		SPAWNTYPE_NORMAL = 0,	// �ʏ�
		SPAWNTYPE_ORDER,		// ����
		SPAWNTYPE_CIRCLE,		// �~��
		SPAWNTYPE_CLOCK,		// ���v���
		SPAWNTYPE_BOTHCLOCK,	// �����
		SPAWNTYPE_MAX
	};

	// �񋓌^��`
	enum PATTERN
	{
		PATTERN_POWERONE = 0,
		PATTERN_CROWD,
		PATTERN_CROWD2,
		PATTERN_POWER_CROWD,
		PATTERN_JUMP,
		PATTERN_BOSS,
		PATTERN_BOMB,
		PATTERN_MAX
	};

	// �񋓌^��`
	enum STATE
	{
		STATE_NONE = 0,		// �����Ȃ����
		STATE_COMBOANIM,	// �R���{���̃A�j���[�V����
		STATE_MAX
	};

	CEnemyManager();
	~CEnemyManager();

	HRESULT Init(void);
	void Uninit(void);
	virtual void Update(void);

	static CEnemyManager *Create(const std::string pTextFile);
	HRESULT ReadText(const std::string pTextFile);	// �O���t�@�C���ǂݍ��ݏ���
	void SetEnemy(int nPattern, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 rotBase, int movetype = 0);	// �G�z�u
	void SetEnemy(int nIdx, int nPattern, int nCntEnemy, int nParent, CWaveManager::SetInfo setinfo, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int movetype = 0);	// �G�z�u
	void SetBoss(int nPattern, D3DXVECTOR3 pos, D3DXVECTOR3 rot);	// �{�X�z�u
	static int GetPatternNum(void);
	static void Release(int nIdx);		// �j��
	static void ReleaseBoss(int nIdx);	// �j��
	void SetWaveEnemy(int nStage, int nWave);		// �E�F�[�u���̔z�u����
	void SetSpawnWave(int nStage, int nWave);		// �X�|�[���̋N���ݒ�
	void SetEnemyInfo(CWaveManager::SetInfo *info, int nStage, int nWave, int nSpawnNum);	// ���o�^
	STATE GetState(void) { return m_state; }	// ��Ԏ擾

	void SetKillFlag(void);		// �|�����t���O��ݒ�
	int GetIntervalKill(void);	// �|�����Ԋu�擾
	int GetNumIntervalKill(void);	// �Ԋu���ɓ|�������擾
	int GetNumAll(void);	// �G�̑����擾

protected:

	void UpdateComboAnimation(void);	// �R���{���̉��o

	static int m_nNumAll;		// �G�̑���
	static CEnemy *m_pEnemy[mylib_const::MAX_OBJ];						// �G�ւ̃|�C���^
	static Pattern m_aPattern[mylib_const::MAX_PATTEN_ENEMY];			// �z�u�̎��
	static CWaveManager::SetStageInfo m_aSetEnemyInfo[mylib_const::MAX_WAVE_ENEMY];	// �G�̔z�u���
	static std::string sMotionFileName[mylib_const::MAX_PATTEN_ENEMY];	// ���[�V�����t�@�C����
	static int m_nParent[mylib_const::MAX_OBJ];		// �G�̐e�|�C���^
	static int m_nNumTotalKill;	// �܂Ƃ߂ē|������
	static int m_nIntervalKill;	// �|�����J�E���g�̃C���^�[�o��
	static bool m_bHitStop;		// �q�b�g�X�g�b�v�̔���
	static STATE m_state;		// ���

private:

	void SetSpawnWaveNormal(int nStage, int nWave, int nCntPattern, int *pParent);		// �ʏ�N��
	void SetSpawnWaveOrder(int nStage, int nWave, int nCntPattern, int *pParent);		// ���ԗN��
	void SetSpawnWaveCircle(int nStage, int nWave, int nCntPattern, int *pParent);		// �~��C�N��
	void SetSpawnWaveClock(int nStage, int nWave, int nCntPattern, int *pParent);		// ���v���N��
	void SetSpawnWaveBothClock(int nStage, int nWave, int nCntPattern, int *pParent);	// �����N��
	void SetSlashImpact(D3DXVECTOR3 pos);	// �X���b�V���Ռ��g�ݒ�

	static int m_nCntSpawn;		// �o���J�E���g
	static int m_nPatternNum;	// �o���p�^�[����
	static int m_nNumChara;		// �G�̎�ނ̑���
	static CBoss *m_pBoss[mylib_const::MAX_OBJ];						// �{�X�ւ̃|�C���^
	static bool m_bLoadPattern;	// �p�^�[���ǂݍ��ݔ���
};



#endif