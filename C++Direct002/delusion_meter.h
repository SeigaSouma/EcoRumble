//=============================================================================
// 
//  �ϑz�̃��[�^�[�w�b�_�[ [delusion_meter.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _DELUSION_METER_H_
#define _DELUSION_METER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object2D.h"

class CDelusionPoint;

//==========================================================================
// �N���X��`
//==========================================================================
// �ϑz�̃��[�^�[�N���X��`
class CDelusionMeter : public CObject2D
{
public:

	// �񋓌^��`
	typedef enum
	{
		TYPE_SPEED = 0,	// �X�s�[�h
		TYPE_POWER,		// �p���[
		TYPE_CROWD,		// �Q��
		TYPE_MAX
	}TYPE;

	CDelusionMeter(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CDelusionMeter();

	static CDelusionMeter *Create(void);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static int GetNumAll(void);

private:

	int m_nChangeIdx;			// �ύX����C���f�b�N�X�ԍ�
	int m_nRemainPoint;			// �c��|�C���g
	int m_nPoint[3];			// �v�f�|�C���g
	float m_fLength[3];			// ����
	float m_fDestLength[3];		// �ڕW�̒���
	static int m_nNumAll;		// �ϑz�̃��[�^�[�̑���
	int m_nTexIdx;				// �e�N�X�`���̃C���f�b�N�X�ԍ�
	CDelusionPoint *m_pDelusionPoint;	// �ϑz�|�C���g
	CObject2D *m_pMeterMain;			// �{���̃��[�^�[
	CObject2D *m_pMeter;			// �{���̃��[�^�[
};


#endif