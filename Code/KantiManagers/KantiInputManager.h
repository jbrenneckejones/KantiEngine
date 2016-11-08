#ifndef KANTI_INPUT_MANAGER

enum Action
{
	ACTION_ONE,
	ACTION_TWO,
	ACTION_THREE,
	ACTION_FOUR,
	ACTION_FIVE,
	ACTION_SIX,
	ACTION_SEVEN,
};

enum State
{
	STATE_ONE,
	STATE_TWO,
	STATE_THREE,
	STATE_FOUR
};

enum Range
{
	RANGE_ONE,
	RANGE_TWO,
};

enum RawInputButton
{
	RAW_INPUT_BUTTON_ONE,
	RAW_INPUT_BUTTON_TWO,
	RAW_INPUT_BUTTON_THREE,
	RAW_INPUT_BUTTON_FOUR,
	RAW_INPUT_BUTTON_FIVE,
	RAW_INPUT_BUTTON_SIX,
	RAW_INPUT_BUTTON_SEVEN,
	RAW_INPUT_BUTTON_EIGHT,
	RAW_INPUT_BUTTON_NINE,
	RAW_INPUT_BUTTON_ZERO,
};

enum RawInputAxis
{
	RAW_INPUT_AXIS_MOUSE_X,
	RAW_INPUT_AXIS_MOUSE_Y,
};

struct input_button_data
{
	RawInputButton ID;
	bool32 IsDown;
	bool32 WasDown;
};

class RangeConverter
{
	// Internal helpers
	private:
	struct Converter
	{
		double MinimumInput;
		double MaximumInput;

		double MinimumOutput;
		double MaximumOutput;

		template <typename RangeType>
		RangeType Convert(RangeType invalue) const
		{
			double v = static_cast<double>(invalue);
			if(v < MinimumInput)
				v = MinimumInput;
			else if(v > MaximumInput)
				v = MaximumInput;

			double interpolationfactor = (v - MinimumInput) / (MaximumInput - MinimumInput);
			return static_cast<RangeType>((interpolationfactor * (MaximumOutput - MinimumOutput)) + MinimumOutput);
		}
	};

	// Internal type shortcuts
	private:
	typedef std::map<Range, Converter> ConversionMapT;

	// Construction
	public:
	explicit RangeConverter() // std::wifstream& infile
	{
		/*
		if(!infile)
			throw std::exception("Invalid file provided to RangeConverter constructor");

		unsigned numconversions = AttemptRead<unsigned>(infile);
		for(unsigned i = 0; i < numconversions; ++i)
		{
			Range range;
			Converter converter;

			range = static_cast<Range>(AttemptRead<unsigned>(infile));
			converter.MinimumInput = AttemptRead<double>(infile);
			converter.MaximumInput = AttemptRead<double>(infile);
			converter.MinimumOutput = AttemptRead<double>(infile);
			converter.MaximumOutput = AttemptRead<double>(infile);

			if((converter.MaximumInput < converter.MinimumInput) || (converter.MaximumOutput < converter.MinimumOutput))
				throw std::exception("Invalid input range conversion");

			ConversionMap.insert(std::make_pair(range, converter));
		}
		*/

		unsigned numconversions = 2;
		for(unsigned i = 0; i < numconversions; ++i)
		{
			Range range;
			Converter converter;

			range = static_cast<Range>(i);
			converter.MinimumInput = -1000;
			converter.MaximumInput = 1000;
			converter.MinimumOutput = -1;
			converter.MaximumOutput = 1;

			if((converter.MaximumInput < converter.MinimumInput) || (converter.MaximumOutput < converter.MinimumOutput))
				throw std::exception("Invalid input range conversion");

			ConversionMap.insert(std::make_pair(range, converter));
		}
	}

	// Conversion interface
	public:
	template <typename RangeType>
	RangeType Convert(Range rangeid, RangeType invalue) const
	{
		ConversionMapT::const_iterator iter = ConversionMap.find(rangeid);
		if(iter == ConversionMap.end())
			return invalue;

		return iter->second.Convert<RangeType>(invalue);
	}

	// Internal tracking
	private:
	ConversionMapT ConversionMap;
};

