#ifndef _MODULEINPUT_H_
#define _MODULEINPUT_H_

#include "Module.h"
#include "Main/Globals.h"
#include "Helper/Config.h"

#include <SDL_scancode.h>
#include <SDL_mouse.h>
#include <MathGeoLib.h>
#include <map>
#include <string>
#include <vector>

struct SDL_Cursor;
typedef int32_t Sint32;
typedef unsigned __int8 Uint8;

const int MAX_KEYS = 286;
const int MAX_MOUSE_BUTTONS = 5;

enum class KeyState
{
	IDLE, 
	DOWN, 
	REPEAT, 
	UP
};

enum class KeyCode
{
	None = SDL_SCANCODE_UNKNOWN,

	A = SDL_SCANCODE_A,
	B = SDL_SCANCODE_B,
	C = SDL_SCANCODE_C,
	D = SDL_SCANCODE_D,
	E = SDL_SCANCODE_E,
	F = SDL_SCANCODE_F,
	G = SDL_SCANCODE_G,
	H = SDL_SCANCODE_H,
	I = SDL_SCANCODE_I,
	J = SDL_SCANCODE_J,
	K = SDL_SCANCODE_K,
	L = SDL_SCANCODE_L,
	M = SDL_SCANCODE_M,
	N = SDL_SCANCODE_N,
	O = SDL_SCANCODE_O,
	P = SDL_SCANCODE_P,
	Q = SDL_SCANCODE_Q,
	R = SDL_SCANCODE_R,
	S = SDL_SCANCODE_S,
	T = SDL_SCANCODE_T,
	U = SDL_SCANCODE_U,
	V = SDL_SCANCODE_V,
	W = SDL_SCANCODE_W,
	X = SDL_SCANCODE_X,
	Y = SDL_SCANCODE_Y,
	Z = SDL_SCANCODE_Z,
	Alpha1 = SDL_SCANCODE_1,
	Alpha2 = SDL_SCANCODE_2,
	Alpha3 = SDL_SCANCODE_3,
	Alpha4 = SDL_SCANCODE_4,
	Alpha5 = SDL_SCANCODE_5,
	Alpha6 = SDL_SCANCODE_6,
	Alpha7 = SDL_SCANCODE_7,
	Alpha8 = SDL_SCANCODE_8,
	Alpha9 = SDL_SCANCODE_9,
	Alpha0 = SDL_SCANCODE_0,
	Return = SDL_SCANCODE_RETURN,
	Escape = SDL_SCANCODE_ESCAPE,
	BackSpace = SDL_SCANCODE_BACKSPACE,
	Tab = SDL_SCANCODE_TAB,
	Space = SDL_SCANCODE_SPACE,
	Minus = SDL_SCANCODE_MINUS,
	Equals = SDL_SCANCODE_EQUALS,
	LeftBracket = SDL_SCANCODE_LEFTBRACKET,
	RightBracket = SDL_SCANCODE_RIGHTBRACKET,
	BackSlash = SDL_SCANCODE_BACKSLASH,
	Nonushash = SDL_SCANCODE_NONUSHASH,
	Semicolon = SDL_SCANCODE_SEMICOLON,
	Apostrophe = SDL_SCANCODE_APOSTROPHE,
	Grave = SDL_SCANCODE_GRAVE,
	Comma = SDL_SCANCODE_COMMA,
	Period = SDL_SCANCODE_PERIOD,
	Slash = SDL_SCANCODE_SLASH,
	CapsLock = SDL_SCANCODE_CAPSLOCK,
	F1 = SDL_SCANCODE_F1,
	F2 = SDL_SCANCODE_F2,
	F3 = SDL_SCANCODE_F3,
	F4 = SDL_SCANCODE_F4,
	F5 = SDL_SCANCODE_F5,
	F6 = SDL_SCANCODE_F6,
	F7 = SDL_SCANCODE_F7,
	F8 = SDL_SCANCODE_F8,
	F9 = SDL_SCANCODE_F9,
	F10 = SDL_SCANCODE_F10,
	F11 = SDL_SCANCODE_F11,
	F12 = SDL_SCANCODE_F12,
	PrintScreen = SDL_SCANCODE_PRINTSCREEN,
	ScrollLock = SDL_SCANCODE_SCROLLLOCK,
	Pause = SDL_SCANCODE_PAUSE,
	Insert = SDL_SCANCODE_INSERT,
	Home = SDL_SCANCODE_HOME,
	PageUp = SDL_SCANCODE_PAGEUP,
	Delete = SDL_SCANCODE_DELETE,
	End = SDL_SCANCODE_END,
	PageDown = SDL_SCANCODE_PAGEDOWN,
	RightArrow = SDL_SCANCODE_RIGHT,
	LeftArrow = SDL_SCANCODE_LEFT,
	DownArrow = SDL_SCANCODE_DOWN,
	UpArrow = SDL_SCANCODE_UP,
	NumLockClear = SDL_SCANCODE_NUMLOCKCLEAR,
	KeypadDivide = SDL_SCANCODE_KP_DIVIDE,
	KeypadMultiply = SDL_SCANCODE_KP_MULTIPLY,
	KeypadMinus = SDL_SCANCODE_KP_MINUS,
	KeypadPlus = SDL_SCANCODE_KP_PLUS,
	KeypadEnter = SDL_SCANCODE_KP_ENTER,
	Keypad1 = SDL_SCANCODE_KP_1,
	Keypad2 = SDL_SCANCODE_KP_2,
	Keypad3 = SDL_SCANCODE_KP_3,
	Keypad4 = SDL_SCANCODE_KP_4,
	Keypad5 = SDL_SCANCODE_KP_5,
	Keypad6 = SDL_SCANCODE_KP_6,
	Keypad7 = SDL_SCANCODE_KP_7,
	Keypad8 = SDL_SCANCODE_KP_8,
	Keypad9 = SDL_SCANCODE_KP_9,
	Keypad0 = SDL_SCANCODE_KP_0,
	KeypadPeriod = SDL_SCANCODE_KP_PERIOD,
	NonusBackSlash = SDL_SCANCODE_NONUSBACKSLASH,
	Aplication = SDL_SCANCODE_APPLICATION,
	Power = SDL_SCANCODE_POWER,
	KeypadEquals = SDL_SCANCODE_KP_EQUALS,
	F13 = SDL_SCANCODE_F13,
	F14 = SDL_SCANCODE_F14,
	F15 = SDL_SCANCODE_F15,
	F16 = SDL_SCANCODE_F16,
	F17 = SDL_SCANCODE_F17,
	F18 = SDL_SCANCODE_F18,
	F19 = SDL_SCANCODE_F19,
	F20 = SDL_SCANCODE_F20,
	F21 = SDL_SCANCODE_F21,
	F22 = SDL_SCANCODE_F22,
	F23 = SDL_SCANCODE_F23,
	F24 = SDL_SCANCODE_F24,
	Execute = SDL_SCANCODE_EXECUTE,
	Help = SDL_SCANCODE_HELP,
	Menu = SDL_SCANCODE_MENU,
	Select = SDL_SCANCODE_SELECT,
	Stop = SDL_SCANCODE_STOP,
	Again = SDL_SCANCODE_AGAIN,
	Undo = SDL_SCANCODE_UNDO,
	Cut = SDL_SCANCODE_CUT,
	Copy = SDL_SCANCODE_COPY,
	Paste = SDL_SCANCODE_PASTE,
	Find = SDL_SCANCODE_FIND,
	Mute = SDL_SCANCODE_MUTE,
	VolumeUp = SDL_SCANCODE_VOLUMEUP,
	VolumeDown = SDL_SCANCODE_VOLUMEDOWN,
	LockingCapsLock = 130,
	LockingNumLock = 131,
	LockingScrollLock = 132,
	KeypadComma = SDL_SCANCODE_KP_COMMA,
	EqualsAs400 = SDL_SCANCODE_KP_EQUALSAS400,
	International1 = SDL_SCANCODE_INTERNATIONAL1,
	International2 = SDL_SCANCODE_INTERNATIONAL2,
	International3 = SDL_SCANCODE_INTERNATIONAL3,
	International4 = SDL_SCANCODE_INTERNATIONAL4,
	International5 = SDL_SCANCODE_INTERNATIONAL5,
	International6 = SDL_SCANCODE_INTERNATIONAL6,
	International7 = SDL_SCANCODE_INTERNATIONAL7,
	International8 = SDL_SCANCODE_INTERNATIONAL8,
	International9 = SDL_SCANCODE_INTERNATIONAL9,
	Lang1 = SDL_SCANCODE_LANG1,
	Lang2 = SDL_SCANCODE_LANG2,
	Lang3 = SDL_SCANCODE_LANG3,
	Lang4 = SDL_SCANCODE_LANG4,
	Lang5 = SDL_SCANCODE_LANG5,
	Lang6 = SDL_SCANCODE_LANG6,
	Lang7 = SDL_SCANCODE_LANG7,
	Lang8 = SDL_SCANCODE_LANG8,
	Lang9 = SDL_SCANCODE_LANG9,
	AltErase = SDL_SCANCODE_ALTERASE,
	SysReq = SDL_SCANCODE_SYSREQ,
	Cancel = SDL_SCANCODE_CANCEL,
	Clear = SDL_SCANCODE_CLEAR,
	Prior = SDL_SCANCODE_PRIOR,
	Return2 = SDL_SCANCODE_RETURN2,
	Separator = SDL_SCANCODE_SEPARATOR,
	Quit = SDL_SCANCODE_OUT,
	Oper = SDL_SCANCODE_OPER,
	ClearAgain = SDL_SCANCODE_CLEARAGAIN,
	Crsel = SDL_SCANCODE_CRSEL,
	Exsel = SDL_SCANCODE_EXSEL,
	Keypad00 = SDL_SCANCODE_KP_00,
	Keypad000 = SDL_SCANCODE_KP_000,
	ThousandsSeparator = SDL_SCANCODE_THOUSANDSSEPARATOR,
	DecimalSeparator = SDL_SCANCODE_DECIMALSEPARATOR,
	CurrencyUnit = SDL_SCANCODE_CURRENCYUNIT,
	CurrencySubUnit = SDL_SCANCODE_CURRENCYSUBUNIT,
	KeypadLeftParen = SDL_SCANCODE_KP_LEFTPAREN,
	KeypadRightParen = SDL_SCANCODE_KP_RIGHTPAREN,
	KeypadLeftBrace = SDL_SCANCODE_KP_LEFTBRACE,
	KeypadaRightBrace = SDL_SCANCODE_KP_RIGHTBRACE,
	KeypadTab = SDL_SCANCODE_KP_TAB,
	KeypadBackspace = SDL_SCANCODE_KP_BACKSPACE,
	KeypadA = SDL_SCANCODE_KP_A,
	KeypadB = SDL_SCANCODE_KP_B,
	KeypadC = SDL_SCANCODE_KP_C,
	KeypadD = SDL_SCANCODE_KP_D,
	KeypadE = SDL_SCANCODE_KP_E,
	KeypadF = SDL_SCANCODE_KP_F,
	KeypadXor = SDL_SCANCODE_KP_XOR,
	KeypadPower = SDL_SCANCODE_KP_POWER,
	KeypadPercent = SDL_SCANCODE_KP_PERCENT,
	KeypadLess = SDL_SCANCODE_KP_LESS,
	KeypadGreater = SDL_SCANCODE_KP_GREATER,
	KeypadAmpersand = SDL_SCANCODE_KP_AMPERSAND,
	KeypadDbLampersand = SDL_SCANCODE_KP_DBLAMPERSAND,
	KeypadVerticalBar = SDL_SCANCODE_KP_VERTICALBAR,
	KeypadDbVerticalBar = SDL_SCANCODE_KP_DBLVERTICALBAR,
	KeypadColon = SDL_SCANCODE_KP_COLON,
	KeypadHash = SDL_SCANCODE_KP_HASH,
	KeypadSpace = SDL_SCANCODE_KP_SPACE,
	KeypadAt = SDL_SCANCODE_KP_AT,
	KeypadExclam = SDL_SCANCODE_KP_EXCLAM,
	KeypadMemStore = SDL_SCANCODE_KP_MEMSTORE,
	KeypadMemRecall = SDL_SCANCODE_KP_MEMRECALL,
	KeypadMemClear = SDL_SCANCODE_KP_MEMCLEAR,
	KeypadMemAdd = SDL_SCANCODE_KP_MEMADD,
	KeypadMemSubtract = SDL_SCANCODE_KP_MEMSUBTRACT,
	KeypadMemMultiply = SDL_SCANCODE_KP_MEMMULTIPLY,
	KeypadMemDivide = SDL_SCANCODE_KP_MEMDIVIDE,
	KeypadPlusMinus = SDL_SCANCODE_KP_PLUSMINUS,
	KeypadClear = SDL_SCANCODE_KP_CLEAR,
	KeypadClearEntry = SDL_SCANCODE_KP_CLEARENTRY,
	KeypadBinary = SDL_SCANCODE_KP_BINARY,
	KeypadOctal = SDL_SCANCODE_KP_OCTAL,
	KeypadDecimal = SDL_SCANCODE_KP_DECIMAL,
	KeypadHexadecimal = SDL_SCANCODE_KP_HEXADECIMAL,
	LeftControl = SDL_SCANCODE_LCTRL,
	LeftShift = SDL_SCANCODE_LSHIFT,
	LeftAlt = SDL_SCANCODE_LALT,
	LeftCommand = SDL_SCANCODE_LGUI,
	RightControl = SDL_SCANCODE_RCTRL,
	RightShift = SDL_SCANCODE_RSHIFT,
	RightAlt = SDL_SCANCODE_RALT,
	RightCommand = SDL_SCANCODE_RGUI,
	Mode = SDL_SCANCODE_MODE,
	AudioNext = SDL_SCANCODE_AUDIONEXT,
	AudioPrev = SDL_SCANCODE_AUDIOPREV,
	AudioStop = SDL_SCANCODE_AUDIOSTOP,
	AudioPlay = SDL_SCANCODE_AUDIOPLAY,
	AudioMute = SDL_SCANCODE_AUDIOMUTE,
	MediaSelect = SDL_SCANCODE_MEDIASELECT,
	Www = SDL_SCANCODE_WWW,
	Mail = SDL_SCANCODE_MAIL,
	Calculator = SDL_SCANCODE_CALCULATOR,
	Computer = SDL_SCANCODE_COMPUTER,
	AcSearch = SDL_SCANCODE_AC_SEARCH,
	AcHome = SDL_SCANCODE_AC_HOME,
	AcBack = SDL_SCANCODE_AC_BACK,
	AcForward = SDL_SCANCODE_AC_FORWARD,
	AcStop = SDL_SCANCODE_AC_STOP,
	AcRefresh = SDL_SCANCODE_AC_REFRESH,
	AcBookmarks = SDL_SCANCODE_AC_BOOKMARKS,
	BrightnessDown = SDL_SCANCODE_BRIGHTNESSDOWN,
	BrightnessUp = SDL_SCANCODE_BRIGHTNESSUP,
	DisplaySwitch = SDL_SCANCODE_DISPLAYSWITCH,
	IllumToggle = SDL_SCANCODE_KBDILLUMTOGGLE,
	IllumDown = SDL_SCANCODE_KBDILLUMDOWN,
	IllumUp = SDL_SCANCODE_KBDILLUMUP,
	Eject = SDL_SCANCODE_EJECT,
	Sleep = SDL_SCANCODE_SLEEP,
	App1 = SDL_SCANCODE_APP1,
	App2 = SDL_SCANCODE_APP2,
	AudioRewind = SDL_SCANCODE_AUDIOREWIND,
	AudioFastForward = SDL_SCANCODE_AUDIOFASTFORWARD,
};

