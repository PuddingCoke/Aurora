﻿#include<Aurora/Input/Keyboard.h>

std::map<Keyboard::Key, Event> Keyboard::keyDownEvents=
{
	{Space, Event()},
	{Apostrophe, Event()},
	{Comma, Event()},
	{Minus, Event()},
	{Period, Event()},
	{Slash, Event()},
	{Num0, Event()},
	{Num1, Event()},
	{Num2, Event()},
	{Num3, Event()},
	{Num4, Event()},
	{Num5, Event()},
	{Num6, Event()},
	{Num7, Event()},
	{Num8, Event()},
	{Num9, Event()},
	{Semicolon, Event()},
	{Equal, Event()},
	{A, Event()},
	{B, Event()},
	{C, Event()},
	{D, Event()},
	{E, Event()},
	{F, Event()},
	{G, Event()},
	{H, Event()},
	{I, Event()},
	{J, Event()},
	{K, Event()},
	{L, Event()},
	{M, Event()},
	{N, Event()},
	{O, Event()},
	{P, Event()},
	{Q, Event()},
	{R, Event()},
	{S, Event()},
	{T, Event()},
	{U, Event()},
	{V, Event()},
	{W, Event()},
	{X, Event()},
	{Y, Event()},
	{Z, Event()},
	{Left_Bracket, Event()},
	{Backslash, Event()},
	{Right_Bracket, Event()},
	{Grave_Accent, Event()},
	{Escape, Event()},
	{Enter, Event()},
	{Tab, Event()},
	{Backspace, Event()},
	{Insert, Event()},
	{Delete, Event()},
	{Right, Event()},
	{Left, Event()},
	{Down, Event()},
	{Up, Event()},
	{Page_Up, Event()},
	{PAGE_Down, Event()},
	{Home, Event()},
	{End, Event()},
	{Caps_Lock, Event()},
	{Scroll_Lock, Event()},
	{Num_Lock, Event()},
	{Print_Screen, Event()},
	{Pause, Event()},
	{F1, Event()},
	{F2, Event()},
	{F3, Event()},
	{F4, Event()},
	{F5, Event()},
	{F6, Event()},
	{F7, Event()},
	{F8, Event()},
	{F9, Event()},
	{F10, Event()},
	{F11, Event()},
	{F12, Event()},
	{F13, Event()},
	{F14, Event()},
	{F15, Event()},
	{F16, Event()},
	{F17, Event()},
	{F18, Event()},
	{F19, Event()},
	{F20, Event()},
	{F21, Event()},
	{F22, Event()},
	{F23, Event()},
	{F24, Event()},
	{F25, Event()},
	{Numpad0, Event()},
	{Numpad1, Event()},
	{Numpad2, Event()},
	{Numpad3, Event()},
	{Numpad4, Event()},
	{Numpad5, Event()},
	{Numpad6, Event()},
	{Numpad7, Event()},
	{Numpad8, Event()},
	{Numpad9, Event()},
	{Numpad_Decimal, Event()},
	{Numpad_Divide, Event()},
	{Numpad_Multiply, Event()},
	{Numpad_Subtract, Event()},
	{Numpad_Add, Event()},
	{Numpad_Enter, Event()},
	{Numpad_Equal, Event()},
	{Left_Shift, Event()},
	{Left_Control, Event()},
	{Left_Alt, Event()},
	{Left_Super, Event()},
	{Right_Shift, Event()},
	{Right_Control, Event()},
	{Right_Alt, Event()},
	{Right_Super, Event()},
	{Menu, Event()}
};

