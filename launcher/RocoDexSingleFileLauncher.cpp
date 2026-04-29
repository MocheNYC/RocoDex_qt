#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>

#include <filesystem>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#define IDR_PAYLOAD 101

namespace fs = std::filesystem;

namespace {

constexpr wchar_t kRuntimeDirName[] = L"RocoDexQt_runtime";
constexpr wchar_t kAppDirName[] = L"app";
constexpr wchar_t kPayloadFileName[] = L"payload.zip";
constexpr wchar_t kMarkerFileName[] = L"payload.marker";
constexpr wchar_t kAppExeName[] = L"RocoDexQt.exe";

struct Payload {
    const void *data = nullptr;
    DWORD size = 0;
};

std::wstring lastErrorMessage(const std::wstring &prefix) {
    const DWORD err = GetLastError();
    wchar_t *buffer = nullptr;
    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&buffer),
        0,
        nullptr);

    std::wstring message = prefix;
    if (buffer) {
        message += L"\n\n";
        message += buffer;
        LocalFree(buffer);
    }
    return message;
}

fs::path moduleDirectory() {
    std::vector<wchar_t> buffer(MAX_PATH);
    DWORD size = 0;
    while (true) {
        size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (size == 0) {
            throw std::runtime_error("GetModuleFileNameW failed");
        }
        if (size < buffer.size() - 1) {
            break;
        }
        buffer.resize(buffer.size() * 2);
    }

    fs::path modulePath(std::wstring(buffer.data(), size));
    return modulePath.parent_path();
}

Payload loadPayload() {
    HRSRC resource = FindResourceW(nullptr, MAKEINTRESOURCEW(IDR_PAYLOAD), RT_RCDATA);
    if (!resource) {
        throw std::runtime_error("Embedded payload resource was not found");
    }

    HGLOBAL handle = LoadResource(nullptr, resource);
    if (!handle) {
        throw std::runtime_error("Embedded payload resource could not be loaded");
    }

    Payload payload;
    payload.data = LockResource(handle);
    payload.size = SizeofResource(nullptr, resource);
    if (!payload.data || payload.size == 0) {
        throw std::runtime_error("Embedded payload resource is empty");
    }
    return payload;
}

std::wstring markerFor(const Payload &payload) {
    return L"payload-size=" + std::to_wstring(payload.size) + L"\n";
}

bool fileExists(const fs::path &path) {
    std::error_code ec;
    return fs::exists(path, ec) && fs::is_regular_file(path, ec);
}

std::wstring readTextFile(const fs::path &path) {
    std::wifstream stream(path);
    std::wstring content;
    std::getline(stream, content, L'\0');
    return content;
}

void writeTextFile(const fs::path &path, const std::wstring &content) {
    std::wofstream stream(path, std::ios::trunc);
    if (!stream) {
        throw std::runtime_error("Unable to write marker file");
    }
    stream << content;
}

void writePayloadToDisk(const Payload &payload, const fs::path &payloadPath) {
    HANDLE file = CreateFileW(
        payloadPath.c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Unable to create payload file");
    }

    const auto *cursor = static_cast<const BYTE *>(payload.data);
    DWORD remaining = payload.size;
    while (remaining > 0) {
        const DWORD chunk = remaining > 4 * 1024 * 1024 ? 4 * 1024 * 1024 : remaining;
        DWORD written = 0;
        if (!WriteFile(file, cursor, chunk, &written, nullptr) || written == 0) {
            CloseHandle(file);
            throw std::runtime_error("Unable to write payload file");
        }
        cursor += written;
        remaining -= written;
    }

    CloseHandle(file);
}

std::wstring powershellQuote(const fs::path &path) {
    std::wstring value = path.wstring();
    std::wstring escaped;
    escaped.reserve(value.size() + 2);
    escaped.push_back(L'\'');
    for (const wchar_t ch : value) {
        if (ch == L'\'') {
            escaped += L"''";
        } else {
            escaped.push_back(ch);
        }
    }
    escaped.push_back(L'\'');
    return escaped;
}

std::wstring windowsDirectory() {
    std::vector<wchar_t> buffer(MAX_PATH);
    const UINT size = GetWindowsDirectoryW(buffer.data(), static_cast<UINT>(buffer.size()));
    if (size == 0 || size >= buffer.size()) {
        return L"C:\\Windows";
    }
    return std::wstring(buffer.data(), size);
}

