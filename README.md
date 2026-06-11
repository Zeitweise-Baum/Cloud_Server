CloudServer_Proton – Eigenständiger Webserver für NAS-Umgebungen

Ein eigenständiger, multithreaded HTTP-Webserver in C++, entwickelt für den Einsatz in privaten Netzwerkumgebungen (Home-Lab, NAS). Das Projekt bietet eine funktionale Plattform zum Speichern, Verwalten und Teilen von Dateien mit integriertem Benutzer-Management, Admin-Dashboard und Sicherheitsmechanismen.

Dieses Projekt dient primär als Lernressource und Praxisbeispiel für angehende Entwickler, Studierende und Quereinsteiger im Bereich Systemprogrammierung, Netzwerkkommunikation und Server-Architektur.

Wichtiger Hinweis zur Lizenz

Dieses Projekt wird unter einer Nicht-Kommerziellen Open-Source-Lizenz bereitgestellt.

    Zulässige Nutzung: Private Anwendung, Studium, Ausbildung, Umschulung, Quereinstieg in IT-Berufe, Abschlussarbeiten und nicht-kommerzielle Forschungsprojekte. Der Code darf als Basis für eigene Lernprojekte verwendet werden.
    Verbotene Nutzung: Jegliche kommerzielle Nutzung ist strengstens untersagt. Dies schließt den Verkauf der Software, das Anbieten als bezahlter Hosting-Dienst (SaaS), die Integration in gewinnorientierte Produkte sowie die Nutzung in Unternehmensumgebungen zur Kosteneinsparung ein.

Die vollständigen Bedingungen finden Sie in der LICENSE-Datei.
Hauptfunktionen
Benutzer-Management

    Registrierung und Login von Benutzern.
    Rollenbasierte Zugriffskontrolle (Normaler User vs. Administrator).
    Passwortänderung durch den Benutzer.
    Verwaltung von Warnungen und Sperren (Bans) durch Administratoren.
    Persistente Speicherung der Benutzerdaten in einer textbasierten Datenbank (users.txt).

Dateisystem & Storage

    Individuelle Speicherbereiche pro Benutzer.
    Upload-Funktion für Binärdateien.
    Download-Funktion für hochgeladene Dateien.
    Interne "Inbox" zum Versenden von Dateien zwischen Benutzern innerhalb des Systems.
    Verwaltung der Ordnerstruktur durch Folder_Manager.

Administration

    Live-Monitoring aller aktiven Sitzungen und Benutzerstatus.
    Zentrale Übersicht aller gespeicherten Dateien über alle Benutzer hinweg.
    Sofortige Maßnahmen: Warnung, Sperre (Ban), Freigabe (Unban) oder Löschung von Benutzerkonten inkl. zugehöriger Daten.
    Erstellung neuer Benutzerkonten durch Administratoren.

Sicherheit & Netzwerk

    Schutz vor Directory-Traversal-Angriffen.
    Grundlegende SQL-Injection-Erkennung (Input-Validierung).
    Captcha-Integration zur Verhinderung automatisierter Login-Versuche.
    Detailliertes Logging von Systemereignissen, Sicherheitswarnungen und Benutzeraktionen.
    Multithreading-Architektur zur gleichzeitigen Bedienung mehrerer Clients.

Technische Anforderungen

Um diesen Server zu kompilieren und auszuführen, werden folgende Voraussetzungen benötigt:

    Betriebssystem: Linux (Ubuntu, Debian, Alpine etc.) oder Windows mit WSL2 (Windows Subsystem for Linux).
    Compiler: GCC oder Clang mit Unterstützung für C++17 Standard.
    Bibliotheken: Access zu <sys/socket.h>, <netinet/in.h>, <filesystem> und Pthreads.
    Portzugriff: Zugriff auf Port 8080 (oder Konfiguration im Quellcode).

Installation und Build-Prozess
1. Repository klonen

git clone https://github.com/<DEIN_USERNAME>/CloudServer_Proton.git
cd CloudServer_Proton

2. Kompilieren

Erstellen Sie die ausführbare Datei mit dem C++ Compiler. Da mehrere Quelldateien genutzt werden, wird meist nur die main.cpp als Entry-Point benötigt, sofern diese andere .cpp Dateien korrekt includet (wie im aktuellen Projektstand implementiert).

g++ -std=c++17 -pthread main.cpp -o CloudServer

