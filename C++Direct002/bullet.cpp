//=============================================================================
// 
//  �e���� [bullet.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "bullet.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "explosion.h"
#include "2D_effect.h"
#include "3D_effect.h"
#include "sound.h"
#include "particle.h"
#include "score.h"
#include "enemy.h"
#include "collision.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\bullet_03.png"
#define WIDTH			(30.0f)							// ����
#define HEIGHT			(30.0f)							// �c��
#define MOVE_SPEED		(15.0f)							// �ړ����x
#define ANIM_SPEED		(4)								// �ǂݍ��݊Ԋu
#define MAX_PATTERN_U	(1)								// U�̕�����
#define MAX_PATTERN_V	(1)								// V�̕�����
#define MAX_PATTERN		(MAX_PATTERN_U)					// �A�j���[�V�����p�^�[���̍ő吔
#define MOVE_U			(1.0f / (float)MAX_PATTERN_U)	// U���W�ړ���
#define MOVE_V			(1.0f / (float)MAX_PATTERN_V)	// V���W�ړ���

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
const char *CBullet::m_apTextureFile[] =				// �e�N�X�`���̃t�@�C��
{
	NULL,
	"data\\TEXTURE\\bullet_01.png",
	"data\\TEXTURE\\bullet_03.png",
};
int CBullet::m_nNumAll = 0;		// �e�̑���

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CBullet::CBullet(int nPriority) : CObjectBillboard(nPriority), m_nLifeMax(40)
{
	// �l�̃N���A
	m_type = TYPE_NONE;
	m_nLife = 0;

	// �e�N�X�`���f�[�^�̔z�񕪌J��Ԃ�
	for (int nCntTex = 0; nCntTex < sizeof(m_apTextureFile) / sizeof(*m_apTextureFile); nCntTex++)
	{
		m_nTexIdx[nCntTex] = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
	}

	// �������Z
	m_nNumAll++;
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CBullet::CBullet(TYPE type, int nPriority) : CObjectBillboard(nPriority), m_nLifeMax(40)
{
	// �l�̃N���A
	m_type = type;
	m_nLife = 0;

	// �������Z
	m_nNumAll++;
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CBullet::~CBullet()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CBullet *CBullet::Create(TYPE type)
{
	// �����p�̃I�u�W�F�N�g
	CBullet *pBullet = NULL;

	if (pBullet == NULL)
	{// NULL��������

		// �������̊m��
		pBullet = DEBUG_NEW CBullet;

		//if (pBullet->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pBullet;
		//	return NULL;
		//}

		if (pBullet != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ��ސݒ�
			pBullet->m_type = type;

			// ����������
			pBullet->Init();
		}

		return pBullet;
	}

	return NULL;
}

//==========================================================================
// ��������(�I�[�o�[���[�h)
//==========================================================================
CBullet *CBullet::Create(TYPE type, const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	// �����p�̃I�u�W�F�N�g
	CBullet *pBullet = NULL;

	if (pBullet == NULL)
	{// NULL��������

		// �������̊m��
		pBullet = DEBUG_NEW CBullet;

		//if (pBullet->GetID() < 0)
		//{// �������m�ۂɎ��s���Ă�����

		//	delete pBullet;
		//	return NULL;
		//}

		if (pBullet != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ��ސݒ�
			pBullet->m_type = type;

			// �ʒu���蓖��
			pBullet->SetPosition(pos);

			// �������蓖��
			pBullet->SetRotation(rot);

			// ����������
			pBullet->Init();
		}

		return pBullet;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CBullet::Init(void)
{
	HRESULT hr;

	// �e��ϐ��̏�����
	SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// �T�C�Y

	switch (m_type)	// �ړ��ʐݒ�
	{
	case TYPE_PLAYER:
		SetMove(D3DXVECTOR3(
			sinf(D3DX_PI * 0.5f + GetRotation().y) * MOVE_SPEED,
			0.0f,
			cosf(D3DX_PI * 0.5f + GetRotation().y) * MOVE_SPEED));
		break;

	case TYPE_ENEMY:
		SetMove(D3DXVECTOR3(
			sinf(D3DX_PI * 0.5f + GetRotation().y) * -MOVE_SPEED,
			0.0f,
			cosf(D3DX_PI * 0.5f + GetRotation().y) * -MOVE_SPEED));
		break;
	}

	m_nLife = m_nLifeMax;	// ����

	// �e�N�X�`���f�[�^�̔z�񕪌J��Ԃ�
	for (int nCntTex = 0; nCntTex < sizeof(m_apTextureFile) / sizeof(*m_apTextureFile); nCntTex++)
	{
		// �e�N�X�`���̊��蓖��
		m_nTexIdx[nCntTex] = CManager::GetTexture()->Regist(m_apTextureFile[nCntTex]);

		// �e�N�X�`���̊��蓖��
		BindTexture(m_nTexIdx[nCntTex]);
	}

	// ��ނ̐ݒ�
	SetType(TYPE_BULLET);

	// ����������
	hr = CObjectBillboard::Init();

	if (FAILED(hr))
	{// ���s�����Ƃ�

		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CBullet::Uninit(void)
{
	// �I������
	CObjectBillboard::Uninit();

	// �������Z
	m_nNumAll--;
}

//==========================================================================
// �X�V����
//==========================================================================
void CBullet::Update(void)
{
	// �ʒu�X�V
	UpdatePos();

	if (CollisionFrame() == true)
	{// ��ʊO�o����
		return;
	}

	// �G�ƒe�̓����蔻��
	if (Collision() == true)
	{// �G�Ɠ������Ă�����
		return;
	}

	// �������Z
	m_nLife--;

	if (m_nLife <= 0)
	{// �������s������

		// �����̐���
		CExplosion::Create(GetPosition());
		my_particle::Create(GetPosition(), my_particle::TYPE_OFFSETTING);

		// �e�̍폜
		Uninit();
		return;
	}

	int n = 0;

	// ���_���ݒ�
	SetVtx();
}

//==========================================================================
// �ړ�����
//==========================================================================
void CBullet::UpdatePos(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ʒu�X�V
	pos += move;
	/*move.y = 50.0f;
	nnnn++;
	pos.x = 0.0f * (nnnn * nnnn) + move.x * nnnn;
	pos.y = -GRAVITY * 0.1f * (nnnn * nnnn) + move.y * nnnn + 300.0f;
	pos.z = 0.0f * (nnnn * nnnn) + move.z * nnnn;*/

	// �G�t�F�N�g�ݒ�
	switch (m_type)
	{
	case TYPE_PLAYER:
		CEffect3D::Create(
			pos,			// �ʒu
			D3DXVECTOR3(	// �ړ���
				sinf(D3DX_PI * 0.5f + GetRotation().y) * -5.0f,
				0.0f,
				cosf(D3DX_PI * 0.5f + GetRotation().y) * -5.0f),
			D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f),	// �F
			80.0f,								// ���a
			10,									// ����
			CEffect3D::MOVEEFFECT_SUB,			// ���Z���
			CEffect3D::TYPE_NORMAL);

		CEffect3D::Create(
			pos,			// �ʒu
			D3DXVECTOR3(	// �ړ���
				sinf(D3DX_PI * 0.5f + GetRotation().y) * -5.0f,
				0.0f,
				cosf(D3DX_PI * 0.5f + GetRotation().y) * -5.0f),
			D3DXCOLOR(1.0f, 0.6f, 0.3f, 1.0f),	// �F
			40.0f,								// ���a
			10,									// ����
			CEffect3D::MOVEEFFECT_SUB,			// ���Z���
			CEffect3D::TYPE_NORMAL);
		break;

	case TYPE_ENEMY:
		CEffect3D::Create(
			pos,			// �ʒu
			D3DXVECTOR3(	// �ړ���
				sinf(D3DX_PI * 0.5f + GetRotation().y) * 5.0f,
				0.0f,
				cosf(D3DX_PI * 0.5f + GetRotation().y) * 5.0f),
			D3DXCOLOR(0.3f, 1.0f, 0.3f, 1.0f),	// �F
			80.0f,								// ���a
			10,									// ����
			CEffect3D::MOVEEFFECT_SUB,			// ���Z���
			CEffect3D::TYPE_NORMAL);

		CEffect3D::Create(
			pos,			// �ʒu
			D3DXVECTOR3(	// �ړ���
				sinf(D3DX_PI * 0.5f + GetRotation().y) * 5.0f,
				0.0f,
				cosf(D3DX_PI * 0.5f + GetRotation().y) * 5.0f),
			D3DXCOLOR(1.0f, 0.6f, 0.3f, 1.0f),	// �F
			40.0f,								// ���a
			10,									// ����
			CEffect3D::MOVEEFFECT_SUB,			// ���Z���
			CEffect3D::TYPE_NORMAL);
		break;
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);
}

//==========================================================================
// ��ʂ̔���
//==========================================================================
bool CBullet::CollisionFrame(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// ��ʊO�ō폜
	//if (pos.x < 0.0f || pos.x > SCREEN_WIDTH || pos.y < 0.0f || pos.y > SCREEN_HEIGHT)
	//{
	//	// �e�̍폜
	//	Uninit();
	//	return true;
	//}

	return false;
}

//==========================================================================
// �G�Ƃ̓����蔻��
//==========================================================================
bool CBullet::Collision(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �T�C�Y�擾
	D3DXVECTOR2 size = GetSize();

	// �ڕW�̃^�C�v
	//switch (m_type)
	//{
	//case TYPE_PLAYER:
	//	
	//	// �G�Ƃ̓����蔻��
	//	if (collision::Collision(pos, size, GetObject2D(), CObject::TYPE_ENEMY) == true)
	//	{// �������Ă�����

	//		CManager::GetScore()->Add(300);

	//		// �e�̏I������
	//		Uninit();

	//		return true;
	//	}
	//	break;

	//case TYPE_ENEMY:
	//	// �G�Ƃ̓����蔻��
	//	if (collision::Collision(pos, size, GetObject2D(), CObject::TYPE_PLAYER) == true)
	//	{// �������Ă�����

	//		// �e�̏I������
	//		Uninit();

	//		return true;
	//	}
	//}



	

	return false;
}

//==========================================================================
// �`�揈��
//==========================================================================
void CBullet::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// �r���{�[�h�̕`��
	CObjectBillboard::Draw();

	// �A���t�@�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CBullet::SetVtx(void)
{
	// ���_�ݒ�
	CObjectBillboard::SetVtx();
}

//==========================================================================
// �����擾
//==========================================================================
int CBullet::GetNumAll(void)
{
	return m_nNumAll;
}
