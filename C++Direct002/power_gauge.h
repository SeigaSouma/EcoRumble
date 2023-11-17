//=============================================================================
// 
//  �p���[�Q�[�W�w�b�_�[ [power_gauge.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _POWERGAUGE_H_
#define _POWERGAUGE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �A�C�e���N���X��`
class CPowerGauge : public CObject
{
public:

	// �񋓌^��`
	typedef enum
	{
		LEVEL_1 = 0,	// 1�i�K��
		LEVEL_2,		// 2
		LEVEL_3,		// 3
		LEVEL_MAX		// �ő�
	}LEVEL;

	CPowerGauge(int nPriority = 7);
	~CPowerGauge();

	static CPowerGauge *Create(void);

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetVtx(void);

	void UpdateChargeCount(void);		// �`���[�W�J�E���g�X�V����
	void SetChargeCount(int nValue);	// �`���[�W�J�E���g�ݒ菈��
	int GetLevel(void);			// �`���[�W�i�K�擾
	bool IsFever(void) { return m_bFever; }			// FEVER���̔���
private:

	// �񋓌^��`
	typedef enum
	{
		VTX_BLACK = 0,	// ���Q�[�W
		VTX_RED,		// �ԃQ�[�W
		VTX_GAUGE,		// �s���N�Q�[�W
		VTX_FRAM,		// �g
		VTX_MAX
	}VTX;

	// �\���̒�`
	struct SPowerGauge
	{
		CObject2D *pObj2D;	// 2D�I�u�W�F�N�g�̃I�u�W�F�N�g
		float fMaxWidth;		// ���̍ő�l
		float fWidthDest;		// ���̍���
		float fMaxHeight;		// �����̍ő�l
	};

	void GaugeDecrement(int nCntGauge);		// ��������
	void ChangeColor(int nCntGauge);		// �F�X�V

	int m_Level;								// �`���[�W�i�K
	int m_nCntWaitTime;							// �҂�����
	int m_nCntCharge;							// �`���[�W�J�E���g
	int m_nMaxCharge;							// �ő�`���[�W�J�E���g
	bool m_bFever;								// FEVER��
	SPowerGauge m_sPowerGauge[VTX_MAX];
	int m_nTexIdx[VTX_MAX];			// �e�N�X�`���̃C���f�b�N�X�ԍ�
	static const char *m_apTextureFile[VTX_MAX];			// �e�N�X�`���̃t�@�C��
};


#endif