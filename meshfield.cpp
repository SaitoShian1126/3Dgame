//----------------------------
//
// ���b�V���t�B�[���h�\������
//
//----------------------------

#include "main.h" 
#include "meshfield.h"

//�}�N����`
#define MESHFIELD_X_BLOCK	(2)		//x�����̃u���b�N
#define MESHFIELD_Z_BLOCK	(2)		//z�����̃u���b�N
#define MESHFIELD_VERTEX	((MESHFIELD_X_BLOCK + 1) * (MESHFIELD_Z_BLOCK + 1))								//���_�� ((x�u���b�N�� + 1) * (z�u���b�N�� + 1))
#define MESHFIELD_INDEX		((MESHFIELD_X_BLOCK + 1) * 2 * MESHFIELD_Z_BLOCK + (MESHFIELD_Z_BLOCK - 1) * 2)	//�C���f�b�N�X�� ((x�u���b�N�� + 1) * 2 * z�u���b�N�� + (z�u���b�N�� - 1) * �k�ރ|���S���p���_��(z�����ɑ�������2���k�ރ|���S���p���_����������))
#define MESHFIELD_PRIMITIVE	(MESHFIELD_X_BLOCK * (MESHFIELD_Z_BLOCK * 2) + (MESHFIELD_Z_BLOCK - 1) * 4)		//�|���S���� (x�u���b�N�� * z�u���b�N�� * 2 + (z�u���b�N�� - 1) * �k�ރ|���S����(z�����ɑ�������4���k�ރ|���S������������))

//�O���[�o���ϐ��錾
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffMeshField = NULL;			//���_�o�b�t�@�ւ̃|�C���^
LPDIRECT3DTEXTURE9 g_pTextureMeshField = NULL;				//�e�N�X�`���ւ̃|�C���^
LPDIRECT3DINDEXBUFFER9 g_pIdxBuffMeshField;					//�C���f�b�N�X�o�b�t�@�ւ̃|�C���^
D3DXVECTOR3 g_posMeshField;									//�ʒu
D3DXVECTOR3 g_rotMeshField;									//����
D3DXMATRIX g_mtxWorldMeshField;								//���[���h�}�g���b�N�X

