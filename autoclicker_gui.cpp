#define _WIN32_WINNT 0x0601
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <thread>
#include <atomic>
#include <random>
#include <chrono>
#include <string>
#include <cstdlib>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dwmapi.lib")

#define C_BG        RGB(0x1E, 0x1E, 0x2E)
#define C_CARD      RGB(0x28, 0x28, 0x3E)
#define C_BORDER    RGB(0x44, 0x44, 0x66)
#define C_ACCENT    RGB(0x7C, 0x5C, 0xFA)
#define C_ACCENT2   RGB(0x58, 0x9A, 0xFA)
#define C_TEXT      RGB(0xE0, 0xE0, 0xF0)
#define C_TEXTDIM   RGB(0x88, 0x88, 0xAA)
#define C_EDITBG    RGB(0x30, 0x30, 0x48)
#define C_STATUSOK  RGB(0x4E, 0xD0, 0x8A)
#define C_STATUSRUN RGB(0xFA, 0xC8, 0x3A)

#define IDC_INTERVAL    101
#define IDC_RANDOM      102
#define IDC_MODE_HOLD   103
#define IDC_MODE_TOGGLE 104
#define IDC_STATUS      105
#define IDC_APPLY       107
#define IDC_ABOUT       108
#define IDC_CLICK_KEY   109
#define IDC_SET_KEY     110
#define IDC_TRIGGER_KEY 111
#define IDC_SET_TRIGGER 112
#define IDC_LANG        113

struct LangStr { const wchar_t* zh; const wchar_t* en; };

static bool g_lang_zh = true;

inline const wchar_t* T(const LangStr& s) { return g_lang_zh ? s.zh : s.en; }

static const LangStr L_TITLE       = {L"\u8FDE\u70B9\u5DE5\u5177", L"Auto Clicker"};
static const LangStr L_SUBTITLE    = {L"v1.0  \u6309 F5 \u5F00\u542F/\u5173\u95ED", L"v1.0  Press F5 to toggle"};
static const LangStr L_INTERVAL    = {L"\u70B9\u51FB\u95F4\u9694", L"Interval"};
static const LangStr L_RANDOM      = {L"\u968F\u673A\u6270\u52A8", L"Jitter"};
static const LangStr L_TRIGGERKEY  = {L"\u89E6\u53D1\u952E", L"Trigger"};
static const LangStr L_CLICKKEY    = {L"\u8FDE\u70B9\u952E", L"Click Key"};
static const LangStr L_MODE        = {L"\u89E6\u53D1\u6A21\u5F0F", L"Mode"};
static const LangStr L_TOGGLE      = {L"\u5207\u6362  (\u6309\u4E00\u6B21\u5F00/\u5173)", L"Toggle  (press once)"};
static const LangStr L_HOLD        = {L"\u6309\u4F4F  (\u6301\u7EED\u8FDE\u70B9)", L"Hold  (hold to click)"};
static const LangStr L_APPLY       = {L"\u5E94\u7528", L"Apply"};
static const LangStr L_HELP        = {L"\u5E2E\u52A9", L"Help"};
static const LangStr L_SET         = {L"\u8BBE\u7F6E", L"Set"};
static const LangStr L_PRESS_KEY   = {L"\u8BF7\u6309\u4E0B\u4EFB\u610F\u952E...", L"Press any key..."};
static const LangStr L_ST_OFF      = {L"\u672A\u5F00\u542F\u2014\u2014\u6309 F5 \u5F00\u542F", L"Off \u2014 Press F5 to enable"};
static const LangStr L_ST_IDLE     = {L"\u5F85\u673A\u4E2D\u2014\u2014\u7B49\u5F85\u89E6\u53D1\u952E", L"Ready \u2014 Waiting for trigger"};
static const LangStr L_ST_RUN      = {L"\u8FDE\u70B9\u8FDB\u884C\u4E2D...", L"Clicking..."};
static const LangStr L_NOT_SET     = {L"\u672A\u8BBE\u7F6E", L"Not set"};
static const LangStr L_LBTN        = {L"\u9F20\u6807\u5DE6\u952E", L"LButton"};
static const LangStr L_RBTN        = {L"\u9F20\u6807\u53F3\u952E", L"RButton"};
static const LangStr L_MBTN        = {L"\u9F20\u6807\u4E2D\u952E", L"MButton"};
static const LangStr L_XBTN1       = {L"\u4FA7\u952E1", L"XButton1"};
static const LangStr L_XBTN2       = {L"\u4FA7\u952E2", L"XButton2"};

