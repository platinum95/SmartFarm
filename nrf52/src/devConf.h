#include "SensorBase.h"
#include "RpcBase.h"

/*
Device configuration file. Unique to each device type. Configures the
sensors used and the RPC callbacks required to function.
*/
#define ADC_ACQUISITION_TIME    ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 10)

// Force sensor for use on the water-level node
ADCSensor< 16 > ForceSensor( "F",          // Key name
                             CONFIG_ADC_0_NAME,     // ADC device name
                             0,                     // Channel ID
                             ADC_ACQUISITION_TIME,  // Aquisition time
                             NRF_SAADC_INPUT_AIN1 );// Input ID

// Soil moisture sensor for use on the field-status node
ADCSensor< 16 > SoilSensor( "M", 
                             CONFIG_ADC_0_NAME,
                             2,
                             ADC_ACQUISITION_TIME,
                             NRF_SAADC_INPUT_AIN0 );

// Temp/humidity sensor for use on the field-status node
DriverSensor dht( "DHT11", 
                  ( KeyChan[] ){ 
                    KeyChan{ "T", SENSOR_CHAN_AMBIENT_TEMP },
                    KeyChan{ "H", SENSOR_CHAN_HUMIDITY }
                  }, 2 );

#include "RpcConf.h"

// Servo actuating RPC callback for use on the water-level node
RPCB( setServoCb, setServo, setServo );
// LED RPC callback
RPCB( setLight, putLights, putLights );


// Set the lists used by the main code
SensorBase * sensors[] = { &ForceSensor };
RpCallback * rpcList[] = { &setLight, &setServoCb };