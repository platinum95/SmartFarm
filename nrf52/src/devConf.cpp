// #include "devConf.h"

// #define ADC_ACQUISITION_TIME    ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 10)

// ADCSensor< 16 > ForceSensor( "WaterLevel",          // Key name
//                              CONFIG_ADC_0_NAME,     // ADC device name
//                              0,                     // Channel ID
//                              ADC_ACQUISITION_TIME,  // Aquisition time
//                              NRF_SAADC_INPUT_AIN1 );// Input ID

// ADCSensor< 16 > SoilSensor( "M", 
//                              CONFIG_ADC_0_NAME,
//                              2,
//                              ADC_ACQUISITION_TIME,
//                              NRF_SAADC_INPUT_AIN2 );

// DriverSensor dht( "DHT11", 
//                   ( KeyChan[] ){ 
//                     KeyChan{ "T", SENSOR_CHAN_AMBIENT_TEMP },
//                     KeyChan{ "H", SENSOR_CHAN_HUMIDITY }
//                   }, 2 );



// #include "lights.h"

// RpCallback setLight = {
//     callback    : &putLights2,
//     name        : "putLights"
// };


// SensorBase * sensors[] = { &ForceSensor, &SoilSensor, &dht };
// RpCallback * rpcList[]  = { &setLight };