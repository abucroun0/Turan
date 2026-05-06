#include "../main.h"
#include "gui.h"
#include "CBinder.h"
#include "../game/game.h"
#include "../game/crosshair.h"
#include "../net/netgame.h"
#include "../game/RW/RenderWare.h"
#include "../chatwindow.h"
#include "../playertags.h"
#include "../dialog.h"
#include "../keyboard.h"
#include "../CSettings.h"
#include "..//scoreboard.h"
#include "../util/CJavaWrapper.h"
#include "../KillList.h"
#include "../voice/CVoiceChatClient.h"
#include "../CDebugInfo.h"
#include "../GButton.h"
#include "../game/Custom/MasterCheat.h"
#include <android/log.h>

extern CVoiceChatClient* pVoice;
extern CScoreBoard* pScoreBoard;
extern CChatWindow *pChatWindow;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CSettings *pSettings;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern CJavaWrapper *g_pJavaWrapper;
extern KillList *pKillList;
extern CCrossHair *pCrossHair;
extern CGame* pGame;
extern CGButton *pGButton;
extern MasterCheat *g_pMasterCheat;

//RwTexture*   p_logoTexture = nullptr;

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_Init();
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();

/*
	Все координаты GUI-элементов задаются
	относительно разрешения 1920x1080
*/


#define LOG_TAG "Turan"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#define MULT_X	0.00052083333f	// 1/1920
#define MULT_Y	0.00092592592f 	// 1/1080
int CGUI::GetEat(){
	return eat;
}
CGUI::CGUI()
{
	Log(OBFUSCATE("Initializing GUI.."));

	m_bMouseDown = 0;
	m_vTouchPos = ImVec2(-1, -1);
	m_bNextClear = false;
	m_bNeedClearMousePos = false;

	// setup ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplRenderWare_Init();

	// scale
	m_vecScale.x = io.DisplaySize.x * MULT_X;
	m_vecScale.y = io.DisplaySize.y * MULT_Y;
	// font Size
	m_fFontSize = ScaleY( pSettings->GetReadOnly().fFontSize );

	Log(OBFUSCATE("GUI | Scale factor: %f, %f Font size: %f"), m_vecScale.x, m_vecScale.y, m_fFontSize);

	// setup style
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = ScaleY(55.0f);
	style.WindowBorderSize = 0.0f;
	ImGui::StyleColorsDark();

	// load fonts
	char path[0xFF];
	sprintf(path, OBFUSCATE("%sSAMP/fonts/%s"), g_pszStorage, pSettings->GetReadOnly().szFont);
	// cp1251 ranges
	static const ImWchar ranges[] =
	{
		0x0020, 0x0080,
		0x00A0, 0x00C0,
		0x0400, 0x0460,
		0x0490, 0x04A0,
		0x2010, 0x2040,
		0x20A0, 0x20B0,
		0x2110, 0x2130,
		0
	};

	Log(OBFUSCATE("GUI | Loading font: %s"), pSettings->GetReadOnly().szFont);
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	Log(OBFUSCATE("GUI | ImFont pointer = 0x%X"), m_pFont);

	m_pFontGTAWeap = LoadFont(OBFUSCATE("gtaweap3.ttf"), 0);
	style.WindowRounding = 0.0f;

	m_pSplashTexture = nullptr;
	m_pSplashTexture = (RwTexture*)LoadTextureFromDB(OBFUSCATE("txd"), OBFUSCATE("splash_icon"));
	
	//p_logoTexture = (RwTexture*)LoadTextureFromDB("samp","UserLogo");

	CRadarRect::LoadTextures();

	m_bKeysStatus = false;
	m_fFuel = 100.0f;       // Bak to'liq
    m_fMileage = 0.0f;      // Kilometraj 0
    m_bSeatBelt = false;    // Kamar ochiq
}

CGUI::~CGUI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
}

bool g_IsVoiceServer()
{
	return true;
}

