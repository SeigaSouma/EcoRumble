//=============================================================================
// 
//  �e�w�b�_�[ [bullet.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _BULLET_H_
#define _BULLET_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "objectBillboard.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �e�N���X��`
class CBullet : public CObjectBillboard
{
public:

	// �񋓌^��`
	typedef enum
	{
		TYPE_NONE = 0,	// �e
		TYPE_PLAYER,	// �v���C���[�̒e
		TYPE_ENEMY,		// �G�̒e
		TYPE_MAX
	}TYPE;

	CBullet(int nPriority = 2);
	CBullet(TYPE type, int nPriority = 2);
	~CBullet();

	static HRESULT Load(void);
	static void Unload(void);
	static CBullet *Create(TYPE type);
	static CBullet *Create(TYPE type, const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetVtx(void);

	static int GetNumAll(void);

private:
	void UpdatePos(void);		// �ړ�
	bool CollisionFrame(void);	// ��ʊO����
	bool Collision(void);	// �L�����Ƃ̓����蔻��

	TYPE m_type;							// �e�̎��
	int m_nLife;							// ����
	const int m_nLifeMax;					// �����̍ő�l
	static int m_nNumAll;					// �e�̑���
	static const char *m_apTextureFile[];	// �e�N�X�`���̃t�@�C��
	int m_nTexIdx[TYPE_MAX];				// �e�N�X�`���̃C���f�b�N�X�ԍ�
};


#endif