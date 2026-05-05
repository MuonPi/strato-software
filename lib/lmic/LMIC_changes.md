# LMIC Changes for Raspi Usage

1. define EU-Band otherwise some commands may not exist

    in file "lmic_project_config":
    ```c++
    #define CFG_eu868 1
    ```

2. change SPI commands
    
    in file "hal.cpp" at line 226 in function "lmic_hal_spi_trx()"

    replace this:
    ```c++
    SPI.transfer(cmd);

    for (; len > 0; --len, ++buf) {
        u1_t data = is_read ? 0x00 : *buf;
        data = SPI.transfer(data);
        if (is_read)
            *buf = data;
    }
    ```

    through this:
    ```c++
    SPI.transfer(cmd, buf, len, is_read);
    ```

3. add files for Arduino commands in Raspi translation

    the folder "raspi" and the CMakeLists.txt has to be copied next to the folder "arduino-lmic"