class SplashWindow {
public:
    SplashWindow() {
        WNDCLASSW wc{};
        wc.lpfnWndProc = &SplashWindow::windowProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"RocoDexQtSingleFileSplash";
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        RegisterClassW(&wc);

        const int width = 520;
        const int height = 292;
        const int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
        const int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

        hwnd_ = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            wc.lpszClassName,
            L"RocoDex Qt",
            WS_POPUP,
            x,
            y,
            width,
            height,
            nullptr,
            nullptr,
            wc.hInstance,
            this);

        if (!hwnd_) {
            throw std::runtime_error("Unable to create splash window");
        }

        ShowWindow(hwnd_, SW_SHOW);
        UpdateWindow(hwnd_);
        SetTimer(hwnd_, 1, 45, nullptr);
    }

    ~SplashWindow() {
        if (hwnd_) {
            KillTimer(hwnd_, 1);
            DestroyWindow(hwnd_);
        }
    }

    void setStatus(const std::wstring &status) {
        status_ = status;
        InvalidateRect(hwnd_, nullptr, FALSE);
    }

    void pumpMessages() {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    HWND hwnd() const { return hwnd_; }

private:
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        SplashWindow *self = nullptr;
        if (message == WM_NCCREATE) {
            auto *create = reinterpret_cast<CREATESTRUCTW *>(lParam);
            self = static_cast<SplashWindow *>(create->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        } else {
            self = reinterpret_cast<SplashWindow *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        if (self) {
            return self->handleMessage(hwnd, message, wParam, lParam);
        }
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    LRESULT handleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
        case WM_TIMER:
            frame_ = (frame_ + 1) % 240;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        case WM_PAINT:
            paint(hwnd);
            return 0;
        default:
            return DefWindowProcW(hwnd, message, wParam, lParam);
        }
    }

    void paint(HWND hwnd) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rc;
        GetClientRect(hwnd, &rc);

        HBRUSH bg = CreateSolidBrush(RGB(248, 250, 252));
        FillRect(hdc, &rc, bg);
        DeleteObject(bg);

        RECT accent{0, 0, rc.right, 6};
        HBRUSH accentBrush = CreateSolidBrush(RGB(41, 121, 255));
        FillRect(hdc, &accent, accentBrush);
        DeleteObject(accentBrush);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(15, 23, 42));

        HFONT titleFont = CreateFontW(34, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                      CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
        HFONT oldFont = reinterpret_cast<HFONT>(SelectObject(hdc, titleFont));
        RECT titleRect{54, 60, rc.right - 54, 104};
        DrawTextW(hdc, L"RocoDex Qt", -1, &titleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, oldFont);
        DeleteObject(titleFont);

        HFONT textFont = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                     CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Microsoft YaHei UI");
        oldFont = reinterpret_cast<HFONT>(SelectObject(hdc, textFont));
        SetTextColor(hdc, RGB(71, 85, 105));
        RECT statusRect{56, 120, rc.right - 56, 154};
        DrawTextW(hdc, status_.c_str(), -1, &statusRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        const wchar_t *subText = L"首次启动会准备本地运行环境，之后将直接进入应用。";
        SetTextColor(hdc, RGB(100, 116, 139));
        RECT subRect{56, 154, rc.right - 56, 188};
        DrawTextW(hdc, subText, -1, &subRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, oldFont);
        DeleteObject(textFont);

        const int barX = 56;
        const int barY = 218;
        const int barW = rc.right - 112;
        const int barH = 10;
        RECT barBg{barX, barY, barX + barW, barY + barH};
        HBRUSH barBgBrush = CreateSolidBrush(RGB(226, 232, 240));
        FillRect(hdc, &barBg, barBgBrush);
        DeleteObject(barBgBrush);

        const int pulseW = barW / 3;
        const int travel = barW + pulseW;
        const int pulseX = barX + ((frame_ * 7) % travel) - pulseW;
        RECT pulse{pulseX, barY, pulseX + pulseW, barY + barH};
        if (pulse.left < barX) {
            pulse.left = barX;
        }
        if (pulse.right > barX + barW) {
            pulse.right = barX + barW;
        }
        if (pulse.right > pulse.left) {
            HBRUSH pulseBrush = CreateSolidBrush(RGB(41, 121, 255));
            FillRect(hdc, &pulse, pulseBrush);
            DeleteObject(pulseBrush);
        }

        EndPaint(hwnd, &ps);
    }

    HWND hwnd_ = nullptr;
    int frame_ = 0;
    std::wstring status_ = L"正在准备启动...";
};

