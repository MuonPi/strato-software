import glob
import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates



# Nextbox: https://next.hessenbox.de/index.php/s/HawKdj7Siwms5w4




DATA_DIR = "/home/nico/Schreibtisch/Strato_7"

MISSION_NAME = "Strato7"

DATE = "2026-08-12"
START_TIME = "18:35:00"
END_TIME = "22:30:00"

PLOT_DIR = os.path.join(DATA_DIR, "plots")
MERGE_DIR = os.path.join(DATA_DIR, "merge")

FIGURE_SIZE = (16, 9)

os.makedirs(PLOT_DIR, exist_ok=True)
os.makedirs(MERGE_DIR, exist_ok=True)




def generate_merge_file(value_name, column_names):

    files = glob.glob(os.path.join(DATA_DIR, value_name + "_" + DATE + "*"))
    files = [file for file in files]

    if not files:
        print("no data found, quitting ...")
        exit()

    dataframes = []

    for file in files:

        print(f"reading: {os.path.basename(file)}")

        df = pd.read_csv(file, sep=";", header=None)
        df = df.dropna(axis=1, how="all")

        df = df.rename(columns={0: "timestamp"})
        df["timestamp"] = pd.to_datetime(df["timestamp"], format="%Y-%m-%d_%H-%M-%S")

        for column in df.columns[1:]:
            df[column] = pd.to_numeric(df[column], errors="coerce")

        dataframes.append(df)

    data = pd.concat(dataframes, ignore_index=True)
    data = data.sort_values("timestamp")

    start = pd.to_datetime( f"{DATE} {START_TIME}" )
    end = pd.to_datetime( f"{DATE} {END_TIME}" )
    data = data[ (data["timestamp"] >= start) & (data["timestamp"] <= end)]

    if data.empty:
        print("no data in this time, quitting ...")
        exit()

    data.columns = column_names

    merge_file = os.path.join(MERGE_DIR, MISSION_NAME + "_" + value_name + ".csv")

    data.to_csv(merge_file, index=False)
    print("created merge file: ", merge_file)





def plot_merge_file_versus_time(title, y_label, merge_file, channels):

    merge_file_path = os.path.join(MERGE_DIR, merge_file)
    df = pd.read_csv(merge_file_path)
    df["timestamp"] = pd.to_datetime(df["timestamp"])
    print(df)



    plt.figure(figsize=FIGURE_SIZE)

    for channel in channels:
        if channel not in df.columns:
            print("channel does not exist, quitting ...")
            exit()

    plt.plot(df["timestamp"], df[channels], label=channels)

    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%H:%M:%S"))

    plt.xlabel("time")
    plt.ylabel(y_label)
    plt.title(title)

    plt.legend()

    plt.grid(True)
    plt.tight_layout()

    plot_file = os.path.join(PLOT_DIR, os.path.splitext(os.path.basename(merge_file))[0] + ".svg")
    plt.savefig(plot_file, dpi=800, bbox_inches="tight")
    print("saved plot: ", plot_file)

    plt.show()








def generate_logfile_temperature_bme280():
    value_name = "temperature_bme280"
    column_names = ["timestamp", "temperature[°C]"]
    generate_merge_file(value_name, column_names)

def generate_logfile_pressure_bme280():
    value_name = "pressure_bme280"
    column_names = ["timestamp", "pressure[hPa]"]
    generate_merge_file(value_name, column_names)

def generate_logfile_humidity_bme280():
    value_name = "humidity_bme280"
    column_names = ["timestamp", "humidity[%]"]
    generate_merge_file(value_name, column_names)

def generate_logfile_temperature_bme280_2():
    value_name = "temperature_bme280_2"
    column_names = ["timestamp", "temperature[°C]"]
    generate_merge_file(value_name, column_names)

def generate_logfile_pressure_bme280_2():
    value_name = "pressure_bme280_2"
    column_names = ["timestamp", "pressure[hPa]"]
    generate_merge_file(value_name, column_names)

def generate_logfile_humidity_bme280_2():
    value_name = "humidity_bme280_2"
    column_names = ["timestamp", "humidity[%]"]
    generate_merge_file(value_name, column_names)

def generate_logfile_spectrum_as7343():
    value_name = "spectrum_as7343"
    column_names = ["timestamp", "channel1", "channel2", "channel3", "channel4", "channel5", "channel6", "channel7", "channel8", "channel9", "channel10", "channel11", "channel12", "channel13", "channel14", 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29]
    generate_merge_file(value_name, column_names)

