#ifndef __XR_OBJECT_H__
#define __XR_OBJECT_H__

#include "fbasicvisual.h"
#include "xr_collide_form.h"
#include "fcontroller.h"

// refs
class ENGINE_API CCFModel;
class ENGINE_API CInifile;
class ENGINE_API CSector;
class ENGINE_API CCustomHUD;
class ENGINE_API NET_Packet;
struct ENGINE_API NodeCompressed;

enum ESectorMode						{EPM_AT_LOAD, EPM_AUTO_POLYGONAL, EPM_AUTO_AI };

//-----------------------------------------------------------------------------------------------------------
//CXR_Object
//������������ ����� ��� ��������� ��������
//-----------------------------------------------------------------------------------------------------------
class	ENGINE_API						CObject :	
	public DLL_Pure, 
	public CController,
	public pureDeviceDestroy,
	public pureDeviceCreate
{
protected:
	// some property variables
	bool								rbEnabled;
	bool								rbVisible;
	char*								ObjectName;

	CRandom								Random;
protected:
	// Geometric
	Fvector								vPosition;
	Fvector								vScale;
	Fmatrix								mRotate;
	CCFModel*							cfModel;

	LPSTR								pVisualName;
	FBasicVisual*						pVisual;

	// Visibility detection
	CSector*							pSector;
	ESectorMode							SectorMode;
	Fvector								vPositionPrevious;
	void								DetectSector	();

	// Shadow
	Shader*								sh_Shader;
	float								sh_Size;

	// Information and status
	void								StatusBegin		();
public:
	DWORD								net_ID;
	BOOL								net_Local;
	IC BOOL								Local()			{ return net_Local;		}
	IC BOOL								Remote()		{ return !net_Local;	}

	DWORD								dwMinUpdate;	// minimal bound of update time (sample: 20ms)
	DWORD								dwMaxUpdate;	// maximal bound of update time (sample: 200ms)
	DWORD								dwTimeStamp;	// last update global device time

	// AI connection
	DWORD								AI_NodeID;
	NodeCompressed*						AI_Node;
	float								AI_Lighting;

	// Geometry management
	Fmatrix								mTransform;
	Fmatrix								mTransformPredicted;
	DWORD								dwFrame_Predicted;
	void								UpdateTransform		(void);
	virtual void						PredictTransform	(void) 
	{ 
		if	(dwFrame_Predicted!=Device.dwFrame) {
			dwFrame_Predicted=Device.dwFrame;
			mTransformPredicted.set(mTransform);
		}
	}

	IC void								Center			(Fvector& C) const	{ VERIFY(pVisual); mTransform.transform_tiny(C,pVisual->bv_Position);}
	IC float							Radius			() const			{ VERIFY(pVisual); return pVisual->bv_Radius;}
	IC Fvector&							Position		() 					{ return vPosition; }
	IC Fvector&							Direction		() 					{ return mRotate.k; }
	IC Fmatrix&							Rotation		()					{ return mRotate;	}
	IC FBasicVisual*					Visual			()					{ return pVisual;   }
	IC CCFModel*						CFORM			() const			{ return cfModel;	}
	IC void								ForcePosition	(Fvector& P)		{ vPosition.set(P); UpdateTransform(); }
	IC Shader*							shadowShader	()					{ return sh_Shader;	}
	IC float							shadowSize		()					{ return sh_Size;	}

	// Name management
	IC const char *						cName			()					{ return ObjectName; }

	// Visible property
	PropertyGP(getVisible,setVisible)	BOOL bVisible;
	void								setVisible		(bool _Visible)		{ rbVisible = _Visible; /*if (pVisual) pVisual->bVisible = _Visible;*/ }
	BOOL								getVisible		()					{ return rbVisible;	}

	// Enabled property
	PropertyGP(getEnable,setEnable)		BOOL bEnabled;
	void __forceinline					setEnable		(bool _enabled)		{ rbEnabled = _enabled; if (cfModel) cfModel->Enable(_enabled); }
	BOOL __forceinline					getEnable		()					{ return rbEnabled;	}

	//---------------------------------------------------------------------
										CObject			();
	virtual								~CObject		();

	virtual void						Load			(CInifile* ini, const char * section);

	// Update
	virtual void						OnMoveVisible	(void );
	virtual void						Update			(DWORD dt);
	virtual void						net_Export		(NET_Packet* P) {};					// export to server
	virtual void						net_Import		(NET_Packet* P) {};					// import from server
	virtual BOOL						net_Relevant	()				{ return FALSE; };	// relevant for export to server

	// Collision
	virtual void						OnNear			(CObject* near)		{};

	// HUD
	virtual void						OnHUDDraw		(CCustomHUD* hud){;}

	// Device dependance
	virtual void						OnDeviceDestroy	();
	virtual void						OnDeviceCreate	();
};

#endif //__XR_OBJECT_H__
