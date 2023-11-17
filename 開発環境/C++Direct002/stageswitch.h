//=============================================================================
// 
// �X�e�[�W�؂�ւ����� [stageswitch.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _STAGESWITCH_H_
#define _STAGESWITCH_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "main.h"
#include "object2D.h"

//==========================================================================
// �O���錾
//==========================================================================
class CNumber;

//==========================================================================
// �N���X��`
//==========================================================================
// �X�e�[�W�؂�ւ��N���X
class CStageSwitch : public CObject2D
{
public:

	CStageSwitch(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CStageSwitch();

	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static CStageSwitch *Create(void);	// ��������

private:
	// �񋓌^��`
	enum VTX
	{
		VTX_TEXT = 0,		// �^�C�g�����S
		VTX_MAX
	};

#define NUM_STAGE	(2)	// �X�e�[�W�̐�

	void SetValue(int nValue);	// �l�̐ݒ菈��
	void Release(void);	// �J������
	void UpdateGame(void);	// �Q�[���̍X�V
	void UpdateTitle(void);	// �^�C�g���̍X�V

	int m_nLife;	// ����
	CNumber *m_apNumber[NUM_STAGE];			// �����̃I�u�W�F�N�g
	static const char *m_apTextureFile[];	// �e�N�X�`���̃t�@�C��
	static int m_nTexIdx[VTX_MAX];			// �e�N�X�`���̃C���f�b�N�X�ԍ�
	static int m_nNumberTexIdx;				// �����e�N�X�`���̃C���f�b�N�X�ԍ�
};

#endif