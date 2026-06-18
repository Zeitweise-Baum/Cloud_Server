#ifndef DESIGN_SUCCESS_H
#define DESIGN_SUCCESS_H

#include <string>

namespace UI {
    // ✅ Nur noch 2 Parameter: message und title (KEIN type mehr!)
    std::string getSuccessPage(const std::string& message, const std::string& title = "Operation erfolgreich") {
        // ✅ IMMER GRÜN - Farbe fest definiert, kann nicht geändert werden!
        std::string color = "#4ecca3";
        
        return R"html(
            <!DOCTYPE html>
            <html lang="de">
            <head>
                <meta charset="UTF-8">
                <meta http-equiv="refresh" content="3;url=/">
                <title>Erfolg</title>
                <style>
                    body { 
                        margin: 0; padding: 0; 
                        background-color: #0a0f1d; 
                        font-family: 'Courier New', monospace; 
                        color: )html" + color + R"html(; 
                        display: flex; justify-content: center; align-items: center; 
                        height: 100vh;
                    }
                    .success-box { 
                        border: 2px solid )html" + color + R"html(; 
                        padding: 40px; 
                        border-radius: 15px; 
                        background: rgba(10, 20, 40, 0.9); 
                        box-shadow: 0 0 30px )html" + color + R"html(; 
                        text-align: center;
                        max-width: 500px;
                        animation: fadeIn 0.5s ease-in-out;
                    }
                    @keyframes fadeIn {
                        from { opacity: 0; transform: translateY(-20px); }
                        to { opacity: 1; transform: translateY(0); }
                    }
                    h1 { 
                        color: )html" + color + R"html(; 
                        font-size: 48px; 
                        margin-bottom: 20px; 
                        text-shadow: 0 0 10px )html" + color + R"html(;
                    }
                    h2 { 
                        color: #fff; 
                        font-size: 24px; 
                        margin-bottom: 30px; 
                        letter-spacing: 1px;
                    }
                    p { 
                        color: #ccc; 
                        margin-bottom: 30px; 
                        line-height: 1.6; 
                        font-size: 16px;
                    }
                    .loader {
                        width: 100%;
                        height: 4px;
                        background: #333;
                        margin-top: 20px;
                        border-radius: 2px;
                        overflow: hidden;
                    }
                    .loader-bar {
                        height: 100%;
                        background: )html" + color + R"html(;
                        width: 0%;
                        animation: load 3s linear forwards;
                    }
                    @keyframes load {
                        to { width: 100%; }
                    }
                    .btn { 
                        display: inline-block;
                        margin-top: 20px;
                        background: transparent; 
                        border: 1px solid )html" + color + R"html(; 
                        color: )html" + color + R"html(; 
                        padding: 12px 25px; 
                        border-radius: 5px; 
                        cursor: pointer; 
                        font-weight: bold; 
                        text-decoration: none;
                        transition: all 0.3s;
                    }
                    .btn:hover { 
                        background: )html" + color + R"html(; 
                        color: #0a0f1d; 
                        box-shadow: 0 0 15px )html" + color + R"html(;
                    }
                </style>
            </head>
            <body>
                <div class="success-box">
                    <h1>✅</h1>
                    <h2>)html" + title + R"html(</h2>
                    <p>)html" + message + R"html(</p>
                    <p style="font-size: 14px; color: #888;">Du wirst in 3 Sekunden zur Startseite zurückkehren...</p>
                    <div class="loader">
                        <div class="loader-bar"></div>
                    </div>
                    <a href="/" class="btn">Jetzt weiter</a>
                </div>
            </body>
            </html>
        )html";
    }
}

#endif