std::map<Keyboard::Key, Event> Keyboard::keyUpEvents =
{
	{Space, Event()},
	{Apostrophe, Event()},
	{Comma, Event()},
	{Minus, Event()},
	{Period, Event()},
	{Slash, Event()},
	{Num0, Event()},
	{Num1, Event()},
	{Num2, Event()},
	{Num3, Event()},
	{Num4, Event()},
	{Num5, Event()},
	{Num6, Event()},
	{Num7, Event()},
	{Num8, Event()},
	{Num9, Event()},
	{Semicolon, Event()},
	{Equal, Event()},
	{A, Event()},
	{B, Event()},
	{C, Event()},
	{D, Event()},
	{E, Event()},
	{F, Event()},
	{G, Event()},
	{H, Event()},
	{I, Event()},
	{J, Event()},
	{K, Event()},
	{L, Event()},
	{M, Event()},
	{N, Event()},
	{O, Event()},
	{P, Event()},
	{Q, Event()},
	{R, Event()},
	{S, Event()},
	{T, Event()},
	{U, Event()},
	{V, Event()},
	{W, Event()},
	{X, Event()},
	{Y, Event()},
	{Z, Event()},
	{Left_Bracket, Event()},
	{Backslash, Event()},
	{Right_Bracket, Event()},
	{Grave_Accent, Event()},
	{Escape, Event()},
	{Enter, Event()},
	{Tab, Event()},
	{Backspace, Event()},
	{Insert, Event()},
	{Delete, Event()},
	{Right, Event()},
	{Left, Event()},
	{Down, Event()},
	{Up, Event()},
	{Page_Up, Event()},
	{PAGE_Down, Event()},
	{Home, Event()},
	{End, Event()},
	{Caps_Lock, Event()},
	{Scroll_Lock, Event()},
	{Num_Lock, Event()},
	{Print_Screen, Event()},
	{Pause, Event()},
	{F1, Event()},
	{F2, Event()},
	{F3, Event()},
	{F4, Event()},
	{F5, Event()},
	{F6, Event()},
	{F7, Event()},
	{F8, Event()},
	{F9, Event()},
	{F10, Event()},
	{F11, Event()},
	{F12, Event()},
	{F13, Event()},
	{F14, Event()},
	{F15, Event()},
	{F16, Event()},
	{F17, Event()},
	{F18, Event()},
	{F19, Event()},
	{F20, Event()},
	{F21, Event()},
	{F22, Event()},
	{F23, Event()},
	{F24, Event()},
	{F25, Event()},
	{Numpad0, Event()},
	{Numpad1, Event()},
	{Numpad2, Event()},
	{Numpad3, Event()},
	{Numpad4, Event()},
	{Numpad5, Event()},
	{Numpad6, Event()},
	{Numpad7, Event()},
	{Numpad8, Event()},
	{Numpad9, Event()},
	{Numpad_Decimal, Event()},
	{Numpad_Divide, Event()},
	{Numpad_Multiply, Event()},
	{Numpad_Subtract, Event()},
	{Numpad_Add, Event()},
	{Numpad_Enter, Event()},
	{Numpad_Equal, Event()},
	{Left_Shift, Event()},
	{Left_Control, Event()},
	{Left_Alt, Event()},
	{Left_Super, Event()},
	{Right_Shift, Event()},
	{Right_Control, Event()},
	{Right_Alt, Event()},
	{Right_Super, Event()},
	{Menu, Event()}
};

std::unordered_map<Keyboard::Key, bool> Keyboard::keyDownMap =
{
	{Space, false},
	{Apostrophe, false},
	{Comma, false},
	{Minus, false},
	{Period, false},
	{Slash, false},
	{Num0, false},
	{Num1, false},
	{Num2, false},
	{Num3, false},
	{Num4, false},
	{Num5, false},
	{Num6, false},
	{Num7, false},
	{Num8, false},
	{Num9, false},
	{Semicolon, false},
	{Equal, false},
	{A, false},
	{B, false},
	{C, false},
	{D, false},
	{E, false},
	{F, false},
	{G, false},
	{H, false},
	{I, false},
	{J, false},
	{K, false},
	{L, false},
	{M, false},
	{N, false},
	{O, false},
	{P, false},
	{Q, false},
	{R, false},
	{S, false},
	{T, false},
	{U, false},
	{V, false},
	{W, false},
	{X, false},
	{Y, false},
	{Z, false},
	{Left_Bracket, false},
	{Backslash, false},
	{Right_Bracket, false},
	{Grave_Accent, false},
	{Escape, false},
	{Enter, false},
	{Tab, false},
	{Backspace, false},
	{Insert, false},
	{Delete, false},
	{Right, false},
	{Left, false},
	{Down, false},
	{Up, false},
	{Page_Up, false},
	{PAGE_Down, false},
	{Home, false},
	{End, false},
	{Caps_Lock, false},
	{Scroll_Lock, false},
	{Num_Lock, false},
	{Print_Screen, false},
	{Pause, false},
	{F1, false},
	{F2, false},
	{F3, false},
	{F4, false},
	{F5, false},
	{F6, false},
	{F7, false},
	{F8, false},
	{F9, false},
	{F10, false},
	{F11, false},
	{F12, false},
	{F13, false},
	{F14, false},
	{F15, false},
	{F16, false},
	{F17, false},
	{F18, false},
	{F19, false},
	{F20, false},
	{F21, false},
	{F22, false},
	{F23, false},
	{F24, false},
	{F25, false},
	{Numpad0, false},
	{Numpad1, false},
	{Numpad2, false},
	{Numpad3, false},
	{Numpad4, false},
	{Numpad5, false},
	{Numpad6, false},
	{Numpad7, false},
	{Numpad8, false},
	{Numpad9, false},
	{Numpad_Decimal, false},
	{Numpad_Divide, false},
	{Numpad_Multiply, false},
	{Numpad_Subtract, false},
	{Numpad_Add, false},
	{Numpad_Enter, false},
	{Numpad_Equal, false},
	{Left_Shift, false},
	{Left_Control, false},
	{Left_Alt, false},
	{Left_Super, false},
	{Right_Shift, false},
	{Right_Control, false},
	{Right_Alt, false},
	{Right_Super, false},
	{Menu, false}
};

bool Keyboard::getKeyDown(const Key& key)
{
	return keyDownMap[key];
}

int Keyboard::addKeyDownEvent(const Key& key, std::function<void(void)> func)
{
	return keyDownEvents[key] += func;
}

int Keyboard::addKeyUpEvent(const Key& key, std::function<void(void)> func)
{
	return keyUpEvents[key] += func;
}

void Keyboard::removeKeyDownEvent(const Key& key, const int& id)
{
	keyDownEvents[key] -= id;
}

void Keyboard::removeKeyUpEvent(const Key& key, const int& id)
{
	keyUpEvents[key] -= id;
}