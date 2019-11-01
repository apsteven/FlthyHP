////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                  ///
///                                    Flthy Holoprojectors v1.1                                     ///
///                                       by Ryan Sondgeroth                                         ///
///                                         aka FlthyMcNsty                                          ///
///                                                                                                  ///
///      Combines the movement and display functions of each Holoprojector into a single easy        ///
///     to control sketch.  It utilizes 7 LED NeoPixel boards inside each HP to produce a more       ///
///        life-like representation of a hologram projection. Servo control is handled by a          ///
///      Adafruit 16 Channel I2C Breakout board, giving increased flexibility over the I2C bus,      ///
///     as well as the ability to operate the HP Servos with Neopixel LEDs from the same sketch.     ///
///       The regular arduino servo library doesn't get along with Adafruits Neopixel library,       ///
///                  Big Happy Dude's Slow Servo Library is much better any way, :-D                 ///
///                                                                                                  ///
///                          Rhyno45 is a Wanker, in case Ya'll didn't know!                         ///
///                                                                                                  ///
///                                                                                                  ///
///         Version 1.1: Adeded One Time HP Twitch Function                                          ///
///                                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                  ///
///     Commands and Structure                                                                       ///
///                                                                                                  ///.
///     DT##C                                                                                        ///
///     D - the HP designator with F-Front HP, R-Rear HP, T-Top HP, and A-All 3 HPs                  ///
///     T - the Sequence Type is either 0-Led Fuctions and 1-Servo Functions                         ///
///    ## - the Sequence Value including leading zero if necessary, ie sequence 3 is 03              ///
///     C - (Optional), the Color integer value from list below:                                     ///
///        Basic Color Integer Values                                                                ///
///           0 = Off (Black)                                                                        ///
///           1 = Red                                                                                ///
///           2 = Yellow                                                                             ///
///           3 = Green                                                                              ///
///           4 = Cyan (Aqua)                                                                        ///
///           5 = Blue                                                                               ///
///           6 = Magenta                                                                            ///
///           7 = Orange                                                                             ///
///           8 = Purple                                                                             ///
///           9 = White                                                                              ///
///                                                                                                  ///
///     D001   - Leia Sequence, Random shades of blue to mimic Leia Hologram                         ///
///     D002C  - Color Projector Sequence, Like Leia above but using color command value             ///
///     D003C  - Dim Pulse Sequence, Color slowly pulses on and off                                  ///
///     D004C  - Cycle Sequence, using color command value                                           ///
///     D005C  - Short Circuit, Led flashes on and off with interval slowing over time               ///
///     D006C  - Toggles Color, Simply sets LEDs tp solid color value.                               ///
///     D007   - Rainbow Sequence                                                                    ///
///     D098   - Clears LED, Disables Auto LED Sequence                                              ///
///     D099   - Clears LED, Enables Auto LED Sequence                                               ///
///                                                                                                  ///
///     D101   - Sends HP to the Center Position                                                     ///
///     D102   - Sends HP to the Down Position                                                       ///
///     D103   - Sends HP to the Up Position                                                         ///
///     D104   - Enables RC Control on HP                                                            ///
///     D198   - Disables Auto HP Twitch                                                             ///
///     D199   - Enables Auto HP Twitch                                                              ///
///                                                                                                  ///
///       S1   - Leia Mode (Front HP in Down Position, Leia LED Sequence, all other HPs disabled)    ///
///       S8   - Clear all LEDs, Disable Auto HP Twitch, Disable Auto LED Sequence                   ///
///       S9   - Clear all LEDs, Enable Auto HP Twitch, Enable Auto LED Sequence                     ///
///                                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                  ///
///                                  Required Libraries                                              ///
///                                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Wire.h>                        //
#include <Adafruit_NeoPixel.h>           // Source: https://github.com/adafruit/Adafruit_NeoPixel
#include <Adafruit_PWMServoDriver.h>     // Source: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <Servos.h>                      // Source: https://drive.google.com/file/d/0B5B8A65frsBgZ3VpeGxpM1lzaFE/edit  Thanks BHD!

