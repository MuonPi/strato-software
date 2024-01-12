//write the arduino functions empty

int OUTPUT=1;
int INPUT=0;

void pinMode(u1_t x, int y){
    printf("pinmode");
}
void digitalWrite(u1_t x, u1_t){
    printf("digitalwrite");
}
bool digitalRead(u1_t x){
    printf("digitalread");
    return true;
}


uint8_t MSBFIRST=8;
uint8_t SPI_MODE0=8;

class SPISettings
{
 SPISettings(double x, uint8_t y, uint8_t z);
};

class SPIclass
{
    void begin(){
        printf("begin");
    };
};
SPIclass SPI;