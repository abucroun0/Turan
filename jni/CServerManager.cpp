
#include "CServerManager.h"
#include "dialog.h"

#include "main.h"
#include "util/CJavaWrapper.h"

extern CDialogWindow *pDialogWindow;

#ifdef FLIN
	const char* g_szServerNames[MAX_SERVERS] = {
		OBFUSCATE("{ff9729}[1]{ffffff} GOLDEN RP | Server: 01"),
		OBFUSCATE("{ff9729}[2]{ffffff} GOLDEN RP | Server: 02"),
		//OBFUSCATE("{ff9729}[3]{ffffff} IRAN  RP | Server: 03"),
		//OBFUSCATE("{ff9729}[4]{ffffff} AMAR  RP | Server: 04"),

		//OBFUSCATE("{ff0000}VIP Server:{FFCC00} DENIZ CIMP By USER"),
		//OBFUSCATE("")
	};

	const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
		CServerInstance::create(OBFUSCATE("188.127.241.74"), 1, 18, 2622, false), // 0
		CServerInstance::create(OBFUSCATE("188.127.241.74"), 1, 16, 2622, false) // 1
		//CServerInstance::create(OBFUSCATE("play.gta-sa.ir"), 1, 16, 7777, false), // 2
		//CServerInstance::create(OBFUSCATE("85.133.205.94"), 1, 16, 7777, false), // 3

		//CServerInstance::create(OBFUSCATE("5.57.39.53"), 1, 20, 6565, false), // 4
		//CServerInstance::create(OBFUSCATE("moz"), 1, 20, 7777, false) // 5
	};
#elif JUST
	const char* g_szServerNames[MAX_SERVERS] = {
		OBFUSCATE("")
	};

	const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
			CServerInstance::create(NUM_TO_STR_IP(213, 159, 212, 12), 16, 7777, false)
	};
#else
	const char* g_szServerNames[MAX_SERVERS] = {
		OBFUSCATE("")
	};

	const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
			CServerInstance::create(NUM_TO_STR_IP(51, 75, 34, 217), 16, 7777, false)
	};
#endif

