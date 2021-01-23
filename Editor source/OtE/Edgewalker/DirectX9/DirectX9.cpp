// On the Edge Editor
// 
// Copyright © 2004-2021 Jukka Tykkyläinen
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, copy, modify, merge, publish, 
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or 
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "DirectX9.h"
#include "IDirectXResource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool CDirectX9::m_bUsingShader = false;

CDirectX9::CDirectX9()
:	m_pD3D(NULL), m_pD3DDevice(NULL), m_pD3DXSprite(NULL),
	m_bZBuffer(FALSE), m_bHAL(TRUE), m_bDeviceLost(FALSE),
	m_pEffect(NULL), m_pVertDecl(NULL)
{
}

CDirectX9::~CDirectX9()
{
	Release();
}

char CDirectX9::GetFormatBPP(D3DFORMAT format)
{
	switch(format)
	{
	// 32 bit modes
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
      return 32;
      break;

    // 24 bit modes
    case D3DFMT_R8G8B8:
      return 24;
      break;

    // 16 bit modes
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
      return 16;
      break;

    // 8 bit modes
    case D3DFMT_A8P8:
    case D3DFMT_P8:
      return 8;
      break;

    default:
       return 0;
	}
}

bool CDirectX9::CheckFormat(D3DFORMAT format, bool bWindowed, bool bHAL)
{
	if(FAILED(m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, (bHAL)? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
		format, format, bWindowed)))
		return FALSE;
	return TRUE;
}

HRESULT CDirectX9::Create(int iWidth, int iHeight, int iBPP, bool bWindowed, LPCTSTR strTitle, DWORD dwFlags, bool bVSync, bool bZBuffer, bool bMaximizeBox)
{
#ifdef _DEBUG
	if (bWindowed == FALSE)
	{
		bWindowed = TRUE;
		CPrinter::log("Forcing application to windowed mode (debug).");
	}
#endif

	if(FAILED(CreateAppWindow(iWidth, iHeight, bWindowed, strTitle, bMaximizeBox)))
	{
		CPrinter::log("Failed creating application window", LOG_ERROR);
		return E_FAIL;
	}

	CPrinter::log("Application window created succesfully");

	HRESULT hres;
	if((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		CPrinter::log("DirectX 9 not installed", LOG_ERROR);
		return E_FAIL;
	}

	D3DDISPLAYMODE d3ddpm;
	hres = m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddpm);
	if(FAILED(hres))
	{
		CPrinter::log("GetAdapterDisplayMode failed", LOG_ERROR);
		return E_FAIL;
	}

	ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	m_d3dpp.Windowed = bWindowed;
	m_d3dpp.BackBufferFormat = d3ddpm.Format;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	m_d3dpp.hDeviceWindow = GetWindow();

	D3DFORMAT Format, altFormat;

	
	m_bHAL = FALSE;

	if(bWindowed == TRUE)
	{
		CPrinter::log("Starting in windowed mode...");

		m_d3dpp.BackBufferFormat = d3ddpm.Format;

		//Set HAL if supported
		if(CheckFormat(d3ddpm.Format, TRUE, TRUE) == TRUE)
		{
			m_bHAL = TRUE;
		}
		else
		{
			CPrinter::log("Hardware acceleration not supported. Switching to software emulation mode.");
			if(CheckFormat(d3ddpm.Format, TRUE, FALSE) == FALSE)
			{
				CPrinter::log("Couldn't begin sotware emulation.");
				Release();
				return FALSE;
			}
		}
	}
	else
	{
		/**
		 * Fullscreen mode
		 */
		CPrinter::log("Starting in fullscreen mode...");

		m_d3dpp.Windowed = FALSE;

		if(bVSync)
			m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

		m_d3dpp.BackBufferWidth = iWidth;
		m_d3dpp.BackBufferHeight = iHeight;
		//m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

		switch(iBPP)
		{
		case 32:
			Format = D3DFMT_X8R8G8B8;
			altFormat = D3DFMT_X8R8G8B8;
			break;
		case 24:
			CPrinter::log("24-bit mode is slow", LOG_WARNING);
			Format = D3DFMT_R8G8B8;
			altFormat = D3DFMT_R8G8B8;
			break;
		case 16:
			Format = D3DFMT_R5G6B5;
			altFormat = D3DFMT_X1R5G5B5;
			break;
		case 8:
			CPrinter::log("8 bit 3D rendering not supported", LOG_FATALERROR);
			return E_FAIL;
		default:
			CPrinter::log("No acceptable BPP given", LOG_FATALERROR);
			return E_FAIL;
		}

		if(CheckFormat(Format, FALSE, TRUE) == TRUE)
		{
			m_bHAL = TRUE;
		}
		else
		{
			if(CheckFormat(altFormat, FALSE, TRUE) == TRUE)
			{
				m_bHAL = TRUE;
				Format = altFormat;
			}
			else
			{
				if(CheckFormat(Format, FALSE, FALSE) == FALSE)
				{
					if(CheckFormat(altFormat, FALSE, FALSE) == FALSE)
					{
						CPrinter::log("Couldn't find working color format", LOG_ERROR);
						Release();
						return E_FAIL;
					}
					else
						Format = altFormat;
				}
			}
		}

		m_d3dpp.BackBufferFormat = Format;
	}

	if((m_bZBuffer = bZBuffer) == TRUE)
	{
		m_d3dpp.EnableAutoDepthStencil = TRUE;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	}
	else
		m_d3dpp.EnableAutoDepthStencil = FALSE;

	//Create D3D Device
	if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, (m_bHAL) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF, 
		m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp, &m_pD3DDevice)))
	{
		// Failed creating D3D device with hardware vertex processing. Trying now with software mode
		CPrinter::log("Hardware acceleration not supported, switching to software mode");

		if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, (m_bHAL) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF, 
		m_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dpp, &m_pD3DDevice)))
		{
			CPrinter::log("Couldn't begin software emulation", LOG_ERROR);
			Release();
			return E_FAIL;
		}
	}

	// Create the D3DXSprite interface
	hres = D3DXCreateSprite(m_pD3DDevice,&m_pD3DXSprite);
	if(FAILED(hres))
	{
		CPrinter::log("Failed creating sprite interface", LOG_ERROR);
		Release();
		return E_FAIL;
	}

	CPrinter::log("Direct3D succesfully initialized");

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	RECT r = {0, 0, m_iWidth, m_iHeight};
	SetViewport(r);

	SetDefaultRenderStates();

	// initialise the timer
	m_Timer.Start();

	// call pure virtual OnCreate
	hres = OnCreate();
	if (FAILED(hres))
	{
		Release();
		return hres;
	}

	SetActive(TRUE);

	return S_OK;
}