//���b�V���t�B�[���h�̏���������
void InitMeshField(void)
{
	//�f�o�C�X�ւ̃|�C���^
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MESHFIELD_VERTEX,	//���_���̌v�Z�}�N����`
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffMeshField,
		NULL);

	//���_���ւ̃|�C���^
	VERTEX_3D * pVtx = NULL;

	//���_�o�b�t�@�����b�N
	g_pVtxBuffMeshField->Lock(0, 0, (void**)&pVtx, 0);

	//�e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/Field001.jpg",
		&g_pTextureMeshField);

	//�C���f�b�N�X�o�b�t�@�̓ǂݍ���
	pDevice->CreateIndexBuffer(sizeof(WORD) * MESHFIELD_INDEX,	//�C���f�b�N�X���̌v�Z�}�N����`
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&g_pIdxBuffMeshField,
		NULL);

	//z�����̌J��Ԃ�
	for (int nCntMeshFieldZ = 0; nCntMeshFieldZ < MESHFIELD_Z_BLOCK + 1; nCntMeshFieldZ++)
	{
		//x�����̌J��Ԃ�
		for (int nCntMeshFieldX = 0; nCntMeshFieldX < MESHFIELD_X_BLOCK + 1; nCntMeshFieldX++)
		{
			//���_���W�̐ݒ�(���[���h���W�ł͂Ȃ����[�J�����W��ݒ�)
			pVtx[nCntMeshFieldX + (nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1))].pos = D3DXVECTOR3(-20.0f + (nCntMeshFieldX * 20.0f), 0.0f, 20.0f - (nCntMeshFieldZ * 20.0f));
			//�e���_�̖@���̐ݒ�(���x�N�g���̑傫����1�ɂ���K�v������)
			pVtx[nCntMeshFieldX + (nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1))].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			//���_�J���[�̐ݒ�						
			pVtx[nCntMeshFieldX + (nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1))].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			//�e�N�X�`�����W�̐ݒ�				  
			pVtx[nCntMeshFieldX + (nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1))].tex = D3DXVECTOR2(nCntMeshFieldX, nCntMeshFieldZ);
		}
	}

	//���_�o�b�t�@���A�����b�N
	g_pVtxBuffMeshField->Unlock();

	//�C���f�b�N�X�o�b�t�@�����b�N
	WORD * pIdx;
	g_pIdxBuffMeshField->Lock(0, 0, (void**)&pIdx, 0);
	int nCntMeshFieldX = 0;
	
	//�C���f�b�N�X�̐ݒ�
	//z�����̌J��Ԃ�
	for (int nCntMeshFieldZ = 0; nCntMeshFieldZ < MESHFIELD_Z_BLOCK; nCntMeshFieldZ++)
	{
		//x�����̌J��Ԃ�
		for (nCntMeshFieldX = 0; nCntMeshFieldX < MESHFIELD_X_BLOCK + 1; nCntMeshFieldX++)
		{
			pIdx[0] = (MESHFIELD_X_BLOCK + 1) + nCntMeshFieldX + nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1);
			pIdx[1] = (MESHFIELD_X_BLOCK + 1) + nCntMeshFieldX + nCntMeshFieldZ * (MESHFIELD_X_BLOCK + 1) - (MESHFIELD_X_BLOCK + 1);
			pIdx += 2;	//pIdx��2���Z����
		}
		//�k�ރ|���S���̐ݒ�
		if (nCntMeshFieldZ != MESHFIELD_Z_BLOCK)
		{
			pIdx[0] =  (MESHFIELD_X_BLOCK + 1) * nCntMeshFieldZ + MESHFIELD_X_BLOCK;
			pIdx[1] = (MESHFIELD_X_BLOCK + 1) * (nCntMeshFieldZ + 2);
			pIdx += 2;	//pIdx��2���Z����
		}
	}
	//���_�o�b�t�@���A�����b�N
	g_pIdxBuffMeshField->Unlock();
}
//���b�V���t�B�[���h�̏I������
void UninitMeshField(void)
{
	//�e�N�X�`���̔j��
	if (g_pTextureMeshField != NULL)
	{
		g_pTextureMeshField->Release();
		g_pTextureMeshField = NULL;
	}

	//���_�o�b�t�@�̔j��
	if (g_pVtxBuffMeshField != NULL)
	{			
		g_pVtxBuffMeshField->Release();
		g_pVtxBuffMeshField = NULL;
	}

	//�C���f�b�N�X�o�b�t�@�̔j��
	if (g_pIdxBuffMeshField != NULL)
	{	   
		g_pIdxBuffMeshField->Release();
		g_pIdxBuffMeshField = NULL;
	}
}
//���b�V���t�B�[���h�̍X�V����
void UpdateMeshField(void)
{

}
//���b�V���t�B�[���h�̕`�揈��
void DrawMeshField(void)
{
	//�f�o�C�X�ւ̃|�C���^
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//�v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans;

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorldMeshField);														//�s�񏉊����֐�(�������̍s���P�ʍs��ɏ�����)

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotMeshField.y, g_rotMeshField.x, g_rotMeshField.z);	//�s���]�֐�(�������Ƀ��[(y)�s�b�`(x)���[��(z)�����̉�]�s����쐬)
	D3DXMatrixMultiply(&g_mtxWorldMeshField, &g_mtxWorldMeshField, &mtxRot);						//�s��|���Z�֐�(��2���� * ��O������������Ɋi�[)

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, g_posMeshField.x, g_posMeshField.y, g_posMeshField.z);			//�s��ړ��֐�(��������x,y,z�����̈ړ��s����쐬)
	D3DXMatrixMultiply(&g_mtxWorldMeshField, &g_mtxWorldMeshField, &mtxTrans);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldMeshField);										//���[���h���W�s��̐ݒ�

	//���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, g_pVtxBuffMeshField, 0, sizeof(VERTEX_3D));

	//�C���f�b�N�X�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetIndices(g_pIdxBuffMeshField);

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	//�e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, g_pTextureMeshField);

	//���b�V���t�B�[���h�̕`��
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, MESHFIELD_VERTEX,0, MESHFIELD_PRIMITIVE);

	//�e�N�X�`���̉���
	pDevice->SetTexture(0, NULL);
}
