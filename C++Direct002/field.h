//=============================================================================
// 
//  �t�B�[���h�w�b�_�[ [field.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _FIELD_H_
#define _FIELD_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object3D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �e�N���X��`
class CField : public CObject3D
{
public:

	CField(int nPriority = 1);
	~CField();

	static HRESULT Load(void);
	static void Unload(void);
	static CField *Create();
	static CField *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static int GetNumAll(void);

private:

#define TEXTURE			"data\\TEXTURE\\line.png"
#define WIDTH			(200.0f)						// ����
#define HEIGHT			(200.0f)						// �c��

	static int m_nNumAll;					// ����
	int m_nTexIdx;				// �e�N�X�`���̃C���f�b�N�X�ԍ�
};


#endif