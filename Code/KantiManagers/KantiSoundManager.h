#ifndef KANTI_SOUND_MANAGER

#include "../KantiPlatform.h"
#include "KantiFileManager.h"
#include <initguid.h>
#include <dsound.h>
#include <stdio.h>

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

class KantiSoundManager
{
public:

	struct sound_id
	{
		uint32 Value;
	};

	struct playing_sound
	{
		Vector2 CurrentVolume;
		Vector2 DecibelCurrentVolume;
		Vector2 TargetVolume;

		real32 DecibelSample;

		sound_id ID;
		real32 SamplesPlayed;
		playing_sound *Next;
	};

	struct audio_state
	{
		memory_index* PermArena;
		playing_sound* FirstPlayingSound;
		playing_sound* FirstFreePlayingSound;

		Vector2 MasterVolume;
	};

	struct wave_data
	{
		char ChunkID[4];
		uint64 ChunkSize;
		char Format[4];
		char SubChunkID[4];
		uint64 SubChunkSize;
		uint32 AudioFormat;
		uint32 NumChannels;
		uint64 SampleRate;
		uint64 BytesPerSecond;
		uint32 BlockAlign;
		uint32 BitsPerSample;
		char DataChunkID[4];
		uint64 DataSize;
	};

	inline KantiSoundManager()
	{
		m_DirectSound = 0;
		m_primaryBuffer = 0;
		m_secondaryBuffer1 = 0;
	}

	inline KantiSoundManager(const KantiSoundManager&)
	{

	}
	inline ~KantiSoundManager()
	{

	}

	inline bool Initialize(HWND Window)
	{
		bool result;


		// Initialize direct sound and the primary sound buffer.
		result = InitializeDirectSound(Window);
		if (!result)
		{
			return false;
		}

		// Load a wave audio file onto a secondary buffer.
		result = LoadWaveFile("sound01.wav", &m_secondaryBuffer1);
		if (!result)
		{
			return false;
		}

		// Play the wave file now that it has been loaded.
		result = PlayWaveFile();
		if (!result)
		{
			return false;
		}

		return true;
	}
	inline void Shutdown()
	{
		// Release the secondary buffer.
		ShutdownWaveFile(&m_secondaryBuffer1);

		// Shutdown the Direct Sound API.
		ShutdownDirectSound();

		return;
	}

	inline bool InitializeDirectSound(HWND Window)
	{
		HRESULT result;
		DSBUFFERDESC bufferDesc;
		WAVEFORMATEX waveFormat;

		// NOTE(casey): Load the library
		HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
		if (!DSoundLibrary)
		{
			return false;
		}

		// NOTE(casey): Get a DirectSound object! - cooperative
		direct_sound_create* DirectSoundCreate = (direct_sound_create *)
			GetProcAddress(DSoundLibrary, "DirectSoundCreate");

		// Initialize the direct sound interface pointer for the default sound device.
		result = DirectSoundCreate(NULL, (LPDIRECTSOUND *)&m_DirectSound, NULL);
		if (FAILED(result))
		{
			return false;
		}

		// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
		result = m_DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY);
		if (FAILED(result))
		{
			return false;
		}

		// Setup the primary buffer description.
		bufferDesc.dwSize = sizeof(DSBUFFERDESC);
		bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
		bufferDesc.dwBufferBytes = 0;
		bufferDesc.dwReserved = 0;
		bufferDesc.lpwfxFormat = NULL;
		bufferDesc.guid3DAlgorithm = GUID_NULL;

		// Get control of the primary sound buffer on the default sound device.
		result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
		if (FAILED(result))
		{
			return false;
		}

		// Setup the format of the primary sound bufffer.
		// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nChannels = 2;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		// Set the primary buffer to be the wave format specified.
		result = m_primaryBuffer->SetFormat(&waveFormat);
		if (FAILED(result))
		{
			return false;
		}

