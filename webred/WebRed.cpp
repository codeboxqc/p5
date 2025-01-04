// WebRed.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WebRed.h"

#define MAX_LOADSTRING 100

#include <windows.h>
#include <wrl.h>
#include <wil/com.h>
#include <webview2.h>
#include <fstream>
#include <sstream>
#include <string>

#include <thread>
#include <sapi.h>
#pragma comment(lib, "sapi.lib")

#include "Resource.h"

 

//************************
//NuGet\Install-Package Microsoft.Web.WebView2 -Version 1.0.2739.15
//***************************

using namespace Microsoft::WRL;

 

void InitWebView2(HWND hwnd);
void StopWebView();
void RunP5Sketch(ICoreWebView2* sender);
std::string LoadResourceString(HINSTANCE hInstance, int resourceId);

HINSTANCE h2;
std::string p5_js;

// Load a resource string (like p5.js) from the executable
std::string LoadResourceString(HINSTANCE hInstance, int resourceId) {
    // Find the resource in the executable
    HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(resourceId), RT_RCDATA);
    if (!hRes) return "";

    // Load the resource into memory
    HGLOBAL hData = LoadResource(hInstance, hRes);
    if (!hData) return "";

    // Get the size of the resource
    DWORD dataSize = SizeofResource(hInstance, hRes);
    if (dataSize == 0) return "";

    // Access the resource data
    void* pData = LockResource(hData);
    if (!pData) return "";

    // Copy the resource data into a std::string
    return std::string(static_cast<char*>(pData), dataSize);
}



// Global WebView2 controller and webview instances
wil::com_ptr<ICoreWebView2Controller> webViewController;
wil::com_ptr<ICoreWebView2> webView;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_SIZE:
        if (webViewController) {
            RECT bounds;
            GetClientRect(hwnd, &bounds);
            webViewController->put_Bounds(bounds);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Load p5.js from resources

 
void InitWebView2(HWND hwnd) {
    CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>([hwnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
            env->CreateCoreWebView2Controller(hwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>([hwnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                if (controller != nullptr) {
                    webViewController = controller;
                    webViewController->get_CoreWebView2(&webView);

                    RECT bounds;
                    GetClientRect(hwnd, &bounds);
                    webViewController->put_Bounds(bounds);

                    // Load a blank HTML page initially
                    webView->NavigateToString(L"<html><head><title>p5.js Example</title></head><body></body></html>");

                    EventRegistrationToken token;
                    webView->add_NavigationCompleted(Callback<ICoreWebView2NavigationCompletedEventHandler>([hwnd](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
                        BOOL isSuccess = FALSE;
                        args->get_IsSuccess(&isSuccess);

                        if (isSuccess == TRUE) {
                            // Load p5.js from resources
                           // HINSTANCE hInstance = GetModuleHandle(nullptr);
                           // std::string p5_js = LoadResourceString(hInstance, IDR_P5JS);

                            // Inject p5.js into the page
                            sender->ExecuteScript(std::wstring(p5_js.begin(), p5_js.end()).c_str(), nullptr);
                            //Sleep(100);
                            // After p5.js is injected, run the p5 sketch
                            RunP5Sketch(sender);
                        }
                        return S_OK;
                        }).Get(), &token);
                }
                return S_OK;
                }).Get());
            return S_OK;
            }).Get());
}
 
 


//////////////////////////////////////////////////////
// Check if p5.js is available
 
std::string S3 = R"(
        if (typeof p5 !== 'undefined') {
            let sketch = function(p) {
                p.setup = function() {
                    p.createCanvas(window.innerWidth, window.innerHeight);
                    p.background(0);
                };
                p.draw = function() {
                    p.fill(255, 0, 0);
                    p.ellipse(p.mouseX, p.mouseY, 50, 50);  // Draw a red ellipse
                };
            };
            new p5(sketch);
        }
    )";
    

 
std::string S2 = R"(
    if (typeof p5 !== 'undefined') {
        let shapes = [];
        let colors = ['#F54A17', '#F5C204', '#1C1A6C', '#F5E5DC', '#0D609C'];

        let sketch = function(p) {
            p.setup = function() {
                p.createCanvas(window.innerWidth, window.innerHeight);
                document.body.style.pointerEvents = 'none';
                p.rectMode(p.CENTER);
                let c = 50;
                let w = p.width / c;
                for (let i = 0; i < c; i++) {
                    for (let j = 0; j < c; j++) {
                        let x = i * w + w / 2;
                        let y = j * w + w / 2;
                        let colNum1 = Math.floor(p.random(colors.length));
                        let colNum2 = Math.floor(p.random(colors.length));
                        shapes.push({ x: x, y: y, w: w, n1: colNum1, n2: colNum2 });
                    }
                }
                p.frameRate(15);
            };

            p.draw = function() {
                p.background(0);
                p.noStroke();
                for (let i of shapes) {
                    let dst = p.dist(p.width / 2, p.height / 2, i.x, i.y);
                    let scaleFactor = Math.sqrt(Math.pow(p.width / 2, 2) + Math.pow(p.height / 2, 2));
                    let pValue = Math.pow(p.map(dst, 0, scaleFactor, 0, 1.4), 2);
                    if (pValue < p.random()) {
                        p.fill(colors[i.n1]);
                        p.square(i.x, i.y, i.w);
                    }
                    if (pValue < p.random()) {
                        p.fill(colors[i.n2]);
                        p.circle(i.x, i.y, i.w * 0.5);
                    }
                }
            };
        };

        new p5(sketch);
    }
)";
 
  
 

std::string S1 = R"(

