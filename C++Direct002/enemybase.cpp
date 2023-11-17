//=============================================================================
// 
//  �G���_���� [enemybase.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "enemybase.h"
#include "calculation.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
std::vector<CEnemyBase::BaseData> CEnemyBase::m_aBaseData[mylib_const::MAX_STAGE] = {};	// �ǂݍ��݃f�[�^
int CEnemyBase::m_nNumAll[mylib_const::MAX_STAGE] = {};		// �ő吔
bool CEnemyBase::m_bLoad = false;	// ���ǂݍ��ݔ���

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyBase::CEnemyBase()
{
	// �l�̃N���A
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyBase::~CEnemyBase()
{

}

//==========================================================================
// ��������
//==========================================================================
CEnemyBase *CEnemyBase::Create(const std::string pTextFile)
{
	// �����p�̃I�u�W�F�N�g
	CEnemyBase *pEnemyBase = NULL;

	if (pEnemyBase == NULL)
	{// NULL��������

		// �������̊m��
		pEnemyBase = DEBUG_NEW CEnemyBase;

		if (pEnemyBase != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			HRESULT hr = pEnemyBase->Init(pTextFile);

			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}
		}
		return pEnemyBase;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyBase::Init(const std::string pTextFile)
{
	// ����������
	HRESULT hr = ReadText(pTextFile);

	if (FAILED(hr))
	{// ���s���Ă�����
		return NULL;
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyBase::Uninit(void)
{
	
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyBase::Update(void)
{
	
}

//==========================================================================
// �e�L�X�g�ǂݍ��ݏ���
//==========================================================================
HRESULT CEnemyBase::ReadText(const std::string pTextFile)
{
	// ���[�h�`�F�b�N
	if (m_bLoad == true)
	{// ���ɓǂݍ��܂�Ă�����
		return S_OK;
	}

	FILE *pFile = NULL;	// �t�@�C���|�C���^��錾

	// �t�@�C�����J��
	pFile = fopen(pTextFile.c_str(), "r");

	if (pFile == NULL)
	{// �t�@�C�����J���Ȃ������ꍇ
		return E_FAIL;
	}

	// �ǂݍ��ݔ���ON
	m_bLoad = true;

	char aComment[MAX_COMMENT];	// �R�����g
	BaseData InitBase = {};		// �������p
	int nCntStage = 0;			// �X�e�[�W�̐�

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
				if (strcmp(aComment, "BASESET") == 0)
				{// �z�u���̓ǂݍ��݂��J�n

					// ���������ǉ�
					m_aBaseData[nCntStage].push_back(InitBase);

					while (strcmp(aComment, "END_BASESET") != 0)
					{// END_BASESET������܂ŌJ��Ԃ�

						// ������̓ǂݍ���
						fscanf(pFile, "%s", &aComment[0]);

						if (strcmp(aComment, "POS") == 0)
						{// POS��������ʒu�ǂݍ���

							fscanf(pFile, "%s", &aComment[0]);					// =�̕�
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].pos.x);	// X���W
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].pos.y);	// Y���W
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].pos.z);	// Z���W
						}

						if (strcmp(&aComment[0], "ROT") == 0)
						{// ROT������������ǂݍ���

							fscanf(pFile, "%s", &aComment[0]);	// =�̕�
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].rot.x);	// X�̌���
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].rot.y);	// Y�̌���
							fscanf(pFile, "%f", &m_aBaseData[nCntStage][m_nNumAll[nCntStage]].rot.z);	// Z�̌���
						}

					}// END_BASESET�̂�����

					// �������Z
					m_nNumAll[nCntStage]++;
				}
			}// END_STAGESET�̂�����
			nCntStage++;
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
// �G���_�f�[�^�擾
//==========================================================================
CEnemyBase::BaseData *CEnemyBase::GetBaseData(int nCntStage)
{
	return &m_aBaseData[nCntStage][0];
}
