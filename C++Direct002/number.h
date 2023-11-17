//=============================================================================
// 
//  �����w�b�_�[ [number.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _NUMBER_H_
#define _NUMBER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object2D.h"

//==========================================================================
// �O���錾
//==========================================================================
class CObject2D;

//==========================================================================
// �N���X��`
//==========================================================================
// �w�i�N���X��`
class CNumber
{
public:

	CNumber(int nPriority = 6);
	~CNumber();

	static CNumber *Create(void);

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Release(void);	// �J������
	CObject2D *GetObject2D(void);

private:
	CObject2D *m_aObject2D;	// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
};

#endif