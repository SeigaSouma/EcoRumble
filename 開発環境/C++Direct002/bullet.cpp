//=============================================================================
// 
//  弾処理 [bullet.cpp]
//  Author : 相馬靜雅
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
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\bullet_03.png"
#define WIDTH			(30.0f)							// 横幅
#define HEIGHT			(30.0f)							// 縦幅
#define MOVE_SPEED		(15.0f)							// 移動速度
#define ANIM_SPEED		(4)								// 読み込み間隔
#define MAX_PATTERN_U	(1)								// Uの分割数
#define MAX_PATTERN_V	(1)								// Vの分割数
#define MAX_PATTERN		(MAX_PATTERN_U)					// アニメーションパターンの最大数
#define MOVE_U			(1.0f / (float)MAX_PATTERN_U)	// U座標移動量
#define MOVE_V			(1.0f / (float)MAX_PATTERN_V)	// V座標移動量

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CBullet::m_apTextureFile[] =				// テクスチャのファイル
{
	NULL,
	"data\\TEXTURE\\bullet_01.png",
	"data\\TEXTURE\\bullet_03.png",
};
int CBullet::m_nNumAll = 0;		// 弾の総数

//==========================================================================
// コンストラクタ
//==========================================================================
CBullet::CBullet(int nPriority) : CObjectBillboard(nPriority), m_nLifeMax(40)
{
	// 値のクリア
	m_type = TYPE_NONE;
	m_nLife = 0;

	// テクスチャデータの配列分繰り返す
	for (int nCntTex = 0; nCntTex < sizeof(m_apTextureFile) / sizeof(*m_apTextureFile); nCntTex++)
	{
		m_nTexIdx[nCntTex] = 0;		// テクスチャのインデックス番号
	}

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// コンストラクタ
//==========================================================================
CBullet::CBullet(TYPE type, int nPriority) : CObjectBillboard(nPriority), m_nLifeMax(40)
{
	// 値のクリア
	m_type = type;
	m_nLife = 0;

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CBullet::~CBullet()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CBullet *CBullet::Create(TYPE type)
{
	// 生成用のオブジェクト
	CBullet *pBullet = NULL;

	if (pBullet == NULL)
	{// NULLだったら

		// メモリの確保
		pBullet = DEBUG_NEW CBullet;

		//if (pBullet->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pBullet;
		//	return NULL;
		//}

		if (pBullet != NULL)
		{// メモリの確保が出来ていたら

			// 種類設定
			pBullet->m_type = type;

			// 初期化処理
			pBullet->Init();
		}

		return pBullet;
	}

	return NULL;
}

//==========================================================================
// 生成処理(オーバーロード)
//==========================================================================
CBullet *CBullet::Create(TYPE type, const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	// 生成用のオブジェクト
	CBullet *pBullet = NULL;

	if (pBullet == NULL)
	{// NULLだったら

		// メモリの確保
		pBullet = DEBUG_NEW CBullet;

		//if (pBullet->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pBullet;
		//	return NULL;
		//}

		if (pBullet != NULL)
		{// メモリの確保が出来ていたら

			// 種類設定
			pBullet->m_type = type;

			// 位置割り当て
			pBullet->SetPosition(pos);

			// 向き割り当て
			pBullet->SetRotation(rot);

			// 初期化処理
			pBullet->Init();
		}

		return pBullet;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBullet::Init(void)
{
	HRESULT hr;

	// 各種変数の初期化
	SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// サイズ

	switch (m_type)	// 移動量設定
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

	m_nLife = m_nLifeMax;	// 寿命

	// テクスチャデータの配列分繰り返す
	for (int nCntTex = 0; nCntTex < sizeof(m_apTextureFile) / sizeof(*m_apTextureFile); nCntTex++)
	{
		// テクスチャの割り当て
		m_nTexIdx[nCntTex] = CManager::GetTexture()->Regist(m_apTextureFile[nCntTex]);

		// テクスチャの割り当て
		BindTexture(m_nTexIdx[nCntTex]);
	}

	// 種類の設定
	SetType(TYPE_BULLET);

	// 初期化処理
	hr = CObjectBillboard::Init();

	if (FAILED(hr))
	{// 失敗したとき

		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBullet::Uninit(void)
{
	// 終了処理
	CObjectBillboard::Uninit();

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CBullet::Update(void)
{
	// 位置更新
	UpdatePos();

	if (CollisionFrame() == true)
	{// 画面外出たら
		return;
	}

	// 敵と弾の当たり判定
	if (Collision() == true)
	{// 敵と当たっていたら
		return;
	}

	// 寿命減算
	m_nLife--;

	if (m_nLife <= 0)
	{// 寿命が尽きたら

		// 爆発の生成
		CExplosion::Create(GetPosition());
		my_particle::Create(GetPosition(), my_particle::TYPE_OFFSETTING);

		// 弾の削除
		Uninit();
		return;
	}

	int n = 0;

	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 移動処理
//==========================================================================
void CBullet::UpdatePos(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// 位置更新
	pos += move;
	/*move.y = 50.0f;
	nnnn++;
	pos.x = 0.0f * (nnnn * nnnn) + move.x * nnnn;
	pos.y = -GRAVITY * 0.1f * (nnnn * nnnn) + move.y * nnnn + 300.0f;
	pos.z = 0.0f * (nnnn * nnnn) + move.z * nnnn;*/

	// エフェクト設定
	switch (m_type)
	{
	case TYPE_PLAYER:
		CEffect3D::Create(
			pos,			// 位置
			D3DXVECTOR3(	// 移動量
				sinf(D3DX_PI * 0.5f + GetRotation().y) * -5.0f,
				0.0f,
				cosf(D3DX_PI * 0.5f + GetRotation().y) * -5.0f),
			D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f),	// 色
			80.0f,								// 半径
			10,									// 寿命
			CEffect3D::MOVEEFFECT_SUB,			// 減算種類
			CEffect3D::TYPE_NORMAL);

		CEffect3D::Create(
			pos,			// 位置
			D3DXVECTOR3(	// 移動量
				sinf(D3DX_PI * 0.5f + GetRotation().y) * -5.0f,
				0.0f,
				cosf(D3DX_PI * 0.5f + GetRotation().y) * -5.0f),
			D3DXCOLOR(1.0f, 0.6f, 0.3f, 1.0f),	// 色
			40.0f,								// 半径
			10,									// 寿命
			CEffect3D::MOVEEFFECT_SUB,			// 減算種類
			CEffect3D::TYPE_NORMAL);
		break;

	case TYPE_ENEMY:
		CEffect3D::Create(
			pos,			// 位置
			D3DXVECTOR3(	// 移動量
				sinf(D3DX_PI * 0.5f + GetRotation().y) * 5.0f,
				0.0f,
				cosf(D3DX_PI * 0.5f + GetRotation().y) * 5.0f),
			D3DXCOLOR(0.3f, 1.0f, 0.3f, 1.0f),	// 色
			80.0f,								// 半径
			10,									// 寿命
			CEffect3D::MOVEEFFECT_SUB,			// 減算種類
			CEffect3D::TYPE_NORMAL);

		CEffect3D::Create(
			pos,			// 位置
			D3DXVECTOR3(	// 移動量
				sinf(D3DX_PI * 0.5f + GetRotation().y) * 5.0f,
				0.0f,
				cosf(D3DX_PI * 0.5f + GetRotation().y) * 5.0f),
			D3DXCOLOR(1.0f, 0.6f, 0.3f, 1.0f),	// 色
			40.0f,								// 半径
			10,									// 寿命
			CEffect3D::MOVEEFFECT_SUB,			// 減算種類
			CEffect3D::TYPE_NORMAL);
		break;
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// 画面の判定
//==========================================================================
bool CBullet::CollisionFrame(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 画面外で削除
	//if (pos.x < 0.0f || pos.x > SCREEN_WIDTH || pos.y < 0.0f || pos.y > SCREEN_HEIGHT)
	//{
	//	// 弾の削除
	//	Uninit();
	//	return true;
	//}

	return false;
}

//==========================================================================
// 敵との当たり判定
//==========================================================================
bool CBullet::Collision(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// サイズ取得
	D3DXVECTOR2 size = GetSize();

	// 目標のタイプ
	//switch (m_type)
	//{
	//case TYPE_PLAYER:
	//	
	//	// 敵との当たり判定
	//	if (collision::Collision(pos, size, GetObject2D(), CObject::TYPE_ENEMY) == true)
	//	{// 当たっていたら

	//		CManager::GetScore()->Add(300);

	//		// 弾の終了処理
	//		Uninit();

	//		return true;
	//	}
	//	break;

	//case TYPE_ENEMY:
	//	// 敵との当たり判定
	//	if (collision::Collision(pos, size, GetObject2D(), CObject::TYPE_PLAYER) == true)
	//	{// 当たっていたら

	//		// 弾の終了処理
	//		Uninit();

	//		return true;
	//	}
	//}



	

	return false;
}

//==========================================================================
// 描画処理
//==========================================================================
void CBullet::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// ビルボードの描画
	CObjectBillboard::Draw();

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CBullet::SetVtx(void)
{
	// 頂点設定
	CObjectBillboard::SetVtx();
}

//==========================================================================
// 総数取得
//==========================================================================
int CBullet::GetNumAll(void)
{
	return m_nNumAll;
}
