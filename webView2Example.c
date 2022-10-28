#include <windows.h>

// comes from Microsoft.Web.WebView2 NuGet package
#include "WebView2.h"

#define APPLICATION_NAME TEXT("Sample WebView2")

ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler *envHandler;
ICoreWebView2CreateCoreWebView2ControllerCompletedHandler *completedHandler;

HWND hWnd = NULL;
ICoreWebView2Controller *webviewController = NULL;
ICoreWebView2 *webviewWindow = NULL;
BOOL bEnvCreated = FALSE;

ULONG HandlerRefCount = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
    {
        if (webviewController != NULL)
        {
            RECT bounds;
            GetClientRect(hWnd, &bounds);
            webviewController->lpVtbl->put_Bounds(
                webviewController,
                bounds);
        };
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}

int s_HandlerRefCount = 0;
ULONG s_HandlerAddRef()
{
    return ++s_HandlerRefCount;
}
ULONG s_HandlerRelease()
{
    --s_HandlerRefCount;
    if (s_HandlerRefCount == 0)
    {
        /*if (completedHandler)
        {
            free(completedHandler->lpVtbl);
            free(completedHandler);
        }
        if (envHandler)
        {
            free(envHandler->lpVtbl);
            free(envHandler);
        }*/
    }
    return s_HandlerRefCount;
}
HRESULT s_HandlerQueryInterface(
    IUnknown *This,
    IID *riid,
    void **ppvObject)
{
    *ppvObject = This;
    HandlerAddRef(This);
    return S_OK;
}
HRESULT s_HandlerInvoke(
    ICoreWebView2 *webview, void *sender, ICoreWebView2WebMessageReceivedEventArgs *args)
{
    wchar_t *message = malloc(sizeof(wchar_t) * 3);
    args->lpVtbl->TryGetWebMessageAsString(args, &message);
    return S_OK;
}

ULONG HandlerAddRef()
{
    return ++HandlerRefCount;
}
ULONG HandlerRelease()
{
    --HandlerRefCount;
    if (HandlerRefCount == 0)
    {
        if (completedHandler)
        {
            free(completedHandler->lpVtbl);
            free(completedHandler);
        }
        if (envHandler)
        {
            free(envHandler->lpVtbl);
            free(envHandler);
        }
    }
    return HandlerRefCount;
}
HRESULT HandlerQueryInterface(
    IUnknown *This,
    IID *riid,
    void **ppvObject)
{
    *ppvObject = This;
    HandlerAddRef(This);
    return S_OK;
}
HRESULT HandlerInvoke(
    IUnknown *This,
    HRESULT errorCode,
    void *arg)
{
    if (!bEnvCreated)
    {
        bEnvCreated = TRUE;
        char ch;
        completedHandler = malloc(sizeof(ICoreWebView2CreateCoreWebView2ControllerCompletedHandler));
        completedHandler->lpVtbl = malloc(sizeof(ICoreWebView2CreateCoreWebView2ControllerCompletedHandlerVtbl));

        completedHandler->lpVtbl->AddRef = HandlerAddRef;
        completedHandler->lpVtbl->Release = HandlerRelease;
        completedHandler->lpVtbl->QueryInterface = HandlerQueryInterface;
        completedHandler->lpVtbl->Invoke = HandlerInvoke;

        ICoreWebView2Environment *env = arg;
        env->lpVtbl->CreateCoreWebView2Controller(
            env,
            hWnd,
            completedHandler);
    }
    else
    {
        ICoreWebView2Controller *controller = arg;

        if (controller != NULL)
        {
            webviewController = controller;
            webviewController->lpVtbl->get_CoreWebView2(
                webviewController,
                &webviewWindow);
            webviewController->lpVtbl->AddRef(webviewController);
        }

        ICoreWebView2Settings *Settings;
        webviewWindow->lpVtbl->get_Settings(webviewWindow, &Settings);
        Settings->lpVtbl->put_IsScriptEnabled(Settings, TRUE);
        Settings->lpVtbl->put_AreDefaultScriptDialogsEnabled(Settings, TRUE);
        Settings->lpVtbl->put_IsWebMessageEnabled(Settings, TRUE);
        Settings->lpVtbl->put_AreDevToolsEnabled(Settings, TRUE);
        Settings->lpVtbl->put_AreDefaultContextMenusEnabled(Settings, TRUE);
        Settings->lpVtbl->put_IsStatusBarEnabled(Settings, TRUE);

        RECT bounds;
        GetClientRect(hWnd, &bounds);

        bounds.left = 200;
        bounds.top = 200;

        webviewController->lpVtbl->put_Bounds(
            webviewController,
            bounds);

        webviewWindow->lpVtbl->Navigate(
            webviewWindow,
            L"https://google.com/");

        ICoreWebView2ExecuteScriptCompletedHandler *a = malloc(sizeof(ICoreWebView2ExecuteScriptCompletedHandler));
        a->lpVtbl = malloc(sizeof(ICoreWebView2ExecuteScriptCompletedHandlerVtbl));

        a->lpVtbl->AddRef = s_HandlerAddRef;
        a->lpVtbl->Release = s_HandlerRelease;
        a->lpVtbl->QueryInterface = s_HandlerQueryInterface;
        a->lpVtbl->Invoke = s_HandlerInvoke;

        EventRegistrationToken token;

        webviewWindow->lpVtbl->add_WebMessageReceived(webviewWindow, a, &token);

        webviewWindow->lpVtbl->AddScriptToExecuteOnDocumentCreated(webviewWindow,
                                                                   L"window.chrome.webview.addEventListener(\'message\', event => console.log(event.data));"
                                                                   L"console.log(\'foo\');"
                                                                   L"window.chrome.webview.postMessage(\'bar'\);",
                                                                   NULL);
    }

    return S_OK;
}

int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,
            int nCmdShow)
{

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "DesktopApp";
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    RegisterClassEx(&wcex);

    hWnd = CreateWindow(
        "DesktopApp",
        APPLICATION_NAME,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1200, 900,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hWnd,
               nCmdShow);
    UpdateWindow(hWnd);

    envHandler = malloc(sizeof(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler));
    envHandler->lpVtbl = malloc(sizeof(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandlerVtbl));

    envHandler->lpVtbl->AddRef = HandlerAddRef;
    envHandler->lpVtbl->Release = HandlerRelease;
    envHandler->lpVtbl->QueryInterface = HandlerQueryInterface;
    envHandler->lpVtbl->Invoke = HandlerInvoke;

    CreateCoreWebView2EnvironmentWithOptions(NULL, NULL, NULL, envHandler);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