////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                  ///
///                                  Start User Adjustable Settings                                  ///
///                                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////////
  ///*****                 Assign Arduino IC2 Address Below                 *****///
  ///*****                                                                  *****///
  ///*****  An I2C address of 25 has been standardized for other HP devices *****///
  ///*****       so we will use it to. It can be changed here if needed.    *****///
  ///*****                                                                  *****///
  //////////////////////////////////////////////////////////////////////////////////
    byte I2CAddress = 0x19;  // 25 in Hexadecmal


  //////////////////////////////////////////////////////////////////////////////////
  ///*****                Assign Servo Board IC2 Address Below              *****///
  ///*****                                                                  *****///
  ///*****           The I2C address of the servo breakout board can        *****/// 
  ///*****       be set below, and corresponds with the address selected    *****///
  ///*****          on the board using the following jumper settings:       *****///
  ///*****                                                                  *****///
  ///*****      Address= 0x40 (64) Offset = binary 00000 (no jumpers)       *****///
  ///*****      Address= 0x41 (65) Offset = binary 00001 (A0)               *****///
  ///*****      Address= 0x42 (66) Offset = binary 00010 (A1)               *****///
  ///*****      Address= 0x43 (67) Offset = binary 00011 (A0 & A1)          *****///
  ///*****      Address= 0x44 (68) Offset = binary 00100 A2)                *****///
  ///*****      Address= 0x45 (69) Offset = binary 00101 (A2 & A0)          *****/// 
  ///*****      Address= 0x46 (70) Offset = binary 00110 (A2 & A1)          *****/// 
  ///*****      Address= 0x47 (71) Offset = binary 00111 (A2, A1, & A0)     *****/// 
  ///*****                 ...So on and so forth                            *****/// 
  ///*****                                                                  *****///
  //////////////////////////////////////////////////////////////////////////////////
    byte ServoI2CAddress = 0x40;  // 64 in Hexadecmal


  //////////////////////////////////////////////////////////////////////////////////
  ///*****               Arduino Digital Pin Assignments for LEDs           *****///
  ///*****                                                                  *****///
  //////////////////////////////////////////////////////////////////////////////////
    int LEDpins[3]   = {2,3,4};   // {Front, Rear, Top}

  //////////////////////////////////////////////////////////////////////////////////////////
  ///*****                         Servo Board Pin Assignments                      *****///
  ///*****                                                                          *****///
  ///*****  HPpins[3][2] = {{Front 1, Front2}, {Rear 1, Rear 2}, {Top 1, Top 2}};   *****///
  ///*****                                                                          *****///
  //////////////////////////////////////////////////////////////////////////////////////////    
    int HPpins[3][2] = {{0,1},{2,3},{4,5}};           // Front HP Pins, Rear HP Pins, Top HP Pins


  //////////////////////////////////////////////////////////////////////////////////
  ///*****       Arduino Digital Pin Assignment for Output Enable (OE)      *****///
  ///*****                                                                  *****///
  ///*****  Output Enable (OE) is used to disable/enable servos between     *****///
  ///*****          movements to minimize unnecessary hum/noise.            *****///
  ///*****                                                                  *****///
  ///*****    The only reasons you would want to disable this is if...      *****///
  ///*****    1. You wish to use an additional microcontroller to send      *****/// 
  ///*****       servo control commands via the Adafruit library.           *****///
  ///*****    2. You want to use additional servos on the same servo        *****///
  ///*****        breakout board as your HP servos.                         *****///
  ///*****                                                                  *****///
  ///*****       Neither scenario is recommended because the increased      *****///
  ///*****   complexity far outweighs the costs savings of not purchasing   *****///
  ///*****                   an additional servo board.                     *****///
  ///*****                                                                  *****///
  //////////////////////////////////////////////////////////////////////////////////
    int oepin = 10;       
    int enableOE = 1;     //  0 = Disabled; 1 = Enabled
  

  //////////////////////////////////////////////////////////////////////////////////
  ///*****                                                                  *****///
  ///*****            Arduino Digital Pin Assignment for RC Input           *****///
  ///*****                                                                  *****///
  //////////////////////////////////////////////////////////////////////////////////
    int RCpin = 9;

  //////////////////////////////////////////////////////////////////////////////////
  ///*****             Pulse Width Range of RC Stick/Controller             *****///
  ///*****               assigned to control HP movement.                   *****///
  //////////////////////////////////////////////////////////////////////////////////
    int RCRange[2] = {1250, 1750};     // {Min, Max}
    

  //////////////////////////////////////////////////////////////////////////////////
  ///*****                          LED Brightness                          *****///
  ///*****                                                                  *****///
  ///*****                 Adjust LED Brightness Level (0-255)              *****///
  //////////////////////////////////////////////////////////////////////////////////
    #define BRIGHT 100  // Set LED Brightness Level (0-255)

  //////////////////////////////////////////////////////////////////////////////////
  ///*****                Enable/Disable LED & Servo Auto Twitch            *****///
  ///*****                                                                  *****///
  ///*****                      Enabled = 1, Disabled = 0                   *****///
  ///*****                                                                  *****///
  //////////////////////////////////////////////////////////////////////////////////
    int enableTwitchFLED  = 1; //Front LED
    int enableTwitchFHP   = 1; //Front Servo
    int enableTwitchRLED  = 1; //Rear LED
    int enableTwitchRHP   = 1; //Rear Servo
    int enableTwitchTLED  = 1; //Top LED
    int enableTwitchTHP   = 1; //Top Servo


  ///////////////////////////////////////////////////////////////////////////////////
  ///*****               LED & Servo Auto Twitch Interval Ranges             *****///
  ///*****                                                                   *****///
  ///*****      Use these values to select the range (min, max) in           *****///
  ///*****     seconds to randomly select the next auto twitch interval.     *****///
  ///*****      This helps give the illusion these displays & movements      *****///
  ///*****                      are totally autonomous.                      *****///
  ///*****                                                                   *****///
  ///*****           **TwitchInterval[2]  = {Min Value, Max Value}           *****///
  ///*****                                                                   *****///
  ///////////////////////////////////////////////////////////////////////////////////  
    
    unsigned int FLEDTwitchInterval[2] = {240,360};  // (4mins-6mins) Enter min and max seconds Front HP Led Twitches  
    unsigned int RLEDTwitchInterval[2] = {300,420};  // (5mins-7mins) Enter min and max seconds Rear HP Led Twitches  
    unsigned int TLEDTwitchInterval[2] = {300,420};  // (5mins-7mins) Enter min and max seconds Top HP Led Twitches  
    
    unsigned int FHPTwitchInterval[2] = {45,120};  // (45s-2mins) Enter min and max seconds Front HP Servo Twitches  
    unsigned int RHPTwitchInterval[2] = {60,180};  // (1min-3mins) Enter min and max seconds Rear HP Servo Twitches  
    unsigned int THPTwitchInterval[2] = {60,180};  // (1min-3mins) Enter min and max seconds Top HP Servo Twitches  

  ///////////////////////////////////////////////////////////////////////////////////
  ///*****                  LED Auto Twitch Run Time Ranges                  *****///
  ///*****                                                                   *****///
  ///*****    Use these values to select the range (min, max) in seconds     *****///
  ///*****   to randomly select how long the sequence runs each time the     *****///
  ///*****                      LED twitch is executed.                      *****///
  ///*****                                                                   *****///
  ///*****      *LEDTwitchRunIntervall[2]  = {Min Value, Max Value}          *****///
  ///*****                                                                   *****///
  ///////////////////////////////////////////////////////////////////////////////////  
    unsigned int FLEDTwitchRunInterval[2] = {5,25};  // (5-25s) Front LED Runtime
    unsigned int RLEDTwitchRunInterval[2] = {5,25};  // (5-25s) Rear LED Runtime   
    unsigned int TLEDTwitchRunInterval[2] = {5,25};  // (5-25s) Top LED Runtime  

  //////////////////////////////////////////////////////////////////////////////////
  ///*****                           Servo Speed                            *****///
  ///*****                                                                  *****///
  ///*****      Tweak these value to adjust the range of movement speeds    *****///
  ///*****     for your HP servos. Values in ms, Lower values results in    *****///
  ///*****      faster movement.   By using a range, the servos will not    *****///
  ///*****     always move at the same speed giving the subtle illusion     *****/// 
  ///*****           that each movement is purposeful and unique.           *****///
  ///*****                   They say its the little things.                *****///
  ///*****                                                                  *****///
  //////////////////////////////////////////////////////////////////////////////////       
    const int SERVO_SPEED[2] = {150, 400};


  //////////////////////////////////////////////////////////////////////////////////
  ///*****                Preset HP Servo Position Coordinates              *****///
  ///*****                                                                  *****///
  ///*****   Since each Builder's droid is assembled differently, we found  *****///
  ///*****   using position coordinates generated randomly on the fly was   *****///
  ///*****   problematic due to the occasional servo movement attempting   *****///
  ///*****   to travel beyond the outer cowl path.  Therefore we use a set  *****///
  ///*****   of 6 position pairs for each HP, set to ensure they lie with   *****///
  ///*****    in a freely accessible position. These coordinates consist    *****///
  ///*****     of the servo position of each servo using its pulse width    *****///
  ///*****    value in microseconds. I suggest using a servo tester with    *****///
  ///*****    a digital display to find these position pairs while the HPs  *****///
  ///*****                      are mounted in the dome.                    *****///
  ///*****                                                                  *****///
  ///*****     The first coordinate pair in each row references the DOWN    *****/// 
  ///*****    position of the HP, the second pair is the CENTER position,   *****/// 
  ///*****   and the 3rd pair the UP position.  The three remaining pairs   *****/// 
  ///*****    are auxiliary positions picked from a position free from      *****/// 
  ///*****                     obstructed servo travel.                     *****/// 
  ///*****                                                                  *****///
  ///*****     Random twitch movement is then chosen to randomly travel     *****/// 
  ///*****     between these 6 safe points minimizing the chance of the     *****/// 
  ///*****            movement being impeded causing excessive and          *****/// 
  ///*****                     unpleasant servo noise                       *****/// 
  ///*****                                                                  *****///
  ///*****          But if you would rather do it the simple way, set       *****///
  ///*****    enableBasicHP to 1 and it will use the Center, Min and Max    *****///
  ///*****    values set in the HPposBasic array. These are based on the    *****///
  ///*****      assumed default center value of a normal servo so it is     *****///
  ///*****    important that your servo linkage is installed in a manner    *****///
  ///*****       where the center servo position corresponds with the       *****///
  ///*****       orientation you want your HP to be in when centered.       *****///
  ///*****                                                                  *****///
  ///*****          RC mode requires position coordinates to work well,     *****///
  ///*****                so you must have enableBasicHP set to 0.          *****///
  ///*****                                                                  *****///
  ///*****     IT IS IMPORTANT TO NOTE...if you disassemble your HPs and    *****///
  ///*****     servo linkage after setting the following values, you will   *****///
  ///*****    most likely need to recheck and reset them as the chances of  *****///
  ///*****       reinstalling them exactly the same way is approximately    *****///
  ///*****                             3,720 to 1.                          *****///
  ///*****                                                                  *****///
  //////////////////////////////////////////////////////////////////////////////////     
    int HPpos[3][6][2] = {{{1492,1964},   // Front HP Down
                           {1492,1565},   // Front HP Center
                           {1492,1218},   // Front HP Up
                           {1281,1443},   // Front HP Aux
                           {1692,1757},   // Front HP Aux
                           {1255,1940}},  // Front HP Aux 
                          {{1601, 1084},  // Rear HP Down
                           {1437,1537},   // Rear HP Center
                           {1385,1954},   // Rear HP Up
                           {1343,1406},   // Rear Aux
                           {1633,1764},   // Rear Aux
                           {1803,1283}},  // Rear Aux
                          {{1547,1038},   // Top HP Down
                           {1431,1395},   // Top HP Center
                           {1473,1793},   // Top HP Up
                           {1633,1737},   // Top Aux
                           {1324,1200},   // Top Aux
                           {1677,1151}}}; // Top Aux

    int enableBasicHP = 1;                         // 0 - Disabled, 1 - Enabled, Basic Servo Positioning
    int HPposBasic[2][3] = {{1500,1300,1700},        // HP Servo 1 Values for Basic Mode (Center, Min, Max)
                            {1500,1300,1700}};       // HP Servo 2 Values for Basic Mode (Center, Min, Max)

  ///////////////////////////////////////////////////////////////////////////////////
  ///*****                      Default Color Settings                       *****///
  ///*****                                                                   *****///
  ///*****     Select Default Colors for certain LED sequences using the     *****///
  ///*****                       integer values below:                       *****///
  ///*****                                                                   *****///
  ///*****             Basic Color Integer Values                            *****///
  ///*****               0 = Off (Black}                                     *****///
  ///*****               1 = Red                                             *****///
  ///*****               2 = Yellow                                          *****///
  ///*****               3 = Green                                           *****///
  ///*****               4 = Cyan (Aqua)                                     *****///
  ///*****               5 = Blue                                            *****///
  ///*****               6 = Magenta                                         *****///
  ///*****               7 = Orange                                          *****///
  ///*****               8 = White                                           *****///
  ///*****                                                                   *****///
  ///////////////////////////////////////////////////////////////////////////////////  
    int defaultColor = 5;     // Blue, Color integer value for the hue of default sequence. This is to appease MKelly and MKarp's whiney little asses, lol
    int shortColor   = 7;     // Orange, Color integer value for the hue of ShortCircuit Message.  


