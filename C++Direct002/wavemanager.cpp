//=============================================================================
// 
//  �G�̃}�l�[�W������ [wavemanager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "debugproc.h"
#include "wavemanager.h"
#include "calculation.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "enemymanager.h"
#include "elevation.h"
#include "fade.h"
#include "stageswitch.h"
#include "camera.h"
#include "sound.h"

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CWaveManager::CWaveManager()
{
	// �l�̃N���A
	m_nNowWave = 0;			// ���݂̃E�F�[�u
	m_nNowStage = 0;		// ���݂̃X�e�[�W
	memset(&m_nNumWave[0], 0, sizeof(m_nNumWave));	// �E�F�[�u�̑���
	//m_nNumWave = 0;		// �E�F�[�u�̑���
	m_nNumStage = 0;		// �X�e�[�W�̑���
	m_bEndWave = false;		// �E�F�[�u�I���̃t���O
	m_bEndStage = false;	// �X�e�[�W�I���̃t���O
	memset(&m_nRespawnThreshold[0], 0, sizeof(m_nRespawnThreshold));	// �E�F�[�u���̓G���X�|�[���K��l
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CWaveManager::~CWaveManager()
{

}

//==========================================================================
// ��������
//==========================================================================
CWaveManager *CWaveManager::Create(const std::string pTextFile)
{
	// �����p�̃I�u�W�F�N�g
	CWaveManager *pModel = NULL;

	if (pModel == NULL)
	{// NULL��������

		// �������̊m��
		pModel = DEBUG_NEW CWaveManager;

		if (pModel != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			HRESULT hr = pModel->Init();

			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}
		}

		return pModel;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CWaveManager::Init(void)
{
	HRESULT hr;

	// �o�����ǂݍ���
	switch (CManager::GetMode())
	{
	case CScene::MODE_TITLE:
		hr = ReadText("data\\TEXT\\set_enemy_info_title.txt");
		break;

	case CScene::MODE_TUTORIAL:
		hr = ReadText("data\\TEXT\\set_enemy_info_tutorial.txt");
		break;

	case CScene::MODE_GAME:
		hr = ReadText("data\\TEXT\\set_enemy_info.txt");
		break;

	case CScene::MODE_RESULT:
		return S_OK;
		break;

	case CScene::MODE_RANKING:
		hr = ReadText("data\\TEXT\\set_enemy_info_title.txt");
		break;

	default:
		return E_FAIL;
		break;
	}

	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	// �E�F�[�u�I���̃t���O���Ă�
	m_bEndWave = true;

	// �X�e�[�W�I���̃t���O���Ă�
	m_bEndStage = false;

	// �e��ϐ�������
	m_nNowWave = -1;
	m_nNowStage = 0;

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CWaveManager::Uninit(void)
{

}

//==========================================================================
// �X�V����
//==========================================================================
void CWaveManager::Update(void)
{
	if (CManager::GetMode() != CScene::MODE_GAME)
	{
		return;
	}

	if (m_bEndWave == false)
	{// �E�F�[�u����������
		return;
	}

	if (m_bEndStage == false)
	{// �X�e�[�W�ύX��
		return;
	}

	// �E�F�[�u��i�߂�
	if (m_nNowWave < mylib_const::MAX_WAVE_ENEMY)
	{
		m_nNowWave++;
	}

	if (m_nNumWave[m_nNowStage] == NULL)
	{// NULL��������
		int n = 0;
		return;
	}

	if (m_nNumWave[m_nNowStage] <= m_nNowWave)
	{// �E�F�[�u�̏I�[�ɗ�����

		// ���݂̃X�e�[�W���Z
		m_nNowStage++;

		// ���݂̃E�F�[�u���Z�b�g
		m_nNowWave = -1;

		// �E�F�[�u���ɕύX����
		m_bEndWave = false;

		// �X�e�[�W�ύX���ɂ���
		m_bEndStage = false;

		if (m_nNumStage <= m_nNowStage)
		{// �X�e�[�W�̏I�[�ɗ�����

			// ���[�h�ݒ�
			CManager::GetFade()->SetFade(CScene::MODE_RESULT);
			return;
		}
		else
		{// �܂��X�e�[�W������

			// �X�e�[�W�؂�ւ�����
			CStageSwitch::Create();
		}
	}

	// �G�z�u
	CGame::GetEnemyManager()->SetSpawnWave(m_nNowStage, m_nNowWave);

	// ���R�g�z�u
	CGame::GetObject3DMesh()->SetWaveNaturalwave(m_nNowStage, m_nNowWave);

	// �E�F�[�u���ɕύX����
	m_bEndWave = false;
}

//==========================================================================
// �E�F�[�u�I���̃t���OON
//==========================================================================
void CWaveManager::SetEnableWaveEnd(void)
{
	m_bEndWave = true;
}

//==========================================================================
// �E�F�[�u�I���̃t���OOFF
//==========================================================================
void CWaveManager::SetDisableWaveEnd(void)
{
	m_bEndWave = false;
}

//==========================================================================
// �X�e�[�W�I���̃t���OON
//==========================================================================
void CWaveManager::SetEnableStageEnd(void)
{
	m_bEndStage = true;
}

//==========================================================================
// �X�e�[�W�I���̃t���OOFF
//==========================================================================
void CWaveManager::SetDisableStageEnd(void)
{
	m_bEndStage = false;
}

//==========================================================================
// �I���̃t���O�擾
//==========================================================================
bool CWaveManager::IsWaveFrag(void)
{
	return m_bEndWave;
}

//==========================================================================
// �X�e�[�W�I���̃t���O�擾
//==========================================================================
bool CWaveManager::IsStageFrag(void)
{
	return m_bEndStage;
}

//==========================================================================
// �e�L�X�g�ǂݍ��ݏ���
//==========================================================================
HRESULT CWaveManager::ReadText(const std::string pTextFile)
{
	FILE *pFile = NULL;	// �t�@�C���|�C���^��錾

	// �t�@�C�����J��
	pFile = fopen(pTextFile.c_str(), "r");

	if (pFile == NULL)
	{//�t�@�C�����J�����ꍇ
		return E_FAIL;
	}

	char aComment[MAX_COMMENT];	// �R�����g
	int nCntSpawnEnemy = 0;			// ���̖ڂ�
	int nCntSpawnWave = 0;			// ���̖ڂ�

	while (1)
	{// END_SCRIPT������܂ŌJ��Ԃ�

		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		// �e�X�e�[�W�̐ݒ�
		if (strcmp(aComment, "STAGESET") == 0)
		{// �X�e�[�W���̓ǂݍ��݂��J�n

			while (strcmp(aComment, "END_STAGESET") != 0)
			{// END_STAGESET������܂ŌJ��Ԃ�

				// ������̓ǂݍ���
				fscanf(pFile, "%s", &aComment[0]);

				// �e�p�^�[���̐ݒ�
				if (strcmp(aComment, "WAVESET") == 0)
				{// �z�u���̓ǂݍ��݂��J�n

					// �X�|�[���l�����Z�b�g
					nCntSpawnEnemy = 0;
					nCntSpawnWave = 0;

					// ���ۑ��p
					std::vector<SetInfo> EnemyLoadInfo = {};
					std::vector<SetInfo> WaveLoadInfo = {};

					while (strcmp(aComment, "END_WAVESET") != 0)
					{// END_WAVESET������܂ŌJ��Ԃ�

						fscanf(pFile, "%s", &aComment[0]);	//�m�F����

						// �ǂݍ��ݗp
						SetInfo InitInfo = {};
						memset(&InitInfo, NULL, sizeof(InitInfo));
						InitInfo.nChara = -1;

						if (strcmp(aComment, "RESPAWNNUM") == 0)
						{// RESPAWNNUM��������G���X�|�[���K��l�ǂݍ���

							int nNum = 0;

							fscanf(pFile, "%s", &aComment[0]);	// =�̕�
							fscanf(pFile, "%d", &nNum);			// ���X�|�[���K��l
							m_nRespawnThreshold[m_nNumStage][m_nNumWave[m_nNumStage]] = nNum;
						}

						// �e�p�^�[���̐ݒ�
						if (strcmp(aComment, "ENEMYSET") == 0)
						{// �z�u���̓ǂݍ��݂��J�n

							// ������
							InitInfo.nChara = -1;
							EnemyLoadInfo.push_back(InitInfo);

							while (strcmp(aComment, "END_ENEMYSET") != 0)
							{// END_ENEMYSET������܂ŌJ��Ԃ�

								fscanf(pFile, "%s", &aComment[0]);	//�m�F����

								if (strcmp(aComment, "TYPE") == 0)
								{// TYPE��������L�����t�@�C���ԍ��ǂݍ���

									fscanf(pFile, "%s", &aComment[0]);	// =�̕�
									fscanf(pFile, "%d", &InitInfo.nType);	// ���
								}

								if (strcmp(aComment, "BASE") == 0)
								{// BASE�������狒�_�ԍ��ǂݍ���

									fscanf(pFile, "%s", &aComment[0]);	// =�̕�
									fscanf(pFile, "%d", &InitInfo.nBase);	// ���_
								}

								if (strcmp(aComment, "SPAWNNUM") == 0)
								{// SPAWNNUM��������o���̐�

									fscanf(pFile, "%s", &aComment[0]);	// =�̕�
									fscanf(pFile, "%d", &InitInfo.nSpawnNum);	// �o�����鐔
								}

								if (strcmp(aComment, "MOVETYPE") == 0)
								{// MOVETYPE��������s���̎�ޓǂݍ���

									fscanf(pFile, "%s", &aComment[0]);	// =�̕�
									fscanf(pFile, "%d", &InitInfo.nMoveType);	// �s���̎��
								}

								if (strcmp(aComment, "SPAWNTYPE") == 0)
								{// SPAWNTYPE��������o���̎�ޓǂݍ���

									fscanf(pFile, "%s", &aComment[0]);			// =�̕�
									fscanf(pFile, "%d", &InitInfo.nSpawnType);	// �o���̎��
								}

								if (strcmp(aComment, "CHARACTER") == 0)
								{// CHARACTER��������F�̎�ޓǂݍ���

									fscanf(pFile, "%s", &aComment[0]);		// =�̕�
									fscanf(pFile, "%d", &InitInfo.nChara);	// �L�����̎��
								}
								
								if (strcmp(aComment, "POS") == 0)
								{// POS��������ʒu�ǂݍ���

									fscanf(pFile, "%s", &aComment[0]);		// =�̕�
									fscanf(pFile, "%f", &InitInfo.pos.x);	// X���W
									fscanf(pFile, "%f", &InitInfo.pos.y);	// Y���W
									fscanf(pFile, "%f", &InitInfo.pos.z);	// Z���W
								}

								if (strcmp(aComment, "ROT") == 0)
								{// ROT������������ǂݍ���

									fscanf(pFile, "%s", &aComment[0]);		// =�̕�
									fscanf(pFile, "%f", &InitInfo.rot.x);	// X����
									fscanf(pFile, "%f", &InitInfo.rot.y);	// Y����
									fscanf(pFile, "%f", &InitInfo.rot.z);	// Z����
								}

							}// END_ENEMYSET�̂�����

							// �ǂݍ��񂾏���n��
							//EnemyLoadInfo[nCntSpawnEnemy] = InitInfo;
							EnemyLoadInfo[nCntSpawnEnemy] = InitInfo;

							// �o�����鐔���Z
							nCntSpawnEnemy++;
						}


						// �e�p�^�[���̐ݒ�
						if (strcmp(aComment, "NATURALWAVESET") == 0)
						{// �z�u���̓ǂݍ��݂��J�n

							// ������
							//WaveLoadInfo.push_back(InitInfo);
							WaveLoadInfo.push_back(InitInfo);

							while (strcmp(aComment, "END_NATURALWAVESET") != 0)
							{// END_NATURALWAVESET������܂ŌJ��Ԃ�

								fscanf(pFile, "%s", &aComment[0]);	//�m�F����

								if (strcmp(aComment, "TYPE") == 0)
								{// TYPE��������L�����t�@�C���ԍ��ǂݍ���

									fscanf(pFile, "%s", &aComment[0]);	// =�̕�
									fscanf(pFile, "%d", &InitInfo.nType);	// ���
								}

								if (strcmp(aComment, "BASE") == 0)
								{// BASE�������狒�_�ԍ��ǂݍ���

									fscanf(pFile, "%s", &aComment[0]);	// =�̕�
									fscanf(pFile, "%d", &InitInfo.nBase);	// ���_
								}

								if (strcmp(aComment, "POS") == 0)
								{// POS��������ʒu�ǂݍ���

									fscanf(pFile, "%s", &aComment[0]);		// =�̕�
									fscanf(pFile, "%f", &InitInfo.pos.x);	// X���W
									fscanf(pFile, "%f", &InitInfo.pos.y);	// Y���W
									fscanf(pFile, "%f", &InitInfo.pos.z);	// Z���W
								}

								if (strcmp(aComment, "ROT") == 0)
								{// ROT������������ǂݍ���

									fscanf(pFile, "%s", &aComment[0]);		// =�̕�
									fscanf(pFile, "%f", &InitInfo.rot.x);	// X����
									fscanf(pFile, "%f", &InitInfo.rot.y);	// Y����
									fscanf(pFile, "%f", &InitInfo.rot.z);	// Z����
								}

							}// END_NATURALWAVESET�̂�����

							// �ǂݍ��񂾏���n��
							//WaveLoadInfo.push_back(InitInfo);
							WaveLoadInfo[nCntSpawnWave] = InitInfo;

							// �o�����鐔���Z
							nCntSpawnWave++;
						}

					}// END_WAVESET�̂�����	

					// �ǂݍ��݂̏���n��
					if (EnemyLoadInfo.size() != 0)
					{
						CGame::GetEnemyManager()->SetEnemyInfo(&EnemyLoadInfo[0], m_nNumStage, m_nNumWave[m_nNumStage], EnemyLoadInfo.size());
					}

					if (WaveLoadInfo.size() != 0)
					{
						CGame::GetObject3DMesh()->SetNaturalWaveInfo(&WaveLoadInfo[0], m_nNumStage, m_nNumWave[m_nNumStage], WaveLoadInfo.size());
					}

					// �E�F�[�u�̑������Z
					m_nNumWave[m_nNumStage]++;
				}
			}// END_STAGESET�̂�����

			// �X�e�[�W�̑������Z
			m_nNumStage++;
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���
			break;
		}
	}

	// �t�@�C�������
	fclose(pFile);

	return S_OK;
}

//==========================================================================
// ���݂̃E�F�[�u�擾
//==========================================================================
int CWaveManager::GetNowWave(void)
{
	return m_nNowWave;
}

//==========================================================================
// ���݂̃X�e�[�W�擾
//==========================================================================
int CWaveManager::GetNowStage(void)
{
	return m_nNowStage;
}

//==========================================================================
// ���X�e�[�W���擾
//==========================================================================
int CWaveManager::GetMaxStage(void)
{
	return m_nNumStage;
}

//==========================================================================
// ���݂̃X�e�[�W�̃E�F�[�u���擾
//==========================================================================
int CWaveManager::GetMaxWave(void)
{
	return m_nNumWave[m_nNowStage];
}

//==========================================================================
// ���X�|�[���̋K��l�擾
//==========================================================================
int CWaveManager::GetRespawnThreshold(void)
{
	return m_nRespawnThreshold[m_nNowStage][m_nNowWave];
}
