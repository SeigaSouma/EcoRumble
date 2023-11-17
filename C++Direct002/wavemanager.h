//=============================================================================
// 
//  �E�F�[�u�Ǘ��w�b�_�[ [wavemanager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _WAVEMANAGER_H_
#define _WAVEMANAGER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "constans.h"

// �O���錾
class CEnemy;
class CBoss;

//==========================================================================
// �N���X��`
//==========================================================================
// �E�F�[�u�Ǘ��N���X��`
class CWaveManager
{
public:

	// �ݒu���
	struct SetInfo
	{
		int nType;			// ���
		int nBase;			// ���Ԗڂ̈ʒu��
		int nMoveType;		// �s���̎��
		int nSpawnType;		// �o���̎��
		int nSpawnNum;		// �X�|�[�����鐔
		int nChara;			// �L�����̎��
		D3DXVECTOR3 pos;	// �ʒu
		D3DXVECTOR3 rot;	// ����
	};

	// �o�����
	struct SetWaveInfo
	{
		SetInfo Info[mylib_const::MAX_OBJ];	// �ݒu���
		int nSetNum;	// �ݒu���鐔
	};

	// �X�e�[�W���
	struct SetStageInfo
	{
		SetWaveInfo aWaveInfo[mylib_const::MAX_WAVE_ENEMY];	// �E�F�[�u�̏��
	};

	CWaveManager();
	~CWaveManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	static CWaveManager *Create(const std::string pTextFile);
	HRESULT ReadText(const std::string pTextFile);	// �O���t�@�C���ǂݍ��ݏ���
	void SetEnableWaveEnd(void);	// �I���̃t���OON
	void SetDisableWaveEnd(void);	// �I���̃t���OOFF
	void SetEnableStageEnd(void);	// �I���̃t���OON
	void SetDisableStageEnd(void);	// �I���̃t���OOFF
	bool IsWaveFrag(void);		// �I���̃t���O�擾
	bool IsStageFrag(void);		// �X�e�[�W�I���̃t���O�擾
	int GetNowWave(void);				// ���݂̃E�F�[�u�擾
	int GetMaxWave(void);				// ���݂̃X�e�[�W�̃E�F�[�u���擾
	int GetNowStage(void);				// ���݂̃X�e�[�W�擾
	int GetMaxStage(void);				// �X�e�[�W���擾
	int GetRespawnThreshold(void);		// ���X�|�[���̋K��l�擾

private:

	int m_nNowWave;		// ���݂̃E�F�[�u
	int m_nNowStage;	// ���݂̃X�e�[�W
	bool m_bEndWave;	// �E�F�[�u�I���̃t���O
	bool m_bEndStage;	// �X�e�[�W�I���̃t���O
	int m_nNumWave[mylib_const::MAX_STAGE];	// �E�F�[�u�̑���
	int m_nRespawnThreshold[mylib_const::MAX_STAGE][mylib_const::MAX_WAVE_ENEMY];	// �E�F�[�u���̓G���X�|�[���K��l
	int m_nNumStage;	// �X�e�[�W�̑���
};



#endif