static const wchar_t* HELP_ZH =
    L"Auto Clicker  v1.0\n"
    L"\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n"
    L"\u300a\u4F7F\u7528\u6B65\u9AA4\u300b\n"
    L"1. \u8BBE\u7F6E\u89E6\u53D1\u952E\u4E0E\u8FDE\u70B9\u952E\uFF0C\u9009\u62E9\u89E6\u53D1\u6A21\u5F0F\n"
    L"2. \u914D\u7F6E\u70B9\u51FB\u95F4\u9694\u548C\u968F\u673A\u6270\u52A8\uFF0C\u70B9\u300a\u5E94\u7528\u300b\n"
    L"3. \u6309 F5 \u5F00\u542F\u5168\u5C40\u5F00\u5173\uFF08\u518D\u6B21\u6309 F5 \u5173\u95ED\uFF09\n"
    L"4. \u6309\u89E6\u53D1\u952E\u5F00\u59CB / \u505C\u6B62\u8FDE\u70B9\n"
    L"\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n"
    L"\u300a\u952E\u4F4D\u8BF4\u660E\u300b\n"
    L"F5        \u2014  \u5168\u5C40\u5F00\u5173\uFF08\u4F18\u5148\u7EA7\u6700\u9AD8\uFF09\n"
    L"\u89E6\u53D1\u952E    \u2014  \u63A7\u5236\u5F00\u59CB / \u505C\u6B62\u8FDE\u70B9\n"
    L"\u8FDE\u70B9\u952E    \u2014  \u88AB\u6A21\u62DF\u91CD\u590D\u6309\u4E0B\u7684\u6309\u952E\n"
    L"\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n"
    L"\u300a\u89E6\u53D1\u6A21\u5F0F\u300b\n"
    L"\u5207\u6362  \u2014  \u6309\u4E00\u6B21\u5F00\u59CB\uFF0C\u518D\u6309\u4E00\u6B21\u505C\u6B62\n"
    L"\u6309\u4F4F  \u2014  \u6301\u7EED\u6309\u4F4F\u671F\u95F4\u8FDE\u70B9\uFF0C\u677E\u5F00\u5373\u505C\n"
    L"\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n"
    L"\u6CE8\uFF1A\u89E6\u53D1\u952E\u548C\u8FDE\u70B9\u952E\u5747\u4E0D\u80FD\u8BBE\u4E3A F5";

static const wchar_t* HELP_EN =
    L"Auto Clicker  v1.0\n"
    L"--------------------\n"
    L"[Steps]\n"
    L"1. Set Trigger key & Click key, choose mode\n"
    L"2. Configure interval & jitter, click Apply\n"
    L"3. Press F5 to enable global switch\n"
    L"   (press F5 again to disable)\n"
    L"4. Press trigger key to start / stop\n"
    L"--------------------\n"
    L"[Keys]\n"
    L"F5          - Global on/off (highest priority)\n"
    L"Trigger key - Controls when clicking starts/stops\n"
    L"Click key   - The key that gets auto-pressed\n"
    L"--------------------\n"
    L"[Modes]\n"
    L"Toggle  - Press once to start, again to stop\n"
    L"Hold    - Hold trigger key to click continuously\n"
    L"--------------------\n"
    L"Note: Trigger & Click keys cannot be F5";

std::atomic<bool> g_running{true};
std::atomic<bool> g_clicking{false};
std::atomic<bool> g_hold_mode{false};
std::atomic<int>  g_interval_ms{50};
std::atomic<int>  g_random_range{10};
std::atomic<int>  g_trigger_key_vk{VK_LBUTTON};
std::atomic<int>  g_click_key_vk{VK_LBUTTON};
std::atomic<bool> g_master_switch{false};
std::atomic<bool> g_simulating_input{false};
std::atomic<bool> g_user_holding_trigger{false};
std::atomic<int>  g_simulated_up_count{0};
bool              g_key_was_pressed = false;
std::atomic<int>  g_hold_release_count{0};