bool runHiddenAndWait(const std::wstring &command, SplashWindow &splash) {
    const std::wstring psPath = windowsDirectory() + L"\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
    std::wstring cmdLine = L"\"" + psPath + L"\" " + command;
    std::vector<wchar_t> mutableCmd(cmdLine.begin(), cmdLine.end());
    mutableCmd.push_back(L'\0');

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi{};

    if (!CreateProcessW(
            psPath.c_str(),
            mutableCmd.data(),
            nullptr,
            nullptr,
            FALSE,
            CREATE_NO_WINDOW,
            nullptr,
            nullptr,
            &si,
            &pi)) {
        throw std::runtime_error("Unable to start extraction process");
    }

    bool ok = false;
    while (true) {
        const DWORD result = MsgWaitForMultipleObjects(1, &pi.hProcess, FALSE, 50, QS_ALLINPUT);
        splash.pumpMessages();
        if (result == WAIT_OBJECT_0) {
            DWORD exitCode = 1;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            ok = exitCode == 0;
            break;
        }
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return ok;
}

bool launchApp(const fs::path &appExe, const fs::path &appDir) {
    std::wstring cmdLine = L"\"" + appExe.wstring() + L"\"";
    std::vector<wchar_t> mutableCmd(cmdLine.begin(), cmdLine.end());
    mutableCmd.push_back(L'\0');

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    const BOOL created = CreateProcessW(
        appExe.c_str(),
        mutableCmd.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        appDir.c_str(),
        &si,
        &pi);

    if (created) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
    return created == TRUE;
}

int runLauncher() {
    Payload payload = loadPayload();
    SplashWindow splash;
    splash.pumpMessages();

    const fs::path baseDir = moduleDirectory();
    const fs::path runtimeDir = baseDir / kRuntimeDirName;
    const fs::path appDir = runtimeDir / kAppDirName;
    const fs::path appExe = appDir / kAppExeName;
    const fs::path payloadZip = runtimeDir / kPayloadFileName;
    const fs::path markerFile = runtimeDir / kMarkerFileName;
    const std::wstring expectedMarker = markerFor(payload);

    std::error_code ec;
    fs::create_directories(runtimeDir, ec);
    if (ec) {
        throw std::runtime_error("Unable to create runtime directory");
    }

    const bool ready = fileExists(appExe) && fileExists(markerFile) && readTextFile(markerFile) == expectedMarker;
    if (!ready) {
        splash.setStatus(L"正在解包本地运行环境...");
        splash.pumpMessages();
        writePayloadToDisk(payload, payloadZip);

        splash.setStatus(L"正在整理应用文件...");
        const std::wstring command =
            L"-NoProfile -ExecutionPolicy Bypass -Command \"$ErrorActionPreference='Stop'; "
            L"if (Test-Path -LiteralPath " +
            powershellQuote(appDir) +
            L") { Remove-Item -LiteralPath " +
            powershellQuote(appDir) +
            L" -Recurse -Force }; "
            L"New-Item -ItemType Directory -Force -Path " +
            powershellQuote(appDir) +
            L" | Out-Null; "
            L"Expand-Archive -LiteralPath " +
            powershellQuote(payloadZip) +
            L" -DestinationPath " +
            powershellQuote(appDir) +
            L" -Force\"";

        if (!runHiddenAndWait(command, splash)) {
            throw std::runtime_error("Extraction failed");
        }
        writeTextFile(markerFile, expectedMarker);
    }
    fs::remove(payloadZip, ec);

    splash.setStatus(L"正在启动 RocoDex Qt...");
    splash.pumpMessages();
    Sleep(180);

    if (!launchApp(appExe, appDir)) {
        MessageBoxW(splash.hwnd(), lastErrorMessage(L"RocoDex Qt 启动失败。").c_str(), L"RocoDex Qt", MB_ICONERROR | MB_OK);
        return 1;
    }

    return 0;
}

} // namespace

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
    try {
        return runLauncher();
    } catch (const std::exception &error) {
        const std::wstring message = L"RocoDex Qt 启动器准备失败。\n\n" + std::wstring(error.what(), error.what() + strlen(error.what()));
        MessageBoxW(nullptr, message.c_str(), L"RocoDex Qt", MB_ICONERROR | MB_OK);
        return 1;
    }
}
