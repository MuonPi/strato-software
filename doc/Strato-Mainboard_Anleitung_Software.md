# Strato-Mainboard Anleitung Software

## SD-Karte vorbereiten

- Raspberry Pi Imager herunterladen
- Leere SD-Karte einstecken mit mindestens 8GB
- Raspberry Pi OS (other) > “Raspberry Pi OS (Legacy, 32-bit) Lite” auswählen
- SD-Karte auswählen
- Einstellungen > SSH aktivieren > Benutzername und Passwort setzen
- Einstellungen > Wifi einrichten > SSID und Passwort eines Hotspot (nur 2,4GHz) setzen
- Einstellungen > Wifi einrichten > Hostname festlegen
- Schreiben

## Raspberry Pi Inbetriebnahme

- Fertige SD-Karte in den Raspberry Pi stecken
- Raspberry Pi kann auf Strato-Mainboard gesteckt werden
- Spannungsversorgung einstecken
- Warten bis der Raspberry Pi sich im Hotspot einloggt
- Konsole öffnen (Windows Powershell oder Linux Terminal) und SSH-Verbundung aufbauen:

  ```bash
  ssh Benutzername@Hostname.local
  ```

- Falls die Fingerprint-Abfrage erscheint, mit yes beantworten
- Einstellungen ändern:

  ```bash
  sudo raspi-config
  ```

- Interface Options > SPI, I2C, Remote GPIO aktivieren
- Interface Options > Serial Port > login shell über serial port deaktivieren > serial port hardware aktivieren

## Installation von benötigten Paketen

- Aktualisierung des Installers: apt

  ```bash
  sudo apt update
  ```

- Werkzeug für den I²C-Bus: i2c-tools

  ```bash
  sudo apt install i2c-tools
  i2c-detect -y 1
  ```

  sollte alle I²C-Sensoradressen anzeigen
- Versionsverwaltung: git

  ```bash
  sudo apt install git
  ```

- Build-System Generator: cmake

  ```bash
  sudo apt install cmake
  ```

- Compiler installieren: gcc

  ```bash
  sudo apt install build-essential
  ```

- Library installieren: gpiod

  ```bash
  sudo apt install libgpiod-dev
  ```

- Library installieren: Qt6

  ```bash
  sudo apt install qtbase5-dev qtchooser qt6-qmake qtbase5-dev-tools pyqt6-dev libqt6serialport5-dev libqt6svg5-dev libqwt-qt6-dev qtdeclarative6-dev
  ```

  Sonstige Tools installieren:

  ```bash
  libcrypto++-dev libcrypto++-doc libcrypto++-utils lftp libmosquitto-dev libconfig++-dev file  
  ```

- Raspberry Pi neu starten

  ```bash
  sudo reboot now
  ```

## Installation der Software

### MuonPi

- Repository herunterladen

  ```bash
  git clone -b high-altitude-flight-rebase https://github.com/MuonPi/muondetector.git
  ```

- Build Verzeichnis erstellen

  ```bash
  cd muondetector
  mkdir build
  ```

- Kompilieren

  ```bash
  cd build
  cmake ..
  make package
  ```

  dies dauert auf dem Pi Zero sehr lange
- Installieren

  ```bash
  sudo apt install ./output/packages/muondetector-daemon_2.1.0libgpiod_armhf.deb
  ```

- Service starten

  ```bash
  sudo systemctl daemon-reload
  sudo systemctl start muondetector-daemon.service
  ```

### Strato-Software

- Repository herunterladen

  ```bash
  git clone https://github.com/MuonPi/strato-software.git
  ```

- Build Verzeichnis erstellen

  ```bash
  cd strato-software
  mkdir build
  ```

- Kompilieren

  ```bash
  cd build
  cmake ..
  make
  ```

- Installieren

  ```bash
  sudo cmake --install .
  ```

- Service starten

  ```bash
  sudo systemctl daemon-reload
  sudo systemctl start strato-software.service
  ```

### Tailscale

- Tailscale herunterladen

  ```bash
  curl -fsSL https://tailscale.com/install.sh | sh
  sudo tailscale up
  ```

- Angegebenen Link im Browser eingeben und mit Tailscale Account verbinden
- Um ein anderes Gerät über Tailscale mit dem Raspberry Pi zu verbinden, muss dort auch Tailscale installiert sein
- Tailscale kann dann eine ssh Verbindung aufbauen

  ```bash
  ssh Benutzername@Hostname
  ```

### Automatisches Speichern von System-Logs

- Das Verzeichnis für System-Logs durch JournalD erstellen

  ```bash
  sudo systemd-tmpfiles --create --prefix /var/log/journal
  ```

- Einschalten, dass System-Logs persistent auf der SD-Karte gespeichert werden

  ```bash
  sudo nano /etc/systemd/journald.conf
  ```

- Storage-Zeile aktivieren und Status ersetzen durch

  ```
  Storage=persistent
  ```

- Logging-Service JournalD neu starten

  ```bash
  sudo systemctl restart systemd-journald
  ```

### Systemuhrzeit automatisch über WLAN festlegen

- Prüfen, ob automatische Systemuhrzeit bereits festgelegt ist, falls ja können die weiteren Schritte übersprungen werden

  ```bash
  timedatectl
  ```

  muss sein:

  ```bash
  System clock synchronized: yes
  NTP service: active
  ```

- Falls nicht, einschalten

  ```bash
  sudo timedatectl set-ntp true
  ```

- Uhrzeit-Service neu starten

  ```bash
  sudo systemctl restart systemd-timesyncd
  ```
  