HWND g_hwnd           = nullptr;
HWND g_hwndStatus     = nullptr;
HWND g_hwndClickKey   = nullptr;
HWND g_hwndTriggerKey = nullptr;
HWND g_hwndLang       = nullptr;
HWND g_hwndTitle      = nullptr;
HWND g_hwndSubtitle   = nullptr;
HWND g_hwndLbl[8]     = {};
HWND g_hwndRToggle    = nullptr;
HWND g_hwndRHold      = nullptr;
HWND g_hwndApply      = nullptr;
HWND g_hwndHelp       = nullptr;
HWND g_hwndSetTrigger = nullptr;
HWND g_hwndSetClick   = nullptr;
HWND g_hwndInterval   = nullptr;
HWND g_hwndRandom     = nullptr;

static HHOOK g_hookMouseTrigger = nullptr;
static HHOOK g_hookF5           = nullptr;
HFONT  g_fontTitle  = nullptr;
HFONT  g_fontMain   = nullptr;
HFONT  g_fontSmall  = nullptr;
HBRUSH g_hBrushBg   = nullptr;
HBRUSH g_hBrushCard = nullptr;
HBRUSH g_hBrushEdit = nullptr;

bool IsMouseButton(int vk) {
    return vk == VK_LBUTTON || vk == VK_RBUTTON || vk == VK_MBUTTON ||
           vk == VK_XBUTTON1 || vk == VK_XBUTTON2;
}

void GetKeyDisplayName(int vk, wchar_t* buf, int bufLen) {
    if (vk == 0) { wcscpy_s(buf, bufLen, T(L_NOT_SET)); return; }
    switch (vk) {
        case VK_LBUTTON:  wcscpy_s(buf, bufLen, T(L_LBTN));  break;
        case VK_RBUTTON:  wcscpy_s(buf, bufLen, T(L_RBTN));  break;
        case VK_MBUTTON:  wcscpy_s(buf, bufLen, T(L_MBTN));  break;
        case VK_XBUTTON1: wcscpy_s(buf, bufLen, T(L_XBTN1)); break;
        case VK_XBUTTON2: wcscpy_s(buf, bufLen, T(L_XBTN2)); break;
        default: {
            UINT scan = MapVirtualKeyW((UINT)vk, MAPVK_VK_TO_VSC);
            if (scan == 0) swprintf_s(buf, bufLen, L"0x%02X", vk);
            else GetKeyNameTextW((LONG)(scan << 16), buf, bufLen);
        }
    }
}

void performSimulatedInput() {
    int vk = g_click_key_vk.load();
    if (vk == 0) return;

    g_simulating_input = true;
    if (IsMouseButton(vk)) {
        if (vk == g_trigger_key_vk.load()) g_simulated_up_count++;
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        switch (vk) {
            case VK_LBUTTON:
                input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;  SendInput(1, &input, sizeof(INPUT));
                input.mi.dwFlags = MOUSEEVENTF_LEFTUP;    break;
            case VK_RBUTTON:
                input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN; SendInput(1, &input, sizeof(INPUT));
                input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;   break;
            case VK_MBUTTON:
                input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN; SendInput(1, &input, sizeof(INPUT));
                input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;   break;
            case VK_XBUTTON1:
                input.mi.dwFlags = MOUSEEVENTF_XDOWN; input.mi.mouseData = XBUTTON1;
                SendInput(1, &input, sizeof(INPUT));
                input.mi.dwFlags = MOUSEEVENTF_XUP;   input.mi.mouseData = XBUTTON1; break;
            case VK_XBUTTON2:
                input.mi.dwFlags = MOUSEEVENTF_XDOWN; input.mi.mouseData = XBUTTON2;
                SendInput(1, &input, sizeof(INPUT));
                input.mi.dwFlags = MOUSEEVENTF_XUP;   input.mi.mouseData = XBUTTON2; break;
            default: g_simulating_input = false; return;
        }
        SendInput(1, &input, sizeof(INPUT));
    } else {
        g_simulating_input = false;
        INPUT inputs[2] = {0};
        inputs[0].type = inputs[1].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = inputs[1].ki.wVk = (WORD)vk;
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(2, inputs, sizeof(INPUT));
    }
    g_simulating_input = false;
}

int getRandomInterval() {
    int base = g_interval_ms.load(), range = g_random_range.load();
    if (range <= 0) return base;
    thread_local static std::mt19937 gen(std::random_device{}());
    int r = base + std::uniform_int_distribution<>(-range, range)(gen);
    return r > 0 ? r : 1;
}