////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                  ///
///                                  End User Adjustable Settings                                    ///
///                                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////
///*****  Sequence/Function Varaiables, Containers & Counters *****///
//////////////////////////////////////////////////////////////////////
unsigned long tF=0;
unsigned long tR=0;
unsigned long tT=0;

unsigned long tFcounter=0;
unsigned long tRcounter=0;
unsigned long tTcounter=0;

unsigned long Finterval= 100;
unsigned long Rinterval= 100;
unsigned long Tinterval= 100;

unsigned long SCintervalF= 10;
unsigned long SCintervalR= 10;
unsigned long SCintervalT= 10;

int SCloopF=0;
int SCloopR=0;
int SCloopT=0;
int SCflagF=0;
int SCflagR=0;
int SCflagT=0;

byte FFrame = 0;  
byte RFrame = 0;
byte TFrame = 0;

int RCinputCh1;

long twitchFLEDTime = 4000;  // LEDS Start Up 4 seconds after boot
long twitchRLEDTime = 4000;  // LEDS Start Up 4 seconds after boot
long twitchTLEDTime = 4000;  // LEDS Start Up 4 seconds after boot
long twitchFHPTime  = 4000;  // HPs Twitch seconds after boot
long twitchRHPTime  = 4000;  // HPs Twitch seconds after boot
long twitchTHPTime  = 4000;  // HPs Twitch seconds after boot

long twitchFLEDRunTime = (1000*random(FLEDTwitchRunInterval[0],FLEDTwitchRunInterval[1]));  // Randomly sets initial Front LED Twitch Run Time value
long twitchRLEDRunTime = (1000*random(RLEDTwitchRunInterval[0],RLEDTwitchRunInterval[1]));  // Randomly sets initial Reart LED Twitch Run Time value
long twitchTLEDRunTime = (1000*random(TLEDTwitchRunInterval[0],TLEDTwitchRunInterval[1]));  // Randomly sets initial Top LED Twitch Run Time value


long int OECounter = 0;
int OEFlag = 0;

//////////////////////////////////////////////////////////////////////
///*****        Command Varaiables, Containers & Flags        *****///
//////////////////////////////////////////////////////////////////////
int commandInt;
char inputBuffer[5];  
String inputString = "";         // a string to hold incoming data
volatile boolean stringComplete  = false;      // whether the serial string is complete

int typeState;
int functionState;
int colorState;

byte FHP_command[4] = {0,0,0,0};  
byte RHP_command[4] = {0,0,0,0}; 
byte THP_command[4] = {0,0,0,0};
byte FLED_command[4] = {0,0,0,0};  
byte RLED_command[4] = {0,0,0,0}; 
byte TLED_command[4] = {0,0,0,0};

int commandLength;

//////////////////////////////////////////////////////////////////////
///*****                  Color Values & Labels               *****///
//////////////////////////////////////////////////////////////////////
   
    const uint32_t red     = 0xFF0000;
    const uint32_t orange  = 0xFF8000;
    const uint32_t yellow  = 0xFFFF00;
    const uint32_t green   = 0x00FF00;
    const uint32_t cyan    = 0x00FFFF;
    const uint32_t blue    = 0x0000FF;
    const uint32_t magenta = 0xFF00FF;
    const uint32_t purple  = 0x800080;
    const uint32_t white   = 0xFFFFFF;
    const uint32_t off     = 0x000000;

    const uint32_t basicColors[10][7] = {{    off,      off,      off,      off,      off,      off, off},
                                        {    red,    white, 0xFB6161, 0xFFA0A0, 0xFD5555, 0xFFD3D3, off},
                                        { yellow,    white, 0xFDFD43, 0xFFFF82, 0xFFFFBA, 0xFEDED7, off},
                                        {  green,    white, 0x57FC57, 0x80FC80, 0xBDFFB1, 0xDDFEDD, off},
                                        {   cyan,    white, 0x38FFFF, 0x71FDFD, 0xA4FDFD, 0xCCFEFE, off},
                                        {   blue,    white, 0xACACFF, 0x7676FF, 0x5A5AFF, 0x3D3DFF, off},
                                        {magenta,    white, 0xFB3BFB, 0xFD75FD, 0xFD9EFD, 0xFDCEFD, off},
                                        { orange,    white, 0xFB9B3A, 0xFFBE7D, 0xFCD2A7, 0xFDE9D5, off},
                                        { purple,    white, 0xA131A1, 0x9B449B, 0xBD5FBD, 0xD08BD0, off},
                                        {  white, 0xC2C0C0, 0xB7B6B6, 0x858484, 0xA09F9F, 0xD1D1D1, off}};


