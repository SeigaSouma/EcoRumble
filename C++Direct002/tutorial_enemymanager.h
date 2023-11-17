//=============================================================================
// 
//  �G�̃}�l�[�W���w�b�_�[ [tutorial_enemymanager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TUTORIAL_ENEMYMANAGER_H_
#define _TUTORIAL_ENEMYMANAGER_H_	// ��d�C���N���[�h�h�~

#include "enemymanager.h"

// �O���錾

//==========================================================================
// �N���X��`
//==========================================================================
// �G�̃}�l�[�W���N���X��`
class CTutorialEnemyManager : public CEnemyManager
{

public:

	CTutorialEnemyManager();
	~CTutorialEnemyManager();

	void Update(void) override;
	void SetEnemy(int nIdx, int nPattern, int nCntEnemy, int nParent, CWaveManager::SetInfo setinfo, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int movetype = 0);	// �G�z�u
	void SetSpawnWaveNormal(int nStage, int nWave, int nCntPattern, int *pParent);		// �ʏ�N��
	void SetSpawnWave(int nStage, int nWave);		// �X�|�[���̋N���ݒ�


private:

};



#endif