void clickWorker() {
    while (g_running) {
        if (g_clicking) {
            performSimulatedInput();
            std::this_thread::sleep_for(std::chrono::milliseconds(getRandomInterval()));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

const int MASTER_SWITCH_VK = VK_F5;

LRESULT CALLBACK MouseTriggerHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        int trigger = g_trigger_key_vk.load();
        if (IsMouseButton(trigger)) {
            int btn = 0;
            if      (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP)  btn = VK_LBUTTON;
            else if (wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP)  btn = VK_RBUTTON;
            else if (wParam == WM_MBUTTONDOWN || wParam == WM_MBUTTONUP)  btn = VK_MBUTTON;
            else if (wParam == WM_XBUTTONDOWN || wParam == WM_XBUTTONUP)
                btn = (HIWORD(((MSLLHOOKSTRUCT*)lParam)->mouseData) == 1) ? VK_XBUTTON1 : VK_XBUTTON2;
            if (btn == trigger) {
                bool isDown = (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
                               wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN);
                bool isUp   = !isDown && (wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP ||
                               wParam == WM_MBUTTONUP || wParam == WM_XBUTTONUP);
                if (isDown) {
                    if (g_click_key_vk.load() == trigger && g_simulated_up_count.load() > 0) {
                    } else {
                        g_user_holding_trigger = true;
                    }
                } else if (isUp) {
                    if (g_click_key_vk.load() == trigger && g_simulated_up_count.load() > 0)
                        g_simulated_up_count--;
                    else
                        g_user_holding_trigger = false;
                }
            }
        }
    }
    return CallNextHookEx(g_hookMouseTrigger, nCode, wParam, lParam);
}

LRESULT CALLBACK GlobalF5Hook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        if (p->vkCode == (UINT)MASTER_SWITCH_VK) {
            bool nowOn = !g_master_switch;
            g_master_switch = nowOn;
            if (!nowOn) {
                g_clicking             = false;
                g_user_holding_trigger = false;
                g_simulated_up_count   = 0;
                g_key_was_pressed      = false;
                g_hold_release_count   = 0;
            }
            return 1;
        }
    }
    return CallNextHookEx(g_hookF5, nCode, wParam, lParam);
}

