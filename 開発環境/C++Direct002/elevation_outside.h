//=============================================================================
// 
//  起伏ヘッダー [elevation_outside.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ELEVATIONOUTSIDE_H_
#define _ELEVATIONOUTSIDE_H_	// 二重インクルード防止

#include "elevation.h"

//==========================================================================
// クラス定義
//==========================================================================
// 起伏クラス定義
class CElevationOutside : public CElevation
{
public:
	CElevationOutside(int nPriority = 1);
	~CElevationOutside();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	HRESULT Init(const char *pText) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetVtx(void) override;

	void UPVtxField(D3DXVECTOR3 pos);
	void Release(void);
	void Save(void);	// セーブ
	HRESULT Load(const char *pText);	// ロード

	CElevationOutside *GetElevation(void);
	float GetHeight(D3DXVECTOR3& pos, D3DXVECTOR3& move, bool &bLand, CObject::TYPE type);	// 高さ取得
protected:

private:
	int m_nTexIdx;	// テクスチャのインデックス番号
	POSTYPE m_postype;	// 位置の種類
};



#endif