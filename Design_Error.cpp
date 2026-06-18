#ifndef DESIGN_ERROR_H
#define DESIGN_ERROR_H

#include <string>

namespace UI {
    
    std::string getErrorPage(const std::string& message, const std::string& errorCode = "500", const std::string& title = "Vorgang Abgebrochen") {
        return R"html(
            <!DOCTYPE html>
            <html lang="de">
            <head>
                <meta charset="UTF-8">
                <meta http-equiv="refresh" content="5;url=/">
                <title>Fehler )html" + errorCode + R"html(</title>
                <style>
                    body { 
                        margin: 0; padding: 0; 
                        background-color: #0a0f1d; 
                        font-family: 'Courier New', monospace; 
                        color: #ff4a4a; 
                        display: flex; justify-content: center; align-items: center; 
                        height: 100vh;
                    }
                    .error-box { 
                        border: 2px solid #ff4a4a; 
                        padding: 40px; 
                        border-radius: 15px; 
                        background: rgba(20, 5, 5, 0.9); 
                        box-shadow: 0 0 30px #ff4a4a; 
                        text-align: center;
                        max-width: 500px;
                        animation: shake 0.5s ease-in-out;
                    }
                    @keyframes shake {
                        0%, 100% { transform: translateX(0); }
                        10%, 30%, 50%, 70%, 90% { transform: translateX(-5px); }
                        20%, 40%, 60%, 80% { transform: translateX(5px); }
                    }
                    h1 { 
                        color: #ff4a4a; 
                        font-size: 64px; 
                        margin-bottom: 10px; 
                        text-shadow: 0 0 10px #ff4a4a;
                        margin-top: 0;
                    }
                    h2 { 
                        color: #fff; 
                        font-size: 24px; 
                        margin-bottom: 20px; 
                        letter-spacing: 1px;
                    }
                    p { 
                        color: #ffaaaa; 
                        margin-bottom: 30px; 
                        line-height: 1.6; 
                        font-size: 16px;
                    }
                    .error-code {
                        font-size: 14px;
                        color: #888;
                        margin-bottom: 20px;
                        border-top: 1px solid #ff4a4a;
                        padding-top: 10px;
                    }
                    .btn { 
                        display: inline-block;
                        margin-top: 20px;
                        background: transparent; 
                        border: 1px solid #ff4a4a; 
                        color: #ff4a4a; 
                        padding: 12px 25px; 
                        border-radius: 5px; 
                        cursor: pointer; 
                        font-weight: bold; 
                        text-decoration: none;
                        transition: all 0.3s;
                    }
                    .btn:hover { 
                        background: #ff4a4a; 
                        color: #0a0f1d; 
                        box-shadow: 0 0 15px #ff4a4a;
                    }
                </style>
            </head>
            <body>
                <div class="error-box">
                    <h1>⚠️</h1>
                    <h2>Vorgang Abgebrochen</h2>
                    <p>)html" + message + R"html(</p>
                    <div class="error-code">Fehlercode: )html" + errorCode + R"html(</div>
                    <p style="font-size: 14px; color: #888;">Du wirst in 5 Sekunden zur Startseite zurückkehren...</p>
                    <a href="/" class="btn">Jetzt weiter</a>
                </div>
            </body>
            </html>
        )html";
    }
}

#endif
