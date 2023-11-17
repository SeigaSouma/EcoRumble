//=============================================================================
// 
//  �����G�t�F�N�g�w�b�_�[ [confusion_effect.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _CONFUSION_EFFECT_H_
#define _CONFUSION_EFFECT_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �O���錾
//==========================================================================
class CObjectBillboard;

//==========================================================================
// �}�N����`
//==========================================================================
#define MAX_CONFUSION	(4)	// �����G�t�F�N�g�̐�

//==========================================================================
// �N���X��`
//==========================================================================
// �����G�t�F�N�g�N���X��`
class CConfusionEffect : public CObject
{
public:
	CConfusionEffect(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CConfusionEffect();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void UpdatePosition(D3DXVECTOR3 pos);
	static CConfusionEffect *Create(D3DXVECTOR3 pos, float fDistance, int nLife);
private:

	D3DXVECTOR3 m_posOrigin;			// ���_
	D3DXVECTOR3 m_rotOrigin;			// ����
	float m_fDistance;					// ���_����̋���
	int m_nLife;						// �̗�
	CObjectBillboard *m_pObjBillboard[MAX_CONFUSION];	// �r���{�[�h�̃|�C���^
	static int m_nTexIdx;				// �e�N�X�`���̃C���f�b�N�X�ԍ�
};


#endif