class InputContext
{
	// Construction and destruction
	public:
	InputContext::InputContext()
		: Conversions(NULL)
	{
		/*
		(const std::wstring& contextfilename)

		std::wifstream infile(contextfilename.c_str());

		unsigned rangecount = AttemptRead<unsigned>(infile);
		for(unsigned i = 0; i < rangecount; ++i)
		{
			RawInputAxis axis = static_cast<RawInputAxis>(AttemptRead<unsigned>(infile));
			Range range = static_cast<Range>(AttemptRead<unsigned>(infile));
			RangeMap[axis] = range;
		}

		unsigned statecount = AttemptRead<unsigned>(infile);
		for(unsigned i = 0; i < statecount; ++i)
		{
			RawInputButton button = static_cast<RawInputButton>(AttemptRead<unsigned>(infile));
			State state = static_cast<State>(AttemptRead<unsigned>(infile));
			StateMap[button] = state;
		}

		unsigned actioncount = AttemptRead<unsigned>(infile);
		for(unsigned i = 0; i < actioncount; ++i)
		{
			RawInputButton button = static_cast<RawInputButton>(AttemptRead<unsigned>(infile));
			Action action = static_cast<Action>(AttemptRead<unsigned>(infile));
			ActionMap[button] = action;
		}

		Conversions = new RangeConverter(infile);

		unsigned sensitivitycount = AttemptRead<unsigned>(infile);
		for(unsigned i = 0; i < sensitivitycount; ++i)
		{
			Range range = static_cast<Range>(AttemptRead<unsigned>(infile));
			double sensitivity = AttemptRead<double>(infile);
			SensitivityMap[range] = sensitivity;
		}
		*/


		RawInputAxis axis1 = static_cast<RawInputAxis>(0);
		Range range1 = static_cast<Range>(0);
		RangeMap[axis1] = range1;

		RawInputAxis axis2 = static_cast<RawInputAxis>(1);
		Range range2 = static_cast<Range>(1);
		RangeMap[axis2] = range2;
		

		unsigned statecount = 4;
		for(unsigned i = 0; i < statecount; ++i)
		{
			RawInputButton button = static_cast<RawInputButton>(i);
			State state = static_cast<State>(i);
			StateMap[button] = state;
		}

		unsigned actioncount = 5;
		for(unsigned i = 0; i < actioncount; ++i)
		{
			RawInputButton button = static_cast<RawInputButton>(i + 4);
			Action action = static_cast<Action>(i);
			ActionMap[button] = action;
		}

		Conversions = new RangeConverter();

		unsigned sensitivitycount = 2;
		for(unsigned i = 0; i < sensitivitycount; ++i)
		{
			Range range = static_cast<Range>(i);
			double sensitivity = 50;
			SensitivityMap[range] = sensitivity;
		}
	}
	~InputContext()
	{
		delete Conversions;
	}

	// Mapping interface
	public:
	bool MapButtonToAction(RawInputButton Button, Action& out) const
	{
		std::map<RawInputButton, Action>::const_iterator iter = ActionMap.find(Button);
		if(iter == ActionMap.end())
			return false;

		out = iter->second;
		return true;
	}
	bool MapButtonToState(RawInputButton Button, State& out) const
	{
		std::map<RawInputButton, State>::const_iterator iter = StateMap.find(Button);
		if(iter == StateMap.end())
			return false;

		out = iter->second;
		return true;
	}
	bool MapAxisToRange(RawInputAxis axis, Range& out) const
	{
		std::map<RawInputAxis, Range>::const_iterator iter = RangeMap.find(axis);
		if(iter == RangeMap.end())
			return false;

		out = iter->second;
		return true;
	}

	double GetSensitivity(Range range) const
	{
		std::map<Range, double>::const_iterator iter = SensitivityMap.find(range);
		if(iter == SensitivityMap.end())
			return 1.0;

		return iter->second;
	}

	const RangeConverter& GetConversions() const
	{ 
		return *Conversions; 
	}

	// Internal tracking
	private:
	std::map<RawInputButton, Action> ActionMap;
	std::map<RawInputButton, State> StateMap;
	std::map<RawInputAxis, Range> RangeMap;

	std::map<Range, double> SensitivityMap;
	RangeConverter* Conversions;
};

// Helper structure
struct MappedInput
{
	std::set<Action> Actions;
	std::set<State> States;
	std::map<Range, double> Ranges;

