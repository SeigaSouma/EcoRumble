//=============================================================================
// 
//  ���U���g�w�b�_�[ [result.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _RESULT_H_
#define _RESULT_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "manager.h"

//==========================================================================
// �O���錾
//==========================================================================
class CResultScreen;

//==========================================================================
// �N���X��`
//==========================================================================
// ���U���g�N���X��`
class CResult : public CScene
{
public:

	CResult();
	~CResult();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	CPlayer *GetPlayer(void) override;
	CPlayer **GetPlayer(int n) override;
	static CResultScreen *GetResultScreen(void);
	static void SetEnableArrival(void);

private:
	CPlayer *m_pPlayer;		// �v���C���[�̃I�u�W�F�N�g
	static bool m_bAllArrival;		// �S�ē�����������
	static CResultScreen *m_pResultScreen;
};



#endif