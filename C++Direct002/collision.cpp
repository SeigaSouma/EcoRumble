//=============================================================================
// 
//  コリジョン処理 [collision.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "collision.h"
#include "object2D.h"
#include "explosion.h"
#include "particle.h"
#include "manager.h"

#include "score.h"
#include "sound.h"
#include "particle.h"
#include "enemy.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 当たり判定
//==========================================================================
bool collision::Collision(D3DXVECTOR3 SelfPos, D3DXVECTOR2 SelfSize, CObject::TYPE TargetType)
{
#if TOPCUR
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULLが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObj->GetNext();

			// 種類の取得
			CObject::TYPE type = pObj->GetType();

			if (type == TargetType)
			{// 目的の種類だったら

				// 2D情報取得
				CObject2D *pObj2D = pObj->GetObject2D();

				// 位置取得
				D3DXVECTOR3 TargetPos = pObj2D->GetPosition();

				// サイズ取得
				D3DXVECTOR2 TargetSize = pObj2D->GetSize();

				// 当たり判定
				if (
					SelfPos.x - SelfSize.x < TargetPos.x + TargetSize.x &&
					SelfPos.x + SelfSize.x > TargetPos.x - TargetSize.x &&
					SelfPos.y - SelfSize.y < TargetPos.y + TargetSize.y &&
					SelfPos.y + SelfSize.y > TargetPos.y - TargetSize.y)
				{// 対象と敵が重なったら

					// 当たった後の処理
					HitAction(TargetType, pObj2D);

					// 当たった
					return true;
				}
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
	}
#endif
	// 当たってない
	return false;
}

//==========================================================================
// 当たり判定(オーバーロード)
//==========================================================================
bool collision::Collision(D3DXVECTOR3 SelfPos, D3DXVECTOR2 SelfSize, CObject2D *pSelfObj2D, CObject::TYPE TargetType)
{
#if TOPCUR
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULLが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObj->GetNext();

			// 種類の取得
			CObject::TYPE type = pObj->GetType();

			if (type == TargetType)
			{// 目的の種類だったら

				// 2D情報取得
				CObject2D *pObj2D = pObj->GetObject2D();

				// 位置取得
				D3DXVECTOR3 TargetPos = pObj2D->GetPosition();

				// サイズ取得
				D3DXVECTOR2 TargetSize = pObj2D->GetSize();

				// 当たり判定
				if (
					SelfPos.x - SelfSize.x < TargetPos.x + TargetSize.x &&
					SelfPos.x + SelfSize.x > TargetPos.x - TargetSize.x &&
					SelfPos.y - SelfSize.y < TargetPos.y + TargetSize.y &&
					SelfPos.y + SelfSize.y > TargetPos.y - TargetSize.y)
				{// 対象と敵が重なったら

					// 当たった後の処理
					HitAction(TargetType, pSelfObj2D, pObj2D);

					// 当たった
					return true;
				}
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
	}
#endif
	// 当たってない
	return false;
}

//==========================================================================
// 当たり判定(オーバーロード)
//==========================================================================
bool collision::Collision(float fPosLeft, float fPosRight, float fPosUp, float fPosDown, CObject::TYPE TargetType)
{
#if TOPCUR
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULLが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObj->GetNext();

			// 種類の取得
			CObject::TYPE type = pObj->GetType();

			if (type == TargetType)
			{// 目的の種類だったら

				// 2D情報取得
				CObject2D *pObj2D = pObj->GetObject2D();

				// 位置取得
				D3DXVECTOR3 TargetPos = pObj2D->GetPosition();

				// サイズ取得
				D3DXVECTOR2 TargetSize = pObj2D->GetSize();

				// 当たり判定
				if (
					fPosLeft < TargetPos.x + TargetSize.x &&
					fPosRight > TargetPos.x - TargetSize.x &&
					fPosUp < TargetPos.y + TargetSize.y &&
					fPosDown > TargetPos.y - TargetSize.y)
				{// 対象と敵が重なったら

					// 当たった後の処理
					HitAction(TargetType, pObj2D);

					// 当たった
					return true;
				}
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
	}
#endif

	// 当たってない
	return false;
}

//==========================================================================
// 当たったあとの処理
//==========================================================================
void collision::HitAction(CObject::TYPE TargetType, CObject2D *pObj2D)
{
	// 位置取得
	D3DXVECTOR3 TargetPos = pObj2D->GetPosition();

	switch (TargetType)
	{
	case CObject::TYPE_ITEM:
		// パーティクル生成
		my_particle::Create(TargetPos, my_particle::TYPE_RESULTMESSAGE);

		// 発射音
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_KYUPIIN);

		// 終了処理
		pObj2D->Uninit();
		break;

	case CObject::TYPE_BULLET:
		// 爆発の生成
		CExplosion::Create(TargetPos);

		// パーティクル生成
		my_particle::Create(TargetPos, my_particle::TYPE_MANYTXT_MINI);

		// 対象の終了処理
		pObj2D->Uninit();
		break;

	default:
		break;
	}
}

