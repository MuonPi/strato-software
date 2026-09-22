import glob
import os
import pandas as pd
import matplotlib.pyplot as plt


# ==================================================
# EINSTELLUNGEN
# ==================================================

DATA_DIR = "/home/nico/Schreibtisch/Strato_7"

# Welche Dateien sollen eingelesen werden?
FILE_PATTERN = "*spectrum*.txt"

# Name der zusammengeführten Datei
OUTPUT_FILE = "strato7_spectrum_as7343.txt"

# Zeitraum
DATE = "2026-08-12"
START_TIME = "18:35:00"
END_TIME = "22:30:00"

# Kanäle
CHANNELS_TO_PLOT = [1,2,3,4,5,6,7,8,9,10,11,12,13,14]

# Plot-Größe
FIGURE_SIZE = (16, 9)

# Beschriftungen
Y_LABEL = "Intensity"
PLOT_TITLE = "UV Spectrum"

PLOT_DIR = os.path.join(DATA_DIR, "plots")
os.makedirs(PLOT_DIR, exist_ok=True)
output_path = os.path.join(PLOT_DIR, OUTPUT_FILE)


# ==================================================
# 1. Dateien suchen
# ==================================================

output_path = os.path.join(DATA_DIR, OUTPUT_FILE)

files = glob.glob(
    os.path.join(DATA_DIR, FILE_PATTERN)
)

# Gesamtdatei nicht wieder einlesen
files = [
    file for file in files
    if os.path.abspath(file) != os.path.abspath(output_path)
]

print(f"{len(files)} Dateien gefunden.")

if not files:
    print("Keine Dateien gefunden.")
    exit()


# ==================================================
# 2. Dateien einlesen
# ==================================================

dataframes = []

for file in files:

    print(f"Lese: {os.path.basename(file)}")

    df = pd.read_csv(
        file,
        sep=";",
        header=None
    )

    # Letzte leere Spalte entfernen
    df = df.dropna(axis=1, how="all")

    # Erste Spalte = Timestamp
    df = df.rename(columns={0: "timestamp"})

    # Timestamp
    df["timestamp"] = pd.to_datetime(
        df["timestamp"],
        format="%Y-%m-%d_%H-%M-%S"
    )

    # Alle Messwerte in Zahlen umwandeln
    for column in df.columns[1:]:
        df[column] = pd.to_numeric(
            df[column],
            errors="coerce"
        )

    dataframes.append(df)


# ==================================================
# 3. Zusammenführen
# ==================================================

data = pd.concat(
    dataframes,
    ignore_index=True
)

# Chronologisch sortieren
data = data.sort_values("timestamp")


# ==================================================
# 4. Anzahl der Kanäle feststellen
# ==================================================

value_columns = [
    column
    for column in data.columns
    if column != "timestamp"
]

number_of_channels = len(value_columns)

print(f"\nGefundene Messkanäle: {number_of_channels}")


# ==================================================
# 5. Gesamtdatei speichern
# ==================================================

# plots-Ordner
PLOT_DIR = os.path.join(DATA_DIR, "plots")

# Ordner erstellen, falls er noch nicht existiert
os.makedirs(PLOT_DIR, exist_ok=True)

# Pfad zur Gesamtdatei
output_path = os.path.join(PLOT_DIR, OUTPUT_FILE)

data.to_csv(
    output_path,
    sep=";",
    header=False,
    index=False,
    date_format="%Y-%m-%d_%H-%M-%S"
)

print(f"Gesamtdatei erstellt:")
print(output_path)


# ==================================================
# 6. Zeitraum auswählen
# ==================================================

start = pd.to_datetime(
    f"{DATE} {START_TIME}"
)

end = pd.to_datetime(
    f"{DATE} {END_TIME}"
)

filtered = data[
    (data["timestamp"] >= start) &
    (data["timestamp"] <= end)
]


print("\nZeitraum:")
print(f"  Start: {start}")
print(f"  Ende:  {end}")
print(f"  Datenpunkte: {len(filtered)}")


if filtered.empty:
    print("Keine Daten in diesem Zeitraum gefunden.")
    exit()

# ==================================================
# 7. Plot
# ==================================================

plt.figure(figsize=FIGURE_SIZE)

for channel in CHANNELS_TO_PLOT:

    index = channel - 1

    if index >= len(value_columns):
        print(f"Warnung: CH {channel} existiert nicht.")
        continue

    column = value_columns[index]

    plt.plot(
        filtered["timestamp"],
        filtered[column],
        label=f"CH {channel}"
    )

plt.xlabel("Zeit")
plt.ylabel(Y_LABEL)
plt.title(PLOT_TITLE)

plt.legend()

plt.grid(True)
plt.tight_layout()


# ==================================================
# 8. Plot automatisch speichern
# ==================================================

# Dateiname des Plots
plot_filename = os.path.splitext(OUTPUT_FILE)[0] + ".svg"

plot_path = os.path.join(
    PLOT_DIR,
    plot_filename
)

plt.savefig(
    plot_path,
    dpi=800,
    bbox_inches="tight"
)

print(f"Plot gespeichert:")
print(plot_path)


# Plot anzeigen
plt.show()