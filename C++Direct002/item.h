//=============================================================================
// 
//  アイテムヘッダー [bullet.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ITEM_H_
#define _ITEM_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// アイテムクラス定義
class CItem : public CObject2D
{
public:

	// 列挙型定義
	typedef enum
	{
		TYPE_NONE = 0,	// なにもなし
		TYPE_ITEM,		// アイテム
		TYPE_MAX
	}TYPE;

	CItem(int nPriority = mylib_const::DEF2D_PRIORITY);
	CItem(TYPE type, int nPriority = mylib_const::DEF2D_PRIORITY);
	~CItem();

	static HRESULT Load(void);
	static void Unload(void);
	static CItem *Create(TYPE type);
	static CItem *Create(TYPE type, const D3DXVECTOR3 pos);

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetVtx(void);

	static int GetNumAll(void);

private:

	TYPE m_type;									// アイテムの種類
	static int m_nNumAll;							// アイテムの総数
	static const char *m_apTextureFile[];			// テクスチャのファイル
	int m_nTexIdx[TYPE_MAX];					// テクスチャのインデックス番号
};


#endif