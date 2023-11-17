//=============================================================================
// 
//  敵拠点ヘッダー [enemybase.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYBASE_H_
#define _ENEMYBASE_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"

// 前方宣言

//==========================================================================
// クラス定義
//==========================================================================
// 敵拠点クラス定義
class CEnemyBase
{
public:

	// 構造体定義
	struct BaseData
	{
		D3DXVECTOR3 pos;		// 位置
		D3DXVECTOR3 rot;		// 向き
	};

	CEnemyBase();
	~CEnemyBase();

	HRESULT Init(const std::string pTextFile);
	void Uninit(void);
	void Update(void);

	static CEnemyBase *Create(const std::string pTextFile);
	HRESULT ReadText(const std::string pTextFile);		// 外部ファイル読み込み処理
	static BaseData *GetBaseData(int nCntStage);		// 敵拠点データ取得

private:

	static int m_nNumAll[mylib_const::MAX_STAGE];		// 最大数
	static std::vector<BaseData> m_aBaseData[mylib_const::MAX_STAGE];	// 配置の種類
	static bool m_bLoad;		// 情報読み込み判定
};



#endif