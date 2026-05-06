#pragma once
#include "../vendor/imgui/imgui.h"
#include "..//vendor/raknet/SingleProducerConsumer.h"

enum eTouchType
{
	TOUCH_POP = 1,
	TOUCH_PUSH = 2,
	TOUCH_MOVE = 3
};

struct BUFFERED_COMMAND_TEXTDRAW
{
	uint16_t textdrawId;
};

class CGUI
{
public:
	CGUI();
	~CGUI();

	void Render();
	void SetFuel(float fuel);
	void ShowSpeed();
	void ToggleSeatBelt();
	//void UpdateVehicleStats(float fuel, float mileage, bool seatbelt);
	void SetHealth(float fHealth);
	int GetHealth();
	int bHealth;
	void SetEngine(int engine);
	int bEngine;
	void SetLights(int lights);
	int bLights;
	void SetDoor(int door);
	int bDoor;
	void SetMeliage(float meliage);
    int bMeliage = 0;
	int m_fuel;
	
	int GetEat();
    int eat;
    
	void SetEat(float eate);
	float ScaleX(float x) { return m_vecScale.x * x; }
	float ScaleY(float y) { return m_vecScale.y * y; }
	ImFont* GetFont() { return m_pFont; }
	ImFont* GetSampFont() { return m_pSampFont; }
	float GetFontSize() { return m_fFontSize; }
    ImFont* LoadFont(char *font, float fontsize);
	bool OnTouchEvent(int type, bool multi, int x, int y);
    void AddText(ImFont* font, ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size);
	void RenderText(ImVec2& pos, ImU32 col, bool bOutline, const char* text_begin, const char* text_end = nullptr);
	void RenderTextForChatWindow(ImVec2& pos, ImU32 col, bool bOutline, const char* text_begin, const char* text_end = nullptr);
    void  RenderTextWithSize(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end, float font_size);
	void PushToBufferedQueueTextDrawPressed(uint16_t textdrawId);
    void RenderTextDeathMessage(ImVec2& pos, ImU32 col, bool bOutline, const char* text_begin, const char* text_end = nullptr, float font_size = 0.0f, ImFont *font = NULL, bool bOutlineUseTextColor = false);
    ImFont* GetWeaponFont() { return m_pFontGTAWeap; }
	void SetupDefaultStyle();
private:
	void PreProcessInput();
	void PostProcessInput();
	void RenderRakNetStatistics();
	void RenderVersion();
	void ProcessPushedTextdraws();
	
	struct RwTexture* b_tab;
	struct RwTexture* b_l;
	struct RwTexture* b_h;
	struct RwTexture* b_r;
	struct RwTexture* b_alt;
	struct RwTexture* b_g;
	struct RwTexture* b_n;
	struct RwTexture* b_y;
	struct RwTexture* b_x;
	struct RwTexture* b_ctrl;
	struct RwTexture* b_f;
	struct RwTexture* b_menu;
	struct RwTexture* b_gps;
	struct RwTexture* b_cars;
	struct RwTexture* b_eng;
	struct RwTexture* b_ca;
	struct RwTexture* b_cb;
	


private:
	DataStructures::SingleProducerConsumer<BUFFERED_COMMAND_TEXTDRAW> m_BufferedCommandTextdraws;

	struct RwTexture* m_pSplashTexture;
	ImFont* 	m_pFont;
	ImFont*		m_pSampFont;
    ImFont*      m_pFontGTAWeap;
	ImVec2		m_vecScale;
	float 		m_fFontSize;
	float       m_fFuel;      // ◀◀◀ QO'SHILADI - yoqilg'i
    float       m_fMileage;   // ◀◀◀ QO'SHILADI - kilometraj
    bool        m_bSeatBelt;   // ◀◀◀ QO'SHILADI - kamar holati

	bool		m_bNeedClearMousePos;
	bool		m_bMouseDown;
	ImVec2		m_vTouchPos;
	bool		m_bNextClear;

	bool 		m_bKeysStatus;
};