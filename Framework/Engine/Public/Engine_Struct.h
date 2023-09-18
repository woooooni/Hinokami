#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagKeyFrame
	{
		float		fTime;

		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vPosition;

	}KEYFRAME;

	/* 빛의 정보를 담아놓기 위한 구조체. */
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_POINT, TYPE_DIRECTIONAL, TYPE_END };

		TYPE			eType;

		XMFLOAT4		vDirection;

		XMFLOAT4		vPosition;
		float			fRange;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;

	}LIGHTDESC;

	typedef struct tagMaterialDesc
	{
		char			strName[MAX_PATH];

		XMFLOAT4		vAmbient;
		XMFLOAT4		vDiffuse;
		XMFLOAT4		vSpecular;
		XMFLOAT4		vEmissive;
		class CTexture* pTexture[AI_TEXTURE_TYPE_MAX];
	}MATERIALDESC;

	typedef struct tagLineIndices16
	{
		unsigned short		_0, _1;
	}LINEINDICES16;

	typedef struct tagLineIndices32
	{
		unsigned long		_0, _1;
	}LINEINDICES32;

	typedef struct tagFaceIndices16
	{
		unsigned short		_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	}FACEINDICES32;

	///////////////
	/// VtxType ///
	///////////////
	// D3DDECLUSAGE
	typedef struct tagVertexTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexture;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertexTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXTEX_DECLARATION;

	typedef struct tagVertexNormalTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexNormalTexture_Declaration
	{
		static const unsigned int iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXNORTEX_DECLARATION;

	typedef struct tagVertexModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
		XMFLOAT3		vTangent;
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXMODEL_DECLARATION;

	typedef struct tagVertexAnimModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexture;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex; /* 이 정점에 영향을 주는 뼈의 인덱스 네개. */
		XMFLOAT4		vBlendWeight; /* 영향르 주고 있는 각 뼈대의 영향 비율 */
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;




	typedef struct tagVertexCubeTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexture;
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertexCubeTexture_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCUBETEX_DECLARATION;


	typedef struct tagVertexColor
	{
		XMFLOAT3		vPosition;
		XMFLOAT4		vColor;
	}VTXCOL;

	typedef struct ENGINE_DLL tagVertexColor_Declaration
	{
		static const unsigned int iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC Elements[iNumElements];
	}VTXCOL_DECLARATION;


	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		HWND		hWnd;
		WINMODE		eWinMode;
		unsigned int	iWinSizeX, iWinSizeY;

	} GRAPHIC_DESC;

	typedef struct tagMyHierarchyNode
	{
		char cName[MAX_PATH];
		char cParent[MAX_PATH];
		int  iDepth;
		XMFLOAT4X4 matTransform;
	} DATA_NODE;

	typedef struct tagMyMaterialData
	{
		char cNames[AI_TEXTURE_TYPE_MAX][MAX_PATH];
	} DATA_MATERIAL;

	typedef struct tagMyBoneData
	{
		char		cNames[MAX_PATH];
		XMFLOAT4X4	matOffset;
	} DATA_BONE;

	typedef struct tagMyMeshData
	{
		char				cName[MAX_PATH];
		int					iMaterialIndex;

		int					NumVertices;
		// VTXMODEL* pNonAnimVertices;
		// VTXANIMMODEL* pAnimVertices;

		int					iNumPrimitives;
		// FACEINDICES32* pIndices;

		int					iNumBones;
		DATA_BONE* pBones;

	} DATA_MESH;

	typedef struct tagMyChannelData
	{
		char szName[MAX_PATH];
		int iNumKeyFrames;
		KEYFRAME* pKeyFrames;
	} DATA_CHANNEL;

	typedef struct tagAnimData
	{
		char szName[MAX_PATH];

		bool bLoop;
		int	iNumChannels;

		float fDuration;
		float fTickPerSecond;

		DATA_CHANNEL* pChannel;
	} DATA_ANIM;

	typedef struct tagSceneData
	{

		int iNodeCount;
		DATA_NODE* pNodesData;

		int iMaterialCount;
		DATA_MATERIAL* pMaterialData;

		int iMeshCount;
		DATA_MESH* pMeshData;

		int iNumAnimations;
		DATA_ANIM* pAnimData;

	} DATA_SCENE;

}


#endif // Engine_Struct_h__
