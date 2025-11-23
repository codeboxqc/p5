✅ Simple README (Easy Text)
WebRed – Fullscreen WebView2 + p5.js Renderer from exe window

 if you compile you stiil need .js scrip in the zip to test
 

WebRed is a lightweight Windows application that opens a fullscreen borderless window and displays interactive p5.js animations using Microsoft WebView2.

⭐ What this program does

Creates a fullscreen window with no borders (WS_POPUP)

Initializes WebView2 inside the window

Loads p5.js directly from the EXE resources (no internet required)

Injects custom p5.js sketch code

Runs animations at native resolution

Supports switching between different sketches

Shuts down cleanly when the window closes

📌 Features

Portable – only one EXE, p5.js is embedded as binary resource

No browser needed – uses WebView2 runtime

Fast GPU-accelerated rendering through Edge engine

Custom sketches (change between S1, S2, S3 in code)

Perfect for visual displays, kiosks, or art installations

🔧 How it works (simple)

Loads p5.js from IDR_P5JS resource

Creates a blank HTML document

Injects p5.js into the WebView

Injects your sketch JavaScript code (S1, S2, S3)

Renders fullscreen animation

📁 Files Included

WebRed.cpp – main application

WebRed.h

resource.h

p5.js stored inside .rc file as RT_RCDATA

▶ Running

Just run WebRed.exe
It will immediately display your fullscreen p5.js animation.

🛠 Requirements

Windows 10/11

WebView2 Runtime installed

Visual Studio 2022

Microsoft.Web.WebView2 NuGet package
