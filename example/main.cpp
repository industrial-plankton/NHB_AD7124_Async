#include <Arduino.h>
#include <NHB_AD7124_Async.h>

const uint8_t csPin = 10;

Ad7124 adc(csPin, 4000000);

// The filter select bits determine the filtering and ouput data rate
//     1 = Minimum filter, Maximum sample rate
//  2047 = Maximum filter, Minumum sample rate
//
// For this example I'll use a setting of 45, which will give us about 20 sps
// with this channel configuration.
//
// Of course you can always take your readings at a SLOWER rate than
// the output data rate. (i.e. logging a reading every  30 seconds)
//
// NOTE: Actual output data rates in single conversion mode will be slower
// than calculated using the formula in the datasheet. This is because of
// the settling time plus the time it takes to enable or change the channel.
int filterSelBits = 45; // 13 ~= 50 sps, 18 ~= 40 sps

void setup()
{

    // Initialize serial and wait for port to open:
    Serial.begin(115200);
    while (!Serial && millis() < 4000)
    {
        // wait for serial port to connect. Needed for native USB port only
    }

    Serial.println("AD7124 Multiple sensor Subscription type example");

    // Initializes the AD7124 device
    adc.begin(SPI);

    // Configuring ADC in Full Power Mode (Fastest)
    adc.setAdcControl(AD7124_OpMode_SingleConv, AD7124_FullPower, true);

    // Set the "setup" configurations for different sensor types. There are 7 differnet "setups"
    // in the ADC that can be configured. There are 8 setups that can be configured. Each
    // setup holds settings for the reference used, the gain setting, filter type, and rate

    adc.setup[0].setConfig(AD7124_Ref_ExtRef1, AD7124_Gain_128, true); // Load Cell:           External reference tied to excitation, Gain = 128, Bipolar = True
    adc.setup[1].setConfig(AD7124_Ref_Internal, AD7124_Gain_32, true); // Thermocouple:        Internal reference, Gain = 128, Bipolar = True
    adc.setup[2].setConfig(AD7124_Ref_ExtRef1, AD7124_Gain_1, false);  // Ratiometric Voltage: External reference tied to excitation, Gain = 1, Bipolar = False
    adc.setup[3].setConfig(AD7124_Ref_Internal, AD7124_Gain_1, true);  // IC Temp sensor:      Internal reference, Gain = 1, Bipolar = True

    // Filter settings for each setup. In this example they are all the same so
    // this really could have just been done with a loop
    adc.setup[0].setFilter(AD7124_Filter_SINC3, filterSelBits);
    adc.setup[1].setFilter(AD7124_Filter_SINC3, filterSelBits);
    adc.setup[2].setFilter(AD7124_Filter_SINC3, filterSelBits);
    adc.setup[3].setFilter(AD7124_Filter_SINC3, filterSelBits);

    // Set channels, i.e. what setup they use and what pins they are measuring on

    adc.setChannel(0, 0, AD7124_Input_AIN0, AD7124_Input_AIN1, true); // Channel 0 - Load cell
    adc.setChannel(1, 1, AD7124_Input_AIN2, AD7124_Input_AIN3, true); // Channel 1 - Type K Thermocouple 1
    adc.setChannel(2, 1, AD7124_Input_AIN4, AD7124_Input_AIN5, true); // Channel 2 - Type K Thermocouple 2
    adc.setChannel(3, 2, AD7124_Input_AIN6, AD7124_Input_AVSS, true); // Channel 3 - Single ended potentiometer 1
    adc.setChannel(4, 2, AD7124_Input_AIN7, AD7124_Input_AVSS, true); // Channel 4 - Single ended potentiometer 2
    adc.setChannel(5, 3, AD7124_Input_TEMP, AD7124_Input_AVSS, true); // Channel 5 - ADC IC temperature

    // For thermocouples we have to set the voltage bias on the negative input pin
    // for the channel, in this case it's AIN3 and AIN5
    adc.setVBias(AD7124_VBias_AIN3, true);
    adc.setVBias(AD7124_VBias_AIN5, true);

    // Turn on excitation voltage. If you were only taking readings periodically
    // you could turn this off between readings to save power.
    adc.setPWRSW(1);

    adc.subscribeChannel(0, 1);
    adc.subscribeChannel(1, 1);
    adc.subscribeChannel(2, 5);
    adc.subscribeChannel(3, 5);
    adc.subscribeChannel(4, 5);
    adc.subscribeChannel(5, 5);
}

// -----------------------------------------------------------------------------

void loop()
{
    adc.processSubscriptions();

    Serial.println("Values:");
    Serial.println(adc.getLastReading(0));
    Serial.println(adc.getLastReading(1));
    Serial.println(adc.getLastReading(2));
    Serial.println(adc.getLastReading(3));
    Serial.println(adc.getLastReading(4));
    Serial.println(adc.getLastReading(5));
}