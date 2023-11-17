//=============================================================================
// 
//  �N���w�b�_�[ [elevation_outside.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ELEVATIONOUTSIDE_H_
#define _ELEVATIONOUTSIDE_H_	// ��d�C���N���[�h�h�~

#include "elevation.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �N���N���X��`
class CElevationOutside : public CElevation
{
public:
	CElevationOutside(int nPriority = 1);
	~CElevationOutside();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	HRESULT Init(const char *pText) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetVtx(void) override;

	void UPVtxField(D3DXVECTOR3 pos);
	void Release(void);
	void Save(void);	// �Z�[�u
	HRESULT Load(const char *pText);	// ���[�h

	CElevationOutside *GetElevation(void);
	float GetHeight(D3DXVECTOR3& pos, D3DXVECTOR3& move, bool &bLand, CObject::TYPE type);	// �����擾
protected:

private:
	int m_nTexIdx;	// �e�N�X�`���̃C���f�b�N�X�ԍ�
	POSTYPE m_postype;	// �ʒu�̎��
};



#endif