//////////////////////////////////////////////////////////////////////
///*****               Initialize NeoPixel Strips             *****///
//////////////////////////////////////////////////////////////////////
Adafruit_NeoPixel stripF = Adafruit_NeoPixel(7, LEDpins[0], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripR = Adafruit_NeoPixel(7, LEDpins[1], NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripT = Adafruit_NeoPixel(7, LEDpins[2], NEO_GRB + NEO_KHZ800);

//////////////////////////////////////////////////////////////////////
///*****                 Initialize Servo Board              *****///
//////////////////////////////////////////////////////////////////////
Servos servos(ServoI2CAddress);


void setup() {
  
  randomSeed(analogRead(0));               // Seeds psuedo-random number generator with current value on unconnected anolog pin to make random more randomy.
 
  Serial.begin(9600);                      // Starts Serial with a baudrate of 9600
  inputString.reserve(10);                 // reserve 50 bytes for the inputString
 
  Wire.begin(I2CAddress);                  // Connects to I2C Bus and establishes address.
  Wire.onReceive(i2cEvent);                // Register event so when we receive something we jump to i2cEvent();


  
//////////////////////////////////////////////////////
///                  RC Input Setup                ///
//////////////////////////////////////////////////////

  pinMode(RCpin, INPUT);

//////////////////////////////////////////////////////
///                     OE Setup                   ///
//////////////////////////////////////////////////////
  if(enableOE==1) {pinMode(oepin, OUTPUT);}

//////////////////////////////////////////////////////
///                 HP Servo Setup                 ///
//////////////////////////////////////////////////////
    centerHP(0);    // Centers Front HP
    centerHP(1);    // Centers Rear HP
    centerHP(2);    // Centers Top HP

///////////////////////////////////////////////////////
///                  HP LED Setup                  ///
//////////////////////////////////////////////////////    

//***  FRONT HP SET UP  ***///
  stripF.begin(); 
  stripF.setBrightness(BRIGHT);
  stripF.show();
//***  REAR HP SET UP  ***///
  stripR.begin(); 
  stripR.setBrightness(BRIGHT);
  stripR.show();
//***  TOP HP SET UP   ***///
  stripT.begin(); 
  stripT.setBrightness(BRIGHT);
  stripT.show();


  if(enableBasicHP == 1) {Serial.println("Basic HP Positioning Enabled"); }
  else {Serial.println("Basic HP Positioning Disabled");}
}

void loop(){
  Servos::move(millis());  // this performs the actual moves
  tF=millis();
  tR=millis();
  tT=millis();


  if(stringComplete) {
    inputString.toCharArray(inputBuffer, 6);inputString="";                    // If a complete Command string has been flagged, write the string to array to process. 
    commandLength = (sizeof(inputBuffer) / sizeof(inputBuffer[0]));            //  Determines length of command character array.  
    if( inputBuffer[0]=='F' ||      // Front HP
        inputBuffer[0]=='R' ||      // Rear HP
        inputBuffer[0]=='T' ||      // Top HP
        inputBuffer[0]=='A'         // All three HPs
      ) {  
        if(commandLength >= 2) {typeState = inputBuffer[1]-'0';}
        if(commandLength >= 4) {functionState = (inputBuffer[2]-'0')*10+(inputBuffer[3]-'0');} 
        if(commandLength >= 5) {colorState = inputBuffer[4]-'0';} 
        else {
              if(functionState==5) {colorState = shortColor;}
              else {colorState = defaultColor;}
        }
        if(inputBuffer[0]=='F' || inputBuffer[0]=='A') {
          if (typeState == 0) { FLED_command[0]   = '\0';  // Flushes Array
                                FLED_command[0] = functionState;
                                FLED_command[1] = colorState; 
                                varResets(0);
                                }
          else if (typeState == 1) { FHP_command[0]   = '\0';  // Flushes Array
                                     FHP_command[0] = functionState;
                                     enableTwitchRHP=0;           // Toggle Auto HP Twitch off so it doesn't interupt sequence
                                     }
        }

        if(inputBuffer[0]=='R' || inputBuffer[0]=='A') {
          if (typeState == 0) { RLED_command[0]   = '\0';  // Flushes Array
                                RLED_command[0] = functionState;
                                RLED_command[1] = colorState; 
                                varResets(1);
                                }
          else if (typeState == 1) { RHP_command[0]   = '\0';  // Flushes Array
                                     RHP_command[0] = functionState;
                                     enableTwitchRHP=0;           // Toggle Auto HP Twitch off so it doesn't interupt sequence
                                     }
        } 
      
        if(inputBuffer[0]=='T' || inputBuffer[0]=='A') {
          if (typeState == 0) { TLED_command[0]   = '\0';  // Flushes Array
                                TLED_command[0] = functionState;
                                TLED_command[1] = colorState; 
                                varResets(2);
                                }
          else if (typeState == 1) { THP_command[0]   = '\0';  // Flushes Array
                                     THP_command[0] = functionState;
                                     enableTwitchTHP=0;           // Toggle Auto HP Twitch off so it doesn't interupt sequence
                                     }
        }
      }
      else if (inputBuffer[0]=='S') {                     // Major Sequences (Both LEDS and HP Servos)
        if(commandLength >= 2) {
             functionState = (inputBuffer[1]-'0');
             switch (functionState) { 
               case 1: varResets(0); varResets(1); varResets(2);                         // Resets the Variables for LEDs on all 3 HPs
                       enableTwitchFHP=0;enableTwitchRHP=0;enableTwitchTHP=0;            // Disables Auto HP Twith on all HPs
                       downHP(0);                                                        // Moves Front HP to Down Position
                       ledOFF(1);ledOFF(2);                                              // Turns Off LEDs on Rear and Top HPs
                       RLED_command[0]   = '\0'; TLED_command[0]   = '\0';               // Flushes Command Array for Rear and Top HP
                       FLED_command[0] = 1;                                              // Set Leia LED Sequence to run each loop.
                       break;
               case 8: enableTwitchFHP=0;enableTwitchRHP=0;enableTwitchTHP=0;            // Disables Auto HP Twith on all HPs'
                       FLED_command[0]='\0';RLED_command[0] ='\0';TLED_command[0]='\0';  // Flushes Command Array for Rear and Top HP
                       ledOFF(0);ledOFF(1);ledOFF(2);                                    // Turns Off LEDs on all HPs
                       enableTwitchFLED=0;enableTwitchRLED=0;enableTwitchTLED=0;         // Disables Auto LED Twith on all HPs
                       break;
               case 9: enableTwitchFHP=1;enableTwitchRHP=1;enableTwitchTHP=1;            // Enables Auto HP Twith on all HPs'
                       FLED_command[0]='\0';RLED_command[0] ='\0';TLED_command[0]='\0';  // Flushes Command Array for Rear and Top HP
                       ledOFF(0);ledOFF(1);ledOFF(2);                                    // Turns Off LEDs on all HPs
                       enableTwitchFLED=1;enableTwitchRLED=1;enableTwitchTLED=1;         // Enables Auto LED Twith on all HPs
                       break;
             }
         }
      }
      ///***  Clear States and Reset for next command.  ***///
       inputString = "";
       stringComplete = false; 
       inputBuffer[0] = '\0';
       int displayState;
       int typeState;
       int colorState;
       int functionState;   
    }

      
  
  switch (FLED_command[0]) {
    case 1: leiaLED(0);break;
    case 2: colorProjectorLED(0,FLED_command[1]); break; 
    case 3: dimPulse(0, FLED_command[1]); break;
    case 4: cycle(0,FLED_command[1]); break;
    case 5: ShortCircuit(0,FLED_command[1]); break;
    case 6: ledColor(0,FLED_command[1]);FLED_command[0]='\0'; break;
    case 7: rainbow(0); break;   
    case 98: ledOFF(0); enableTwitchFLED=0; FLED_command[0]='\0'; break;   // Clear Function, Disable Random LED Twitch
    case 99: ledOFF(0); enableTwitchFLED=1; FLED_command[0]='\0'; break;   // Clear Function, Enable Random LED Twitch
    default: break;
  }

  switch (FHP_command[0]) {
    case 1: centerHP(0); FHP_command[0]='\0'; break; 
    case 2: downHP(0); FHP_command[0]='\0'; break;
    case 3: upHP(0); FHP_command[0]='\0'; break;
    case 4: RCHP(0);break;
    case 5: twitchHP(0); FHP_command[0]='\0'; break;
    case 98: enableTwitchFHP=0; FHP_command[0]='\0'; break;   // Clear Function, Disable Servo Random Twitch
    case 99: enableTwitchFHP=1; FHP_command[0]='\0'; break;   // Clear Function, Enable Servo Random Twitch
    default: break;
  }




  switch (RLED_command[0]) {
    case 1: leiaLED(1);break;
    case 2: colorProjectorLED(1,RLED_command[1]); break; 
    case 3: dimPulse(1,RLED_command[1]); break;
    case 4: cycle(1,RLED_command[1]); break;
    case 5: ShortCircuit(1,RLED_command[1]); break;
    case 6: ledColor(1,RLED_command[1]); RLED_command[0]='\0'; break;
    case 7: rainbow(1); break;
    case 98: ledOFF(1); enableTwitchRLED=0; RLED_command[0]='\0'; break;   // Clear Function, Disable Random LED Twitch
    case 99: ledOFF(1); enableTwitchRLED=1; RLED_command[0]='\0'; break;   // Clear Function, Enable Random LED Twitch
    default: break;
  }

  switch (RHP_command[0]) {
    case 1: centerHP(1); RHP_command[0]='\0'; break; 
    case 2: downHP(1); RHP_command[0]='\0'; break;
    case 3: upHP(1); RHP_command[0]='\0'; break;
    case 4: RCHP(1);break;
    case 5: twitchHP(1); RHP_command[0]='\0'; break;
    case 98: enableTwitchRHP=0; RHP_command[0]='\0'; break;   // Clear Function, Disable Servo Random Twitch
    case 99: enableTwitchRHP=1; RHP_command[0]='\0'; break;   // Clear Function, Enable Servo Random Twitch
    default: break;
  }


  switch (TLED_command[0]) {
    case 1: leiaLED(2);break;
    case 2: colorProjectorLED(2,TLED_command[1]); break; 
    case 3: dimPulse(2,TLED_command[1]); break;
    case 4: cycle(2,TLED_command[1]); break;
    case 5: ShortCircuit(2,TLED_command[1]); break;
    case 6: ledColor(2,TLED_command[1]); TLED_command[0]='\0'; break;
    case 7: rainbow(2); break;
    case 98: ledOFF(2); enableTwitchTLED=0; TLED_command[0]='\0'; break;   // Clear Function, Disable Random LED Twitch
    case 99: ledOFF(2); enableTwitchTLED=1; TLED_command[0]='\0'; break;   // Clear Function, Enable Random LED Twitch
    default: break;
  }

  switch (THP_command[0]) {
    case 1: centerHP(2); THP_command[0]='\0'; break; 
    case 2: downHP(2); THP_command[0]='\0'; break;
    case 3: upHP(2); THP_command[0]='\0'; break;
    case 4: RCHP(2);break;
    case 5: twitchHP(2); THP_command[0]='\0'; break;
    case 98: enableTwitchTHP=0; THP_command[0]='\0'; break;   // Clear Function, Disable Servo Random Twitch
    case 99: enableTwitchTHP=1; THP_command[0]='\0'; break;   // Clear Function, Random Twitch
    default: break;
  }

  
 

  if (tF > twitchFLEDTime && enableTwitchFLED == 1)   {leiaLEDtwitch(0);}
  if (tR > twitchRLEDTime && enableTwitchRLED == 1)   {leiaLEDtwitch(1);}
  if (tF > twitchTLEDTime && enableTwitchTLED == 1)   {leiaLEDtwitch(2);}
  
  if (tF > twitchFHPTime && enableTwitchFHP == 1) { twitchHP(0); twitchFHPTime = (1000*random(FHPTwitchInterval[0],FHPTwitchInterval[1]))+millis();} 
  if (tR > twitchRHPTime && enableTwitchRHP == 1) { twitchHP(1); twitchRHPTime = (1000*random(RHPTwitchInterval[0],RHPTwitchInterval[1]))+millis();}   
  if (tT > twitchTHPTime && enableTwitchTHP == 1) { twitchHP(2); twitchTHPTime = (1000*random(THPTwitchInterval[0],THPTwitchInterval[1]))+millis();} 

  if(millis()-OECounter>=SERVO_SPEED[1] && OEFlag == 1) {        // Checks the time since the last enable Servo flag was set, to ensure the servos were given plenty of 
              digitalWrite(oepin, HIGH);                         // time to execute thier full movement before diasabling servos on breakoutboard.    This is done to stop 
              OEFlag = 0;                                        // servo hum if it is a problem.
  }
              
}

int mapPulselength(int microseconds) {
  int y = map(microseconds, 1000, 2000, 200, 550);
  return y;
}


//////////////////////////////////////////////////////////////////////////////////////////
///*****                       OE Servo Enable Function                           *****///
//////////////////////////////////////////////////////////////////////////////////////////

 void enableServos() {
  if(enableOE==1) {                // If OE control of the Servo Break Out Board is enabled
   digitalWrite(oepin, LOW);       // Set OE Pin to low to enable Servo Movement 
   OEFlag = 1;                     // Set OE Flag so loop knows servos are enabled
   OECounter = millis();           // Set OE Timer to current millis so we can eventually run disableServos after movement is complete 
  }
 }

 
//////////////////////////////////////////////////////////////////////////////////////////
///*****                     HP Twitch/Movement Functions                         *****///
//////////////////////////////////////////////////////////////////////////////////////////
    void centerHP(byte mode){
     int pos1;
     int pos2; 
     enableServos();
     if(enableBasicHP == 1) {
        pos1 = mapPulselength(HPposBasic[0][0]);
        pos2 = mapPulselength(HPposBasic[1][0]);
      }
      else {
        pos1 = mapPulselength(HPpos[0][1][0]);
        pos2 = mapPulselength(HPpos[0][1][1]);
      }
      switch (mode) {
        case 0: servos.moveTo(HPpins[0][0],SERVO_SPEED[0],pos1);servos.moveTo(HPpins[0][1],SERVO_SPEED[0],pos2); break;
        case 1: servos.moveTo(HPpins[1][0],SERVO_SPEED[0],pos1);servos.moveTo(HPpins[1][1],SERVO_SPEED[0],pos2); break;
        case 2: servos.moveTo(HPpins[2][0],SERVO_SPEED[0],pos1);servos.moveTo(HPpins[2][1],SERVO_SPEED[0],pos2); break;
        default: break;
      }
    }
   
    void downHP(byte mode){
      if(enableBasicHP != 1) {      //  Only Works when Basic Position Mode is Disabled because it relies on specific position Coordinates
        enableServos();
        switch (mode) {
          case 0: servos.moveTo(HPpins[0][0],SERVO_SPEED[0],mapPulselength(HPpos[0][0][0]));servos.moveTo(HPpins[0][1],SERVO_SPEED[0],mapPulselength(HPpos[0][0][1])); break;
          case 1: servos.moveTo(HPpins[1][0],SERVO_SPEED[0],mapPulselength(HPpos[1][0][0]));servos.moveTo(HPpins[1][1],SERVO_SPEED[0],mapPulselength(HPpos[1][0][1])); break;
          case 2: servos.moveTo(HPpins[2][0],SERVO_SPEED[0],mapPulselength(HPpos[2][0][0]));servos.moveTo(HPpins[2][1],SERVO_SPEED[0],mapPulselength(HPpos[2][0][1])); break;
          default: break;
        }
      }
    }
    
    void upHP(byte mode){      //  Only Works when Basic Position Mode is Disabled because it relies on specific position Coordinates
      if(enableBasicHP != 1) {
        enableServos();
        switch (mode) {
          case 0: servos.moveTo(HPpins[0][0],SERVO_SPEED[0],mapPulselength(HPpos[0][2][0]));servos.moveTo(HPpins[0][1],SERVO_SPEED[0],mapPulselength(HPpos[0][2][1])); break;
          case 1: servos.moveTo(HPpins[1][0],SERVO_SPEED[0],mapPulselength(HPpos[1][2][0]));servos.moveTo(HPpins[1][1],SERVO_SPEED[0],mapPulselength(HPpos[1][2][1])); break;
          case 2: servos.moveTo(HPpins[2][0],SERVO_SPEED[0],mapPulselength(HPpos[2][2][0]));servos.moveTo(HPpins[2][1],SERVO_SPEED[0],mapPulselength(HPpos[2][2][1])); break;
          default: break;
        }
      }
    }


    
    void twitchHP(byte mode) {
     int pos1;
     int pos2; 
     enableServos();   
     if(enableBasicHP == 1) {
        pos1 = mapPulselength(random(HPposBasic[0][1],HPposBasic[0][2]));
        pos2 = mapPulselength(random(HPposBasic[1][1],HPposBasic[1][2]));
      }
      else {
        int posInt = random(0,6);
        pos1 = mapPulselength(HPpos[0][posInt][0]);
        pos2 = mapPulselength(HPpos[0][posInt][1]);
      }
     int speed = random(SERVO_SPEED[0],SERVO_SPEED[1]);
      switch (mode) {
        case 0: servos.moveTo(HPpins[0][0],speed,pos1);servos.moveTo(HPpins[0][1],speed,pos2); break;
        case 1: servos.moveTo(HPpins[1][0],speed,pos1);servos.moveTo(HPpins[1][1],speed,pos2); break;
        case 2: servos.moveTo(HPpins[2][0],speed,pos1);servos.moveTo(HPpins[2][1],speed,pos2); break;
        default: break;
      }     
    }

    void RCHP(byte mode) {
       
       int servo1;
       int servo2;
       if(enableBasicHP != 1) {
           RCinputCh1 = pulseIn(RCpin, HIGH); // each channel
           Serial.println(RCinputCh1);
           enableServos();
           switch (mode) {
            case 0: servo1 = map(RCinputCh1, RCRange[0], RCRange[1], HPpos[0][0][0], HPpos[0][2][0]);
                    servo2 = map(RCinputCh1, RCRange[0], RCRange[1], HPpos[0][0][1], HPpos[0][2][1]);
                    servos.moveTo(HPpins[0][0],0,mapPulselength(servo1));servos.moveTo(HPpins[0][1],0,mapPulselength(servo2)); break;
            case 1: servo1 = map(RCinputCh1, RCRange[0], RCRange[1], HPpos[1][0][0], HPpos[1][2][0]);
                    servo2 = map(RCinputCh1, RCRange[0], RCRange[1], HPpos[1][0][1], HPpos[1][2][1]);
                    servos.moveTo(HPpins[1][0],0,mapPulselength(servo1));servos.moveTo(HPpins[1][1],0,mapPulselength(servo2)); break;
            case 2: servo1 = map(RCinputCh1, RCRange[0], RCRange[1], HPpos[2][0][0], HPpos[2][2][0]);
                    servo2 = map(RCinputCh1, RCRange[0], RCRange[1], HPpos[2][0][1], HPpos[2][2][1]);
                    servos.moveTo(HPpins[2][0],0,mapPulselength(servo1));servos.moveTo(HPpins[2][1],0,mapPulselength(servo2)); break;
            default: break;
          } 
       }      
    }


//////////////////////////////////////////////////////////////////////////////////////////
///*****                            HP LED Functions                              *****///
//////////////////////////////////////////////////////////////////////////////////////////

     void ledOFF(byte mode) {
      switch (mode) {
        case 0: for(int i=0; i<=6; i++) {stripF.setPixelColor(i,off);} stripF.show(); break;
        case 1: for(int i=0; i<=6; i++) {stripR.setPixelColor(i,off);} stripR.show(); break;
        case 2: for(int i=0; i<=6; i++) {stripT.setPixelColor(i,off);} stripT.show(); break;
        default: break;
      }
    }

      
     void ledColor(byte mode, int c) {
      switch (mode) {
        case 0: for(int i=0; i<=6; i++) {stripF.setPixelColor(i,basicColors[c][0]);} stripF.show(); break;
        case 1: for(int i=0; i<=6; i++) {stripR.setPixelColor(i,basicColors[c][0]);} stripR.show(); break;
        case 2: for(int i=0; i<=6; i++) {stripT.setPixelColor(i,basicColors[c][0]);} stripT.show(); break; 
        default: break;
      }
    }


      
      //////////////////////////////////////////////////////////
      ///*****       Leia Function          *****///
      //////////////////////////////////////////////////////////
      void leiaLED(byte mode) {
        switch (mode) {
            case 0: if ((millis() - tFcounter) > Finterval) {
                      for(int i=0; i<=6; i++) { stripF.setPixelColor(i,basicColors[defaultColor][random(0,7)]); }
                      stripF.show();
                      tFcounter=millis();
                      Finterval=random(50,150);
                     }
                     break;
            case 1: if ((millis() - tRcounter) > Rinterval) {
                      for(int i=0; i<=6; i++) { stripR.setPixelColor(i,basicColors[defaultColor][random(0,7)]); }
                      stripR.show();
                      tRcounter=millis();
                      Rinterval=random(50,150);
                    }
                    break;
            case 2: if ((millis() - tTcounter) > Tinterval) {
                      for(int i=0; i<=6; i++) { stripT.setPixelColor(i,basicColors[defaultColor][random(0,7)]); }
                      stripT.show();
                      tTcounter=millis();
                      Tinterval=random(50,150);
                     }
                     break;
          }
        }
        
 
      void leiaLEDtwitch(int mode) {             
          switch (mode) {
            case 0: if((millis()-twitchFLEDTime) < twitchFLEDRunTime) {leiaLED(0);} else {ledOFF(0);twitchFLEDTime = (1000*random(FLEDTwitchInterval[0],FLEDTwitchInterval[1]))+millis();twitchFLEDRunTime = (1000*random(FLEDTwitchRunInterval[0],FLEDTwitchRunInterval[1])); } break;
            case 1: if((millis()-twitchRLEDTime) < twitchRLEDRunTime) {leiaLED(1);} else {ledOFF(1);twitchRLEDTime = (1000*random(RLEDTwitchInterval[0],RLEDTwitchInterval[1]))+millis();twitchRLEDRunTime = (1000*random(RLEDTwitchRunInterval[0],RLEDTwitchRunInterval[1])); } break;
            case 2: if((millis()-twitchTLEDTime) < twitchTLEDRunTime) {leiaLED(2);} else {ledOFF(2);twitchTLEDTime = (1000*random(TLEDTwitchInterval[0],TLEDTwitchInterval[1]))+millis();twitchTLEDRunTime = (1000*random(TLEDTwitchRunInterval[0],TLEDTwitchRunInterval[1])); } break;
            default: break;
          }
      }

      //////////////////////////////////////////////////////////
      ///*****       Coler Projector Function          *****///
      ///                                                   ///
      ///   Same as Leia Function above but can pic color   ///
      ///                                                   ///
      //////////////////////////////////////////////////////////
      void colorProjectorLED(byte mode, int c) {
        switch (mode) {
            case 0: if ((millis() - tFcounter) > Finterval) {
                      for(int i=0; i<=6; i++) { stripF.setPixelColor(i,basicColors[c][random(0,7)]); }
                      stripF.show();
                      tFcounter=millis();
                      Finterval=random(50,150);
                     }
                     break;
            case 1: if ((millis() - tRcounter) > Rinterval) {
                      for(int i=0; i<=6; i++) { stripR.setPixelColor(i,basicColors[c][random(0,7)]); }
                      stripR.show();
                      tRcounter=millis();
                      Rinterval=random(50,150);
                    }
                    break;
            case 2: if ((millis() - tTcounter) > Tinterval) {
                      for(int i=0; i<=6; i++) { stripT.setPixelColor(i,basicColors[c][random(0,7)]); }
                      stripT.show();
                      tTcounter=millis();
                      Tinterval=random(50,150);
                     }
                     break;
          }
        }
        
    /////////////////////////////////////////////////////////
    ///*****           Dim Pulse Function            *****///
    /////////////////////////////////////////////////////////
        
      void dimPulse(byte mode, int c) {
        int interval = 15;
        long elapsed;
        int frame;
        switch (mode) {
            case 0: if ((millis() - tFcounter) > Finterval) {
                      elapsed = millis() - tFcounter;
                      frame = elapsed/interval;
                      if(frame >= 64) {tFcounter = millis();}     
                      if (frame > 32) {frame = 32-(frame - 32);}
                      if(elapsed>=interval) {
                        for(int i=0; i<=6; i++) {stripF.setPixelColor(i, dimColorVal(c,(frame*8)));}
                        stripF.show();
                      }
                    }         
                    break;
            case 1: if ((millis() - tRcounter) > Rinterval) {
                      elapsed = millis() - tFcounter;
                      frame = elapsed/interval;
                      if(frame >= 64) {tRcounter = millis();}     
                      if (frame > 32) {frame = 32-(frame - 32);}
                      if(elapsed>=interval) {
                        for(int i=0; i<=6; i++) {stripR.setPixelColor(i, dimColorVal(c,(frame*8)));}
                        stripR.show();
                      }
                    }
                    break;
            case 2: if ((millis() - tTcounter) > Tinterval) {
                       elapsed = millis() - tTcounter;
                       frame = elapsed/interval;
                       if(frame >= 64) {tTcounter = millis();}     
                       if (frame > 32) {frame = 32-(frame - 32);}
                       if(elapsed>=interval) {
                         for(int i=0; i<=6; i++) {stripT.setPixelColor(i, dimColorVal(c,(frame*8)));}
                         stripT.show();
                        }
                    }
                    break;
        }
      }


      void ShortCircuit(byte mode, int c) {
       //int interval;
        switch (mode) {
            case 0: if(SCloopF<=20) {
                    if ((millis() - tFcounter) > SCintervalF) {
                      if(SCflagF==0) {
                        for(int i=0; i<=6; i++) { stripF.setPixelColor(i,off); }  
                        SCflagF=1;
                        SCintervalF = 10+(SCloopF*random(15,25));
                        } 
                      else {
                        for(int i=0; i<=6; i++) { stripF.setPixelColor(i,basicColors[c][random(0,7)]); }
                        stripF.show();
                        SCflagF=0;
                        SCloopF++;
                        }
                       tFcounter = millis();
                       stripF.show();
                      }  
                    }     
                    break;
            case 1: if(SCloopR<=20) {
                    if ((millis() - tRcounter) > SCintervalR) {
                      if(SCflagR==0) {
                        for(int i=0; i<=6; i++) { stripR.setPixelColor(i,off); }
                        SCflagR=1;
                        SCintervalR = 10+(SCloopR*random(15,20));
                        } 
                      else {
                        for(int i=0; i<=6; i++) { stripR.setPixelColor(i,basicColors[c][random(0,7)]); }
                        SCflagR=0;
                        SCloopR++;
                        }
                       tRcounter = millis();
                       stripR.show();
                      }
                      }         
                    break;
            case 2: //interval = 10+(SCloopT*20);
                   if(SCloopT<=20) {
                    if ((millis() - tTcounter) > SCintervalT) {
                      if(SCflagT==0) {
                        for(int i=0; i<=6; i++) { stripT.setPixelColor(i,off); }
                        SCflagT=1;
                        SCintervalT = 10+(SCloopT*random(15,25));
                        } 
                      else {
                        for(int i=0; i<=6; i++) { stripT.setPixelColor(i,basicColors[c][random(0,7)]); }
                        SCflagT=0;
                        SCloopT++;
                        }
                        tTcounter = millis();
                        stripT.show();
                      }      
                   }
        }
      }

            
    /////////////////////////////////////////////////////////
    ///*****           Cycle Function            *****///
    /////////////////////////////////////////////////////////

      void cycle(byte mode, int c) { 
        byte frame;
        int interval = 75;
        switch (mode) {
            case 0: if ((millis() - tFcounter) > interval) {
                      tFcounter=millis();
                      if (FFrame >= 6) {FFrame=0;}
                      for (int i = 0; i <=5; i++) { 
                        if (i == FFrame) {stripF.setPixelColor(i+1, basicColors[c][0]);}
                        else {stripF.setPixelColor(i+1, off);}                      
                      }
                      stripF.show(); 
                      FFrame++;
                    }
                    break;
            case 1: if ((millis() - tRcounter) > interval) {
                      tRcounter=millis();
                      if (RFrame >= 6) {RFrame=0;}
                      for (int i = 0; i <=5; i++) { 
                        if (i == RFrame) {stripR.setPixelColor(i+1, basicColors[c][0]);}
                        else {stripR.setPixelColor(i+1, off);}                      
                      }
                      stripR.show();  
                      RFrame++;
                    }
                    break;
            case 2: if ((millis() - tTcounter) > interval) {
                       tTcounter=millis();
                       if (TFrame >= 6) {TFrame=0;}
                       for (int i = 0; i <=5; i++) { 
                          if (i == TFrame) {stripT.setPixelColor(i+1, basicColors[c][0]);}
                          else {stripT.setPixelColor(i+1, off);}                      
                        }
                        stripT.show();  
                        TFrame++;
                     }
                     break;
        }
      }




      ///////////////////////////////////////////////////////
      ///*****           Rainbow Functions           *****///
      ///////////////////////////////////////////////////////
      
      void rainbow(byte mode) {
        int interval = 10;
        long elapsed;
        byte frame;        
        switch (mode) {
            case 0: elapsed = millis() - tFcounter;
                    frame = elapsed/interval;
                    if(frame > 256*5) { tFcounter=millis(); }
                    else {        
                      for(int i=0; i< 7; i++) {
                        stripF.setPixelColor(i, Wheel(((i * 256 / 7) + frame) & 255));
                      }
                      if(elapsed>=interval) {stripF.show();}
                     }
                     break;
            case 1: elapsed = millis() - tRcounter;
                    frame = elapsed/interval;
                    if(frame > 256*5) { tRcounter=millis(); }
                    else {        
                      for(int i=0; i< 7; i++) {
                        stripR.setPixelColor(i, Wheel(((i * 256 / 7) + frame) & 255));
                      }
                      if(elapsed>=interval) {stripR.show();}
                     }
                     break;
            case 2: elapsed = millis() - tTcounter;
                    frame = elapsed/interval;
                    if(frame > 256*5) { tTcounter=millis(); }
                    else {        
                      for(int i=0; i< 7; i++) {
                        stripT.setPixelColor(i, Wheel(((i * 256 / 7) + frame) & 255));
                      }
                      if(elapsed>=interval) {stripT.show();}
                     }
                     break;
        }
      }


      /////////////////////////////////////////////////////////
      ///*****             Wheel Function              *****///
      /////////////////////////////////////////////////////////
      /// Input a value 0 to 255 to get a color value. The  ///   
      /// colours are a transition r - g - b - back to r.   ///
      /////////////////////////////////////////////////////////
      
      uint32_t Wheel(byte WheelPos) {
        if(WheelPos < 85) {
         return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        } else if(WheelPos < 170) {
         WheelPos -= 85;
         return Color(255 - WheelPos * 3, 0, WheelPos * 3);
        } else {
         WheelPos -= 170;
         return Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
      } 
      
      /////////////////////////////////////////////////////////
      ///*****          Dim Color Function 1           *****///
      /////////////////////////////////////////////////////////
      /// Input a value 0 to 255 to get a color value at a  ///   
      /// specific brightness.                              ///
      /// Takes and int color value and returns an          ///
      /// uint32_tcolor value.                             ///
      /// Used for soft fade in/outs.                      ///
      /////////////////////////////////////////////////////////  
      
      uint32_t dimColorVal(int c, int brightness) {
        switch (c) {                                                
          case 1: return Color(255/brightness, 0, 0); break;
          case 2: return Color(255/brightness, 255/brightness, 0);break;
          case 3: return Color(0, 255/brightness, 0); break;
          case 4: return Color(0, 255/brightness, 255/brightness);break;
          case 5: return Color(0, 0, 255/brightness); break;
          case 6: return Color(255/brightness, 0, 255/brightness); break;
          case 7: return Color(255/brightness, 180/brightness, 0); break;
          case 8: return Color(255/brightness, 255/brightness, 255/brightness); break;
          case 9: return off; break;
          default: return off; break;                                          
        }   
      }  

      /////////////////////////////////////////////////////////
      ///*****    RGB to Hex Color Value Converter      *****///
      /////////////////////////////////////////////////////////
      ///    Converts and returns a color's 3 seperat RGB   /// 
      ///       value as an uint32_t value.                 ///   
      /////////////////////////////////////////////////////////        

      uint32_t Color(byte r, byte g, byte b)
       {
           uint32_t c;
           c = r;
           c <<= 8;
           c |= g;
           c <<= 8;
           c |= b;
           return c;
       } 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                               /////
/////                          Consolidates Variable Reset in Single Function                       /////
/////                                                                                               /////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////            

              
       void varResets(byte mode) {
               switch (mode) {
                  case 0: FFrame=0;
                          SCflagF=0;
                          SCloopF=0;
                          SCintervalF= 10;
                          ledOFF(0);                      // Clears Any Remaining Lit LEDs 
                          enableTwitchFLED=0;             // Toggle Auto LED Twitch off so it doesn't interupt sequence
                          break;
                  case 1: RFrame=0;
                          SCflagR=0;
                          SCloopR=0;
                          SCintervalR= 10;
                          ledOFF(1);                      // Clears Any Remaining Lit LEDs 
                          enableTwitchRLED=0;             // Toggle Auto LED Twitch off so it doesn't interupt sequence
                          break;
                  case 2: TFrame=0;
                          SCflagT=0;
                          SCloopT=0;
                          SCintervalT= 10;
                          ledOFF(2);                      // Clears Any Remaining Lit LEDs 
                          enableTwitchTLED=0;             // Toggle Auto LED Twitch off so it doesn't interupt sequence
                          break;
               }
         }


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                               /////
/////                             Serial & I2C Communication Functions                              /////
/////                                                                                               /////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////  
    
      
      /////////////////////////////////////////////////////////
      ///*****          Serial Event Function          *****///
      /////////////////////////////////////////////////////////
      /// This routine is run between loop() runs, so using ///
      /// delay inside loop can delay response.  Multiple   ///
      /// bytes of data may be available.                   ///
      /////////////////////////////////////////////////////////
      
      void serialEvent() {
        while (Serial.available()) {                         // Loop through all incoming bytes
          char inChar = (char)Serial.read();                 // Receive each byte as a character        
          if (inChar == '\r' || inChar == '\n') {            // if the incoming character is a carriage return (\r) or newline (\n), it denotes end of command
            stringComplete = true;                           // Once done, set a flag so the main loop can do something about it. 
          } 
          else { 
            inputString += inChar;                           // Add each Character to inputString     
          }
        }
      }
      /////////////////////////////////////////////////////////
      ///*****            I2C Event Function           *****///
      /////////////////////////////////////////////////////////
      ///  This routine is run when an onRecieve event is   ///
      ///     triggered.  Multiple bytes of data may be     ///
      ///                    available.                     ///
      /////////////////////////////////////////////////////////            
      
     void i2cEvent(int howMany)
      {  
         inputString = "";                                     // Ensures inputString is empty
         while(Wire.available())  {                            // loop through all incoming bytes
           char inChar = (char)Wire.read();                    // Receive each byte as a character
           inputString += inChar;                              // Add each Character to inputString
         }
         stringComplete = true;                                // Once done, set a flag so the main loop can do something about it. 
      }
