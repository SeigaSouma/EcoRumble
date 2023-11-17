//=============================================================================
// 
//  ���̃t�B�[���h�w�b�_�[ [waterfield.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _WATERFIELD_H_
#define _WATERFIELD_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "meshfield.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ���b�V���t�B�[���h�N���X��`
class CWaterField : public CMeshField
{
public:
	
	CWaterField(int nPriority = 5);
	~CWaterField();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	CWaterField *GetObject3DMesh(void);
protected:

private:

	// �����o�֐�
	void Scroll(void);

	// �����o�ϐ�
	float m_fTexU;	// U�X�N���[���p
	float m_fTexV;	// V�X�N���[���p
	int m_nTimeInWater[mylib_const::MAX_OBJ];	// ���ɓ����Ă鎞��
};



#endif