void hotkeyMonitor() {
    while (g_running) {
        if (g_master_switch) {
            int trigger = g_trigger_key_vk.load();
            bool triggerPressed = IsMouseButton(trigger)
                ? g_user_holding_trigger.load()
                : (trigger > 0 && (GetAsyncKeyState(trigger) & 0x8000) != 0);

            if (g_hold_mode) {
                if (IsMouseButton(trigger)) {
                    g_clicking = triggerPressed;
                } else {
                    if (triggerPressed) { g_hold_release_count = 0; g_clicking = true; }
                    else { if (++g_hold_release_count >= 5) g_clicking = false; }
                }
            } else {
                if (triggerPressed && !g_key_was_pressed) { g_clicking = !g_clicking; g_key_was_pressed = true; }
                else if (!triggerPressed) g_key_was_pressed = false;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void UpdateStatusDisplay(HWND) {
    if (!g_hwndStatus || !IsWindow(g_hwndStatus)) return;
    if (!g_master_switch)   SetWindowTextW(g_hwndStatus, T(L_ST_OFF));
    else if (g_clicking)    SetWindowTextW(g_hwndStatus, T(L_ST_RUN));
    else                    SetWindowTextW(g_hwndStatus, T(L_ST_IDLE));
    if (g_hwnd) InvalidateRect(g_hwnd, nullptr, FALSE);
}

void UpdateClickKeyDisplay() {
    if (!g_hwndClickKey || !IsWindow(g_hwndClickKey)) return;
    wchar_t buf[64]; GetKeyDisplayName(g_click_key_vk.load(), buf, 64);
    SetWindowTextW(g_hwndClickKey, buf);
}

void UpdateTriggerKeyDisplay() {
    if (!g_hwndTriggerKey || !IsWindow(g_hwndTriggerKey)) return;
    wchar_t buf[64]; GetKeyDisplayName(g_trigger_key_vk.load(), buf, 64);
    SetWindowTextW(g_hwndTriggerKey, buf);
}

void SyncFromControls(HWND hwnd) {
    wchar_t buf[32];
    if (GetDlgItemTextW(hwnd, IDC_INTERVAL, buf, 32)) g_interval_ms  = std::max(1, _wtoi(buf));
    if (GetDlgItemTextW(hwnd, IDC_RANDOM,   buf, 32)) g_random_range = std::max(0, _wtoi(buf));
    g_hold_mode = (IsDlgButtonChecked(hwnd, IDC_MODE_HOLD) == BST_CHECKED);
}

void ApplyLanguage(HWND hwnd) {
    SetWindowTextW(hwnd,             L"Auto Clicker");
    SetWindowTextW(g_hwndTitle,      T(L_TITLE));
    SetWindowTextW(g_hwndSubtitle,   T(L_SUBTITLE));
    SetWindowTextW(g_hwndLbl[0],     T(L_INTERVAL));
    SetWindowTextW(g_hwndLbl[1],     T(L_RANDOM));
    SetWindowTextW(g_hwndLbl[2],     T(L_TRIGGERKEY));
    SetWindowTextW(g_hwndLbl[3],     T(L_CLICKKEY));
    SetWindowTextW(g_hwndLbl[4],     T(L_MODE));
    SetWindowTextW(g_hwndRToggle,    T(L_TOGGLE));
    SetWindowTextW(g_hwndRHold,      T(L_HOLD));
    SetWindowTextW(g_hwndApply,      T(L_APPLY));
    SetWindowTextW(g_hwndHelp,       T(L_HELP));
    SetWindowTextW(g_hwndSetTrigger, T(L_SET));
    SetWindowTextW(g_hwndSetClick,   T(L_SET));
    SetWindowTextW(g_hwndLang,       g_lang_zh ? L"EN" : L"\u4E2D");
    UpdateClickKeyDisplay();
    UpdateTriggerKeyDisplay();
    UpdateStatusDisplay(hwnd);
    InvalidateRect(hwnd, nullptr, TRUE);
}

static int  g_captured_vk    = 0;
static int  g_capture_target = 0;
static HHOOK g_hookKb        = nullptr;
static HHOOK g_hookMs        = nullptr;
static HWND  g_captureHwnd   = nullptr;

LRESULT CALLBACK LowLevelKbHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        if (p->vkCode == VK_ESCAPE) g_captured_vk = -1;
        else if (p->vkCode != (UINT)MASTER_SWITCH_VK) g_captured_vk = (int)p->vkCode;
        else return CallNextHookEx(g_hookKb, nCode, wParam, lParam);
        PostMessageW(g_captureHwnd, WM_USER + 100, 0, 0);
    }
    return CallNextHookEx(g_hookKb, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMsHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;
        int vk = 0;
        if      (wParam == WM_LBUTTONDOWN) vk = VK_LBUTTON;
        else if (wParam == WM_RBUTTONDOWN) vk = VK_RBUTTON;
        else if (wParam == WM_MBUTTONDOWN) vk = VK_MBUTTON;
        else if (wParam == WM_XBUTTONDOWN) vk = (HIWORD(p->mouseData) == 1) ? VK_XBUTTON1 : VK_XBUTTON2;
        if (vk) { g_captured_vk = vk; PostMessageW(g_captureHwnd, WM_USER + 100, 0, 0); }
    }
    return CallNextHookEx(g_hookMs, nCode, wParam, lParam);
}

void ShowCaptureDialog(HWND hwnd, int target, HWND btnHwnd) {
    g_captured_vk = 0; g_capture_target = target; g_captureHwnd = hwnd;
    SetWindowTextW(btnHwnd, T(L_PRESS_KEY));
    g_hookKb = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKbHook, GetModuleHandle(nullptr), 0);
    g_hookMs = SetWindowsHookExW(WH_MOUSE_LL,    LowLevelMsHook, GetModuleHandle(nullptr), 0);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) && msg.message != WM_USER + 100) {
        TranslateMessage(&msg); DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(g_hookKb); UnhookWindowsHookEx(g_hookMs);
    g_hookKb = g_hookMs = nullptr; g_captureHwnd = nullptr;
    if (g_captured_vk > 0) {
        if (target == 0) { g_click_key_vk  = g_captured_vk; UpdateClickKeyDisplay(); }
        else             { g_trigger_key_vk = g_captured_vk; UpdateTriggerKeyDisplay(); }
    }
    SetWindowTextW(btnHwnd, T(L_SET));
}

void CALLBACK StatusTimerProc(HWND hwnd, UINT, UINT_PTR, DWORD) { UpdateStatusDisplay(hwnd); }