void CDirectX9::Release()
{
	if(m_pVertDecl)
	{
		m_pVertDecl->Release();
		m_pVertDecl = NULL;
	}
	if(m_pEffect)
	{
		m_pEffect->Release();
		m_pEffect = NULL;
	}
	if(m_pD3DXSprite)
	{
		m_pD3DXSprite->Release();
		m_pD3DXSprite = NULL;
	}
	if(m_pD3DDevice)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	if(m_pD3D)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}

	IApplication::Release();
}

bool CDirectX9::RestoreDevice()
{
	CPrinter::log("IDirect3DDevice9 lost, trying to reset...", LOG_ID_D3DDEVICELOST);

	OnDeviceLost();

	HRESULT hres = m_pD3DDevice->Reset(&m_d3dpp);
	if(FAILED(hres))
	{
		if(hres == D3DERR_DRIVERINTERNALERROR)
		{
			CPrinter::log("IDirect3DDevice9::Reset failed: internal driver error (fatal). Shutting down...", LOG_ERROR);
			Close();
		}
		else if(hres == D3DERR_OUTOFVIDEOMEMORY)
		{
			CPrinter::log("Failed restarting Direct3DDevice object: out of video memory", LOG_ERROR);
		}
		else if(hres == D3DERR_DEVICELOST)
		{
			CPrinter::log("Failed restarting Direct3DDevice object: device lost", LOG_ID_D3DDEVICERESTOREFAILED, LOG_ERROR);
		}
		else if(hres == D3DERR_INVALIDCALL)
		{
			CPrinter::log("Failed restarting Direct3DDevice object: invalid call", LOG_ID_D3DDEVICERESTOREFAILED, LOG_ERROR);
		}
		else
			CPrinter::log("Failed restarting Direct3DDevice object.", LOG_ID_D3DDEVICERESTOREFAILED, LOG_ERROR);

		return FALSE;
	}

	SetDefaultRenderStates();
	CPrinter::log("Direct3DDevice succesfully recovered");

	m_bDeviceLost = FALSE;

	return OnRestore();
}