//==========================================================================
// 当たったあとの処理(オ－バーロード)
//==========================================================================
void collision::HitAction(CObject::TYPE TargetType, CObject2D *pSelfObj2D, CObject2D *pTargetObj2D)
{
	// 位置取得
	D3DXVECTOR3 TargetPos = pTargetObj2D->GetPosition();

	// 自分の種類取得
	CObject::TYPE SelfType = pSelfObj2D->GetType();
	//CEnemy *pEnemy;
	// 自分の種類
	switch (SelfType)
	{
	case CObject::TYPE_BULLET:

		// 対象の種類別処理
		switch (TargetType)
		{
		case CObject::TYPE_PLAYER:

			// 対象の終了処理
			pTargetObj2D->Uninit();

			// 爆発の生成
			CExplosion::Create(TargetPos);

			// パーティクル生成
			my_particle::Create(TargetPos, my_particle::TYPE_MANYTXT_MINI);
			break;

		case CObject::TYPE_ENEMY:

			//pSelfObj2D->GetTemplate<CEnemy>()->Hit(1);
			//pEnemy = pSelfObj2D->GetTemplate<CEnemy>();

			//pTargetObj2D->GetEnemy()->Hit(1);

			// 対象の終了処理
			pTargetObj2D->Uninit();

			// 爆発の生成
			CExplosion::Create(TargetPos);

			// パーティクル生成
			my_particle::Create(TargetPos, my_particle::TYPE_MANYTXT_MINI);
			break;

		default:
			break;
		}
		break;
	}

	switch (TargetType)
	{
	case CObject::TYPE_ITEM:
		// パーティクル生成
		my_particle::Create(TargetPos, my_particle::TYPE_RESULTMESSAGE);

		// 発射音
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_KYUPIIN);

		// 終了処理
		pTargetObj2D->Uninit();
		break;

	case CObject::TYPE_BULLET:
		// 爆発の生成
		CExplosion::Create(TargetPos);

		// パーティクル生成
		my_particle::Create(TargetPos, my_particle::TYPE_MANYTXT_MINI);

		// 対象の終了処理
		pTargetObj2D->Uninit();
		break;

	default:
		break;
	}
}

//==========================================================================
// ブロックの当たり判定
//==========================================================================
bool collision::CollisionBlock(CObject2D *(&pSelfObj2D))
{
	// 着地したかどうか
	bool bLand = false;

	// 位置取得
	D3DXVECTOR3 pos = pSelfObj2D->GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = pSelfObj2D->GetMove();

	// サイズ取得
	D3DXVECTOR2 size = pSelfObj2D->GetSize();

	// 前回の位置取得
	D3DXVECTOR3 posOld = pSelfObj2D->GetOldPosition();

	// 位置更新
	pos.x += move.x;

	// 位置設定
	pSelfObj2D->SetPosition(pos);

	// Xのブロック判定
	CollisionBlockX(pSelfObj2D);
	pos = pSelfObj2D->GetPosition();
	move = pSelfObj2D->GetMove();

	// 位置設定
	pSelfObj2D->SetPosition(pos);

	// 位置更新
	pos.y += move.y;

	// 位置設定
	pSelfObj2D->SetPosition(pos);

	// Yのブロック判定
	bLand = collision::CollisionBlockY(pSelfObj2D);
	pos = pSelfObj2D->GetPosition();
	move = pSelfObj2D->GetMove();

	if (pos.y >= SCREEN_HEIGHT)
	{// 地面に付いた

		bLand = true;	// 着地状態
		move.y = 0.0f;
	}

	// 位置設定
	pSelfObj2D->SetPosition(pos);

	// 移動量設定
	pSelfObj2D->SetMove(move);

	return bLand;
}

