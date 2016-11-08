#ifndef KANTI_DEBUG_MANAGER

#define K_ERROR(MESSAGE) ErrorHandler(MESSAGE, __FILE__, __LINE__)

class KantiDebugManager
{
	public:

	k_internal platform_game_error* PlatformError;
	k_internal platform_debug_console* PlatformConsole;

	k_internal inline void ErrorOutput(KString ErrorMessage, KString File, uint32 LineNumber)
	{
		PlatformError(ErrorMessage, File, LineNumber);
	}

	k_internal inline void DebugConsole(KString Message)
	{
		PlatformConsole(Message);
	}
};

platform_game_error* KantiDebugManager::PlatformError = nullptr;
platform_debug_console* KantiDebugManager::PlatformConsole = nullptr;


k_internal inline void
ErrorHandler(KString ErrorMessage, KString File, uint32 LineNumber)
{
	KantiDebugManager::PlatformError(ErrorMessage, File, LineNumber);
}

k_internal inline void
DebugMessage(KString Message)
{
	KantiDebugManager::PlatformConsole(Message);
}

#define KANTI_DEBUG_MANAGER
#endif