void CDirectX9::OnDeviceLost()
{
	IDirectXResource::UpdateOnDeviceLost();
	if(FAILED(m_pD3DXSprite->OnLostDevice()))
	{
		CPrinter::logError("Failed releasing D3D sprite interface after device lost");
	}
}

bool CDirectX9::OnRestore()
{
	bool succeed = true;
	if(FAILED(m_pD3DXSprite->OnResetDevice()))
	{
		CPrinter::logError("Failed restoring D3D sprite interface after device lost");
		succeed = false;
	}

	if (m_pEffect != NULL)
		m_pEffect->OnResetDevice();

	IDirectXResource::UpdateOnDeviceReset();

	return succeed;
}

void CDirectX9::SetDefaultRenderStates()
{
	/*m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, (m_bZBuffer)?D3DZB_TRUE:D3DZB_FALSE);
	m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA(0,0,0,255));
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	//m_pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	//m_pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);

	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	D3DXMATRIX matIdentity, matProj;

	D3DXMatrixIdentity(&matIdentity);
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, (float)m_iWidth/(float)m_iHeight, 1.0f, DEFAULT_DRAWDIST);

	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &matIdentity);
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &matIdentity);*/

	if(m_pD3DDevice == NULL)
	{
		CPrinter::logError("Couldn't set default render states: device pointer null");
		return;
	}

	// set the world matrix into the identity
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &mWorld);
	
	// initialize the camera
	D3DXMATRIX mCamera;
	D3DXMatrixLookAtLH(&mCamera, &D3DXVECTOR3(0.f, 0.f, -50.f), 
		&D3DXVECTOR3(0.f, 0.f, 0.f), &D3DXVECTOR3(0.f, 1.f, 0.f));
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &mCamera);

	// initialize the projection
	D3DXMATRIX mProjection;
	D3DXMatrixPerspectiveFovLH(&mProjection, D3DX_PI/4, (float)m_iWidth/(float)m_iHeight, 1.0f, DEFAULT_DRAWDIST);
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mProjection);

	// initialize the basic light
	D3DLIGHT9 light;
	::memset(&light, 0, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Direction.z = 1.0f;
	light.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	light.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	light.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);

	m_pD3DDevice->SetLight(0, &light);
	m_pD3DDevice->LightEnable(0, TRUE);

	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0x00FFFFFF);

	// initialize the basic material
	D3DMATERIAL9 material;
	::memset(&material, 0, sizeof(D3DMATERIAL9));
	material.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	material.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	m_pD3DDevice->SetMaterial(&material);

	m_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);

	// alpha blending settings
	m_pD3DDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// set the texture sampling states
	int i;
	for(i=0;i<8;i++)
	{
		m_pD3DDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pD3DDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pD3DDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		m_pD3DDevice->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		m_pD3DDevice->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		//m_pD3DDevice->SetSamplerState(i, D3DSAMP_MAXMIPLEVEL, 0);
	}

	// set the default shaders
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader(NULL);
}

void CDirectX9::SetViewport(RECT &r)
{
	D3DVIEWPORT9 v;
	v.X = r.left;
	v.Y = r.top;
	v.Width = r.right - r.left;
	v.Height = r.bottom - r.top;
	v.MinZ = 0.0f;
	v.MaxZ = 1.0f;
	m_pD3DDevice->SetViewport(&v);
}

