#ifndef KANTI_CAMERA_MANAGER

#include "KantiPlatform.h"
#include "KantiMath.h"
#include "KantiIntrinsics.h"

#include "KantiInputManager.h"

class KantiCameraManager
{
	private:
	real32 FOV;
	real32 ZNear, ZFar;

	k_internal KantiCameraManager* MainCamera;

	void UpdateViewMatrix()
	{
		if(isnan(GetOrientation().E[0].X))
		{
			int x = 0;
		}

		Matrix4x4 RotationMatrix = GetOrientation();
		Matrix4x4 TranslateMatrix;

		// RotationMatrix = GetOrientation();
		// RotationMatrix = Matrix4x4::Rotate(RotationMatrix, ToRadians(Rotation.ToEuler().X), Vector3(1.0f, 0.0f, 0.0f));
		// RotationMatrix = Matrix4x4::Rotate(RotationMatrix, ToRadians(Rotation.ToEuler().Y), Vector3(0.0f, 1.0f, 0.0f));
		// RotationMatrix = Matrix4x4::Rotate(RotationMatrix, ToRadians(Rotation.ToEuler().Z), Vector3(0.0f, 0.0f, 1.0f));

		TranslateMatrix = Matrix4x4::Translate(Matrix4x4(), Position);

		if (Type == CameraType::FPS)
		{
			Matrices.View = RotationMatrix * TranslateMatrix;
		}
		else
		{
			Matrices.View = TranslateMatrix * RotationMatrix;
		}
	};

	public:
	enum CameraType { LOOKAT, FPS };
	CameraType Type = CameraType::LOOKAT;

	Quaternion Rotation = Quaternion();
	Vector3 Position = Vector3();

	Vector3 View = Vector3(0.0f, 0.0f, 0.0f);

	real32 RotationSpeed = 1.0f;
	real32 MovementSpeed = 0.1f;

	real32 RightAngle = 0.0f;
	real32 UpAngle = 0.0f;

	inline void SetOrientation(real32 Right, real32 Up)
	{
		RightAngle = Right;
		UpAngle = Up;

		UpdateViewMatrix();
	}

	inline void OffsetOrientation(real32 Right, real32 Up)
	{
		RightAngle += Right * RotationSpeed;
		UpAngle += Up * RotationSpeed;

		UpdateViewMatrix();
	}

	inline Quaternion GetOffsetRotation()
	{
		Quaternion Result = Quaternion::FromAxisAngle(Vector3(1, 0, 0), -UpAngle);
		Result *= Quaternion::FromAxisAngle(Vector3(0, 1, 0), RightAngle);

		return (Result);
	}

	inline Matrix4x4 GetOrientation()
	{
		Matrix4x4 Result = GetOffsetRotation().ToMatrix();

		return (Result);
	}

	bool32 LeftHandCoordinate = true;

	struct
	{
		bool32 Left = false;
		bool32 Right = false;
		bool32 Up = false;
		bool32 Down = false;
	} Keys;

	struct
	{
		Matrix4x4 Perspective;
		Matrix4x4 View;
	} Matrices;

	KantiCameraManager()
	{
		KantiInputManager::InputManager->Mapper.PushContext("maincontext");
		KantiInputManager::InputManager->Mapper.AddCallback(Input, 0);
	}

	k_internal inline KantiCameraManager* GetMainCamera()
	{
		if(!MainCamera)
		{
			MainCamera = new KantiCameraManager();
		}

		return MainCamera;
	}

	k_internal void Input(MappedInput& inputs)
	{
		MainCamera->Keys.Up = inputs.States.find(STATE_ONE) != inputs.States.end();
		MainCamera->Keys.Left = inputs.States.find(STATE_TWO) != inputs.States.end();
		MainCamera->Keys.Down = inputs.States.find(STATE_THREE) != inputs.States.end();
		MainCamera->Keys.Right = inputs.States.find(STATE_FOUR) != inputs.States.end();		

		real32 AxisX = (real32)inputs.Ranges[RANGE_ONE];
		real32 AxisY = (real32)inputs.Ranges[RANGE_TWO];
		MainCamera->OffsetOrientation(AxisX, AxisY);
	}

	inline bool32 IsMoving()
	{
		return Keys.Left || Keys.Right || Keys.Up || Keys.Down;
	}

	inline void SetPerspective(real32 NewFOV, real32 AspectRatio, real32 Near, real32 Far)
	{
		this->FOV = NewFOV;
		this->ZNear = Near;
		this->ZFar = Far;
		Matrices.Perspective = Matrix4x4::Perspective(ToRadians(NewFOV), AspectRatio, ZNear, ZFar);
	};

	inline void UpdateAspectRatio(real32 AspectRatio)
	{
		Matrices.Perspective = Matrix4x4::Perspective(ToRadians(FOV), AspectRatio, ZNear, ZFar);
	}

	inline void SetRotation(Quaternion NewRotation)
	{
		this->Rotation = NewRotation;
		UpdateViewMatrix();
	};

	inline void Rotate(real32 Angle, Vector3 DeltaRotation)
	{
		this->Rotation.RotateTo(Angle, DeltaRotation);
		UpdateViewMatrix();
	}

	inline void SetTranslation(Vector3 Translation)
	{
		this->Position = Translation;
		UpdateViewMatrix();
	};

	inline void Translate(Vector3 DeltaTranslation)
	{
		this->Position += DeltaTranslation;
		UpdateViewMatrix();
	}

	inline void Update(real32 DeltaTime)
	{
		if (Type == CameraType::FPS)
		{
			if (IsMoving())
			{
				Vector3 CameraFront = GetOffsetRotation() * Vector3::Forward();
				Vector3 CameraLeft = GetOffsetRotation() * Vector3::Left();
				// CameraFront.X = -Cos(ToRadians(Rotation.X)) * Sin(ToRadians(Rotation.Y));
				// CameraFront.Y = Sin(ToRadians(Rotation.X));
				// CameraFront.Z = Cos(ToRadians(Rotation.X)) * Cos(ToRadians(Rotation.Y));
				// CameraFront = Vector3::Normalize(CameraFront);

				real32 MoveSpeed = DeltaTime * MovementSpeed;

				if (Keys.Up)
					Position += (CameraFront * MoveSpeed) * (LeftHandCoordinate ? -1.0f : 1.0f);
				if (Keys.Down)
					Position -= (CameraFront * MoveSpeed) * (LeftHandCoordinate ? -1.0f : 1.0f);
				if (Keys.Left)
					Position -= CameraLeft * MoveSpeed;
				if (Keys.Right)
					Position += CameraLeft * MoveSpeed;

				UpdateViewMatrix();
			}
		}
	};

	static void *operator new(memory_index Size)
	{
		return MemAlloc(Size, 8);
	}

	static void operator delete(void *Block)
	{
		MemDealloc(Block);
	}

};

KantiCameraManager* KantiCameraManager::MainCamera = nullptr;

#define KANTI_CAMERA_MANAGER
#endif