//==========================================================================
// ブロックの当たり判定(Y座標)
//==========================================================================
bool collision::CollisionBlockY(CObject2D *(&pSelfObj2D))
{
	// 位置取得
	D3DXVECTOR3 pos = pSelfObj2D->GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = pSelfObj2D->GetMove();

	// サイズ取得
	D3DXVECTOR2 size = pSelfObj2D->GetSize();

	// 前回の位置取得
	D3DXVECTOR3 posOld = pSelfObj2D->GetOldPosition();

	// 着地したかどうか
	bool bLand = false;

#if TOPCUR
	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULLが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObj->GetNext();

			// 種類の取得
			CObject::TYPE TargetType = pObj->GetType();

			if (TargetType == CObject::TYPE_BLOCK)
			{// ブロックだったら

				// 2D情報取得
				CObject2D *pObj2D = pObj->GetObject2D();

				// 位置取得
				D3DXVECTOR3 posBlock = pObj2D->GetPosition();

				// 前回の位置取得
				D3DXVECTOR3 posOldBlock = pObj2D->GetOldPosition();

				// サイズ取得
				D3DXVECTOR2 sizeBlock = pObj2D->GetSize();

				if (
					posBlock.x < pos.x + size.x &&		// 現在の位置がブロックの幅にいる
					posBlock.x + sizeBlock.x > pos.x - size.x
					)
				{// ブロックの幅にいる

					if (
						posOld.y <= posOldBlock.y &&	// 前回の位置が、ブロックの位置よりも上
						pos.y >= posBlock.y				// 現在の位置がブロックの位置よりも下
						)
					{// 上からめり込んだ時

						bLand = true;					// 着地状態
						pos.y = posOldBlock.y;			// ブロックの位置に戻す
						move.y = 0.0f;					// 止める
					}
					else if
						(
							posOld.y - size.y > posBlock.y + sizeBlock.y &&	// 前回の頭の位置が、ブロックの底よりも下
							pos.y - size.y < posBlock.y + sizeBlock.y		// 現在の頭の位置がブロックよりも上にいる
							)
					{// 下からめり込んだ時

						pos.y = (posBlock.y + sizeBlock.y) + size.y;	// ブロック底の位置に戻す
						move.y = 0.0f;					// 止める
					}
				}
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
	}
#endif
	// 位置設定
	pSelfObj2D->SetPosition(pos);

	// 移動量設定
	pSelfObj2D->SetMove(move);

	return bLand;
}

//==========================================================================
// ブロックの当たり判定(X座標)
//==========================================================================
bool collision::CollisionBlockX(CObject2D *(&pSelfObj2D))
{
	// 位置取得
	D3DXVECTOR3 pos = pSelfObj2D->GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = pSelfObj2D->GetMove();

	// サイズ取得
	D3DXVECTOR2 size = pSelfObj2D->GetSize();

	// 前回の位置取得
	D3DXVECTOR3 posOld = pSelfObj2D->GetOldPosition();

	// 着地したかどうか
	bool bLand = false;
#if TOPCUR

	for (int nCntPriority = 0; nCntPriority < mylib_const::PRIORITY_NUM; nCntPriority++)
	{
		// 先頭を保存
		CObject *pObj = CObject::GetTop(nCntPriority);

		while (pObj != NULL)
		{// NULLが来るまで無限ループ

			// 次のオブジェクトを一時保存
			CObject *pObjNext = pObj->GetNext();

			// 種類の取得
			CObject::TYPE TargetType = pObj->GetType();

			if (TargetType == CObject::TYPE_BLOCK)
			{// ブロックだったら

				// 2D情報取得
				CObject2D *pObj2D = pObj->GetObject2D();

				// 位置取得
				D3DXVECTOR3 posBlock = pObj2D->GetPosition();

				// 前回の位置取得
				D3DXVECTOR3 posOldBlock = pObj2D->GetOldPosition();

				// サイズ取得
				D3DXVECTOR2 sizeBlock = pObj2D->GetSize();

				// 横の判定
				if (posBlock.y < pos.y &&
					posBlock.y + sizeBlock.y > pos.y - size.y
					)
				{// 現在の位置がブロックの高さにいる

					if (
						posOld.x + size.x <= posOldBlock.x &&	// 前回の位置がブロックの位置よりも左
						pos.x + size.x >= posBlock.x			// 現在の位置がブロックの位置よりも右
						)
					{//左からめり込んだ場合

						pos.x = posOldBlock.x - size.x;	//ブロックの位置に戻す
						move.x = 0.0f;	// 止める
					}
					else if
						(
							posOld.x - size.x >= posOldBlock.x + sizeBlock.x &&	// 前回の位置がブロックの位置より右
							pos.x - size.x <= posBlock.x + sizeBlock.x			// 現在の位置がブロックの幅より左
							)
					{// 右からめり込んだ場合

						pos.x = posOldBlock.x + sizeBlock.x + size.x;	// ブロックの位置に戻す
						move.x = 0.0f;	// 止める
					}
				}
			}

			// 次のオブジェクトを代入
			pObj = pObjNext;
		}
	}
#endif
	// 位置設定
	pSelfObj2D->SetPosition(pos);

	// 移動量設定
	pSelfObj2D->SetMove(move);

	return bLand;
}