bool CDirectX9::BeginScene(bool bBackBuffer, bool bZBuffer, D3DCOLOR color)
{
	HRESULT hres;

	if(m_pD3DDevice == NULL)
		return FALSE;

	if(m_bDeviceLost)
	{
		if(FAILED(hres = m_pD3DDevice->TestCooperativeLevel()))
		{
			if(hres == D3DERR_DEVICELOST)
				return FALSE;
			else if(hres == D3DERR_DEVICENOTRESET)
				RestoreDevice();
		}
	}

	DWORD flags = 0;
	if(bBackBuffer)
		flags |= D3DCLEAR_TARGET;

	if(bZBuffer)
		flags |= D3DCLEAR_ZBUFFER;

	hres = m_pD3DDevice->Clear(0, NULL, flags, D3DCOLOR_RGBA(0,0,0, 255), 1.0f, 0);

	if(FAILED(hres))
	{
		// DirectX9 Clear failed
		CPrinter::log("DirectX9 clear failed", LOG_ID_DXCLEAR_FAILED, LOG_ERROR);
	}

	if(FAILED(m_pD3DDevice->BeginScene()))
	{
		// Couldn't begin scene
		CPrinter::log("Direct3DDevice::BeginScene failed", LOG_ID_BEGINSCENE_FAILED, LOG_ERROR);
		/*if(FAILED(hres = m_pD3DDevice->TestCooperativeLevel()))
		{
			if(hres == D3DERR_DEVICENOTRESET)
			{
				//CPrinter::log("Device reset 2");
				RestoreDevice();
			}
		}*/
		return FALSE;
	}

	return TRUE;
}

bool CDirectX9::EndScene()
{
	HRESULT hres;
	hres = m_pD3DDevice->EndScene();

	m_bUsingShader = false;

	if(FAILED(hres))
	{
		CPrinter::log("Direct3DDevice::EndScene failed", LOG_ID_ENDSCENE_FAILED, LOG_ERROR);
		return FALSE;
	}

	// Flip backbuffer to screen
	hres = m_pD3DDevice->Present(NULL, NULL, GetWindow(), NULL);
	if(FAILED(hres))
	{
		/*if(FAILED(hres = m_pD3DDevice->TestCooperativeLevel()))
		{
			if(hres == D3DERR_DEVICENOTRESET)
			{
				//CPrinter::log("Device reset 1");
				RestoreDevice();
			}
		}*/

		if(hres == D3DERR_DRIVERINTERNALERROR)
		{
			CPrinter::log("Direct3DDevice::Present failed: internal driver error. Shutting down...", LOG_ERROR);
			Close();
		}
		else if(hres == D3DERR_DEVICELOST || D3DERR_DEVICENOTRESET)
		{
			CPrinter::log("Direct3DDevice::Present failed: device lost", LOG_ID_PRESENT_FAILED, LOG_ERROR);
			m_bDeviceLost = TRUE;
		}
		else if(hres == D3DERR_INVALIDCALL)
			CPrinter::log("Direct3DDevice::Present failed: invalid internal call", LOG_ID_PRESENT_FAILED, LOG_ERROR);

		return FALSE;
	}

	return TRUE;
}

bool CDirectX9::BeginSprite(DWORD dwFlags)
{
	m_bUsingShader = false;

	if(FAILED(m_pD3DXSprite->Begin(dwFlags | D3DXSPRITE_ALPHABLEND)))
	{
		CPrinter::log("ID3DXSprite::BeginSprite failed", LOG_ID_BEGINSPRITE_FAILED, LOG_ERROR);
		return FALSE;
	}
	return TRUE;
}

bool CDirectX9::EndSprite()
{
	if(FAILED(m_pD3DXSprite->End()))
	{
		CPrinter::log("ID3DXSprite::EndSprite failed", LOG_ID_ENDSPRITE_FAILED, LOG_ERROR);
		return FALSE;
	}
	return TRUE;
}

UINT CDirectX9::BeginShader()
{
	if(m_pEffect == NULL)
		return 0;

	UINT cPass;
	if(FAILED(m_pEffect->Begin( &cPass, 0 )))
	{
		CPrinter::log("BeginShader failed", LOG_ID_BEGINSHADER_FAILED, LOG_ERROR);
		return 0;
	}

	if(FAILED(m_pEffect->BeginPass(0)))
	{
		CPrinter::log("Shader's BeginPass failed", LOG_ID_BEGINSHADERPASS_FAILED, LOG_ERROR);
		return 0;
	}

	m_bUsingShader = true;

	return cPass;
}

