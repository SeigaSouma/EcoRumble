//=============================================================================
// 
//  �ϑz�̃|�C���g�w�b�_�[ [delusion_point.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _DELUSION_POINT_H_
#define _DELUSION_POINT_H_	// ��d�C���N���[�h�h�~


#include "main.h"
#include "object2D.h"

//==========================================================================
// �O���錾
//==========================================================================
class CNumber;

//==========================================================================
// �N���X��`
//==========================================================================
// �ϑz�̃E�B���h�E�N���X��`
class CDelusionPoint : public CObject2D
{
public:

	CDelusionPoint(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CDelusionPoint();

	static CDelusionPoint *Create(void);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetValue(int nNum);	// �l�̐ݒ�

private:

	int m_nNum;					// �l
	CNumber *m_apNumber;		// �����̃I�u�W�F�N�g
	int m_nTexIdx;				// �e�N�X�`���̃C���f�b�N�X�ԍ�
};



#endif