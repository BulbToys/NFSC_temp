#include <Windows.h>

#include "..\core\bulbtoys.h"
#include "nfsc.h"

BOOL APIENTRY DllMain(HMODULE instance, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		// Version check :3
		if (!strncmp(reinterpret_cast<char*>(0x9F03D5), ":33", 3))
		{
			BulbToys::SetupParams params;
			params.instance = instance;
			params.GetDevice = +[](){
				return Read<IDirect3DDevice9*>(0xAB0ABC);
			};

			// NOTE: NFSCO will not support ASI plugins
			if (NFSC::BulbToys_IsNFSCO())
			{
				params.settings_file = "NFSCO_BulbToys.ini";
			}
			else
			{
				params.settings_file = "NFSC_BulbToys.ini";
			}

			BulbToys::Setup(params);
		}
		else
		{
			Error("Only the v1.4 English nfsc.exe (6,88 MB (7.217.152 bytes)) is supported.");
			return FALSE;
		}
	}

	return TRUE;
}