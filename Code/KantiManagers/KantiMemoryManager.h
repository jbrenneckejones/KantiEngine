#ifndef KANTI_ALLOCATOR

namespace AllocatorMath
{
	template <class T> T* AllocateNew(class KantiMemoryManager& Allocator)
	{
		return new (KantiMemoryManager.Allocate(sizeof(T), alignof(T))) T;
	}

	template <class T> T* AllocateNew(class KantiAllocator& Allocator, const T& Type)
	{
		return new (KantiMemoryManager.Allocate(sizeof(T), alignof(T))) T(Type);
	}

	template<class T> void DeallocateDelete(class KantiMemoryManager& Allocator, T& Object)
	{
		Object.~T();
		KantiMemoryManager.Deallocate(&Object);
	}

	template<class T> T* AllocateArray(class KantiMemoryManager& Allocator, memory_index Length)
	{
		Assert(Length != 0);

		uint8 HeaderSize = sizeof(memory_index) / sizeof(T);

		if(sizeof(memory_index) % sizeof(T) > 0)
			HeaderSize += 1;

		//Allocate extra space to store array length in the bytes before the array
		T* Pointer = ((T*)KantiAllocator.Allocate(sizeof(T)*(Length + HeaderSize), alignof(T))) + HeaderSize;

		*(((memory_index*)Pointer) - 1) = Length;

		for(memory_index Index = 0; Index < Length; Index++)
			new (&Pointer[Index]) T;

		return Pointer;
	}

	template<class T> void DeallocateArray(class KantiMemoryManager& Allocator, T* Array)
	{
		Assert(Array != nullptr);

		memory_index Length = *(((memory_index*)Array) - 1);

		for(memory_index Index = 0; Index < Length; Index++)
			Array[Index].~T();

		//Calculate how much extra memory was allocated to store the length before the array
		uint8 HeaderSize = sizeof(memory_index) / sizeof(T);

		if(sizeof(memory_index) % sizeof(T) > 0)
			HeaderSize += 1;

		KantiMemoryManager.Deallocate(Array - HeaderSize);
	}

	inline void* AlignForward(void* Address, uint8 Alignment)
	{
		return (void*)((reinterpret_cast<uintptr>(Address) + static_cast<uintptr>(Alignment - 1)) & static_cast<uintptr>(~(Alignment - 1)));
	}

	inline const void* AlignForward(const void* Address, uint8 Alignment)
	{
		return (void*)((reinterpret_cast<uintptr>(Address) + static_cast<uintptr>(Alignment - 1)) & static_cast<uintptr>(~(Alignment - 1)));
	}

	inline void* AlignBackward(void* Address, uint8 Alignment)
	{
		return (void*)(reinterpret_cast<uintptr>(Address) & static_cast<uintptr>(~(Alignment - 1)));
	}

	inline const void* AlignBackward(const void* Address, uint8 Alignment)
	{
		return (void*)(reinterpret_cast<uintptr>(Address) & static_cast<uintptr>(~(Alignment - 1)));
	}

	inline uint8 AlignForwardAdjustment(const void* Address, uint8 Alignment)
	{
		uint8 MemoryAdjustment = Alignment - (reinterpret_cast<uintptr>(Address) & static_cast<uintptr>(Alignment - 1));

		if(MemoryAdjustment == Alignment)
			return 0; //already aligned

		return MemoryAdjustment;
	}

	inline uint8 AlignForwardAdjustmentWithHeader(const void* Address, uint8 Alignment, uint8 HeaderSize)
	{
		uint8 MemoryAdjustment = AlignForwardAdjustment(Address, Alignment);

		uint8 NeededSpace = HeaderSize;

		if(MemoryAdjustment < NeededSpace)
		{
			NeededSpace -= MemoryAdjustment;

			//Increase adjustment to fit header
			MemoryAdjustment += Alignment * (NeededSpace / Alignment);

			if(NeededSpace % Alignment > 0)
				MemoryAdjustment += Alignment;
		}

		return MemoryAdjustment;
	}

	inline uint8 AlignBackwardAdjustment(const void* Address, uint8 Alignment)
	{
		uint8 MemoryAdjustment = reinterpret_cast<uintptr>(Address) & static_cast<uintptr>(Alignment - 1);

		if(MemoryAdjustment == Alignment)
			return 0; //already aligned

		return MemoryAdjustment;
	}

	inline void* Add(void* Pointer, memory_index Memory)
	{
		return (void*)(reinterpret_cast<uintptr>(Pointer) + Memory);
	}

	inline const void* Add(const void* Pointer, memory_index Memory)
	{
		return (const void*)(reinterpret_cast<uintptr>(Pointer) + Memory);
	}