enum class MouseButton
{
	Left = SDL_BUTTON_LEFT,
	Middle = SDL_BUTTON_MIDDLE,
	Right = SDL_BUTTON_RIGHT,
	X1 = SDL_BUTTON_X1,
	X2 = SDL_BUTTON_X2
};

struct GameInput
{
	std::string name;
	std::vector<KeyCode> keys;
	std::vector<MouseButton> mouse_buttons;

	void Save(Config &config)
	{
		config.AddString(name, "Name");
		config.AddUInt(keys.size(), "SizeKeys");
		for(unsigned int i = 0; i < keys.size(); ++i)
		{
			std::string name_k("k" + std::to_string(i));
			
			config.AddUInt((uint64_t)keys[i], name_k);
		}

		config.AddUInt(mouse_buttons.size(), "SizeMouse");
		for (unsigned int j = 0; j < mouse_buttons.size(); ++j)
		{
			std::string name_m("m" + std::to_string(j));
			config.AddUInt((uint64_t)mouse_buttons[j], name_m);
		}
	}

	void Load(Config &config)
	{
		config.GetString("Name", name, "DefaultName");
		unsigned int size_keys = config.GetUInt("SizeKeys", 0);
		for(int i = 0; i < size_keys; ++i)
		{
			std::string name_k("k" + std::to_string(i));
			keys.push_back((KeyCode)config.GetUInt(name_k, 0));
		}

		unsigned int size_mouse = config.GetUInt("SizeMouse", 0);
		for (int j = 0; j < size_mouse; ++j)
		{
			std::string name_m("m" + std::to_string(j));
			mouse_buttons.push_back((MouseButton)config.GetUInt(name_m, 0));
		}
	}
};

