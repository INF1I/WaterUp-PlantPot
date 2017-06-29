# The communication library
The communication library is written to open an connection to the
local wifi network so communication can be established between the
 pant pot and mqtt broker which in turn communicates with the mobile
 application.
 
 ## Communication public API

### Communication( Configuration * config );
> The constructor will initiate the communication library with some default
> values and will save an reference to the configuration library.
> <br>`@param potConfiguration`  An pointer to the configuration library.
     
### void setup();
>This function is used to initiate the Arduino/Huzzah board. It gets
>executed whenever the board is first powered up or after an rest. It will
>initiate the communication settings and launce a access point if the
>there are no valid wifi settings stored.
 
### void connect();
>This function is used to check if there is an connection to the mqtt broker.
>If not it will attempt to pen one.
     
 
     /**
      * This function will return the pointer to the configuration object that
      * contains communication and plant care settings.
      *
      * @return *Configuration
      */
     Configuration* getConfiguration();
 
     /**
      * This function will publish statistics about the pot's current state to the
      * mqtt broker.
      *
      * @param groundMoistureLevel   The current percentage of moisture in the ground.
      * @param waterReservoirLevel   The current percentage of water left in the reservoir.
      */
     void publishStatistic(int groundMoistureLevel, int waterReservoirLevel);
 
     /**
      * This function will publish warnings about the reservoir water level to the mqtt
      * broker. Like messages of an low water level or an empty reservoir.
      *
      * @param warningType   The type of warning to be send.
      */
     void publishWarning( uint8_t warningType);
 
     /**
      * This function will start listening for configuration send by the mqtt broker.
      */
     void listenForConfiguration();