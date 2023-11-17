//=============================================================================
// 
//  定数ヘッダー [constans.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _CONSTANS_H_
#define _CONSTANS_H_	// 二重インクルード防止

#include "main.h"

//==========================================================================
// 名前空間
//==========================================================================
// 定数定義
namespace mylib_const
{
	const int MAX_OBJ = 1600;			// オブジェクトの最大数
	const int PRIORITY_NUM = 9;			// 優先順位の最大数
	const int DEFAULT_PRIORITY = 3;		// 優先順位のデフォルト
	const int DEF2D_PRIORITY = 4;		// 優先順位の2Dデフォルト
	const int MAX_STRING = 512;			// 最大文字数
	const int MAX_MODEL = 64;			// モデル最大数
	const int MAX_PARTS = 32;			// パーツ最大数
	const int MAX_PATTEN_ENEMY = 64;	// 敵の最大パターン
	const int MAX_WAVE_ENEMY = 32;		// 敵の最大パターン
	const int MAX_STAGE = 4;			// ステージの最大数
	const int MAX_BASE_ENEMY = 32;		// 敵の拠点最大数
	const int MAX_ENEMY = 256;			// 敵の最大数
	const int ENEMY_PRIORITY = 4;		// 敵の優先順位
	const float GRAVITY = 1.95f;		// 重力
	const int MAX_WAVE = 128;			// 波の最大数
	const int MAX_VTX = 6561;			// 頂点の最大数
	const float KILL_Y = -800.0f;		// 消滅する座標
	const int DIRECTWAVE_DMG = 5;		// 直線波のダメージ
	const int IMPACTWAVE_DMG = 1;		// 円形波のダメージ
	const int COMBONUM_1 = 4;			// 1段階目のコンボの数
	const int COMBONUM_2 = 8;			// 2段階目のコンボの数
	const int COMBONUM_3 = 19;			// 3段階目のコンボの数
	const int COMBOBONUS_1 = 100;		// 1段階目のコンボボーナス
	const int COMBOBONUS_2 = 300;		// 2段階目のコンボボーナス
	const int COMBOBONUS_3 = 1000;		// 3段階目のコンボボーナス
	const D3DXCOLOR FIELDCOLOR_01 = D3DXCOLOR(0.6f, 0.4f, 0.1f, 1.0f);	// フィールドの色
}

#define BBB(n)	5 + (n)

#endif