//-----------------------
//
// ビルボード表示処理
//
//-----------------------

#include "main.h" 
#include "billboard.h"
#include "camera.h"
#include "shadow.h"

//グローバル変数宣言
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffBillboard = NULL;			//頂点バッファへのポインタ
LPDIRECT3DTEXTURE9 g_pTextureBillboard = {};				//頂点バッファへのポインタ
D3DXVECTOR3 g_posBillboard;									//位置
D3DXVECTOR3 g_rotBillboard;									//向き
D3DXMATRIX g_mtxWorldBillboard;								//ワールドマトリックス
Billboard g_aBillboard[MAX_BILLBOARD];						//ビルボードの情報
int g_aIdxShadow;											//影の情報

//ビルボードの初期化処理
void InitBillboard(void)
{
	//デバイスへのポインタ
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/bullet000.png",
		&g_pTextureBillboard);

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_BILLBOARD,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffBillboard,
		NULL);

	//**************
	// 初期化
	//**************
	for (int nCntBillboard = 0; nCntBillboard < MAX_BILLBOARD; nCntBillboard++)
	{
		g_aBillboard[nCntBillboard].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);						//位置を初期化する
		g_aBillboard[nCntBillboard].bUse = false;												//使用していない状態にする
	}
	g_aIdxShadow = SetShadow(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));		//影の設定処理

//頂点情報へのポインタ
	VERTEX_3D * pVtx = NULL;

	//頂点バッファをロック
	g_pVtxBuffBillboard->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntBillboard = 0; nCntBillboard < MAX_BILLBOARD; nCntBillboard++)
	{
		//頂点座標の設定(ワールド座標ではなくローカル座標を設定)
		pVtx[0].pos = D3DXVECTOR3(-3.0f, 3.0f, 1.0f);
		pVtx[1].pos = D3DXVECTOR3(3.0f, 3.0f, 1.0f);
		pVtx[2].pos = D3DXVECTOR3(-3.0f, -3.0f, 1.0f);
		pVtx[3].pos = D3DXVECTOR3(3.0f, -3.0f, 1.0f);

		//各頂点の法線の設定(※ベクトルの大きさは1にする必要がある)
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		//頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		//テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}

	//頂点バッファをアンロック
	g_pVtxBuffBillboard->Unlock();
}
//ビルボードの終了処理
void UninitBillboard(void)
{
	//テクスチャの破棄
	if (g_pTextureBillboard != NULL)
	{
		g_pTextureBillboard->Release();
		g_pTextureBillboard = NULL;
	}

	//頂点バッファの破棄
	if (g_pVtxBuffBillboard != NULL)
	{
		g_pVtxBuffBillboard->Release();
		g_pVtxBuffBillboard = NULL;
	}
}
//ビルボードの更新処理
void UpdateBillboard(void)
{
	int nCntBillboard = 0;

	//影の位置の更新処理
	SetPositionShadow(g_aIdxShadow, D3DXVECTOR3(g_aBillboard[nCntBillboard].pos.x, 0.0f, g_aBillboard[nCntBillboard].pos.z));
}
//ビルボードの描画処理
void DrawBillboard(void)
{
	//デバイスへのポインタ
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	int nCntBillboard = 0;
	for (int nCntBillboard = 0; nCntBillboard < MAX_BILLBOARD; nCntBillboard++)
	{
		if (g_aBillboard[nCntBillboard].bUse == true)
		{
			//計算用マトリックス
			D3DXMATRIX mtxRot, mtxTrans;

			//ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aBillboard[nCntBillboard].mtxWorld);																	//行列初期化関数(第一引数の行列を単位行列に初期化)

			//ビューマトリックスを取得
			D3DXMATRIX mtxView;
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			//カメラの逆行列を設定	(2付くやつを消せばビルボードが上に向かない)
			g_aBillboard[nCntBillboard].mtxWorld._11 = mtxView._11;
			//g_aBillboard[nCntBillboard].mtxWorld._12 = mtxView._21;
			g_aBillboard[nCntBillboard].mtxWorld._13 = mtxView._31;
			//g_aBillboard[nCntBillboard].mtxWorld._21 = mtxView._12;
			//g_aBillboard[nCntBillboard].mtxWorld._22 = mtxView._22;
			//g_aBillboard[nCntBillboard].mtxWorld._23 = mtxView._32;
			g_aBillboard[nCntBillboard].mtxWorld._31 = mtxView._13;
			//g_aBillboard[nCntBillboard].mtxWorld._32 = mtxView._23;
			g_aBillboard[nCntBillboard].mtxWorld._33 = mtxView._33;

			//位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aBillboard[nCntBillboard].pos.x, g_aBillboard[nCntBillboard].pos.y, g_aBillboard[nCntBillboard].pos.z);		//行列移動関数(第一引数にx,y,z方向の移動行列を作成)
			D3DXMatrixMultiply(&g_aBillboard[nCntBillboard].mtxWorld, &g_aBillboard[nCntBillboard].mtxWorld, &mtxTrans);

			//ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aBillboard[nCntBillboard].mtxWorld);													//ワールド座標行列の設定

			//頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffBillboard, 0, sizeof(VERTEX_3D));

			//頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//ライトを無効にする
			pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

			//zバッファ
			pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, D3DCMP_ALWAYS);

			//アルファテストを有効
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

			//テクスチャの設定
			pDevice->SetTexture(0, g_pTextureBillboard);

			//ビルボードの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4 * nCntBillboard, 2);

			//テクスチャの解除
			pDevice->SetTexture(0, NULL);

			//アルファテストを無効に戻す
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

			//ライトを有効に戻す
			pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		}
	}
}
//ビルボードの設定処理
void SetBillboard(D3DXVECTOR3 pos)
{
	int nCntBillboard;
	for (nCntBillboard = 0; nCntBillboard < MAX_BILLBOARD; nCntBillboard++)
	{
		if (g_aBillboard[nCntBillboard].bUse == false)
		{
			g_aBillboard[nCntBillboard].pos = pos;
			g_aBillboard[nCntBillboard].bUse = true;
			break;
		}
	}
}