extern float g_fMicrophoneButtonPosX;
extern float g_fMicrophoneButtonPosY;

extern uint32_t g_uiLastTickVoice;

void CGUI::PreProcessInput()
{
	ImGuiIO& io = ImGui::GetIO();

	io.MousePos = m_vTouchPos;
	io.MouseDown[0] = m_bMouseDown;

	if (!m_bNeedClearMousePos && m_bNextClear)
		m_bNextClear = false;

	if (m_bNeedClearMousePos && m_bNextClear)
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNextClear = true;
	}
}

void CGUI::PostProcessInput()
{
	ImGuiIO& io = ImGui::GetIO();

	if (m_bNeedClearMousePos && io.MouseDown[0])
		return;

	if (m_bNeedClearMousePos && !io.MouseDown[0])
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNextClear = true;
	}
}

/*#include "..//CDebugInfo.h"
extern CGame* pGame;

void CGUI::SetHealth(float fhpcar){
   bHealth = static_cast<int>(fhpcar);
}

int CGUI::GetHealth(){
	return 1;//static_cast<int>(pVehicle->GetHealth());
}

void CGUI::SetDoor(int door){
	bDoor = door;
}

void CGUI::SetEngine(int engine){
	bEngine = engine;
}

void CGUI::SetLights(int lights){
	bLights = lights;
}

void CGUI::SetMeliage(float meliage){
	bMeliage = static_cast<int>(meliage);
}

void CGUI::SetEat(float eate){
	eat = static_cast<int>(eate);
}

void CGUI::SetFuel(float fuel){
   m_fuel = static_cast<int>(fuel);
}*/

// gui.cpp faylida CGUI::ShowSpeed() metodini toping (taxminan 200-qator)
// O'RINGA ALMASHTIRING:

void CGUI::SetFuel(float fuelPercent)
{
    m_fFuel = fuelPercent;
    
    // Chegaralash
    if (m_fFuel < 0.0f) m_fFuel = 0.0f;
    if (m_fFuel > 100.0f) m_fFuel = 100.0f;
    
    LOGI("SetFuel: %.1f%%", m_fFuel);
}