	// Consumption helpers
	void EatAction(Action action)
	{
		Actions.erase(action);
	}
	void EatState(State state)
	{
		States.erase(state);
	}
	void EatRange(Range range)
	{
		std::map<Range, double>::iterator iter = Ranges.find(range);
		if(iter != Ranges.end())
			Ranges.erase(iter);
	}
};

// Handy type shortcuts
typedef void(*InputCallback)(MappedInput& inputs);

class InputMapper
{
	// Construction and destruction
	public:
	InputMapper()
	{
		/*
		unsigned count;
		std::wifstream infile(L"ContextList.txt");
		if(!(infile >> count))
			throw std::exception("Failed to read ContextList.txt");
		for(unsigned i = 0; i < count; ++i)
		{
			std::wstring name = AttemptRead<std::wstring>(infile);
			std::wstring file = AttemptRead<std::wstring>(infile);
			InputContexts[name] = new InputContext(file);
		}
		*/
	}
	~InputMapper()
	{
		for(std::map<std::string, InputContext*>::iterator iter = InputContexts.begin(); iter != InputContexts.end(); ++iter)
			delete iter->second;
	}

	// Raw input interface
	public:
	void Clear()
	{
		CurrentMappedInput.Actions.clear();
		CurrentMappedInput.Ranges.clear();
		// Note: we do NOT clear states, because they need to remain set
		// across frames so that they don't accidentally show "off" for
		// a tick or two while the raw input is still pending.
	}
	void SetRawButtonState(input_button_data& Button)
	{
		Action action;
		State state;

		if(Button.IsDown && !Button.WasDown)
		{
			if(MapButtonToAction(Button.ID, action))
			{
				CurrentMappedInput.Actions.insert(action);
				return;
			}
		}

		if(Button.IsDown)
		{
			if(MapButtonToState(Button.ID, state))
			{
				CurrentMappedInput.States.insert(state);
				return;
			}
		}

		MapAndEatButton(Button.ID);
	}
	void SetRawAxisValue(RawInputAxis axis, double value)
	{
		for(std::list<InputContext*>::const_iterator iter = ActiveContexts.begin(); iter != ActiveContexts.end(); ++iter)
		{
			const InputContext* context = *iter;

			Range range;
			if(context->MapAxisToRange(axis, range))
			{
				CurrentMappedInput.Ranges[range] = context->GetConversions().Convert(range, value * context->GetSensitivity(range));
				break;
			}
		}
	}

	// Input dispatching interface
	public:
	void Dispatch() const
	{
		MappedInput input = CurrentMappedInput;
		for(std::multimap<int, InputCallback>::const_iterator iter = CallbackTable.begin(); iter != CallbackTable.end(); ++iter)
			(*iter->second)(input);
	}

	// Input callback registration interface
	public:
	void AddCallback(InputCallback callback, int priority)
	{
		CallbackTable.insert(std::make_pair(priority, callback));
	}

	// Context management interface
	public:
	void AddInputContext(std::string Name)
	{
		InputContexts[Name] = new InputContext();
	}

	void PushContext(std::string name)
	{
		std::map<std::string, InputContext*>::iterator iter = InputContexts.find(name);
		if(iter == InputContexts.end())
			throw std::exception("Invalid input context pushed");

		ActiveContexts.push_front(iter->second);
	}
	void PopContext()
	{
		if(ActiveContexts.empty())
			throw std::exception("Cannot pop input context, no contexts active!");

		ActiveContexts.pop_front();
	}

	// Internal helpers
	private:
	bool MapButtonToAction(RawInputButton Button, Action& action) const
	{
		for(std::list<InputContext*>::const_iterator iter = ActiveContexts.begin(); iter != ActiveContexts.end(); ++iter)
		{
			const InputContext* context = *iter;

			if(context->MapButtonToAction(Button, action))
				return true;
		}

		return false;
	}
	bool MapButtonToState(RawInputButton Button, State& state) const
	{
		for(std::list<InputContext*>::const_iterator iter = ActiveContexts.begin(); iter != ActiveContexts.end(); ++iter)
		{
			const InputContext* context = *iter;

			if(context->MapButtonToState(Button, state))
				return true;
		}

		return false;
	}
	void MapAndEatButton(RawInputButton Button)
	{
		Action action;
		State state;

		if(MapButtonToAction(Button, action))
			CurrentMappedInput.EatAction(action);

		if(MapButtonToState(Button, state))
			CurrentMappedInput.EatState(state);
	}

