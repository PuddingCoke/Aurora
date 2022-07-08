#include<Aurora/Keyboard.h>

std::map<Keyboard::Key, Event> Keyboard::keyDownEvents;

std::map<Keyboard::Key, Event> Keyboard::keyUpEvents;

std::unordered_map<Keyboard::Key, bool> Keyboard::keyDownMap;

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

void Keyboard::ini()
{
	keyDownMap.insert(std::pair<Key, bool>(Space, false));
	keyDownMap.insert(std::pair<Key, bool>(Apostrophe, false));
	keyDownMap.insert(std::pair<Key, bool>(Comma, false));
	keyDownMap.insert(std::pair<Key, bool>(Minus, false));
	keyDownMap.insert(std::pair<Key, bool>(Period, false));
	keyDownMap.insert(std::pair<Key, bool>(Slash, false));
	keyDownMap.insert(std::pair<Key, bool>(Num0, false));
	keyDownMap.insert(std::pair<Key, bool>(Num1, false));
	keyDownMap.insert(std::pair<Key, bool>(Num2, false));
	keyDownMap.insert(std::pair<Key, bool>(Num3, false));
	keyDownMap.insert(std::pair<Key, bool>(Num4, false));
	keyDownMap.insert(std::pair<Key, bool>(Num5, false));
	keyDownMap.insert(std::pair<Key, bool>(Num6, false));
	keyDownMap.insert(std::pair<Key, bool>(Num7, false));
	keyDownMap.insert(std::pair<Key, bool>(Num8, false));
	keyDownMap.insert(std::pair<Key, bool>(Num9, false));
	keyDownMap.insert(std::pair<Key, bool>(Semicolon, false));
	keyDownMap.insert(std::pair<Key, bool>(Equal, false));
	keyDownMap.insert(std::pair<Key, bool>(A, false));
	keyDownMap.insert(std::pair<Key, bool>(B, false));
	keyDownMap.insert(std::pair<Key, bool>(C, false));
	keyDownMap.insert(std::pair<Key, bool>(D, false));
	keyDownMap.insert(std::pair<Key, bool>(E, false));
	keyDownMap.insert(std::pair<Key, bool>(F, false));
	keyDownMap.insert(std::pair<Key, bool>(G, false));
	keyDownMap.insert(std::pair<Key, bool>(H, false));
	keyDownMap.insert(std::pair<Key, bool>(I, false));
	keyDownMap.insert(std::pair<Key, bool>(J, false));
	keyDownMap.insert(std::pair<Key, bool>(K, false));
	keyDownMap.insert(std::pair<Key, bool>(L, false));
	keyDownMap.insert(std::pair<Key, bool>(M, false));
	keyDownMap.insert(std::pair<Key, bool>(N, false));
	keyDownMap.insert(std::pair<Key, bool>(O, false));
	keyDownMap.insert(std::pair<Key, bool>(P, false));
	keyDownMap.insert(std::pair<Key, bool>(Q, false));
	keyDownMap.insert(std::pair<Key, bool>(R, false));
	keyDownMap.insert(std::pair<Key, bool>(S, false));
	keyDownMap.insert(std::pair<Key, bool>(T, false));
	keyDownMap.insert(std::pair<Key, bool>(U, false));
	keyDownMap.insert(std::pair<Key, bool>(V, false));
	keyDownMap.insert(std::pair<Key, bool>(W, false));
	keyDownMap.insert(std::pair<Key, bool>(X, false));
	keyDownMap.insert(std::pair<Key, bool>(Y, false));
	keyDownMap.insert(std::pair<Key, bool>(Z, false));
	keyDownMap.insert(std::pair<Key, bool>(Left_Bracket, false));
	keyDownMap.insert(std::pair<Key, bool>(Backslash, false));
	keyDownMap.insert(std::pair<Key, bool>(Right_Bracket, false));
	keyDownMap.insert(std::pair<Key, bool>(Grave_Accent, false));
	keyDownMap.insert(std::pair<Key, bool>(Escape, false));
	keyDownMap.insert(std::pair<Key, bool>(Enter, false));
	keyDownMap.insert(std::pair<Key, bool>(Tab, false));
	keyDownMap.insert(std::pair<Key, bool>(Backspace, false));
	keyDownMap.insert(std::pair<Key, bool>(Insert, false));
	keyDownMap.insert(std::pair<Key, bool>(Delete, false));
	keyDownMap.insert(std::pair<Key, bool>(Right, false));
	keyDownMap.insert(std::pair<Key, bool>(Left, false));
	keyDownMap.insert(std::pair<Key, bool>(Down, false));
	keyDownMap.insert(std::pair<Key, bool>(Up, false));
	keyDownMap.insert(std::pair<Key, bool>(Page_Up, false));
	keyDownMap.insert(std::pair<Key, bool>(PAGE_Down, false));
	keyDownMap.insert(std::pair<Key, bool>(Home, false));
	keyDownMap.insert(std::pair<Key, bool>(End, false));
	keyDownMap.insert(std::pair<Key, bool>(Caps_Lock, false));
	keyDownMap.insert(std::pair<Key, bool>(Scroll_Lock, false));
	keyDownMap.insert(std::pair<Key, bool>(Num_Lock, false));
	keyDownMap.insert(std::pair<Key, bool>(Print_Screen, false));
	keyDownMap.insert(std::pair<Key, bool>(Pause, false));
	keyDownMap.insert(std::pair<Key, bool>(F1, false));
	keyDownMap.insert(std::pair<Key, bool>(F2, false));
	keyDownMap.insert(std::pair<Key, bool>(F3, false));
	keyDownMap.insert(std::pair<Key, bool>(F4, false));
	keyDownMap.insert(std::pair<Key, bool>(F5, false));
	keyDownMap.insert(std::pair<Key, bool>(F6, false));
	keyDownMap.insert(std::pair<Key, bool>(F7, false));
	keyDownMap.insert(std::pair<Key, bool>(F8, false));
	keyDownMap.insert(std::pair<Key, bool>(F9, false));
	keyDownMap.insert(std::pair<Key, bool>(F10, false));
	keyDownMap.insert(std::pair<Key, bool>(F11, false));
	keyDownMap.insert(std::pair<Key, bool>(F12, false));
	keyDownMap.insert(std::pair<Key, bool>(F13, false));
	keyDownMap.insert(std::pair<Key, bool>(F14, false));
	keyDownMap.insert(std::pair<Key, bool>(F15, false));
	keyDownMap.insert(std::pair<Key, bool>(F16, false));
	keyDownMap.insert(std::pair<Key, bool>(F17, false));
	keyDownMap.insert(std::pair<Key, bool>(F18, false));
	keyDownMap.insert(std::pair<Key, bool>(F19, false));
	keyDownMap.insert(std::pair<Key, bool>(F20, false));
	keyDownMap.insert(std::pair<Key, bool>(F21, false));
	keyDownMap.insert(std::pair<Key, bool>(F22, false));
	keyDownMap.insert(std::pair<Key, bool>(F23, false));
	keyDownMap.insert(std::pair<Key, bool>(F24, false));
	keyDownMap.insert(std::pair<Key, bool>(F25, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad0, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad1, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad2, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad3, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad4, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad5, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad6, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad7, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad8, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad9, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad_Decimal, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad_Divide, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad_Multiply, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad_Subtract, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad_Add, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad_Enter, false));
	keyDownMap.insert(std::pair<Key, bool>(Numpad_Equal, false));
	keyDownMap.insert(std::pair<Key, bool>(Left_Shift, false));
	keyDownMap.insert(std::pair<Key, bool>(Left_Control, false));
	keyDownMap.insert(std::pair<Key, bool>(Left_Alt, false));
	keyDownMap.insert(std::pair<Key, bool>(Left_Super, false));
	keyDownMap.insert(std::pair<Key, bool>(Right_Shift, false));
	keyDownMap.insert(std::pair<Key, bool>(Right_Control, false));
	keyDownMap.insert(std::pair<Key, bool>(Right_Alt, false));
	keyDownMap.insert(std::pair<Key, bool>(Right_Super, false));
	keyDownMap.insert(std::pair<Key, bool>(Menu, false));

	keyDownEvents.insert(std::pair<Key, Event>(Space, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Apostrophe, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Comma, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Minus, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Period, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Slash, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Num0, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Num1, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Num2, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Num3, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Num4, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Num5, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Num6, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Num7, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Num8, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Num9, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Semicolon, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Equal, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(A, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(B, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(C, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(D, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(E, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(G, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(H, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(I, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(J, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(K, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(L, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(M, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(N, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(O, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(P, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Q, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(R, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(S, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(T, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(U, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(V, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(W, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(X, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Y, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Z, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Left_Bracket, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Backslash, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Right_Bracket, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Grave_Accent, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Escape, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Enter, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Tab, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Backspace, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Insert, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Delete, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Right, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Left, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Down, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Up, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Page_Up, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(PAGE_Down, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Home, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(End, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Caps_Lock, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Scroll_Lock, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Num_Lock, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Print_Screen, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Pause, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F1, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F2, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F3, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F4, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F5, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F6, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F7, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F8, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F9, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F10, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F11, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F12, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F13, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F14, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F15, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F16, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F17, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F18, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F19, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F20, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F21, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F22, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F23, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F24, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(F25, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad0, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad1, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad2, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad3, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad4, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad5, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad6, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad7, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad8, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad9, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad_Decimal, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad_Divide, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad_Multiply, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad_Subtract, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad_Add, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad_Enter, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Numpad_Equal, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Left_Shift, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Left_Control, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Left_Alt, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Left_Super, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Right_Shift, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Right_Control, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Right_Alt, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Right_Super, Event()));
	keyDownEvents.insert(std::pair<Key, Event>(Menu, Event()));

	keyUpEvents.insert(std::pair<Key, Event>(Space, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Apostrophe, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Comma, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Minus, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Period, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Slash, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Num0, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Num1, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Num2, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Num3, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Num4, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Num5, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Num6, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Num7, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Num8, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Num9, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Semicolon, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Equal, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(A, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(B, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(C, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(D, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(E, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(G, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(H, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(I, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(J, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(K, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(L, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(M, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(N, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(O, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(P, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Q, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(R, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(S, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(T, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(U, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(V, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(W, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(X, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Y, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Z, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Left_Bracket, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Backslash, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Right_Bracket, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Grave_Accent, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Escape, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Enter, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Tab, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Backspace, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Insert, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Delete, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Right, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Left, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Down, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Up, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Page_Up, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(PAGE_Down, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Home, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(End, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Caps_Lock, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Scroll_Lock, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Num_Lock, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Print_Screen, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Pause, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F1, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F2, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F3, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F4, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F5, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F6, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F7, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F8, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F9, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F10, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F11, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F12, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F13, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F14, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F15, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F16, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F17, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F18, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F19, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F20, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F21, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F22, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F23, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F24, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(F25, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad0, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad1, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad2, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad3, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad4, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad5, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad6, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad7, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad8, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad9, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad_Decimal, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad_Divide, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad_Multiply, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad_Subtract, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad_Add, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad_Enter, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Numpad_Equal, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Left_Shift, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Left_Control, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Left_Alt, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Left_Super, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Right_Shift, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Right_Control, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Right_Alt, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Right_Super, Event()));
	keyUpEvents.insert(std::pair<Key, Event>(Menu, Event()));
}
