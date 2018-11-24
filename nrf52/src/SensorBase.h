#include <zephyr.h>
#include <board.h>
#include <device.h>
#include <misc/printk.h>
#include "config.h"
#include <adc.h>
#include <hal/nrf_saadc.h>

class SensorBase {
public:
    SensorBase(){

    }
    virtual int initialise() = 0;
    virtual char * requestPayload() = 0;
    struct device * devBinding;

protected:
    const char * devName;
    const char * keyName;
    char payloadData[ 128 ];
    size_t payloadLen;
};

template< int bufferSize >
class ADCSensor : public SensorBase {
public:
    ADCSensor( const char * _keyName,
               const char * _adcName,
               uint8_t _channelID, 
               uint8_t _aqTime,
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
        this->payloadLen = keyLen + 8;

        channelConfig.gain             = _gain;
        channelConfig.reference        = _ref;
        channelConfig.acquisition_time = _aqTime;
        channelConfig.channel_id       = _channelID;
        channelConfig.input_positive   = _input;

        sensorConfig.channels    = BIT( _channelID );
        sensorConfig.buffer      = dataBuffer;
        sensorConfig.buffer_size = this->buffSize * sizeof( uint16_t );
        sensorConfig.resolution  = _res;

    }

    int initialise(){
        this->devBinding = device_get_binding( this->devName );
        if( !this->devBinding ){
		    printk("Cannot get ADC device");
		    return -1;
	    }
	    if( auto ret=adc_channel_setup( this->devBinding, &this->channelConfig ) ){
		    printk( "Failed to setup channel %i; error %i",
                    this->channelConfig.channel_id, ret );
            return ret;
	    }
        return 0;
    }

    uint16_t getData(){
        if( !this->devBinding ){
            printk( "Binding not set; ADC possibly not initialised\n" );
            return 0;
        }
        if ( auto ret=adc_read( this->devBinding, &this->sensorConfig ) ) {
            printk( "Failed to read ADC with code %i", ret );
            return 0;
        }
        this->sensorData = ( uint16_t ) *this->dataBuffer;
        return this->sensorData;
    }

    char * requestPayload(){
        this->getData();
        size_t wrote = sprintf( this->payloadData, "%s:%hu",
                                this->keyName, this->sensorData );
        if( wrote + 1 > payloadLen ){
            printk( "Uh-oh, not enough room in payload buffer...\n" );
        }
        return this->payloadData;
    }


private:
    struct adc_channel_cfg channelConfig;
    struct adc_sequence sensorConfig;
    uint16_t dataBuffer[ bufferSize ];
    uint16_t sensorData;
    uint32_t buffSize;    
};
