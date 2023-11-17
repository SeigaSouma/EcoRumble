//=============================================================================
// 
//  �A�C�e���w�b�_�[ [bullet.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ITEM_H_
#define _ITEM_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �A�C�e���N���X��`
class CItem : public CObject2D
{
public:

	// �񋓌^��`
	typedef enum
	{
		TYPE_NONE = 0,	// �Ȃɂ��Ȃ�
		TYPE_ITEM,		// �A�C�e��
		TYPE_MAX
	}TYPE;

	CItem(int nPriority = mylib_const::DEF2D_PRIORITY);
	CItem(TYPE type, int nPriority = mylib_const::DEF2D_PRIORITY);
	~CItem();

	static HRESULT Load(void);
	static void Unload(void);
	static CItem *Create(TYPE type);
	static CItem *Create(TYPE type, const D3DXVECTOR3 pos);

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetVtx(void);

	static int GetNumAll(void);

private:

	TYPE m_type;									// �A�C�e���̎��
	static int m_nNumAll;							// �A�C�e���̑���
	static const char *m_apTextureFile[];			// �e�N�X�`���̃t�@�C��
	int m_nTexIdx[TYPE_MAX];					// �e�N�X�`���̃C���f�b�N�X�ԍ�
};


#endif