void CGUI::ShowSpeed()
{
    static bool speedShown = false;
    static uint32_t lastUpdateTime = 0;
    static uint32_t lastFuelTick = 0;
    
    // ===== 1. FPS CHEKLASH (LAG QILMASLIK UCHUN) =====
    uint32_t currentUpdateTime = GetTickCount();
    if (currentUpdateTime - lastUpdateTime < 100) // 10 FPS (har 100ms)
        return;
    lastUpdateTime = currentUpdateTime;

    // ===== 2. GAME/NET TEKSHIRUV =====
    if (!pGame || !pNetGame)
    {
        if (speedShown)
        {
            g_pJavaWrapper->HideSpeed();
            speedShown = false;
        }
        return;
    }

    CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
    if (!pPlayerPed || !pPlayerPed->IsInVehicle() || pPlayerPed->IsAPassenger())
    {
        // ✅ BU YERDA MUHIM: Har doim tekshirish, faqat speedShown true bo'lsa emas
        g_pJavaWrapper->HideSpeed();
        speedShown = false;
        return;
    }

    if (pKeyBoard && pKeyBoard->IsOpen())
    {
        if (speedShown)
        {
            g_pJavaWrapper->HideSpeed();
            speedShown = false;
        }
        return;
    }

    CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
    if (!pVehiclePool)
    {
        if (speedShown)
        {
            g_pJavaWrapper->HideSpeed();
            speedShown = false;
        }
        return;
    }

    VEHICLEID vehId = pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle());
    CVehicle* pVehicle = pVehiclePool->GetAt(vehId);
    if (!pVehicle)
    {
        if (speedShown)
        {
            g_pJavaWrapper->HideSpeed();
            speedShown = false;
        }
        return;
    }

    // ===== 3. SPEED HISOBLASH =====
    VECTOR moveSpeed;
    pVehicle->GetMoveSpeedVector(&moveSpeed);

    int i_speed = (int)(
        sqrt(
            moveSpeed.X * moveSpeed.X +
            moveSpeed.Y * moveSpeed.Y +
            moveSpeed.Z * moveSpeed.Z
        ) * 180.0f
    );

    // ===== 4. CAR HP (0–100) =====
    float health = pVehicle->GetHealth();
    bHealth = (int)((health / 1000.0f) * 100.0f);
    if (bHealth > 100) bHealth = 100;
    if (bHealth < 0)   bHealth = 0;

    // ===== 5. ENGINE STATUS =====
    bEngine = pVehicle->GetEngineState() ? 1 : 0;
    
    // ===== 6. LIGHTS =====
    bLights = pVehicle->m_bServerLights ? 1 : 0;

    // ===== 7. DOOR STATUS =====
    bDoor = (pVehicle->GetDoorState() != 0) ? 1 : 0;

    // ===== 8. FUEL VA MILEAGE HISOBLASH (FAQAT HAR 1 SEKUNDDA) =====
    uint32_t currentFuelTime = GetTickCount();
    
    if (lastFuelTick == 0)
        lastFuelTick = currentFuelTime;
    
    uint32_t delta = currentFuelTime - lastFuelTick;
    
    if (delta >= 1000) // Har 1 soniyada
    {
        lastFuelTick = currentFuelTime;
        
        // Serverdan yoqilg'i kelmasa, clientda hisoblaymiz
        if (i_speed > 10 && m_fFuel > 0.0f)
        {
            m_fFuel -= 0.05f; // 0.05% / sekund (serverdan kelganda bu ishlamaydi)
            if (m_fFuel < 0.0f)
                m_fFuel = 0.0f;
        }
        
        // Masofa hisoblash
        m_fMileage += (i_speed / 3600.0f); // km/soat → km/sek
    }

    // ===== 9. SEATBELT =====
    if (i_speed > 60 && !m_bSeatBelt)
        m_bSeatBelt = true;
    else if (i_speed < 10 && m_bSeatBelt)
        m_bSeatBelt = false;

    // ===== 10. HUD KO'RSATISH (FAQAT 1 MARTA) =====
    if (!speedShown)
    {
        g_pJavaWrapper->ShowSpeed();
        speedShown = true;
    }

    // ===== 11. JAVA UCHUN QIYMATLAR =====
    int fuelHud = (int)(m_fFuel + 0.5f);      // Foizni butun songa
    if (fuelHud > 100) fuelHud = 100;
    if (fuelHud < 0) fuelHud = 0;
    
    int mileageHud = (int)(m_fMileage + 0.5f);

    // ===== 12. JAVA'GA YUBORISH (OPTIMIZE - LAG QILMASLIK) =====
    static int lastSpeed = -1;
    static int lastFuel = -1;
    static int lastHealth = -1;
    static int lastMileage = -1;
    static int lastEngine = -1;
    static int lastLights = -1;
    static int lastSeatBelt = -1;
    static int lastDoor = -1;
    
    bool needUpdate = false;
    
    if (lastSpeed != i_speed || lastFuel != fuelHud || lastHealth != bHealth ||
        lastMileage != mileageHud || lastEngine != bEngine || lastLights != bLights ||
        lastSeatBelt != (m_bSeatBelt ? 1 : 0) || lastDoor != bDoor)
    {
        needUpdate = true;
        
        lastSpeed = i_speed;
        lastFuel = fuelHud;
        lastHealth = bHealth;
        lastMileage = mileageHud;
        lastEngine = bEngine;
        lastLights = bLights;
        lastSeatBelt = m_bSeatBelt ? 1 : 0;
        lastDoor = bDoor;
    }
    
    if (needUpdate)
    {
        g_pJavaWrapper->UpdateSpeedInfo(
            i_speed,
            fuelHud,
            bHealth,
            mileageHud,
            bEngine,
            bLights,
            m_bSeatBelt ? 1 : 0,
            bDoor
        );
    }
}


extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendALT(JNIEnv* pEnv, jobject thiz)
	{
		                  CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if (pPlayerPool)
			{
				CLocalPlayer* pLocalPlayer;
				if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
					LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] = true;
				else
					LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;
			}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendCTRL(JNIEnv* pEnv, jobject thiz)
	{
		                  LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION] = true;
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendY(JNIEnv* pEnv, jobject thiz)
	{
		                  LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendN(JNIEnv* pEnv, jobject thiz)
	{
		                  LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendH(JNIEnv* pEnv, jobject thiz)
	{
	                                    LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendG(JNIEnv* pEnv, jobject thiz)
	{
	                                    CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	                                    CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
		                  if (pVehiclePool)
		                  {
		                  	VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
		 	                  if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
			                  {
			                  	CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
			                  	if (pVehicle)
			                  	{
						CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
						if (pPlayerPool)
						{
							CLocalPlayer* pLocalPlayer;
                                                                                                                              pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, true);
							pLocalPlayer->SendEnterVehicleNotification(ClosetVehicleID, true);
						}
				                   }
			                  }
	                  	}     
	}
}




void CGUI::SetupDefaultStyle() {
	ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 8.0f;
    style->FramePadding = ImVec2(6, 6);
    style->FrameRounding = 12.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = ScaleY(60.0f);
    style->ScrollbarRounding = 6.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 1.0f;

    style->WindowBorderSize = 0.0f;
    style->ChildBorderSize  = 0.1f;
    style->PopupBorderSize  = 0.1f;
    style->FrameBorderSize  = 0.1f;
  //  style->FrameSize  = 5.1f;

    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.70f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 0.80f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 0.90f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 0.75f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 0.00f, 0.60f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 0.70f, 0.00f, 0.60f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

    colors[ImGuiCol_Button] = ImVec4(1.00f, 0.40f, 0.00f, 0.60f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.00f, 0.70f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);

    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.70f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
 
}

void CGUI::Render()
{
	
	PreProcessInput();

	ProcessPushedTextdraws();
	if (pChatWindow)
		pChatWindow->ProcessPushedCommands();

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	RenderVersion();
	//RenderRakNetStatistics();

	if (pCrossHair)
		pCrossHair->Render();

	if (pKeyBoard)
		pKeyBoard->ProcessInputCommands();

	if (pPlayerTags) pPlayerTags->Render();

	if(pNetGame && pNetGame->GetLabelPool())
		pNetGame->GetLabelPool()->Draw();

	if(pGame) CGUI::ShowSpeed();

    if(pKillList)
        pKillList->Render();

    ImVec2 ppp(10, 600);

    if(g_pMasterCheat)
        g_pMasterCheat->Render();

	if (pChatWindow)
		pChatWindow->Render();
	else RenderText(ppp, IM_COL32_WHITE, true, "FUCK YOU");

	if (pScoreBoard)
		pScoreBoard->Draw();

	if (pKeyBoard)
		pKeyBoard->Render();

	if (pDialogWindow)
		pDialogWindow->Render();

	/*if (pNetGame && !pDialogWindow->m_bIsActive && !pDialogWindow->m_bActiveDialog && pGame->IsToggledHUDElement(HUD_ELEMENT_BUTTONS))
	{
		SetupDefaultStyle();
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * 3.5, ImGui::GetFontSize() * 2.5);
		ImGui::SetNextWindowPos(ImVec2(2.0f, io.DisplaySize.y / 2.8 - vecButSize.x / 2));
		ImGui::Begin(OBFUSCATE("###keys"), nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::Button(m_bKeysStatus ? OBFUSCATE("<<") : OBFUSCATE(">>"), ImVec2(89.5, 79.5)))
		{
			if (m_bKeysStatus)
				m_bKeysStatus = false;
			else m_bKeysStatus = true;
		}
		
		ImGui::SameLine();
		if (ImGui::Button(OBFUSCATE("Alt"), ImVec2(89.5, 79.5)))
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if (pPlayerPool)
			{
				CLocalPlayer* pLocalPlayer;
				if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
				{
					LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] = true;
				}
				else 
				{
					LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;
				}
						
				if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle())
				{
					g_pJavaWrapper->ShowG();
				}
				else 
				{
					g_pJavaWrapper->HideG();
				}
			}
		}
		else 
		{
			g_pJavaWrapper->HideG();
		}

		ImGui::SameLine();
		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
		if (pVehiclePool) 
		{
			if (pGButton->p_GButtonTexture) pGButton->RenderButton();
			else
			{
				CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
				if (pPlayerPool)
				{
					CLocalPlayer* pLocalPlayer;
					if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
					{
						VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
						if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
						{
							CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
							if (pVehicle)
							{
								if (pVehicle->GetDistanceFromLocalPlayerPed() < 5.0f)
								{
									if (ImGui::Button(OBFUSCATE("G"), ImVec2(89.5, 79.5)))
									{
										if (pNetGame)
										{
											pLocalPlayer = pPlayerPool->GetLocalPlayer();
											if (pLocalPlayer)
												pLocalPlayer->HandlePassengerEntryEx();
										}
									}
									ImGui::SameLine();
								}
							}
						}
					}
				}
			}
		}
		
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
		{
			if (ImGui::Button(OBFUSCATE("L. Ctrl"), ImVec2(89.5, 79.5)))
			LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION] = true;
		}

		ImGui::SameLine();
		
		if(!pScoreBoard->m_bToggle) {
			if (ImGui::Button("TAB", ImVec2(89.5, 79.5)))
			{
				pScoreBoard->Toggle();
			}
		}
		else {
			if (ImGui::Button("X", ImVec2(89.5, 79.5)))
			{
				pScoreBoard->Toggle();
			}
		}

		ImGui::SameLine();

		if (m_bKeysStatus)
		{
			ImGui::SameLine();
			if (ImGui::Button(OBFUSCATE("Y"), ImVec2(79.5, 79.5)))
			{
				LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;
			}

			ImGui::SameLine();
			if (ImGui::Button(OBFUSCATE("N"), ImVec2(79.5, 79.5)))
			{
				LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
			}

			ImGui::SameLine();
			if (ImGui::Button(OBFUSCATE("H"), ImVec2(79.5, 79.5)))
			{
				LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;
			}
		}

		ImGui::End();
	}*/

	if (pNetGame)
	{
		if (pVoice && g_IsVoiceServer())
		{
			if (pVoice->IsRecording() && GetTickCount() - g_uiLastTickVoice >= 20000)
			{
				char buf[64];
				sprintf(&buf[0], "%d", (int)((30000 - (GetTickCount() - g_uiLastTickVoice)) / 1000) + 1);
				ImVec2 test(ScaleX(pSettings->GetReadOnly().fButtonMicrophoneX + pSettings->GetReadOnly().fButtonMicrophoneSize / 2.0f) - ImGui::CalcTextSize(&buf[0]).x / 2.0f, ScaleY(g_fMicrophoneButtonPosY) - GetFontSize() * 2.6f);
				//RenderText(test, 0xFF0000FF, true, &buf[0]);
			}

			ImVec2 centre(ScaleX(1880.0f), ScaleY(35.0f));
			if (pVoice->IsDisconnected())
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(0.8f, 0.0f, 0.0f));

			if (pVoice->GetNetworkState() == VOICECHAT_CONNECTING || pVoice->GetNetworkState() == VOICECHAT_WAIT_CONNECT)
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(1.0f, 1.0f, 0.0f));

			if (pVoice->GetNetworkState() == VOICECHAT_CONNECTED)
				ImGui::GetBackgroundDrawList()->AddCircleFilled(centre, 18.0f, ImColor(0.0f, 0.8f, 0.0f));
		}
	}

	CDebugInfo::Draw();
	CBinder::Render();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

	PostProcessInput();
}

