//----------------------------
//
// メッシュフィールド表示処理
//
//----------------------------

#include "main.h" 
#include "meshfield.h"

//マクロ定義
#define MESHFIELD_X_BLOCK	(2)		//x方向のブロック
#define MESHFIELD_Z_BLOCK	(2)		//z方向のブロック
#define MESHFIELD_VERTEX	((MESHFIELD_X_BLOCK + 1) * (MESHFIELD_Z_BLOCK + 1))								//頂点数 ((xブロック数 + 1) * (zブロック数 + 1))
#define MESHFIELD_INDEX		((MESHFIELD_X_BLOCK + 1) * 2 * MESHFIELD_Z_BLOCK + (MESHFIELD_Z_BLOCK - 1) * 2)	//インデックス数 ((xブロック数 + 1) * 2 * zブロック数 + (zブロック数 - 1) * 縮退ポリゴン用頂点数(z方向に増えたら2ずつ縮退ポリゴン用頂点数が増える))
#define MESHFIELD_PRIMITIVE	(MESHFIELD_X_BLOCK * (MESHFIELD_Z_BLOCK * 2) + (MESHFIELD_Z_BLOCK - 1) * 4)		//ポリゴン数 (xブロック数 * zブロック数 * 2 + (zブロック数 - 1) * 縮退ポリゴン数(z方向に増えたら4ずつ縮退ポリゴン数が増える))

//グローバル変数宣言
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffMeshField = NULL;			//頂点バッファへのポインタ
LPDIRECT3DTEXTURE9 g_pTextureMeshField = NULL;				//テクスチャへのポインタ
LPDIRECT3DINDEXBUFFER9 g_pIdxBuffMeshField;					//インデックスバッファへのポインタ
D3DXVECTOR3 g_posMeshField;									//位置
D3DXVECTOR3 g_rotMeshField;									//向き
D3DXMATRIX g_mtxWorldMeshField;								//ワールドマトリックス

//メッシュフィールドの初期化処理
void InitMeshField(void)
{
	//デバイスへのポインタ
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MESHFIELD_VERTEX,	//頂点数の計算マクロ定義
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffMeshField,
		NULL);

	//頂点情報へのポインタ
	VERTEX_3D * pVtx = NULL;

	//頂点バッファをロック
	g_pVtxBuffMeshField->Lock(0, 0, (void**)&pVtx, 0);

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/Field001.jpg",
		&g_pTextureMeshField);

	//インデックスバッファの読み込み
	pDevice->CreateIndexBuffer(sizeof(WORD) * MESHFIELD_INDEX,	//インデックス数の計算マクロ定義
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&g_pIdxBuffMeshField,
		NULL);

	//z方向の繰り返し
	for (int nCntMeshFieldZ = 0; nCntMeshFieldZ < MESHFIELD_Z_BLOCK + 1; nCntMeshFieldZ++)
	{
		//x方向の繰り返し
		for (int nCntMeshFieldX = 0; nCntMeshFieldX < MESHFIELD_X_BLOCK + 1; nCntMeshFieldX++)
		{
			//頂点座標の設定(ワールド座標ではなくローカル座標を設定)
			pVtx[nCntMeshFieldX + (nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1))].pos = D3DXVECTOR3(-20.0f + (nCntMeshFieldX * 20.0f), 0.0f, 20.0f - (nCntMeshFieldZ * 20.0f));
			//各頂点の法線の設定(※ベクトルの大きさは1にする必要がある)
			pVtx[nCntMeshFieldX + (nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1))].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			//頂点カラーの設定						
			pVtx[nCntMeshFieldX + (nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1))].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			//テクスチャ座標の設定				  
			pVtx[nCntMeshFieldX + (nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1))].tex = D3DXVECTOR2(nCntMeshFieldX, nCntMeshFieldZ);
		}
	}

	//頂点バッファをアンロック
	g_pVtxBuffMeshField->Unlock();

	//インデックスバッファをロック
	WORD * pIdx;
	g_pIdxBuffMeshField->Lock(0, 0, (void**)&pIdx, 0);
	int nCntMeshFieldX = 0;
	
	//インデックスの設定
	//z方向の繰り返し
	for (int nCntMeshFieldZ = 0; nCntMeshFieldZ < MESHFIELD_Z_BLOCK; nCntMeshFieldZ++)
	{
		//x方向の繰り返し
		for (nCntMeshFieldX = 0; nCntMeshFieldX < MESHFIELD_X_BLOCK + 1; nCntMeshFieldX++)
		{
			pIdx[0] = (MESHFIELD_X_BLOCK + 1) + nCntMeshFieldX + nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1);
			pIdx[1] = (MESHFIELD_X_BLOCK + 1) + nCntMeshFieldX + nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1) - (MESHFIELD_X_BLOCK + 1);
			pIdx += 2;	//pIdxを2加算する
		}
		//縮退ポリゴンの設定
		if (nCntMeshFieldZ != MESHFIELD_Z_BLOCK)
		{
			pIdx[0] =  (MESHFIELD_X_BLOCK + 1) * nCntMeshFieldZ + MESHFIELD_X_BLOCK;
			pIdx[1] = (MESHFIELD_X_BLOCK + 1) * (nCntMeshFieldZ + 2);
			pIdx += 2;	//pIdxを2加算する
		}
	}
	//頂点バッファをアンロック
	g_pIdxBuffMeshField->Unlock();
}
//メッシュフィールドの終了処理
void UninitMeshField(void)
{
	//テクスチャの破棄
	if (g_pTextureMeshField != NULL)
	{
		g_pTextureMeshField->Release();
		g_pTextureMeshField = NULL;
	}

	//頂点バッファの破棄
	if (g_pVtxBuffMeshField != NULL)
	{			
		g_pVtxBuffMeshField->Release();
		g_pVtxBuffMeshField = NULL;
	}

	//インデックスバッファの破棄
	if (g_pIdxBuffMeshField != NULL)
	{	   
		g_pIdxBuffMeshField->Release();
		g_pIdxBuffMeshField = NULL;
	}
}
//メッシュフィールドの更新処理
void UpdateMeshField(void)
{

}
//メッシュフィールドの描画処理
void DrawMeshField(void)
{
	//デバイスへのポインタ
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldMeshField);														//行列初期化関数(第一引数の行列を単位行列に初期化)

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotMeshField.y, g_rotMeshField.x, g_rotMeshField.z);	//行列回転関数(第一引数にヨー(y)ピッチ(x)ロール(z)方向の回転行列を作成)
	D3DXMatrixMultiply(&g_mtxWorldMeshField, &g_mtxWorldMeshField, &mtxRot);						//行列掛け算関数(第2引数 * 第三引数を第一引数に格納)

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, g_posMeshField.x, g_posMeshField.y, g_posMeshField.z);			//行列移動関数(第一引数にx,y,z方向の移動行列を作成)
	D3DXMatrixMultiply(&g_mtxWorldMeshField, &g_mtxWorldMeshField, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldMeshField);										//ワールド座標行列の設定

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffMeshField, 0, sizeof(VERTEX_3D));

	//インデックスバッファをデータストリームに設定
	pDevice->SetIndices(g_pIdxBuffMeshField);

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, g_pTextureMeshField);

	//メッシュフィールドの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, MESHFIELD_VERTEX,0, MESHFIELD_PRIMITIVE);

	//テクスチャの解除
	pDevice->SetTexture(0, NULL);
}