class ModuleInput : public Module
{
public:
	ModuleInput() = default;
	~ModuleInput() = default;

	bool Init() override;
	update_status PreUpdate() override;
	bool CleanUp() override;

	bool GetKey(KeyCode key);
	bool GetKeyDown(KeyCode key);
	bool GetKeyUp(KeyCode key);

	bool GetMouseButton(MouseButton button);
	bool GetMouseButtonDown(MouseButton button); 
	bool GetMouseButtonUp(MouseButton button);

	bool GetGameInput(const char* name);
	bool GetGameInputDown(const char* name);
	bool GetGameInputUp(const char* name);

	void CreateGameInput(GameInput game_input);

	float2 GetMousePosition() const;
	float2 GetMouseMotion() const;
	Sint32 GetMouseWheelMotion() const;

	Uint8 GetMouseClicks() const;
	bool IsMouseMoving() const;

private:
	void SaveGameInputs(Config &config);
	void LoadGameInputs(Config &config);

private:
	std::map<KeyCode, KeyState> key_bible;
	std::map<MouseButton, KeyState> mouse_bible;

	//Predefined buttons
	std::map<std::string, GameInput> game_inputs;

	const Uint8 *keys = nullptr;

	float2 mouse_position;
	float2 mouse_motion;
	Sint32 mouse_wheel_motion;

	Uint8 mouse_clicks;
	bool mouse_moving;
};

#endif //_MODULEINPUT_H