// gui.cpp faylida oxiriga (boshqa metodlardan keyin) qo'shing:

void CGUI::ToggleSeatBelt()
{
    m_bSeatBelt = !m_bSeatBelt;
    Log("[GUI] Seat belt %s", m_bSeatBelt ? "ON" : "OFF");
}

bool CGUI::OnTouchEvent(int type, bool multi, int x, int y)
{
	if(!pKeyBoard->OnTouchEvent(type, multi, x, y))
		return false;

	if (!pScoreBoard->OnTouchEvent(type, multi, x, y))
		return false;

	bool bFalse = true;
	if (pNetGame)
	{
		if (pNetGame->GetTextDrawPool()->OnTouchEvent(type, multi, x, y))
		{
			if (!pChatWindow->OnTouchEvent(type, multi, x, y))
				return false;
		}
		else bFalse = false;
	}

	switch(type)
	{
		case TOUCH_PUSH:
		{
			m_vTouchPos = ImVec2(x, y);
			m_bMouseDown = true;
			m_bNeedClearMousePos = false;
			break;
		}

		case TOUCH_POP:
		{
			m_bMouseDown = false;
			m_bNeedClearMousePos = true;
			break;
		}

		case TOUCH_MOVE:
		{
			m_bNeedClearMousePos = false;
			m_vTouchPos = ImVec2(x, y);
			break;
		}
	}

	if (!bFalse)
		return false;

	return true;
}

