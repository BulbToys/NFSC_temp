#include "nfsc.h"

const char* NFSC::FEStateManager::GetName()
{
	switch (this->vtable)
	{
		case 0x9D2420: return "FEStateManager";
		case 0x9D2540: return "FEBootFlowStateManager (PURE)";
		case 0x9D2688: return "FECareerStateManager";
		case 0x9D27A8: return "FEPostPursuitStateManager";
		case 0x9D28F8: return "FESmsStateManager";
		case 0x9D2A40: return "FEPauseStateManager";
		case 0x9D2B60: return "FEConfirmFeStateManager";
		case 0x9D2CA8: return "FEPostRaceStateManager";
		case 0x9D2E10: return "FEWorldMapStateManager";
		case 0x9D2F98: return "FEPhotoModeStateManager";
		case 0x9D30C0: return "FEOptionsStateManager";
		case 0x9D31E8: return "FEMovieStateManager";
		case 0x9D3330: return "FEOnlineMessengerStateManager";
		case 0x9D3480: return "FEOnlineLoginStateManager";
		case 0x9D35A0: return "FEOnlineQuickMatchStateManager";
		case 0x9D36C0: return "FEOnlineCustomMatchStateManager";
		case 0x9D37E8: return "FEOnlineGameroomStateManager";
		case 0x9D3920: return "FELeaderboardStateManager";
		case 0x9D3B20: return "FEManager";
		case 0x9D4430: return "FEMemcardStateManager";
		case 0x9E8B30: return "FEBootFlowStateManager";
		case 0x9E8E30: return "FEOnlineStateManager";
		case 0x9F7C30: return "FEShoppingCartStateManager";
		case 0x9F7E68: return "FEMainStateManager";
		case 0x9F8058: return "FEChallengeStateManager";
		case 0x9F8198: return "FEWingmanStateManager";
		case 0x9F82D0: return "FECrewManagementStateManager";
		case 0x9F8428: return "FEQuickRaceStateManager";
		case 0x9F8560: return "FEAutosculptStateManager";
		case 0x9F8688: return "FETemplateStateManager";
		case 0x9F87A8: return "FEMyCarsStateManager";
		case 0x9F88C8: return "FECarClassSelectStateManager";
		case 0x9F89E8: return "FEGameModeTutorialStateManager";
		case 0x9F8B08: return "FERaceStarterStateManager";
		case 0x9F8C40: return "FECrewLogoStateManager";
		case 0x9F8D60: return "FERewardsCardStateManager";
		case 0x9F8E80: return "FEBossStateManager";
		case 0x9F8FA0: return "FEGameOverStateManager";
		case 0x9F90C0: return "FEGameWonStateManager";
		case 0x9F9200: return "FECreditsStateManager";
		case 0x9F9320: return "FEPressStartStateManager";
		case 0x9FA010: return "FEStatisticsStateManager";
		case 0x9FAD58: return "FECustomizeStateManager";
		case 0x9FB350: return "FECarSelectStateManager";
		case 0x9FB488: return "FEDebugCarStateManager";
		default: return "(INVALID)";
	}
}