bool CDirectX9::EndShader()
{
	if(m_pEffect == NULL)
		return FALSE;

	m_bUsingShader = false;

	if(FAILED(m_pEffect->EndPass()))
	{
		CPrinter::log("Shader's EndPass failed", LOG_ID_ENDSHADERPASS_FAILED, LOG_ERROR);
		return FALSE;
	}

	if(FAILED(m_pEffect->End()))
	{
		CPrinter::log("End Shader failed", LOG_ID_ENDSHADER_FAILED, LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}

void CDirectX9::SetTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX *pMatrix)
{
	m_pD3DDevice->SetTransform(state, pMatrix);
}

void CDirectX9::SetMaterial(CMaterial &material)
{
	m_pD3DDevice->SetMaterial(&material.m_material);
	SetTexture(material.m_pTexture);
}

void CDirectX9::SetTexture(CTexture *pTex)
{
	if(pTex == NULL)
		return;
	m_pD3DDevice->SetTexture(0, pTex->GetTexture());
}

void CDirectX9::SetAmbientLight(D3DCOLOR color)
{
	m_ambientColor = color;
	m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, color);
}

void CDirectX9::Draw2DImage(CTexture *tex, D3DXVECTOR2 pos, D3DCOLOR modColor)
{
	if(tex == NULL)
		return;

	RECT r;
	r.left = 0;
	r.top = 0;
	r.bottom = tex->GetOriginalHeight();
	r.right = tex->GetOriginalWidth();

	//D3DXVECTOR3 rotCenter((float)r.right*0.5f, (float)r.bottom*0.5f, 0);
	//D3DXVECTOR3 rotCenter(0.f, 0.f, 0);
	D3DXVECTOR3 pos3(pos.x, pos.y, 0.f);

	m_pD3DXSprite->Draw(tex->GetTexture(), &r, NULL, &pos3, modColor);
}

D3DLIGHT9 CDirectX9::SetupDirectionalLight(float r, float g, float b, D3DXVECTOR3 dir)
{
	D3DLIGHT9 l;

	ZeroMemory(&l, sizeof(D3DLIGHT9));

	l.Type = D3DLIGHT_DIRECTIONAL;

	l.Diffuse.r = l.Ambient.r = r;
	l.Diffuse.g = l.Ambient.g = g;
	l.Diffuse.b = l.Ambient.b = b;

	D3DXVec3Normalize(&dir, &dir);
	l.Direction = (D3DVECTOR)dir;

	return l;
}

D3DLIGHT9 CDirectX9::SetupDirectionalLight(D3DCOLOR color, D3DXVECTOR3 dir)
{
	D3DLIGHT9 l;

	ZeroMemory(&l, sizeof(D3DLIGHT9));

	l.Type = D3DLIGHT_DIRECTIONAL;

	float r = D3DCOLOR_GetR(color) * ONE_PER_255;
	float g = D3DCOLOR_GetG(color) * ONE_PER_255;
	float b = D3DCOLOR_GetB(color) * ONE_PER_255;
	float a = D3DCOLOR_GetA(color) * ONE_PER_255;

	l.Diffuse.r = l.Ambient.r = r;
	l.Diffuse.g = l.Ambient.g = g;
	l.Diffuse.b = l.Ambient.b = b;
	l.Diffuse.a = l.Ambient.a = a;

	D3DXVec3Normalize(&dir, &dir);
	l.Direction = (D3DVECTOR)dir;

	return l;
}

void CDirectX9::SaveScreenToFile(const char *filename)
{
	IDirect3DSurface9 *screen = NULL;
	if(FAILED(m_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, (IDirect3DSurface9**)&screen)))
	{
		CPrinter::log("Failed taking screenshot: couldn't get backbuffer", LOG_ERROR);
		return;
	}

	HRESULT hr;
	if(FAILED(hr = D3DXSaveSurfaceToFile(filename, D3DXIFF_JPG, screen, NULL, NULL)))
	{
		if(hr == D3DERR_INVALIDCALL)
			CPrinter::log("Failed taking screenshot: invalid call", LOG_ERROR);

		CPrinter::log("Failed taking screenshot", LOG_ERROR);
	}
	screen->Release();
}