	inline void* Subtract(void* Pointer, memory_index Memory)
	{
		return (void*)(reinterpret_cast<uintptr>(Pointer) - Memory);
	}

	inline const void* Subtract(const void* Pointer, memory_index Memory)
	{
		return (const void*)(reinterpret_cast<uintptr>(Pointer) - Memory);
	}
};

class KantiMemoryManager
{
	public:
	KantiMemoryManager(memory_index AloccateMemorySize, void* StartAddress)
	{
		Initialize(AloccateMemorySize, StartAddress);
	}

	~KantiMemoryManager()
	{
		Assert(Members.NumberOfAllocations == 0 && Members.UsedMemory == 0);

		Members.StartAddress = nullptr;
		Members.AllocatedMemorySize = 0;

		MemoryBlocks = nullptr;
	}

	inline void Initialize(memory_index AloccateMemorySize, void* StartAddress)
	{
		MemoryBlocks = (MemoryBlock*)StartAddress;
		Members.StartAddress = StartAddress;
		Members.AllocatedMemorySize = AloccateMemorySize;

		Members.UsedMemory = 0;
		Members.NumberOfAllocations = 0;

		Assert(Members.AllocatedMemorySize > sizeof(MemoryBlock));

		MemoryBlocks->MemorySize = AloccateMemorySize;
		MemoryBlocks->NextMemoryBlock = nullptr;
	}

	// Platform

	k_internal platform_allocate_memory* PlatformMemoryAllocate;
	k_internal platform_deallocate_memory* PlatformMemoryDeallocate;

	// End platform

	void* Allocate(memory_index MemorySize, uint8 Alignment = 4)
	{
		Assert(MemorySize != 0 && Alignment != 0);

		MemoryBlock* PreviousMemoryBlock = nullptr;
		MemoryBlock* FreeMemoryBlock = MemoryBlocks;

		while(FreeMemoryBlock != nullptr)
		{
			//Calculate adjustment needed to keep object correctly aligned
			uint8 MemoryAdjustment = AllocatorMath::AlignForwardAdjustmentWithHeader(FreeMemoryBlock, Alignment, sizeof(AllocationHeader));

			memory_index TotalMemorySize = MemorySize + MemoryAdjustment;

			//If allocation doesn't fit in this FreeBlock, try the next
			if(FreeMemoryBlock->MemorySize < TotalMemorySize)
			{
				PreviousMemoryBlock = FreeMemoryBlock;
				FreeMemoryBlock = FreeMemoryBlock->NextMemoryBlock;
				continue;
			}

			static_assert(sizeof(AllocationHeader) >= sizeof(MemoryBlock), "sizeof(AllocationHeader) < sizeof(FreeBlock)");

			//If allocations in the remaining memory will be impossible
			if(FreeMemoryBlock->MemorySize - TotalMemorySize <= sizeof(AllocationHeader))
			{
				//Increase allocation size instead of creating a new FreeBlock
				TotalMemorySize = FreeMemoryBlock->MemorySize;

				if(PreviousMemoryBlock != nullptr)
				{
					PreviousMemoryBlock->NextMemoryBlock = FreeMemoryBlock->NextMemoryBlock;
				}
				else
				{
					MemoryBlocks = FreeMemoryBlock->NextMemoryBlock;
				}
			}
			else
			{
				//Else create a new FreeBlock containing remaining memory
				MemoryBlock* NextMemoryBlock = (MemoryBlock*)(AllocatorMath::Add(FreeMemoryBlock, TotalMemorySize));
				NextMemoryBlock->MemorySize = FreeMemoryBlock->MemorySize - TotalMemorySize;
				NextMemoryBlock->NextMemoryBlock = FreeMemoryBlock->NextMemoryBlock;

				if(PreviousMemoryBlock != nullptr)
				{
					PreviousMemoryBlock->NextMemoryBlock = NextMemoryBlock;
				}
				else
				{
					MemoryBlocks = NextMemoryBlock;
				}
			}

			uintptr AlignedAddress = (uintptr)FreeMemoryBlock + MemoryAdjustment;

			AllocationHeader* AlocHeader = (AllocationHeader*)(AlignedAddress - sizeof(AllocationHeader));
			AlocHeader->MemorySize = TotalMemorySize;
			AlocHeader->MemoryAdjustment = MemoryAdjustment;

			Members.UsedMemory += TotalMemorySize;
			Members.NumberOfAllocations++;

			Assert(AllocatorMath::AlignForwardAdjustment((void*)AlignedAddress, Alignment) == 0);

			return (void*)AlignedAddress;
		}

		Assert("Couldn't find free block large enough!");

		return nullptr;
	}

