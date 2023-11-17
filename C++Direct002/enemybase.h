//=============================================================================
// 
//  �G���_�w�b�_�[ [enemybase.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYBASE_H_
#define _ENEMYBASE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "constans.h"

// �O���錾

//==========================================================================
// �N���X��`
//==========================================================================
// �G���_�N���X��`
class CEnemyBase
{
public:

	// �\���̒�`
	struct BaseData
	{
		D3DXVECTOR3 pos;		// �ʒu
		D3DXVECTOR3 rot;		// ����
	};

	CEnemyBase();
	~CEnemyBase();

	HRESULT Init(const std::string pTextFile);
	void Uninit(void);
	void Update(void);

	static CEnemyBase *Create(const std::string pTextFile);
	HRESULT ReadText(const std::string pTextFile);		// �O���t�@�C���ǂݍ��ݏ���
	static BaseData *GetBaseData(int nCntStage);		// �G���_�f�[�^�擾

private:

	static int m_nNumAll[mylib_const::MAX_STAGE];		// �ő吔
	static std::vector<BaseData> m_aBaseData[mylib_const::MAX_STAGE];	// �z�u�̎��
	static bool m_bLoad;		// ���ǂݍ��ݔ���
};



#endif