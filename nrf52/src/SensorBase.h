#include <zephyr.h>

#include <misc/printk.h>
#include <adc.h>
#include <hal/nrf_saadc.h>
#include <sensor.h>
#include <stdio.h>
#include <string.h>

/*
The Base classes and hierarchies for the various sensor
configurations that can be set on a per-device level.
*/

// Base class for all sensor types
class SensorBase {
public:
    SensorBase(){
    }
    // These functions must be overloaded by child classes
    virtual int initialise() = 0;
    virtual char * requestPayload() = 0;
    struct device * devBinding;

protected:
    int getDevBinding(){
        if( devName ){
            this->devBinding = device_get_binding( this->devName );
            if( !this->devBinding ){
                printf("Cannot get device binding for %s\n", this->devName );
                return -1;
            }
        }
        return 0;
    }
    const char * devName = nullptr;
    const char * keyName = nullptr;
    char payloadData[ 128 ];
    size_t payloadLen;
};

// Symbolises a key-channel relation
struct KeyChan{
    const char * key;
    sensor_channel channel;
};

// Sensor base that uses the Zephyr driver backend, such as the DHT11
class DriverSensor : public SensorBase {
public:
    DriverSensor( const char * _devName, KeyChan _keyChanPairs[], uint16_t _keyChanLen ){
        this->devName = _devName;
        this->keyChanPairs = _keyChanPairs;
        this->keyChanLength = _keyChanLen;
    }
    int initialise(){
        return SensorBase::getDevBinding();
    }
    char * requestPayload(){
        if( !this->devBinding ){
            // Just return if it hasn't been initialised
            return nullptr;
        }
        int r = sensor_sample_fetch( this->devBinding );
        if ( r ) {
            // This will happen often with the DHT11 due to the inadequate 
            // timing resolution
            printf("sensor_channel_get failed, returned: %d\n", r);
            return nullptr;
        }

        this->payloadData[ 0 ] = 0;
        // Get data from all required channels, append to the payload
        for( int i = 0; i < this->keyChanLength; i++ ){
            struct sensor_value sensorVal;
            r = sensor_channel_get( this->devBinding,
                                    this->keyChanPairs[ i ].channel,
                                    &sensorVal );
            if ( r ) {
                printf("sensor_channel_get failed, returned: %d\n", r);
                break;
            }
            sprintf( this->payloadData + strlen( this->payloadData ),
                     "\"%s\":\"%f\",", this->keyChanPairs[ i ].key,
                     sensor_value_to_double( &sensorVal ) );
        }
        // Remove the final comma
        this->payloadData[ strlen( this->payloadData ) - 1 ] = 0;
        return this->payloadData;
    }

private:
    KeyChan *keyChanPairs;
    uint8_t keyChanLength; 
};

// Class for devices that use the ADC such as soil moisture
template< int bufferSize >
class ADCSensor : public SensorBase {
public:
    ADCSensor( const char * _keyName,
               const char * _adcName,
               uint8_t _channelID, 
               uint16_t _aqTime,
               uint8_t _input=NRF_SAADC_INPUT_DISABLED,
               uint8_t _res=10,
               adc_gain _gain=ADC_GAIN_1_6,
               adc_reference _ref=ADC_REF_INTERNAL ){
        this->devName = _adcName;
        this->keyName = _keyName;
        size_t keyLen = strlen( _keyName );
        // Reading a 16-bit ADC value, max (unsigned) is 65536,
        // call it 6 characters to be safe. Max payload length
        // is keyLen + 6 + 1 (for separator) + 1 (null-terminator)
        this->payloadLen = 128;

        channelConfig.gain             = _gain;
        channelConfig.reference        = _ref;
        channelConfig.acquisition_time = _aqTime;
        channelConfig.channel_id       = _channelID;
        channelConfig.input_positive   = _input;

        sensorConfig.channels    = BIT( _channelID );
        sensorConfig.buffer      = dataBuffer;
        sensorConfig.buffer_size = bufferSize * sizeof( uint16_t );
        sensorConfig.resolution  = _res;

    }

    int initialise(){
        if( SensorBase::getDevBinding() ){
            return -1;
        }

        if( auto ret=adc_channel_setup( this->devBinding, &this->channelConfig ) ){
            printf( "Failed to setup channel %i; error %i",
                    this->channelConfig.channel_id, ret );
            return ret;
        }
        return 0;
    }

    uint16_t getData(){
        if( !this->devBinding ){
            printf( "Binding not set; ADC possibly not initialised\n" );
            return 0;
        }
        if ( auto ret=adc_read( this->devBinding, &this->sensorConfig ) ) {
            printf( "Failed to read ADC with code %i", ret );
            return 0;
        }
        if( ( this->sensorData = ( int16_t ) *this->dataBuffer ) < 0 ){
            this->sensorData = 0;
        }
        
        return this->sensorData;
    }

    char * requestPayload(){
        this->getData();
        size_t wrote = sprintf( this->payloadData, "\"%s\":\"%i\"",
                                this->keyName, this->sensorData );
        if( wrote + 1 > payloadLen ){
            printf( "Uh-oh, not enough room in payload buffer...\n" );
        }
        return this->payloadData;
    }


private:
    struct adc_channel_cfg channelConfig;
    struct adc_sequence sensorConfig;
    uint16_t dataBuffer[ bufferSize ];
    int16_t sensorData;
};
