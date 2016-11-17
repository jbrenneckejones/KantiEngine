

class Entity
{
public:

	// Variables

	KString Name;
	UniqueID InstanceID;

	uint32 Layer;

public:

	// Functions

	Entity()
	{
		Name = "Object";
		InstanceID = GetUUID();
		Layer = 0;
	}

	Entity(KString EntityName)
	{
		InstanceID = GetUUID();
		Name = EntityName;
		Layer = 0;
	}

	virtual ~Entity() {};

	bool32 operator ==(const Entity& Comparer)
	{
		return InstanceID == Comparer.InstanceID;
	}

	bool32 operator ==(Entity& Comparer)
	{
		return InstanceID == Comparer.InstanceID;
	}

	bool32 operator ==(Entity Comparer)
	{
		return InstanceID == Comparer.InstanceID;
	}

	bool32 operator ==(Entity* Comparer)
	{
		if (!Comparer)
		{
			return false;
		}

		return InstanceID == Comparer->InstanceID;
	}
};

class GameComponent : public Entity
{
public:

	// Variables

	class GameEntity* EntityAttachedTo;

public:

	// Functions

	inline GameComponent()
	{
	};

	virtual ~GameComponent() {};

	inline GameComponent(KString ComponentName, GameEntity* AttachedTo)
	{
		Name = ComponentName;
		EntityAttachedTo = AttachedTo;
	}
};

class Transform : public GameComponent
{
public:

	// Variables

	KVector3 Position;
	KVector3 Scale;
	KQuaternion Rotation;

	GameEntity* Parent;

public:

	// Functions

	inline Transform()
	{
		Position = KVector3::Zero;
		Scale = KVector3::One;
		Rotation = KVector3::Zero;
	}
};

class GameEntity : public Entity
{
public:

	// Variables

	bool32 IsActive;

	KList<class GameComponent*> Components;

	Transform EntTransform;

	const GameEntity* GameObject = this;

public:

	// Functions

	inline GameEntity()
	{
	}

	inline GameEntity(KString ObjectName) : Entity(ObjectName)
	{
		Name = ObjectName;
	}

	virtual ~GameEntity() {};

	template<class GameComponent>
	GameComponent* AddComponent();

	template<class GameComponent>
	GameComponent* AddComponent(GameComponent* ComponentPrefab);

	template<class GameComponent>
	GameComponent* GetComponent();
};

class Behaviour : public GameComponent
{
public:

	// Variables

	bool32 IsActive;

public:

	// Functions

	inline Behaviour()
	{
		IsActive = true;
	}

	virtual ~Behaviour() {};
};

class GameBehaviour : public Behaviour
{
public:

	// Variables

public:

	// Functions

	inline GameBehaviour()
	{
		AddHandlers();
	}

	inline void AddHandlers();

	virtual ~GameBehaviour() {};

	// virtual void OnCollisionStart(const Collision& CollideInfo) {};
	// virtual void OnCollisionStay(const Collision& CollideInfo) {};
	// virtual void OnCollisionEnd(const Collision& CollideInfo) {};

	// virtual void OnTriggerEnter(const Collider& Collided) {};
	// virtual void OnTriggerStay(const Collider& Collided) {};
	// virtual void OnTriggerExit(const Collider& Collided) {};

	virtual void Awake() {};
	virtual void Start() {};
	virtual void FixedUpdate() {};
	virtual void Update() {};
};

class Renderer : public GameComponent
{
public:

	// Variables

public:

	// Functions

	inline Renderer() {};

	virtual void Update() = 0;

	virtual void Draw() = 0;

	virtual uint32 GetLayer()
	{
		return EntityAttachedTo->Layer;
	}
};


class KMeshRenderer : public Renderer
{
public:

	// Variables

	// Temp
	KVector3 Position;

	KMatrixData Matrices;
	KMeshData MeshData;

public:

	// Functions

	inline KMeshRenderer()
	{
	}

	inline virtual void Update() override
	{
		Matrices.Projection = KantiCameraManager::GetMainCamera()->Matrices.Perspective;
		Matrices.View = KantiCameraManager::GetMainCamera()->Matrices.View;

		Matrices.Model = KMatrix4x4::Identity(1.0f);

		KQuaternion Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		Matrices.Model.Translate(Position); // EntityAttachedTo->EntTransform.Position);
		Matrices.Model = Matrices.Model * Rotation.ToMatrix(); // EntityAttachedTo->EntTransform.Rotation.ToMatrix();
	}

	inline virtual void Draw() override
	{
		// KEngine::EngineInstance->KRenderer.Up(BufferData);
	}
};
