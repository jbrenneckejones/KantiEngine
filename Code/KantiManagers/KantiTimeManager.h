#ifndef KANTI_TIME_MANAGER

class KantiTimeManager
{
	private:

	k_internal uint64 ElapsedTime;

	k_internal int64 PerfomanceCountFrequency;
	k_internal int64 LastPerformanceCount;
	k_internal int64 EndPerformanceCount;

	k_internal uint64 LastCycleCount;
	k_internal uint64 EndCycleCount;

	k_internal uint64 CyclesElapsed;
	k_internal int64 CounterElapsed;
	k_internal real32 MSPerFrame;
	k_internal real32 FPS;
	k_internal real32 MCPF;

	public:

	k_internal platform_get_performance_frequency* PlatformPerformanceFrequency;

	k_internal platform_get_performance_counter* PlatformPerformanceCounter;

	k_internal platform_get_cycle_count* PlatformCycleCount;

	k_internal void OnInitialize()
	{
		PerfomanceCountFrequency = PlatformPerformanceFrequency();
	}

	k_internal void StartFrame()
	{
		LastPerformanceCount = PerformanceCounter();
		LastCycleCount = CycleCounter();
	}

	k_internal void EndFrame()
	{
		ElapsedTime++;

		EndCycleCount = CycleCounter();
		EndPerformanceCount = PerformanceCounter();

		CyclesElapsed = EndCycleCount - LastCycleCount;
		CounterElapsed = EndPerformanceCount - LastPerformanceCount;
		MSPerFrame = (real32)(1000.0f * (real32)GetCounterElapsed() / (real32)PerfomanceCountFrequency );
		FPS = (real32)(PerfomanceCountFrequency / (real32)GetCounterElapsed());
		MCPF = ((real32)GetCyclesElapsed() / (1000.0f * 1000.0f));

		LastPerformanceCount = EndPerformanceCount;
		LastCycleCount = EndCycleCount;
	}

	k_internal int64 PerformanceFrequency()
	{
		return KantiTimeManager::PlatformPerformanceFrequency();
	}

	k_internal int64 PerformanceCounter()
	{
		return KantiTimeManager::PlatformPerformanceCounter();
	}

	k_internal int64 CycleCounter()
	{
		return KantiTimeManager::PlatformCycleCount();
	}

	k_internal uint64 GetCyclesElapsed()
	{
		return CyclesElapsed;
	}

	k_internal int64 GetCounterElapsed()
	{
		return CounterElapsed;
	}

	k_internal real32 GetMSPerFrame()
	{
		return MSPerFrame;
	}

	k_internal real32 GetFPS()
	{
		return FPS;
	}

	k_internal real32 GetMCPF()
	{
		return MCPF;
	}
};

platform_get_performance_frequency* KantiTimeManager::PlatformPerformanceFrequency = nullptr;

platform_get_performance_counter* KantiTimeManager::PlatformPerformanceCounter = nullptr;

platform_get_cycle_count* KantiTimeManager::PlatformCycleCount = nullptr;

uint64 KantiTimeManager::ElapsedTime = 0;

int64 KantiTimeManager::PerfomanceCountFrequency = 0;
int64 KantiTimeManager::LastPerformanceCount = 0;
int64 KantiTimeManager::EndPerformanceCount = 0;

uint64 KantiTimeManager::LastCycleCount = 0;
uint64 KantiTimeManager::EndCycleCount = 0;

uint64 KantiTimeManager::CyclesElapsed = 0;
int64 KantiTimeManager::CounterElapsed = 0;
real32 KantiTimeManager::MSPerFrame = 0.0f;
real32 KantiTimeManager::FPS = 0.0f;
real32 KantiTimeManager::MCPF = 0.0f;

#define KANTI_TIME_MANAGER
#endif