	void Deallocate(void* Pointer)
	{
		Assert(Pointer != nullptr);

		AllocationHeader* Header = (AllocationHeader*)AllocatorMath::Subtract(Pointer, sizeof(AllocationHeader));

		uintptr   BlockStart = reinterpret_cast<uintptr>(Pointer) - Header->MemoryAdjustment;
		memory_index BlockSize = Header->MemorySize;
		uintptr   BlockEnd = BlockStart + BlockSize;

		MemoryBlock* PreviousFreeBlock = nullptr;
		MemoryBlock* CurrentFreeBlock = MemoryBlocks;

		while(CurrentFreeBlock != nullptr)
		{
			if((uintptr)CurrentFreeBlock >= BlockEnd)
			{
				break;
			}

			PreviousFreeBlock = CurrentFreeBlock;
			CurrentFreeBlock = CurrentFreeBlock->NextMemoryBlock;
		}

		if(PreviousFreeBlock == nullptr)
		{
			PreviousFreeBlock = (MemoryBlock*)BlockStart;
			PreviousFreeBlock->MemorySize = BlockSize;
			PreviousFreeBlock->NextMemoryBlock = MemoryBlocks;

			MemoryBlocks = PreviousFreeBlock;
		}
		else if((uintptr)PreviousFreeBlock + PreviousFreeBlock->MemorySize == BlockStart)
		{
			PreviousFreeBlock->MemorySize += BlockSize;
		}
		else
		{
			MemoryBlock* Temp = (MemoryBlock*)BlockStart;
			Temp->MemorySize = BlockSize;
			Temp->NextMemoryBlock = PreviousFreeBlock->NextMemoryBlock;
			PreviousFreeBlock->NextMemoryBlock = Temp;

			PreviousFreeBlock = Temp;
		}

		if(CurrentFreeBlock != nullptr && (uintptr)CurrentFreeBlock == BlockEnd)
		{
			PreviousFreeBlock->MemorySize += CurrentFreeBlock->MemorySize;
			PreviousFreeBlock->NextMemoryBlock = CurrentFreeBlock->NextMemoryBlock;
		}

		Members.NumberOfAllocations--;
		Members.UsedMemory -= BlockSize;
	}

	void* ReAllocate(void* Pointer, memory_index SourceSize, memory_index NewSize, uint8 Alignment = 4)
	{
		void* NewMemory = Allocate(NewSize, Alignment);

		MemoryCopy(NewMemory, Pointer, SourceSize);

		Deallocate(Pointer);

		return NewMemory;
	}

	void MemoryCopy(void* Destination, void* Source, memory_index SourceSize)
	{
		// Typecast src and dest addresses to (char *)
		char* CharSource = (char *)Source;
		char* CharDestination = (char *)Destination;

		// Copy contents of src[] to dest[]
		for(uint32 Index = 0; Index < SourceSize; Index++)
		{
			CharDestination[Index] = CharSource[Index];
		}
	}

	void MemoryMove(void** Destination, void** Source, memory_index SourceSize, uint8 SourceAlignment = 4)
	{
		*Destination = Allocate(SourceSize, SourceAlignment);

		// Typecast src and dest addresses to (char *)
		char *CharSource = (char *)Source;
		char *CharDestination = (char *)Destination;

		// Copy contents of src[] to dest[]
		for(uint32 Index = 0; Index < SourceSize; Index++)
		{
			CharDestination[Index] = CharSource[Index];
		}
	}

	struct AllocatorMembers
	{
		void*               StartAddress;
		memory_index        AllocatedMemorySize;

		memory_index        UsedMemory;
		memory_index        NumberOfAllocations;
	};

	private:

	struct AllocationHeader
	{
		memory_index MemorySize;
		uint8     MemoryAdjustment;
	};

	struct MemoryBlock
	{
		memory_index     MemorySize;
		MemoryBlock*     NextMemoryBlock;
	};

	KantiMemoryManager(const KantiMemoryManager&); //Prevent copies because it might cause errors
	KantiMemoryManager& operator=(const KantiMemoryManager&);

	MemoryBlock* MemoryBlocks;

	void* GetStart() const
	{
		return Members.StartAddress;
	}

	memory_index GetSize() const
	{
		return Members.AllocatedMemorySize;
	}

	memory_index GetUsedMemory() const
	{
		return Members.UsedMemory;
	}

	memory_index GetNumAllocations() const
	{
		return Members.NumberOfAllocations;
	}

	protected:

	AllocatorMembers Members;
};

platform_allocate_memory* KantiMemoryManager::PlatformMemoryAllocate = nullptr;
platform_deallocate_memory* KantiMemoryManager::PlatformMemoryDeallocate = nullptr;

#define KANTI_ALLOCATOR
#endif
