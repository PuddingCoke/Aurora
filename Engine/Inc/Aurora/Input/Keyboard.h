#pragma once

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include<unordered_map>

#include"Event.h"

class Keyboard
{
public:

	Keyboard() = delete;

	Keyboard(const Keyboard&) = delete;

	void operator=(const Keyboard&) = delete;

	enum Key
	{
		Space = 32,
		Apostrophe = 39,
		Comma = 44,
		Minus = 45,
		Period = 46,
		Slash = 47,
		Num0 = 48,
		Num1 = 49,
		Num2 = 50,
		Num3 = 51,
		Num4 = 52,
		Num5 = 53,
		Num6 = 54,
		Num7 = 55,
		Num8 = 56,
		Num9 = 57,
		Semicolon = 59,
		Equal = 61,
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		Left_Bracket = 91,
		Backslash = 92,
		Right_Bracket = 93,
		Grave_Accent = 96,
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		Page_Up = 266,
		PAGE_Down = 267,
		Home = 268,
		End = 269,
		Caps_Lock = 280,
		Scroll_Lock = 281,
		Num_Lock = 282,
		Print_Screen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,
		Numpad0 = 320,
		Numpad1 = 321,
		Numpad2 = 322,
		Numpad3 = 323,
		Numpad4 = 324,
		Numpad5 = 325,
		Numpad6 = 326,
		Numpad7 = 327,
		Numpad8 = 328,
		Numpad9 = 329,
		Numpad_Decimal = 330,
		Numpad_Divide = 331,
		Numpad_Multiply = 332,
		Numpad_Subtract = 333,
		Numpad_Add = 334,
		Numpad_Enter = 335,
		Numpad_Equal = 336,
		Left_Shift = 340,
		Left_Control = 341,
		Left_Alt = 342,
		Left_Super = 343,
		Right_Shift = 344,
		Right_Control = 345,
		Right_Alt = 346,
		Right_Super = 347,
		Menu = 348
	};

	static bool getKeyDown(const Key& key);

	static int addKeyDownEvent(const Key& key, std::function<void(void)> func);

	static int addKeyUpEvent(const Key& key, std::function<void(void)> func);

	static void removeKeyDownEvent(const Key& key, const int& id);

	static void removeKeyUpEvent(const Key& key, const int& id);

private:

	friend class Aurora;

	static std::map<Key, Event> keyDownEvents;

	static std::map<Key, Event> keyUpEvents;

	static std::unordered_map<Key, bool> keyDownMap;


};

#endif // !_KEYBOARD_H_