void CDirectX9::TakeScreenshot(const char *filename)
{
	SaveScreenToFile(filename);
}

bool CDirectX9::EnableZBuffer(bool bValue)
{
	if(m_pD3DDevice == NULL)
		return FALSE;

	if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, bValue ? D3DZB_TRUE : D3DZB_FALSE)))
		return FALSE;

	return TRUE;
}

bool CDirectX9::EnableLighting(bool bValue)
{
	if(m_pD3DDevice == NULL)
		return FALSE;

	if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, bValue)))
		return FALSE;

	return TRUE;
}

void CDirectX9::RenderWireframeAABB(aabbox3d &aabb, bool doubleFrame, float dist, bool disableZBuffer, D3DXCOLOR color)
{
	if(disableZBuffer)
		EnableZBuffer(false);

	//dx->EnableLighting(false);
	LPDIRECT3DDEVICE9 pd = GetDevice();
	m_pD3DDevice->SetTexture(0, NULL);

	DWORD fvfBack;
	m_pD3DDevice->GetFVF(&fvfBack);
	m_pD3DDevice->SetFVF(D3DFVF_XYZ);

	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m);

	D3DMATERIAL9 material;
	ZeroMemory(&material, sizeof(material));
	/*material.Diffuse.r = 100.f;
	material.Diffuse.g = 100.f;
	material.Diffuse.b = 100.f;
	material.Diffuse.a = 100.f;*/
	material.Diffuse = color;
	material.Ambient = material.Diffuse;
	m_pD3DDevice->SetMaterial(&material);

	if(doubleFrame)
	{
		aabbox3d frame1(D3DXVECTOR3(aabb.minCorner.x + dist, aabb.minCorner.y + dist, aabb.minCorner.z + dist),
			D3DXVECTOR3(aabb.maxCorner.x - dist, aabb.maxCorner.y - dist, aabb.maxCorner.z - dist));

		aabbox3d frame2(D3DXVECTOR3(aabb.minCorner.x - dist, aabb.minCorner.y - dist, aabb.minCorner.z - dist),
			D3DXVECTOR3(aabb.maxCorner.x + dist, aabb.maxCorner.y + dist, aabb.maxCorner.z + dist));

		RenderWireframeAABB(frame1);
		RenderWireframeAABB(frame2);
	}
	else
		RenderWireframeAABB(aabb);

	m_pD3DDevice->SetFVF(fvfBack);

	//dx->EnableLighting(true);
	if(disableZBuffer)
		EnableZBuffer(true);
}

void CDirectX9::RenderWireframeAABB(aabbox3d &aabb)
{
	D3DXVECTOR3 points[8];
	aabb.GetCorners(points);

	WORD indeces[24] = {0,1,1,3,3,2,2,0,
						4,5,5,7,7,6,6,4,
						0,4,1,5,2,6,3,7};

	m_pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 24, 12, 
		indeces, D3DFMT_INDEX16, points, sizeof(D3DXVECTOR3));
}

bool CDirectX9::LoadEffect(const char *effectFilename)
{
	if (effectFilename == NULL)
		return false;

	if(m_pVertDecl == NULL)
	{
		D3DVERTEXELEMENT9 aVertDecl[] =
		{
			{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
			{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			D3DDECL_END()
		};

		if(FAILED(m_pD3DDevice->CreateVertexDeclaration( aVertDecl, &m_pVertDecl ) ))
		{
			CPrinter::log("Failed creating vertex declaration", LOG_ID_VERTEXDECLARATION_FAILED, LOG_ERROR);
			return false;
		}
	}

	if (m_pEffect != NULL)
	{
		m_pEffect->Release();
		m_pEffect = NULL;
	}

	//DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_DEBUG;
	DWORD dwShaderFlags = D3DXSHADER_DEBUG;

	#ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
#ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

	HRESULT hr = D3DXCreateEffectFromFile( m_pD3DDevice, effectFilename, NULL, NULL, dwShaderFlags,
                                        NULL, &m_pEffect, NULL );
	if (FAILED(hr))
	{
		m_pEffect = NULL;
		CPrinter::logError("Failed loading effect");
		return false;
	}
	
	return true;
}

