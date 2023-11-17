//=============================================================================
// 
//  ���U���g��ʃw�b�_�[ [result_screen.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _RESULTSCREEN_H_
#define _RESULTSCREEN_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"
#include "resultmanager.h"

//==========================================================================
// �O���錾
//==========================================================================
class CObject2D;
class CNumber;
class CResultScore;

//==========================================================================
// �}�N����`
//==========================================================================
#define SCORETYPE_DIGIT	(3)	// �X�R�A��ނ̌���
#define MAX_SCORETYPE	(3)	// �X�R�A���

//==========================================================================
// �N���X��`
//==========================================================================
// �w�i�N���X��`
class CResultScreen : public CObject
{
public:

	CResultScreen(int nPriority = 0);
	~CResultScreen();

	static CResultScreen *Create(void);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);
	void SetVtx(int nCntSelect);

	void SetAllArrival(void);	// �S�Ă̓����ݒ�
	bool IsSetNumber(void) { return m_bSetNumber; }

private:
	// �񋓌^��`
	enum VTX
	{
		VTX_LOGO = 0,		// ���U���g���S
		VTX_RESULTSCORE_NORMAL,	// ���U���g�X�R�A
		VTX_RESULTSCORE_RED,	// ���U���g�X�R�A
		VTX_RESULTSCORE_GOLD,	// ���U���g�X�R�A
		VTX_LOGO_MAX
	};

	// �����o�֐�
	void SetValue(int nType);
	void MoveObj2D(int nCntType);
	void MoveNumber(int nCntType);

	static const char *m_apTextureFile[VTX_LOGO_MAX];	// �e�N�X�`���̃t�@�C��
	int m_nTexIdx[VTX_LOGO_MAX];						// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nTexIdxNumber;								// �����e�N�X�`���̃C���f�b�N�X�ԍ�
	float m_fPosDestObj2D[MAX_SCORETYPE];				// �I�u�W�F2D�̖ڕW�ʒu
	float m_fPosDestNumber[MAX_SCORETYPE][SCORETYPE_DIGIT];				// �����̖ڕW�ʒu
	int m_nNumDest[MAX_SCORETYPE];							// �{�[�i�X���̖ڕW�l
	float m_fNum[MAX_SCORETYPE];							// �{�[�i�X��
	bool m_nArrival;										// �����������̔���
	bool m_bSetNumber;										// �ݒ肵�����̔���
	CObject2D *m_pObj2D[VTX_LOGO_MAX];						// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
	CNumber *m_apNumber[MAX_SCORETYPE][SCORETYPE_DIGIT];	// �����̃I�u�W�F�N�g
	CResultScore *m_pResultScore;							// ���U���g�X�R�A�̃I�u�W�F�N�g
};



#endif