void PaintBackground(HDC hdc, HWND hwnd) {
    RECT rc; GetClientRect(hwnd, &rc);

    HBRUSH hBg = CreateSolidBrush(C_BG);
    FillRect(hdc, &rc, hBg);
    DeleteObject(hBg);

    HPEN hPen1 = CreatePen(PS_SOLID, 1, C_ACCENT);
    HPEN hPen2 = CreatePen(PS_SOLID, 1, C_BORDER);
    HPEN hOld  = (HPEN)SelectObject(hdc, hPen1);
    MoveToEx(hdc, 0, 58, nullptr); LineTo(hdc, rc.right, 58);
    SelectObject(hdc, hPen2);
    MoveToEx(hdc, 0, 59, nullptr); LineTo(hdc, rc.right, 59);
    SelectObject(hdc, hOld);
    DeleteObject(hPen1); DeleteObject(hPen2);

    RECT statusRc = {0, rc.bottom - 62, rc.right, rc.bottom};
    HBRUSH hStatusBg = CreateSolidBrush(RGB(0x16, 0x16, 0x26));
    FillRect(hdc, &statusRc, hStatusBg);
    DeleteObject(hStatusBg);

    HPEN hAccent = CreatePen(PS_SOLID, 2, C_ACCENT);
    hOld = (HPEN)SelectObject(hdc, hAccent);
    MoveToEx(hdc, 0, rc.bottom - 62, nullptr); LineTo(hdc, rc.right, rc.bottom - 62);
    SelectObject(hdc, hOld);
    DeleteObject(hAccent);

    const int rh = 50;
    const int rowStarts[] = {70, 70+rh, 70+rh*2, 70+rh*3, 70+rh*4};
    const int rowEnds[]   = {70+rh, 70+rh*2, 70+rh*3, 70+rh*4, 70+rh*4+rh+42};
    for (int i = 0; i < 5; i++) {
        if (i % 2 == 0) {
            RECT row = {0, rowStarts[i], rc.right, rowEnds[i]};
            HBRUSH hRow = CreateSolidBrush(RGB(0x22, 0x22, 0x34));
            FillRect(hdc, &row, hRow);
            DeleteObject(hRow);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        g_hwnd = hwnd;
        INITCOMMONCONTROLSEX icex = {sizeof(icex), ICC_STANDARD_CLASSES};
        InitCommonControlsEx(&icex);

        g_fontTitle = CreateFontW(24, 0, 0, 0, FW_BOLD,   FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Microsoft YaHei UI");
        g_fontMain  = CreateFontW(19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Microsoft YaHei UI");
        g_fontSmall = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Microsoft YaHei UI");

        const int M  = 24;
        const int LW = 110;
        const int VW = 138;
        const int BW = 66;
        const int RH = 50;
        const int C2 = M + LW + 10;
        const int C3 = C2 + VW + 8;
        const int W  = 460;
        int y = 14;

        g_hwndTitle = CreateWindowExW(0, L"STATIC", T(L_TITLE),
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            M, y, 200, 30, hwnd, nullptr, nullptr, nullptr);
        SendMessageW(g_hwndTitle, WM_SETFONT, (WPARAM)g_fontTitle, TRUE);

        g_hwndSubtitle = CreateWindowExW(0, L"STATIC", T(L_SUBTITLE),
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            M + 200, y + 8, 170, 20, hwnd, nullptr, nullptr, nullptr);
        SendMessageW(g_hwndSubtitle, WM_SETFONT, (WPARAM)g_fontSmall, TRUE);

        g_hwndLang = CreateWindowExW(0, L"BUTTON", L"EN",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
            W - 52, y + 2, 44, 24, hwnd, (HMENU)IDC_LANG, nullptr, nullptr);
        SendMessageW(g_hwndLang, WM_SETFONT, (WPARAM)g_fontSmall, TRUE);

        y = 70;

        g_hwndLbl[0] = CreateWindowExW(0, L"STATIC", T(L_INTERVAL),
            WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
            M, y, LW, RH, hwnd, nullptr, nullptr, nullptr);
        g_hwndInterval = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"50",
            WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_TABSTOP,
            C2, y + 10, 70, 28, hwnd, (HMENU)IDC_INTERVAL, nullptr, nullptr);
        CreateWindowExW(0, L"STATIC", L"ms",
            WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
            C2 + 76, y + 10, 28, 28, hwnd, nullptr, nullptr, nullptr);

        y += RH;
        g_hwndLbl[1] = CreateWindowExW(0, L"STATIC", T(L_RANDOM),
            WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
            M, y, LW, RH, hwnd, nullptr, nullptr, nullptr);
        g_hwndRandom = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"10",
            WS_CHILD | WS_VISIBLE | ES_NUMBER | WS_TABSTOP,
            C2, y + 10, 70, 28, hwnd, (HMENU)IDC_RANDOM, nullptr, nullptr);
        CreateWindowExW(0, L"STATIC", L"\u00B1ms",
            WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
            C2 + 76, y + 10, 40, 28, hwnd, nullptr, nullptr, nullptr);

        y += RH;
        g_hwndLbl[2] = CreateWindowExW(0, L"STATIC", T(L_TRIGGERKEY),
            WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
            M, y, LW, RH, hwnd, nullptr, nullptr, nullptr);
        g_hwndTriggerKey = CreateWindowExW(WS_EX_CLIENTEDGE, L"STATIC", T(L_LBTN),
            WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
            C2, y + 9, VW, 30, hwnd, (HMENU)IDC_TRIGGER_KEY, nullptr, nullptr);
        g_hwndSetTrigger = CreateWindowExW(0, L"BUTTON", T(L_SET),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
            C3, y + 9, BW, 30, hwnd, (HMENU)IDC_SET_TRIGGER, nullptr, nullptr);

        y += RH;
        g_hwndLbl[3] = CreateWindowExW(0, L"STATIC", T(L_CLICKKEY),
            WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
            M, y, LW, RH, hwnd, nullptr, nullptr, nullptr);
        g_hwndClickKey = CreateWindowExW(WS_EX_CLIENTEDGE, L"STATIC", T(L_LBTN),
            WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
            C2, y + 9, VW, 30, hwnd, (HMENU)IDC_CLICK_KEY, nullptr, nullptr);
        g_hwndSetClick = CreateWindowExW(0, L"BUTTON", T(L_SET),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
            C3, y + 9, BW, 30, hwnd, (HMENU)IDC_SET_KEY, nullptr, nullptr);

        y += RH;
        g_hwndLbl[4] = CreateWindowExW(0, L"STATIC", T(L_MODE),
            WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
            M, y, LW, RH * 2, hwnd, nullptr, nullptr, nullptr);
        g_hwndRToggle = CreateWindowExW(0, L"BUTTON", T(L_TOGGLE),
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP,
            C2, y + 6, 256, 22, hwnd, (HMENU)IDC_MODE_TOGGLE, nullptr, nullptr);
        g_hwndRHold = CreateWindowExW(0, L"BUTTON", T(L_HOLD),
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_TABSTOP,
            C2, y + 32, 256, 22, hwnd, (HMENU)IDC_MODE_HOLD, nullptr, nullptr);

        y += RH + 42;
        g_hwndApply = CreateWindowExW(0, L"BUTTON", T(L_APPLY),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
            M, y, 116, 34, hwnd, (HMENU)IDC_APPLY, nullptr, nullptr);
        g_hwndHelp = CreateWindowExW(0, L"BUTTON", T(L_HELP),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
            M + 124, y, 90, 34, hwnd, (HMENU)IDC_ABOUT, nullptr, nullptr);

        RECT rcWin; GetClientRect(hwnd, &rcWin);
        g_hwndStatus = CreateWindowExW(0, L"STATIC", T(L_ST_OFF),
            WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE,
            M, rcWin.bottom - 56, rcWin.right - M * 2, 44, hwnd, (HMENU)IDC_STATUS, nullptr, nullptr);

        CheckDlgButton(hwnd, IDC_MODE_TOGGLE, BST_CHECKED);

        for (HWND child = GetWindow(hwnd, GW_CHILD); child; child = GetWindow(child, GW_HWNDNEXT))
            SendMessageW(child, WM_SETFONT, (WPARAM)g_fontMain, TRUE);
        SendMessageW(g_hwndTitle,    WM_SETFONT, (WPARAM)g_fontTitle, TRUE);
        SendMessageW(g_hwndSubtitle, WM_SETFONT, (WPARAM)g_fontSmall, TRUE);
        SendMessageW(g_hwndLang,     WM_SETFONT, (WPARAM)g_fontSmall, TRUE);

        g_hBrushBg   = CreateSolidBrush(C_BG);
        g_hBrushCard = CreateSolidBrush(C_CARD);
        g_hBrushEdit = CreateSolidBrush(C_EDITBG);

        UpdateClickKeyDisplay();
        UpdateTriggerKeyDisplay();
        SetTimer(hwnd, 1, 200, StatusTimerProc);
        g_hookF5           = SetWindowsHookExW(WH_KEYBOARD_LL, GlobalF5Hook,       GetModuleHandle(nullptr), 0);
        g_hookMouseTrigger = SetWindowsHookExW(WH_MOUSE_LL,    MouseTriggerHook,   GetModuleHandle(nullptr), 0);
        return 0;
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORBTN: {
        HDC  hdc  = (HDC)wParam;
        HWND ctrl = (HWND)lParam;
        SetBkMode(hdc, TRANSPARENT);
        if (ctrl == g_hwndStatus) {
            if (!g_master_switch) SetTextColor(hdc, C_TEXTDIM);
            else if (g_clicking)  SetTextColor(hdc, C_STATUSRUN);
            else                  SetTextColor(hdc, C_STATUSOK);
            return (LRESULT)(g_hBrushBg ? g_hBrushBg : GetStockObject(BLACK_BRUSH));
        }
        if (ctrl == g_hwndSubtitle) {
            SetTextColor(hdc, C_TEXTDIM);
            return (LRESULT)(g_hBrushBg ? g_hBrushBg : GetStockObject(BLACK_BRUSH));
        }
        SetTextColor(hdc, C_TEXT);
        SetBkColor(hdc, C_BG);
        wchar_t cls[16]; GetClassNameW(ctrl, cls, 16);
        if (wcsstr(cls, L"Edit")) {
            SetBkColor(hdc, C_EDITBG);
            return (LRESULT)(g_hBrushEdit ? g_hBrushEdit : g_hBrushBg);
        }
        return (LRESULT)(g_hBrushBg ? g_hBrushBg : GetStockObject(BLACK_BRUSH));
    }

    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORSCROLLBAR:
        SetBkColor((HDC)wParam, C_BG);
        return (LRESULT)(g_hBrushBg ? g_hBrushBg : GetStockObject(BLACK_BRUSH));

    case WM_ERASEBKGND:
        PaintBackground((HDC)wParam, hwnd);
        return 1;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_SET_KEY:     ShowCaptureDialog(hwnd, 0, g_hwndSetClick);   break;
        case IDC_SET_TRIGGER: ShowCaptureDialog(hwnd, 1, g_hwndSetTrigger); break;
        case IDC_APPLY:
            SyncFromControls(hwnd);
            UpdateStatusDisplay(hwnd);
            UpdateClickKeyDisplay();
            UpdateTriggerKeyDisplay();
            MessageBeep(MB_OK);
            break;
        case IDC_ABOUT:
            MessageBoxW(hwnd, g_lang_zh ? HELP_ZH : HELP_EN, T(L_HELP), MB_OK | MB_ICONINFORMATION);
            break;
        case IDC_LANG:
            g_lang_zh = !g_lang_zh;
            ApplyLanguage(hwnd);
            break;
        }
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        if (g_hookF5)           { UnhookWindowsHookEx(g_hookF5);           g_hookF5           = nullptr; }
        if (g_hookMouseTrigger) { UnhookWindowsHookEx(g_hookMouseTrigger); g_hookMouseTrigger = nullptr; }
        g_running = false;
        if (g_fontMain)   DeleteObject(g_fontMain);
        if (g_fontTitle)  DeleteObject(g_fontTitle);
        if (g_fontSmall)  DeleteObject(g_fontSmall);
        if (g_hBrushBg)   DeleteObject(g_hBrushBg);
        if (g_hBrushCard) DeleteObject(g_hBrushCard);
        if (g_hBrushEdit) DeleteObject(g_hBrushEdit);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    std::thread clickThread(clickWorker);
    std::thread hotkeyThread(hotkeyMonitor);
    clickThread.detach();
    hotkeyThread.detach();

    WNDCLASSEXW wc    = {0};
    wc.cbSize         = sizeof(wc);
    wc.style          = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc    = WndProc;
    wc.hInstance      = hInstance;
    wc.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName  = L"AutoClickerGUI";
    if (!RegisterClassExW(&wc)) return 1;

    const int W = 480, H = 560;
    int x = (GetSystemMetrics(SM_CXSCREEN) - W) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - H) / 2;

    HWND hwnd = CreateWindowExW(0, L"AutoClickerGUI", L"Auto Clicker",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        x, y, W, H, nullptr, nullptr, hInstance, nullptr);
    if (!hwnd) return 1;

    #ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
    #define DWMWA_USE_IMMERSIVE_DARK_MODE 20
    #endif
    BOOL dark = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    ShowWindow(hwnd, nCmdShow);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
