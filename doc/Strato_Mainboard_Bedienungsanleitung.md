
# Strato-Mainboard Betriebsanleitung


## Vorbereitung

- Schalter auf OFF stellen, Akku abziehen
- Detektor mit dem MuonPi durch UFL-Kabel verbinden. Richtung beachten: BIAS auf dem MuonPi mit BIAS auf dem Detektor, IN auf dem MuonPi mit OUT auf dem Detektor
- Es empfiehlt sich, alle UFL-Stecker am MuonPi und Detekor mit Heißkleber festzukleben, damit sie sich nicht lösen können
- MuonPi auf das Strato-Mainboard stecken, darauf den Raspberry Pi
- MuonPi und Raspberry Pi auf dem Strato Mainboard festschrauben. Es reicht an zwei gegenüberliegenden Ecken.
- Alle verwendeten Sensoren einstecken
- GPS-Antenne einstecken und verschrauben. Sie befindet sich direkt am MuonPi Board.
- LoRa-Antenne einstecken und verschrauben. Sie befindet sich auf dem Strato-Mainboard mit der Kennzeichnung "ANT_LORA"
- Akku einstecken

## Inbetriebnahme

- Hotspot einschalten
- Schalter auf ON stellen
- Am PC mit Powershell oder Terminal mit dem Raspberry Pi verbinden:
  ```bash
  ssh Benutzername@Hostname.local
  ```
- Danach Passwort eingeben
- Beide Programme (muondetector und strato-software) sollten jetzt automatisch laufen
- Prüfen, ob die jeweilige Software läuft:
  ```bash
  systemctl status muondetector-daemon.service
  ```
  ```bash
  systemctl status strato-software.service
  ```
- Bei beiden Softwares muss stehen: active (running)
- Um die jeweilige Software zu stoppen:
  ```bash
  sudo systemctl stop muondetector-daemon.service
  ```
  ```bash
  sudo systemctl stop strato-software.service
  ```
- Um die jeweilige Software zu starten:
  ```bash
  sudo systemctl restart muondetector-daemon.service
  ```
  ```bash
  sudo systemctl restart strato-software.service
  ```
- Um sich Statusmeldungen der jeweiligen Software anzuschauen:

  ```bash
  journalctl -f -u muondetector-daemon.service
  ```
  ```bash
  journalctl -f -u strato-software.service
  ```
- Um zu prüfen, ob der Muondetector richtig läuft, muss die GUI auf dem PC installiert werden unter https://github.com/MuonPi/muondetector/releases/tag/v3.0.1
- Dort muss die richtige IP-Adresse der Raspberry Pi eingetragen werden. Sie lässt sich herausfinden durch:
  ```bash
  ip a
  ```
- Sobald die Muondetector GUI verbunden ist, sollten folgende Werte geprüft werden:
  - XOR-Rate unten links im Bereich 2 - 6 Hz
  - Im Reiter GNSS Data muss ein 3D Fix bei der Standortbestimmung sein
  - Im Reiter Parameters muss die Bias Spannung zwischen 30 und 32 V liegen
- Um zu prüfen, ob die Daten aus der Strato-Software richtig aufgenommen werden, in den Ordner für Log-Dateien gehen:
  ```bash
  cd /var/strato-software/
  ```
- Schauen, welche Dateien vorhanden sind:
  ```bash
  ls
  ```
- Die aktuellste Log-Datei mit der gewünschten Messgröße kann sich angeschaut werden durch den Dateinamen, der durch Messgröße und Zeitstempel definiert ist mit zB:
  ```bash
  tail -f battery_voltage_ads1115_ch0_2026-09-03_16-00-00.txt
  ```
  oder zB:
  ```bash
  tail -f pressure_bme280_2026-09-03_16-00-00.txt
  ```
- Die Werte in der neusten Log-Datei sollten sich sekündlich aktualisieren

## Hinweise

- Stecker und SD-Karte dürfen niemals im laufenden Betrieb ein- und ausgesteckt werden. Der Schalter muss immer auf OFF sein.
- Niemals den Raspberry Pi während des Hoch- oder Runterfahrens ausschalten, das kann zum Zerstören des Dateisystems führen. Beide Vorgänge dauern maximal eine Minute.