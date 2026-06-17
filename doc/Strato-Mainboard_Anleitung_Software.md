# Strato-Mainboard Anleitung Software

## SD-Karte vorbereiten

- "Raspberry Pi Imager" herunterladen
- Leere SD-Karte einstecken mit mindestens 8GB
- Raspberry Pi OS (other) > “Raspberry Pi OS Lite (32-bit)” auswählen
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
- Konsole öffnen (Windows Powershell oder Linux Terminal) und SSH-Verbindung aufbauen:
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

## Netzwerk konfigurieren

- Anzeigen der Netzwerk Konfigurationen
  ```bash
  nmcli connection show
  ```
- Speichern der aktuellen Netzwerk Konfiguration
  ```bash
  sudo nmcli connection clone "netplan..." "Name"
  ```
- Anzeigen der verfügbaren Netzwerke
  ```bash
  nmcli device wifi list
  ```
- Anlegen einer neuen Verbindung, falls erforderlich
  ```bash
  sudo nmcli connection add type wifi ifname wlan0 con-name "Name" ssid "ssid"
  ```
  ```bash
  sudo nmcli connection modify "Name" wifi-sec.key-mgmt wpa-psk
  ```
  ```bash
  sudo nmcli connection modify "Name" wifi-sec.psk "Passwort"
  ```

## Installation von benötigten Paketen

- Aktualisierung des Installers: apt

  ```bash
  sudo apt update
  ```
- Werkzeug für den I²C-Bus: i2c-tools

  ```bash
  sudo apt install i2c-tools
  ```
  ```bash
  i2cdetect -y 1
  ```

  sollte alle I²C-Sensoradressen anzeigen
- Versionsverwaltung: git

  ```bash
  sudo apt install git
  ```

- Build-System Generator: cmake

  ```bash
  sudo apt install cmake cmake-curses-gui
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
  sudo apt install qt6-base-dev
  ```

- Capn Proto

  ```bash
  sudo apt install capnproto libcapnp-dev
  ```

- Mosquitto
  ```bash
  sudo apt install libmosquitto-dev
  ```

  Sonstige Tools installieren:

  ```bash
  sudo apt install libglib2.0-dev pkg-config libsecret-1-dev libboost-all-dev libconfig++-dev 
  ```

- Raspberry Pi neu starten

  ```bash
  sudo reboot now
  ```

## Installation der Software

### MuonPi

- Repository herunterladen

  ```bash
  cd
  ```
  ```bash
  git clone -b dev https://github.com/MuonPi/muondetector_v2.git
  ```

- Build Verzeichnis erstellen

  ```bash
  cd muondetector
  ```
  ```bash
  mkdir build
  ```

- Kompilieren vorbereiten

  ```bash
  cd build
  ```
  ```bash
  ccmake ..
  ```

- Auswählen, dass die GUI nicht mit erstellt werden soll

  ```bash
  MUONDETECTOR_BUILD_GUI            OFF
  MUONDETECTOR_BUILD_TCP_DEBUG_C    OFF
  MUONDETECTOR_BUILD_TCP_DEBUG_S    OFF
  MUONDETECTOR_BUILD_TESTS          OFF
  ```

- Bestätigen mit c und dann g

- Kompilieren

  ```bash
  make
  ```

  dies dauert auf dem Pi Zero sehr lange

- Installieren

  ```bash
  sudo apt install ./output/packages/muondetector-daemon_2.1.0libgpiod_armhf.deb
  ```

- Service starten

  ```bash
  sudo systemctl daemon-reload
  ```
  ```bash
  sudo systemctl start muondetector-daemon.service
  ```

### Strato-Software

- Repository herunterladen

  ```bash
  cd
  ```
  ```bash
  git clone -b mp_connection_v2 https://github.com/MuonPi/strato-software.git
  ```

- Build Verzeichnis erstellen

  ```bash
  cd ~/strato-software
  ```
  ```bash
  mkdir build
  ```

- Kompilieren

  ```bash
  cd build
  ```
  ```bash
  cmake ..
  ```
  ```bash
  make -j2
  ```

- Installieren

  ```bash
  sudo cmake --install .
  ```

- Service starten

  ```bash
  sudo systemctl daemon-reload
  ```
  ```bash
  sudo systemctl start strato-software.service
  ```

### Tailscale

- Tailscale herunterladen

  ```bash
  curl -fsSL https://tailscale.com/install.sh | sh
  ```
  ```bash
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
  