const config = { width: 1536, height: 864 };

let ctx;
let agents = [];
let colors = ['#FE4D03', '#FCAD8A', '#003DCC', '#08AC7E', '#DED9DF', '#f71735', '#f654a9'];

function setup() {
  createCanvas(config.width, config.height); // Use the config object for canvas size
  rectMode(CENTER);
  ctx = drawingContext;
  
  let count = 13;
  let cellSize = width / count;
  for (let i = 0; i < count; i++) {
    for (let j = 0; j < count; j++) {
      let x = cellSize * i + cellSize / 2;
      let y = cellSize * j + cellSize / 2;
      let t = -int(dist(width / 2, height / 2, x, y) / 10);
      if ((i + j) % 2 == 0) agents.push(new Shape(x, y, cellSize * 1.2, t));
    }
  }
}

function draw() {
  background('#061B18');
  for (let i of agents) {
    i.run();
  }
}

function easeInOutQuint(x) {
  return x < 0.5 ? 16 * x * x * x * x * x : 1 - Math.pow(-2 * x + 2, 5) / 2;
}

class Shape {
  constructor(x, y, d, t) {
    this.x = x;
    this.y = y;
    this.d = d;
    this.currentD = d;
    this.t1 = 50;
    this.t2 = this.t1 + 100;
    this.t3 = this.t2 + 30;
    this.t4 = this.t3 + 50;
    this.petalSize0 = this.d * 0.38;
    this.petalSize1 = 0;
    this.petalPosition0 = this.d * 0.5 - (this.petalSize0 / 2);
    this.petalPosition1 = this.d * 0.5;
    this.init();
    this.clr1 = this.clr2 = random(colors);
    while (this.clr1 == this.clr2) {
      this.clr2 = random(colors);
    }
    this.t = t;
    this.minD = this.d * 0.35;
  }

  show() {
    push();
    translate(this.x, this.y);
    scale(1, this.flip);
    noStroke();
    fill(this.clr1);
    for (let i = 0; i < 12; i++) {
      rotate(TAU / 12);
      ellipse(this.petalPosition, 0, this.petalSize, this.petalSize * 0.4);
    }
    fill(this.clr2);
    circle(0, 0, this.currentD);
    pop();
  }

  move() {
    this.t++;
    if (0 < this.t && this.t < this.t1) {
      let n = norm(this.t, 0, this.t1 - 1);
      this.flip = lerp(1, -1, easeInOutQuint(n));
      if (this.flip <= 0) {
        this.currentD = this.minD;
        if (this.toggle) {
          this.clr1 = this.clr2;
          while (this.clr1 == this.clr2) {
            this.clr2 = random(colors);
          }
          this.toggle = false;
        }
      }
    } else if (this.t1 < this.t && this.t < this.t2) {
      let n = norm(this.t, this.t1, this.t2 - 1);
      this.petalSize = lerp(this.petalSize0, this.petalSize1, easeInOutQuint(n));
      this.petalPosition = lerp(this.petalPosition0, this.petalPosition1, easeInOutQuint(n));
    } else if (this.t3 < this.t && this.t < this.t4) {
      let n = norm(this.t, this.t3, this.t4 - 1);
      this.currentD = lerp(this.minD, this.d, easeInOutQuint(n));
    }
    if (this.t4 < this.t) {
      this.init();
    }
  }

  run() {
    this.show();
    this.move();
  }

  init() {
    this.t = -20;
    this.flip = 1;
    this.petalSize = this.petalSize0;
    this.petalPosition = this.petalPosition0;
    this.toggle = true;
  }
}


new p5();
)";




void RunP5Sketch(ICoreWebView2* sender) {

    std::string checkP5LoadedScript = S1;
   // std::string checkP5LoadedScript = S2;
   //  std::string checkP5LoadedScript = S3;

  

    // Execute script to check if p5.js is loaded and start the sketch
    sender->ExecuteScript(std::wstring(checkP5LoadedScript.begin(), checkP5LoadedScript.end()).c_str(), nullptr);
}



/////////////////////////////////////////////////////////////////
// 
// 
// 
// 
// 
// 
// 
// 
// 
// Stop Part: Cleanup, stop the WebView, and remove resources
void StopWebView() {
    if (webViewController) {
        // Close the WebView2 controller and clean up
        webViewController->Close();
        webViewController = nullptr;
        webView = nullptr;
    }
    // Other cleanup code if needed
}



int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Read";


   





    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

   HWND hwnd = CreateWindowEx(
    0,                              // Optional window styles.
    CLASS_NAME,                     // Window class
    L"Read",                        // Window text (optional, can be empty)
    WS_POPUP,                        // Full-screen window style (no borders, title bar, etc.)
    0, 0,                            // Position at top-left corner (0,0)
    GetSystemMetrics(SM_CXSCREEN),  // Full screen width (screen width)
    GetSystemMetrics(SM_CYSCREEN),  // Full screen height (screen height)
    NULL,                           // Parent window
    NULL,                           // Menu
    hInstance,                      // Instance handle
    NULL                            // Additional application data
);

    if (hwnd == NULL) {
        return 0;
    }

    // Load p5.js from resources
    h2 = GetModuleHandle(nullptr);
    p5_js = LoadResourceString(h2, IDR_P5JS);

    // Check if resource loaded successfully
    if (p5_js.empty()) {
        MessageBox(NULL, L"Failed to load p5.js resource.", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }


    ShowWindow(hwnd, nCmdShow);

    // Initialize WebView2 in the window
    InitWebView2(hwnd);
     

   
    

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

     StopWebView();
    return 0;
}





 