	// Internal tracking
	private:
	std::map<std::string, InputContext*> InputContexts;
	std::list<InputContext*> ActiveContexts;

	std::multimap<int, InputCallback> CallbackTable;

	MappedInput CurrentMappedInput;
};

class KantiInputManager
{
	public:

	InputMapper Mapper;
	int32 LastMouseX;
	int32 LastMouseY;

	KantiInputManager()
	{
		Mapper = InputMapper();
	}

	void Input(MappedInput& inputs)
	{
		/*
		AxisX = inputs.Ranges[InputMapping::RANGE_ONE];
		AxisY = inputs.Ranges[InputMapping::RANGE_TWO];

		StateOne = inputs.States.find(InputMapping::STATE_ONE) != inputs.States.end();
		StateTwo = inputs.States.find(InputMapping::STATE_TWO) != inputs.States.end();
		StateThree = inputs.States.find(InputMapping::STATE_THREE) != inputs.States.end();

		if(inputs.Actions.find(InputMapping::ACTION_ONE) != inputs.Actions.end())
			PushLogLine(L"Action 1 fired!");

		if(inputs.Actions.find(InputMapping::ACTION_TWO) != inputs.Actions.end())
			PushLogLine(L"Action 2 fired!");

		if(inputs.Actions.find(InputMapping::ACTION_THREE) != inputs.Actions.end())
			PushLogLine(L"Action 3 fired!");

		if(inputs.Actions.find(InputMapping::ACTION_FOUR) != inputs.Actions.end())
			PushLogLine(L"Action 4 fired!");

		if(inputs.Actions.find(InputMapping::ACTION_FIVE) != inputs.Actions.end())
			PushLogLine(L"Action 5 fired!");

		if(inputs.Actions.find(InputMapping::ACTION_SIX) != inputs.Actions.end())
			PushLogLine(L"Action 6 fired!");

		if(inputs.Actions.find(InputMapping::ACTION_SEVEN) != inputs.Actions.end())
			PushLogLine(L"Action 7 fired!");
			*/
	}

	//
	// Helper for converting VK codes into raw input button codes
	//
	bool32 ConvertVKCodeToRawButton(uint32 VKCode, input_button_data& Button)
	{
		// W = 0x57
		// A = 0x41
		// S = 0x53
		// D = 0x44

		switch(VKCode)
		{
			case 0x30:      Button.ID = RAW_INPUT_BUTTON_ZERO;		break;
			case 0x57:		Button.ID = RAW_INPUT_BUTTON_ONE;		break;
			case 0x41:		Button.ID = RAW_INPUT_BUTTON_TWO;		break;
			case 0x53:		Button.ID = RAW_INPUT_BUTTON_THREE;		break;
			case 0x44:		Button.ID = RAW_INPUT_BUTTON_FOUR;	    break;
			case 0x35:		Button.ID = RAW_INPUT_BUTTON_FIVE;		break;
			case 0x36:		Button.ID = RAW_INPUT_BUTTON_SIX;		break;
			case 0x37:		Button.ID = RAW_INPUT_BUTTON_SEVEN;	    break;
			case 0x38:		Button.ID = RAW_INPUT_BUTTON_EIGHT;	    break;
			case 0x39:		Button.ID = RAW_INPUT_BUTTON_NINE;	    break;
			default:		return false;						    break;
		}

		return true;
	}

	bool32 ConvertLParamToButtonInfo(int64 LParam, input_button_data& Button)
	{
		bool32 IsDown = ((LParam & (1 << 31)) == 0);
		bool32 WasDown = ((LParam & (1 << 30)) != 0);

		Button.IsDown = IsDown;
		Button.WasDown = WasDown;

		return true;
	}

	bool32 ConvertWindowsButton(uint32 VKCode, int64 LParam, input_button_data& Button)
	{
		ConvertVKCodeToRawButton(VKCode, Button);
		ConvertLParamToButtonInfo(LParam, Button);

		return true;
	}
};

#define KANTI_INPUT_MANAGER
#endif
