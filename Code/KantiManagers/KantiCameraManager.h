#ifndef KANTI_CAMERA_MANAGER

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

		KMatrix4x4 RotationMatrix = GetOrientation();
		KMatrix4x4 TranslateMatrix;

		// RotationMatrix = GetOrientation();
		// RotationMatrix = Matrix4x4::Rotate(RotationMatrix, ToRadians(Rotation.ToEuler().X), Vector3(1.0f, 0.0f, 0.0f));
		// RotationMatrix = Matrix4x4::Rotate(RotationMatrix, ToRadians(Rotation.ToEuler().Y), Vector3(0.0f, 1.0f, 0.0f));
		// RotationMatrix = Matrix4x4::Rotate(RotationMatrix, ToRadians(Rotation.ToEuler().Z), Vector3(0.0f, 0.0f, 1.0f));

		TranslateMatrix = KMatrix4x4::Translate(KMatrix4x4(), Position);

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

	KQuaternion Rotation = KQuaternion();
	KVector3 Position = KVector3();

	KVector3 View = KVector3(0.0f, 0.0f, 0.0f);

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

	inline KQuaternion GetOffsetRotation()
	{
		KQuaternion Result = KQuaternion::FromAxisAngle(KVector3(1, 0, 0), -UpAngle);
		Result *= KQuaternion::FromAxisAngle(KVector3(0, 1, 0), RightAngle);

		return (Result);
	}

	inline KMatrix4x4 GetOrientation()
	{
		KMatrix4x4 Result = GetOffsetRotation().ToMatrix();

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
		KMatrix4x4 Perspective;
		KMatrix4x4 View;
	} Matrices;

	KantiCameraManager()
	{
		
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
		Matrices.Perspective = KMatrix4x4::Perspective(ToRadians(NewFOV), AspectRatio, ZNear, ZFar);
	};

	inline void UpdateAspectRatio(real32 AspectRatio)
	{
		Matrices.Perspective = KMatrix4x4::Perspective(ToRadians(FOV), AspectRatio, ZNear, ZFar);
	}

	inline void SetRotation(KQuaternion NewRotation)
	{
		this->Rotation = NewRotation;
		UpdateViewMatrix();
	};

	inline void Rotate(real32 Angle, KVector3 DeltaRotation)
	{
		this->Rotation.RotateTo(Angle, DeltaRotation);
		UpdateViewMatrix();
	}

	inline void SetTranslation(KVector3 Translation)
	{
		this->Position = Translation;
		UpdateViewMatrix();
	};

	inline void Translate(KVector3 DeltaTranslation)
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
				KVector3 CameraFront = GetOffsetRotation() * KVector3::Forward;
				KVector3 CameraLeft = GetOffsetRotation() * KVector3::Left;
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