		return true;
	}

	inline void ShutdownDirectSound()
	{
		// Release the primary sound buffer pointer.
		if (m_primaryBuffer)
		{
			m_primaryBuffer->Release();
			m_primaryBuffer = 0;
		}

		// Release the direct sound interface pointer.
		if (m_DirectSound)
		{
			m_DirectSound->Release();
			m_DirectSound = 0;
		}

		return;
	}

	inline bool LoadWaveFile(char* FileName, IDirectSoundBuffer8** secondaryBuffer)
	{
		int error;
		FILE* filePtr;
		unsigned int count;
		wave_data waveFileHeader;
		WAVEFORMATEX waveFormat;
		DSBUFFERDESC bufferDesc;
		HRESULT result;
		IDirectSoundBuffer* tempBuffer;
		unsigned char* waveData;
		unsigned char* bufferPtr;
		unsigned long bufferSize;


		// Open the wave file in binary.
		error = fopen_s(&filePtr, FileName, "rb");
		if (error != 0)
		{
			return false;
		}

		// Read in the wave file header.
		uint32 Length = 0;
		for (char* Character = FileName; *Character; ++Character)
		{
			Length++;
		}
		// k_string StringFile;
		// StringFile.PushBack(FileName, Length);
		// k_string Contents = GetFileContents(StringFile);
		count = (unsigned int)fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);

		char Buffer[1000];
		fread(&Buffer, sizeof(Buffer), 1, filePtr);
		// count = Contents.Count();
		if (count != 1)
		{
			return false;
		}

		// Check that the chunk ID is the RIFF format.
		if ((waveFileHeader.ChunkID[0] != 'R') || (waveFileHeader.ChunkID[1] != 'I') ||
			(waveFileHeader.ChunkID[2] != 'F') || (waveFileHeader.ChunkID[3] != 'F'))
		{
			return false;
		}

		// Check that the file format is the WAVE format.
		if ((waveFileHeader.Format[0] != 'W') || (waveFileHeader.Format[1] != 'A') ||
			(waveFileHeader.Format[2] != 'V') || (waveFileHeader.Format[3] != 'E'))
		{
			return false;
		}

		// Check that the sub chunk ID is the fmt format.
		if ((waveFileHeader.SubChunkID[0] != 'f') || (waveFileHeader.SubChunkID[1] != 'm') ||
			(waveFileHeader.SubChunkID[2] != 't') || (waveFileHeader.SubChunkID[3] != ' '))
		{
			return false;
		}

		// Check that the audio format is WAVE_FORMAT_PCM.
		if (waveFileHeader.AudioFormat != WAVE_FORMAT_PCM)
		{
			return false;
		}

		// Check that the wave file was recorded in stereo format.
		if (waveFileHeader.NumChannels != 2)
		{
			return false;
		}

		// Check that the wave file was recorded at a sample rate of 44.1 KHz.
		if (waveFileHeader.SampleRate != 44100)
		{
			return false;
		}

		// Ensure that the wave file was recorded in 16 bit format.
		if (waveFileHeader.BitsPerSample != 16)
		{
			return false;
		}

		// Check for the data chunk header.
		if ((waveFileHeader.DataChunkID[0] != 'd') || (waveFileHeader.DataChunkID[1] != 'a') ||
			(waveFileHeader.DataChunkID[2] != 't') || (waveFileHeader.DataChunkID[3] != 'a'))
		{
			return false;
		}

		// Set the wave format of secondary buffer that this wave file will be loaded onto.
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nChannels = 2;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
		bufferDesc.dwSize = sizeof(DSBUFFERDESC);
		bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
		bufferDesc.dwBufferBytes = (DWORD)waveFileHeader.DataSize;
		bufferDesc.dwReserved = 0;
		bufferDesc.lpwfxFormat = &waveFormat;
		bufferDesc.guid3DAlgorithm = GUID_NULL;

		// Create a temporary sound buffer with the specific buffer settings.
		result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
		if (FAILED(result))
		{
			return false;
		}

		// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
		result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
		if (FAILED(result))
		{
			return false;
		}

		// Release the temporary buffer.
		tempBuffer->Release();
		tempBuffer = 0;

		// Move to the beginning of the wave data which starts at the end of the data chunk header.
		fseek(filePtr, sizeof(wave_data), SEEK_SET);

		// Create a temporary buffer to hold the wave file data.
		waveData = new unsigned char[waveFileHeader.DataSize];
		if (!waveData)
		{
			return false;
		}

		// Read in the wave file data into the newly created buffer.
		count = (unsigned int)fread(waveData, 1, waveFileHeader.DataSize, filePtr);
		if (count != waveFileHeader.DataSize)
		{
			return false;
		}

		// Close the file once done reading.
		error = fclose(filePtr);
		if (error != 0)
		{
			return false;
		}

		// Lock the secondary buffer to write wave data into it.
		result = (*secondaryBuffer)->Lock(0, (DWORD)waveFileHeader.DataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
		if (FAILED(result))
		{
			return false;
		}

		// Copy the wave data into the buffer.
		memcpy(bufferPtr, waveData, waveFileHeader.DataSize);

		// Unlock the secondary buffer after the data has been written to it.
		result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
		if (FAILED(result))
		{
			return false;
		}

		// Release the wave data since it was copied into the secondary buffer.
		delete[] waveData;
		waveData = 0;

		return true;
	}

	inline void ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
	{
		// Release the secondary sound buffer.
		if (*secondaryBuffer)
		{
			(*secondaryBuffer)->Release();
			*secondaryBuffer = 0;
		}

		return;
	}

	inline bool PlayWaveFile()
	{
		HRESULT result;


		// Set position at the beginning of the sound buffer.
		result = m_secondaryBuffer1->SetCurrentPosition(0);
		if (FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
		if (FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = m_secondaryBuffer1->Play(0, 0, 0);
		if (FAILED(result))
		{
			return false;
		}

		return true;
	}

	k_internal playing_sound*
	PlaySound(audio_state *AudioState, sound_id SoundID)
	{
		// TIMED_FUNCTION();

		if (!AudioState->FirstFreePlayingSound)
		{
			// AudioState->FirstFreePlayingSound = PushStruct(AudioState->PermArena, playing_sound);
			AudioState->FirstFreePlayingSound->Next = 0;
		}

		playing_sound *PlayingSound = AudioState->FirstFreePlayingSound;
		AudioState->FirstFreePlayingSound = PlayingSound->Next;

		PlayingSound->SamplesPlayed = 0;
		// TODO(casey): Should these default to 0.5f/0.5f for centerred?
		PlayingSound->CurrentVolume = PlayingSound->TargetVolume = Vector2(1.0f, 1.0f);
		PlayingSound->DecibelCurrentVolume = Vector2(0, 0);
		PlayingSound->ID = SoundID;
		PlayingSound->DecibelSample = 1.0f;

		PlayingSound->Next = AudioState->FirstPlayingSound;
		AudioState->FirstPlayingSound = PlayingSound;

		return(PlayingSound);
	}

	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;
	IDirectSoundBuffer8* m_secondaryBuffer1;
};


#define KANTI_SOUND_MANAGER
#endif
