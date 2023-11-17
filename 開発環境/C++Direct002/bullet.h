//=============================================================================
// 
//  弾ヘッダー [bullet.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BULLET_H_
#define _BULLET_H_	// 二重インクルード防止

#include "main.h"
#include "objectBillboard.h"

//==========================================================================
// クラス定義
//==========================================================================
// 弾クラス定義
class CBullet : public CObjectBillboard
{
public:

	// 列挙型定義
	typedef enum
	{
		TYPE_NONE = 0,	// 弾
		TYPE_PLAYER,	// プレイヤーの弾
		TYPE_ENEMY,		// 敵の弾
		TYPE_MAX
	}TYPE;

	CBullet(int nPriority = 2);
	CBullet(TYPE type, int nPriority = 2);
	~CBullet();

	static HRESULT Load(void);
	static void Unload(void);
	static CBullet *Create(TYPE type);
	static CBullet *Create(TYPE type, const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetVtx(void);

	static int GetNumAll(void);

private:
	void UpdatePos(void);		// 移動
	bool CollisionFrame(void);	// 画面外判定
	bool Collision(void);	// キャラとの当たり判定

	TYPE m_type;							// 弾の種類
	int m_nLife;							// 寿命
	const int m_nLifeMax;					// 寿命の最大値
	static int m_nNumAll;					// 弾の総数
	static const char *m_apTextureFile[];	// テクスチャのファイル
	int m_nTexIdx[TYPE_MAX];				// テクスチャのインデックス番号
};


#endif