def generate_logfile_uv_as7331():
    value_name = "uv_as7331"
    column_names = ["timestamp", "UVA", "UVB", "UVC"]
    generate_merge_file(value_name, column_names)




def plot_merge_file_versus_time_temperature_bme280():
    title = MISSION_NAME + " " + DATE + " " + "Temperature versus time"
    y_label = "temperature [°C]"
    merge_file = MISSION_NAME + "_" + "temperature_bme280" + ".csv"
    channels = ["temperature[°C]"]
    plot_merge_file_versus_time(title, y_label, merge_file, channels)

def plot_merge_file_versus_time_pressure_bme280():
    title = MISSION_NAME + " " + DATE + " " + "Pressure versus time"
    y_label = "pressure [hPa]"
    merge_file = MISSION_NAME + "_" + "pressure_bme280" + ".csv"
    channels = ["pressure[hPa]"]
    plot_merge_file_versus_time(title, y_label, merge_file, channels)

def plot_merge_file_versus_time_humidity_bme280():
    title = MISSION_NAME + " " + DATE + " " + "Humidity versus time"
    y_label = "humidity [hPa]"
    merge_file = MISSION_NAME + "_" + "humidity_bme280" + ".csv"
    channels = ["humidity[%]"]
    plot_merge_file_versus_time(title, y_label, merge_file, channels)

def plot_merge_file_versus_time_temperature_bme280_2():
    title = MISSION_NAME + " " + DATE + " " + "Temperature inside versus time"
    y_label = "temperature [°C]"
    merge_file = MISSION_NAME + "_" + "temperature_bme280_2" + ".csv"
    channels = ["temperature[°C]"]
    plot_merge_file_versus_time(title, y_label, merge_file, channels)

def plot_merge_file_versus_time_pressure_bme280_2():
    title = MISSION_NAME + " " + DATE + " " + "Pressure inside versus time"
    y_label = "pressure [hPa]"
    merge_file = MISSION_NAME + "_" + "pressure_bme280_2" + ".csv"
    channels = ["pressure[hPa]"]
    plot_merge_file_versus_time(title, y_label, merge_file, channels)

def plot_merge_file_versus_time_humidity_bme280_2():
    title = MISSION_NAME + " " + DATE + " " + "Humidity inside versus time"
    y_label = "humidity [hPa]"
    merge_file = MISSION_NAME + "_" + "humidity_bme280_2" + ".csv"
    channels = ["humidity[%]"]
    plot_merge_file_versus_time(title, y_label, merge_file, channels)

def plot_merge_file_versus_time_spectrum_as7343():
    title = MISSION_NAME + " " + DATE + " " + "Spectrum versus time"
    y_label = "intensity"
    merge_file = MISSION_NAME + "_" + "spectrum_as7343" + ".csv"
    channels = ["channel1", "channel2", "channel3", "channel4", "channel5", "channel6", "channel7", "channel8", "channel9", "channel10", "channel11", "channel12", "channel13", "channel14"]
    plot_merge_file_versus_time(title, y_label, merge_file, channels)

def plot_merge_file_versus_time_uv_as7331():
    title = MISSION_NAME + " " + DATE + " " + "UV versus time"
    y_label = "intensity"
    merge_file = MISSION_NAME + "_" + "uv_as7331" + ".csv"
    channels = ["UVA", "UVB", "UVC"]
    plot_merge_file_versus_time(title, y_label, merge_file, channels)









generate_logfile_temperature_bme280()
generate_logfile_pressure_bme280()
generate_logfile_humidity_bme280()
# generate_logfile_temperature_bme280_2()
# generate_logfile_pressure_bme280_2()
# generate_logfile_humidity_bme280_2()
generate_logfile_spectrum_as7343()
generate_logfile_uv_as7331()

plot_merge_file_versus_time_temperature_bme280()
plot_merge_file_versus_time_pressure_bme280()
plot_merge_file_versus_time_humidity_bme280()
# plot_merge_file_versus_time_temperature_bme280_2()
# plot_merge_file_versus_time_pressure_bme280_2()
# plot_merge_file_versus_time_humidity_bme280_2()
plot_merge_file_versus_time_spectrum_as7343()
plot_merge_file_versus_time_uv_as7331()