void CGUI::RenderVersion()
{
	//ImVec2 a = ImVec2(20, 893);
	//ImVec2 b = ImVec2(20 + 250, 893 + 160);
	//ImGui::GetOverlayDrawList()->AddImage((ImTextureID)p_logoTexture->raster, a, b);
	return;

	ImGui::GetOverlayDrawList()->AddText(
		ImVec2(ScaleX(10), ScaleY(10)), 
		ImColor(IM_COL32_BLACK), PORT_VERSION);
}

void CGUI::ProcessPushedTextdraws()
{
	BUFFERED_COMMAND_TEXTDRAW* pCmd = nullptr;
	while (pCmd = m_BufferedCommandTextdraws.ReadLock())
	{
		RakNet::BitStream bs;
		bs.Write(pCmd->textdrawId);
		pNetGame->GetRakClient()->RPC(&RPC_ClickTextDraw, &bs, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, 0);
		m_BufferedCommandTextdraws.ReadUnlock();
	}
}

void CGUI::RenderRakNetStatistics()
{
		//StatisticsToString(rss, message, 0);

		/*ImGui::GetOverlayDrawList()->AddText(
			ImVec2(ScaleX(10), ScaleY(400)),
			ImColor(IM_COL32_BLACK), message);*/
}

extern uint32_t g_uiBorderedText;
void CGUI::RenderTextForChatWindow(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = pSettings->GetReadOnly().iFontOutline;

	auto colOutline = ImColor(IM_COL32_BLACK);
	auto colDef = ImColor(col);

	colOutline.Value.w = colDef.Value.w;

	if (bOutline)
	{
		if (g_uiBorderedText)
		{
			posCur.x -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.x += iOffset;
			// right 
			posCur.x += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.x -= iOffset;
			// above
			posCur.y -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.y += iOffset;
			// below
			posCur.y += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, colOutline, text_begin, text_end);
			posCur.y -= iOffset;
		}
		else
		{
			ImColor co(0.0f, 0.0f, 0.0f, 0.4f);
			if (colOutline.Value.w <= 0.4)
				co.Value.w = colOutline.Value.w;

			ImVec2 b(posCur.x + ImGui::CalcTextSize(text_begin, text_end).x, posCur.y + GetFontSize());
			ImGui::GetBackgroundDrawList()->AddRectFilled(posCur, b, co);
		}
	}

	ImGui::GetBackgroundDrawList()->AddText(posCur, col, text_begin, text_end);
}

