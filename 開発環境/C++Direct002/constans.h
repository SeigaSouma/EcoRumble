//=============================================================================
// 
//  �萔�w�b�_�[ [constans.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _CONSTANS_H_
#define _CONSTANS_H_	// ��d�C���N���[�h�h�~

#include "main.h"

//==========================================================================
// ���O���
//==========================================================================
// �萔��`
namespace mylib_const
{
	const int MAX_OBJ = 1600;			// �I�u�W�F�N�g�̍ő吔
	const int PRIORITY_NUM = 9;			// �D�揇�ʂ̍ő吔
	const int DEFAULT_PRIORITY = 3;		// �D�揇�ʂ̃f�t�H���g
	const int DEF2D_PRIORITY = 4;		// �D�揇�ʂ�2D�f�t�H���g
	const int MAX_STRING = 512;			// �ő啶����
	const int MAX_MODEL = 64;			// ���f���ő吔
	const int MAX_PARTS = 32;			// �p�[�c�ő吔
	const int MAX_PATTEN_ENEMY = 64;	// �G�̍ő�p�^�[��
	const int MAX_WAVE_ENEMY = 32;		// �G�̍ő�p�^�[��
	const int MAX_STAGE = 4;			// �X�e�[�W�̍ő吔
	const int MAX_BASE_ENEMY = 32;		// �G�̋��_�ő吔
	const int MAX_ENEMY = 256;			// �G�̍ő吔
	const int ENEMY_PRIORITY = 4;		// �G�̗D�揇��
	const float GRAVITY = 1.95f;		// �d��
	const int MAX_WAVE = 128;			// �g�̍ő吔
	const int MAX_VTX = 6561;			// ���_�̍ő吔
	const float KILL_Y = -800.0f;		// ���ł�����W
	const int DIRECTWAVE_DMG = 5;		// �����g�̃_���[�W
	const int IMPACTWAVE_DMG = 1;		// �~�`�g�̃_���[�W
	const int COMBONUM_1 = 4;			// 1�i�K�ڂ̃R���{�̐�
	const int COMBONUM_2 = 8;			// 2�i�K�ڂ̃R���{�̐�
	const int COMBONUM_3 = 19;			// 3�i�K�ڂ̃R���{�̐�
	const int COMBOBONUS_1 = 100;		// 1�i�K�ڂ̃R���{�{�[�i�X
	const int COMBOBONUS_2 = 300;		// 2�i�K�ڂ̃R���{�{�[�i�X
	const int COMBOBONUS_3 = 1000;		// 3�i�K�ڂ̃R���{�{�[�i�X
	const D3DXCOLOR FIELDCOLOR_01 = D3DXCOLOR(0.6f, 0.4f, 0.1f, 1.0f);	// �t�B�[���h�̐F
}

#define BBB(n)	5 + (n)

#endif