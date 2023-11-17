//=============================================================================
// 
//  �ϑz�̃E�B���h�E�w�b�_�[ [delusion_window.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _DELUSION_WINDOW_H_
#define _DELUSION_WINDOW_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object2D.h"


//==========================================================================
// �N���X��`
//==========================================================================
// �ϑz�̃E�B���h�E�N���X��`
class CDelusionWindow : public CObject2D
{
public:

	CDelusionWindow(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CDelusionWindow();

	static CDelusionWindow *Create(void);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static int GetNumAll(void);

private:

	static int m_nNumAll;			// �ϑz�̃E�B���h�E�̑���
	int m_nTexIdx;					// �e�N�X�`���̃C���f�b�N�X�ԍ�
};


#endif