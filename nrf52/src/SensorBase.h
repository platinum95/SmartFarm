#include <zephyr.h>
#include <board.h>
#include <device.h>
#include <misc/printk.h>
#include "config.h"
#include <adc.h>
#include <hal/nrf_saadc.h>

template< typename dataStruct >
class SensorBase {
public:
    SensorBase();
    virtual int initialise() = 0;
    virtual dataStruct requestData() = 0;
    struct device * devBinding;

private:
    const char * devName;
};

#define ADC_MAX_BUFFER_SIZE 16

template< typename dataStruct >
class ADCSensor : SensorBase< dataStruct > {
public:
    ADCSensor( const char * _adcName,
               uint8_t _channelID, 
               uint8_t _aqTime, 
               uint32_t _bufSize,
               uint8_t _input=NRF_SAADC_INPUT_DISABLED,
               uint8_t _res=10,
               uint8_t _gain=ADC_GAIN_1_6,
               uint8_t _ref=ADC_REF_INTERNAL ){
        this->devName = _adcName;
        this->channelConfig = {
            gain             : _gain,
            reference        : _ref,
            acquisition_time : _aqTime,
            channel_id       : _channelID,
            input_positive   : _input,
        };

        if( _bufSize > ADC_MAX_BUFFER_SIZE ){
            printk( "WARNING: Requesting more data than buffer has room for\n" );
        }
        this->buffSize = _bufSize;
        this->sensorConfig = {
            .channels    = BIT( _channelID ),
		    .buffer      = dataBuffer,
		    .buffer_size = this->buffSize * sizeof( uint16_t ),
		    .resolution  = _res,
        };
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
    }
    uint16_t * getRawData(){
        if( !this->devBinding ){
            printk( "Binding not set; ADC possibly not initialised\n" );
            return nullptr;
        }
        if ( auto ret=adc_read( this->devBinding, &this->sensorConfig ) ) {
            printk( "Failed to read ADC with code %i", ret );
            return nullptr;
        }
    }

    //int cleanup();
    dataStruct requestData(){

    }

private:
    struct adc_channel_cfg channelConfig;
    struct adc_sequence sensorConfig;
    uint16_t dataBuffer[ ADC_MAX_BUFFER_SIZE ];
    uint32_t buffSize;
};
