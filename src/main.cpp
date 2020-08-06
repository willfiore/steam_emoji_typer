#include <iostream>
#include <random>
#include <string>
#include <unordered_map>

#include <Windows.h>

HHOOK h_hook = NULL;
ULONG KEYBOARD_EVENT_TAG = 106728622417682ULL;

bool enabled = false;

std::mt19937 generator = std::mt19937(std::random_device()());

int RandomInt(int a, int b) {
	std::uniform_int_distribution<int> dist(a, b);
	return dist(generator);
}

std::unordered_map<short, std::vector<std::string> > codes = {
	{ 0x41, { "csgoa", "abc"}},
	{ 0x42, { "BIOS", "csgob", "bbbb", "redb" }},
	{ 0x43, { "sfvchip" }},
	{ 0x44, { "sfvdko" }},
	{ 0x45, { "explodeshot" }},
	{ 0x46, { "flame_shot" }},
	{ 0x47, { "gmod" }},
	{ 0x48, { "heavy_machine_gun_weapon_item" }},
	{ 0x49, { "icemen" }},
	{ 0x4A, { "journeyj" }},
	{ 0x4B, { "kickbtn" }},
	{ 0x4C, { "lasershot" }},
	{ 0x4D, { "mllrm" }},
	{ 0x4E, { "nletternkoa" }},
	{ 0x4F, { "cherrydonut", "0" }},
	{ 0x50, { "punchbtn" }},
	{ 0x51, { "_q_" }},
	{ 0x52, { "goldenr" }},
	{ 0x53, { "shotgun_weapon_item" }},
	{ 0x54, { "sfvto" }},
	{ 0x55, { "ultra" }},
	{ 0x56, { "sfvictory" }},
	{ 0x57, { "wh" }},
	{ 0x58, { "TryAgain" }},
	{ 0x59, { "_y_" }},
	{ 0x5A, { "z" }},
};

void AppendChar(std::vector<INPUT>* inputs, char c) {
	short scan = VkKeyScan(c);

	uint8_t vk = static_cast<uint8_t>(scan);
	uint8_t flags = static_cast<uint8_t>(scan >> 8);

	bool shift = flags & 1;

	INPUT input = {};
	input.type = INPUT_KEYBOARD;
	input.ki.dwExtraInfo = KEYBOARD_EVENT_TAG;

	if (shift) {
		input.ki.wVk = VK_SHIFT;
		inputs->push_back(input);
	}

	input.ki.wVk = VkKeyScan(c);
	inputs->push_back(input);

	input.ki.dwFlags = KEYEVENTF_KEYUP;
	inputs->push_back(input);

	if (shift) {
		input.ki.wVk = VK_SHIFT;
		inputs->push_back(input);
	}
}

LRESULT CALLBACK KeyboardHookProc(const int code, const WPARAM wParam, const LPARAM lParam) {

	if (wParam == WM_KEYDOWN) {
		KBDLLHOOKSTRUCT* data = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

		// Keyboard event came from us, ignore it
		if (data->dwExtraInfo == KEYBOARD_EVENT_TAG) {
			return CallNextHookEx(h_hook, code, wParam, lParam);
		}

		bool shift = static_cast<bool>(GetKeyState(VK_SHIFT) >> 8);
		bool ctrl = static_cast<bool>(GetKeyState(VK_CONTROL) >> 8);
		bool alt = static_cast<bool>(GetKeyState(VK_MENU) >> 8);
		bool modifiers = shift || ctrl || alt;

		if (data->vkCode == VK_F8 && ctrl) {
			enabled = !enabled;
		}

		if (!enabled) {
			return CallNextHookEx(h_hook, code, wParam, lParam);
		}

		if (data->vkCode < 65 || data->vkCode > 90 || modifiers) {
			return CallNextHookEx(h_hook, code, wParam, lParam);
		}

		const auto& v = codes.at(data->vkCode);
		const std::string& t = v[RandomInt(0, v.size() - 1)];

		std::vector<INPUT> inputs;

		AppendChar(&inputs, ':');
		for (const auto& c : t) {
			AppendChar(&inputs, c);
		}
		AppendChar(&inputs, ':');

		SendInput(inputs.size(), inputs.data(), sizeof(INPUT));

		return 1;
	}

	return CallNextHookEx(h_hook, code, wParam, lParam);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// NOTIFYICONDATA notify_icon_data = {};
	// notify_icon_data.cbSize = sizeof(notify_icon_data);
	// notify_icon_data.hWnd   = NULL;
	// notify_icon_data.uFlags = NIF_ICON;
	// notify_icon_data.hIcon = LoadIcon(NULL, IDI_QUESTION);
	// CoCreateGuid(&notify_icon_data.guidItem);
	// Shell_NotifyIcon(NIM_ADD, &notify_icon_data);

	h_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, NULL, 0);
	while (GetMessage(NULL, NULL, 0, 0));
	UnhookWindowsHookEx(h_hook);
	return 0;
}