void CGUI::PushToBufferedQueueTextDrawPressed(uint16_t textdrawId)
{
	BUFFERED_COMMAND_TEXTDRAW* pCmd = m_BufferedCommandTextdraws.WriteLock();

	pCmd->textdrawId = textdrawId;

	m_BufferedCommandTextdraws.WriteUnlock();
}

void CGUI::RenderText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = pSettings->GetReadOnly().iFontOutline;

	if (bOutline)
	{
		if (g_uiBorderedText)
		{
			posCur.x -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x += iOffset;
			// right 
			posCur.x += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.x -= iOffset;
			// above
			posCur.y -= iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y += iOffset;
			// below
			posCur.y += iOffset;
			ImGui::GetBackgroundDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
			posCur.y -= iOffset;
		}
		else
		{
			ImVec2 b(posCur.x + ImGui::CalcTextSize(text_begin, text_end).x, posCur.y + GetFontSize());
			if (m_pSplashTexture)
			{
				ImColor co(1.0f, 1.0f, 1.0f, 0.4f);
				ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)m_pSplashTexture->raster, posCur, b, ImVec2(0, 0), ImVec2(1, 1), co);
			}
			else
			{
				ImColor co(0.0f, 0.0f, 0.0f, 0.4f);
				ImGui::GetBackgroundDrawList()->AddRectFilled(posCur, b, co);
			}
		}
	}

	ImGui::GetBackgroundDrawList()->AddText(posCur, col, text_begin, text_end);
}
void CGUI::RenderTextWithSize(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size)
{
    int iOffset = pSettings->GetReadOnly().iFontOutline;

    if (bOutline)
    {
        // left
        posCur.x -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetBackgroundDrawList()->AddText(m_pFont, font_size, posCur, col, text_begin, text_end);
}
ImFont* CGUI::LoadFont(char *font, float fontsize)
{
    ImGuiIO &io = ImGui::GetIO();

    // load fonts
    char path[0xFF];
    sprintf(path, OBFUSCATE("%sSAMP/fonts/%s"), g_pszStorage, font);

    // ranges
    static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x04FF, // Russia
		0x0E00, 0x0E7F, // Thai
		0x2DE0, 0x2DFF, // Cyrillic Extended-A
		0xA640, 0xA69F, // Cyrillic Extended-B
		0xF020, 0xF0FF, // Half-width characters
		0
	};

    ImFont* pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
    return pFont;
}

void CGUI::RenderTextDeathMessage(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size, ImFont *font, bool bOutlineUseTextColor)
{
    int iOffset = bOutlineUseTextColor ? 1 : pSettings->GetReadOnly().iFontOutline;
    if(bOutline)
    {
        // left
        posCur.x -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, bOutlineUseTextColor ? ImColor(col) : ImColor(IM_COL32_BLACK), text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetBackgroundDrawList()->AddText(font == nullptr ? GetFont() : font, font_size == 0.0f ? GetFontSize() : font_size, posCur, col, text_begin, text_end);
}
void CGUI::AddText(ImFont* font, ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size)
{
    int iOffset = pSettings->GetReadOnly().iFontOutline;

    if (bOutline)
    {
        // left
        posCur.x -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.x += iOffset;
        // right
        posCur.x += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.x -= iOffset;
        // above
        posCur.y -= iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.y += iOffset;
        // below
        posCur.y += iOffset;
        ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
        posCur.y -= iOffset;
    }

    ImGui::GetBackgroundDrawList()->AddText(font, font_size, posCur, col, text_begin, text_end);
}