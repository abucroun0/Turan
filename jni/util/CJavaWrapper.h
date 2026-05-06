#pragma once

#include <jni.h>

#include <string>

#define EXCEPTION_CHECK(env) \
	if ((env)->ExceptionCheck()) \ 
	{ \
		(env)->ExceptionDescribe(); \
		(env)->ExceptionClear(); \
		return; \
	}

struct INFO_DIALOG{
    int dialogid;
    int response;
    int listitem;
    char inputtext[256 + 1];
};

class CJavaWrapper
{
	jobject activity;
	
	jmethodID s_showHud;
    jmethodID s_hideHud;
	jmethodID s_updateHudInfo;
	
	jmethodID s_updateLoading;
	
	jmethodID s_showTabWindow;
	jmethodID s_setTabStat;

	jmethodID s_showSpeed;
    jmethodID s_hideSpeed;
    jmethodID s_updateSpeedInfo;

	jmethodID s_showHudButtonG;
	jmethodID s_hideHudButtonG;

	jmethodID s_setPauseState;

	jmethodID s_GetClipboardText;
	jmethodID s_CallLauncherActivity;
	
	
	jmethodID s_ShowInputLayout;
	jmethodID s_HideInputLayout;

	jmethodID s_ShowClientSettings;
	jmethodID s_SetUseFullScreen;
	jmethodID s_MakeDialog;

	jmethodID s_ToastMakeText;


	jmethodID s_BuildDialog;

    jmethodID s_ToggleRender;
    jmethodID s_SetVisibleDialog;

    jmethodID s_SetVisibleKeyboardStandard;
public:
	JNIEnv* GetEnv();

	std::string GetClipboardString();
	void CallLauncherActivity(int type);
	void ShowInputLayout();
	void HideInputLayout();
	void ShowToastText(char* text);

	void ShowClientSettings();

	void SetUseFullScreen(int b);
	
	void UpdateHudInfo(
    int health,
    int armour,
    int hunger,
    int weaponid,
    int ammo,
    int ammoinclip,
    int money,
    int wanted);

	void ShowHud();
    void HideHud();
	void UpdateLoading(int status);
	
	void ShowSpeed();
    void HideSpeed();
    void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock);

	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText);

	void ShowTabWindow();
	void SetTabStat(int id, char* name, int score, int ping);

                  void ShowG();
                  void HideG();

	void SetPauseState(bool a1);

	void BuildDialog(uint16_t wDialogID, char* title, char* content, char* button1, char* button2, uint8_t typeDialog);

	void SetVisibleKeyboard(bool active, int type);

    void ToggleRender(bool active);
    void SetVisibleDialog(bool active);

    void Process();
    bool m_bLastSentActiveDialog = false;
    bool m_bWaitResponseDialog;
    INFO_DIALOG m_infoDialogWaitReponse;
    bool m_bWaitActiveNewDialog;

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();
};

extern CJavaWrapper* g_pJavaWrapper;