Falls Fehler auftreten, stellen Sie sicher, dass alle Header-Dateien (*.h) und Implementierungsdateien (*.cpp) im selben Verzeichnis liegen.
3. Starten des Servers

./CloudServer

Der Server startet standardmäßig und bindet sich an 0.0.0.0:8080.
4. Zugriff

Öffnen Sie einen Webbrowser und navigieren Sie zu:

http://localhost:8080

oder

http://<IHR_LOCAL_IP>:8080

Standard-Zugangsdaten

Nach dem ersten Start werden die Benutzer aus der Datei users.txt geladen. Für Testzwecke sind drei Accounts vordefiniert, mit denen Sie das System sofort ausprobieren können:
Rolle	Benutzername	Passwort	Beschreibung
Administrator	admin	1234	Volle Zugriffsrechte auf Dashboard, User-Management und Datei-Überwachung.
Test-Nutzer 1	Bine Beispiel	1234	Normaler Benutzeraccount zum Testen von Uploads, Downloads und Profil-Änderungen.
Test-Nutzer 2	Max Mustermann	1234	Zweiter normaler Benutzeraccount, ideal zum Testen der Inbox-Funktion (Datei-Sharing) zwischen zwei Nutzern.

Hinweis: Diese Passwörter sind standardmäßig als Klartext gespeichert. Ändern Sie diese umgehend nach dem ersten erfolgreichen Login über das respective Benutzerprofil oder lassen Sie sie beim Experimentieren bestehen.
Projektstruktur

Die Architektur ist modular aufgebaut, um Lesbarkeit und Wartbarkeit zu gewährleisten:

    main.cpp: Initialisierung des Sockets, Listen-Schleife und Thread-Management.
    Network.cpp: HTTP-Request-Parsing, Routing-Logik und Antwortgenerierung.
    User_Manager.cpp: Verwaltung der Benutzerdatenstruktur und Persistenz (Lese/Schreib users.txt).
    Security.cpp: IP-Ermittlung, Input-Validierung und Angriffserkennung.
    Logger.cpp: Konsolen-Ausgabe und Schreiben von Log-Dateien (server_system.log, server_security.log).
    Folder_Manager.cpp: Verwaltung des Verzeichnisbaums (assets/).
    Design_*.cpp: Generierung der HTML-Oberflächen für Login, Dashboard (User/Admin), Erfolg/Nachrichtenseiten.
    Advanced_Features.cpp: Zusatzfunktionen wie Datei-Sharing, Statistiken und Inbox-Logik.
    Captcha.cpp: Generierung von Validierungs-Codes.
    assets/: Laufzeit-Verzeichnis für hochgeladene Dateien und Benutzerordner.

Bekannte Einschränkungen & Sicherheitshinweise

Da dieses Projekt als Bildungs- und Übungswerkzeug konzipiert ist, gelten folgende Einschränkungen:

    Passwort-Sicherheit: Passwörter werden aktuell als Klartext in users.txt gespeichert. Für den produktiven Einsatz muss dies zwingend durch Hashing (z.B. bcrypt, Argon2) ersetzt werden.
    Netzwerk-Protokoll: Der Server kommuniziert nur über HTTP. Es gibt keine TLS/SSL-Verschlüsselung. Die Nutzung ist daher nur im geschützten lokalen Netzwerk (LAN) empfohlen.
    Upload-Handling: Der Datei-Upload beruht auf einem vereinfachten Parsing-Verfahren. Sehr große Dateien oder komplexe MIME-Typen könnten zu Instabilität führen.
    Multithreading: Die aktuelle Implementierung nutzt std::thread::detach. In Produktionssystemen sollte ein Thread-Pool mit gezieltem Joining bevorzugt werden.

Beitragende

Pull Requests sind willkommen, insbesondere für:

    Implementierung von TLS/SSL Support.
    Ersetzung der Klartext-Passwörter durch sichere Hashing-Verfahren.
    Optimierung des Thread-Managements (Thread Pool).
    Erweiterung der UI-Funktionalität.
    Behebung von gefundenen Bugs.

Bitte beachten Sie beim Einreichen von Änderungen, dass diese weiterhin unter der Nicht-Kommerziellen Lizenz bleiben müssen.
Kontakt

Bei Fragen zur technischen Umsetzung, zur Lizenznutzung im Bildungskontext oder bei Interesse an einer separaten kommerziellen Lizenzvereinbarung, wenden Sie sich bitte an:

Email: [julianbeckmann.eu@proton.me]
