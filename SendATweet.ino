/*
  SendATweet

  Demonstrates sending a tweet via a Twitter account using the Temboo Arduino Yun SDK.

  This example code is in the public domain.
*/

#include <Bridge.h>
#include <Temboo.h>
#include <LiquidCrystal.h>
#include "DHT.h"

// contains Temboo account information
#define TEMBOO_ACCOUNT "judyw"  // your Temboo account name 
#define TEMBOO_APP_KEY_NAME "myFirstApp"  // your Temboo app key name
#define TEMBOO_APP_KEY  "V6Kz5nfJyReBxEpgSOOnTqY8AKRKiLSg"  // your Temboo app key

// DHT sensor info
#define DHTPIN 6 // what digital pin we're connected to
#define DHTTYPE DHT11 // what type of sensor you're using!
DHT dht(DHTPIN, DHTTYPE);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// contains Temboo account information

/*** SUBSTITUTE YOUR VALUES BELOW: ***/

// Note that for additional security and reusability, you could
// use #define statements to specify these values in a .h file.
const String TWITTER_ACCESS_TOKEN = "742945992291569664-rHPJJDaiJdodYNd4KolmCfjHfXLsbQH";
const String TWITTER_ACCESS_TOKEN_SECRET = "45sSa9rWzMg437HAXYs6pOxnSQXOBfh8bGWLbfjUUvH7N";
const String TWITTER_API_KEY = "dwX5Dqxd0jCffGrnLkQYGsOJc";
const String TWITTER_API_SECRET = "UJiWt6y5cGq1qVTVzWaZlgXk97hWNLaK2ub9pRZmpINqbM4fIw";

int numRuns = 10;   // execution count, so this sketch doesn't run forever
int maxRuns = 10;  // the max number of times the Twitter Update Choreo should run

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // initialize the serial communications:
  Serial.begin(9600);
  Serial.println("DHTxx test!");
  
  dht.begin();
  
  // for debugging, wait until a serial console is connected
  delay(4000);
  while (!Serial);
  Bridge.begin();
}

void loop()
{
  
  // Wait a few seconds between measurements
  delay(2000);
  
    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  lcd.print("Humidity: ");
  lcd.print(h);
  
  lcd.setCursor(0, 1);
  
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print(" *C ");

  String tempComm = "I'm feeling good :)! ";
  String humiComm = "No worries haha!";
  
  if(t > 30.00) {tempComm = "So hot out here :(! ";}
  if(t < 10.00) {tempComm = "I'm freezing!! ";}
  /*
  if(h > 50.00) {humiComm = "Also I feel like living in a jungle!";}
  if(h < 20.00) {humiComm = "Also I feel like living in a desert";}
  */
  // only try to send the tweet if we haven't already sent it successfully
  if (numRuns <= maxRuns) {

    Serial.println("Running SendATweet - Run #" + String(numRuns++) + "...");

    // define the text of the tweet we want to send
    //String tweetText("My Arduino Yun has been running for " + String(millis()) + " milliseconds.");
    String tweetText("The current tempreture is " + String(t) + " *C, " + tempComm);
     //+ " *C, the current humidity is " + String(h)+"%."
    

    TembooChoreo StatusesUpdateChoreo;

    // invoke the Temboo client
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    StatusesUpdateChoreo.begin();
    
    // set Temboo account credentials
    StatusesUpdateChoreo.setAccountName(TEMBOO_ACCOUNT);
    StatusesUpdateChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    StatusesUpdateChoreo.setAppKey(TEMBOO_APP_KEY);

    // identify the Temboo Library choreo to run (Twitter > Tweets > StatusesUpdate)
    StatusesUpdateChoreo.setChoreo("/Library/Twitter/Tweets/StatusesUpdate");

    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twitter/Tweets/StatusesUpdate/
    // for complete details about the inputs for this Choreo

    // add the Twitter account information
    StatusesUpdateChoreo.addInput("AccessToken", TWITTER_ACCESS_TOKEN);
    StatusesUpdateChoreo.addInput("AccessTokenSecret", TWITTER_ACCESS_TOKEN_SECRET);
    StatusesUpdateChoreo.addInput("ConsumerKey", TWITTER_API_KEY);
    StatusesUpdateChoreo.addInput("ConsumerSecret", TWITTER_API_SECRET);

    // and the tweet we want to send
    StatusesUpdateChoreo.addInput("StatusUpdate", tweetText);
    
    
    // next, we'll define two output filters that let us specify the 
    // elements of the response from Twitter that we want to receive.
    // see the examples at http://www.temboo.com/arduino
    // for more on using output filters

    // tell the Process to run and wait for the results. The
    // return code (returnCode) will tell us whether the Temboo client
    // was able to send our request to the Temboo servers
    unsigned int returnCode = StatusesUpdateChoreo.run();
      
    // a return code of zero (0) means everything worked
    if (returnCode == 0) {
      Serial.println("Success! Tweet sent!");
      
    }else {
      // a non-zero return code means there was an error
      // read and print the error message
      while (StatusesUpdateChoreo.available()) {
      char c = StatusesUpdateChoreo.read();
      Serial.print(c);
      }
    }
    StatusesUpdateChoreo.close();   
 

    // do nothing for the next 90 seconds
    Serial.println("Waiting...");
    delay(25000);
  
    Serial.println("Running ReadATweet - Run #" + String(numRuns++));
    
    TembooChoreo HomeTimelineChoreo;

    // invoke the Temboo client.
    // NOTE that the client must be reinvoked, and repopulated with
    // appropriate arguments, each time its run() method is called.
    HomeTimelineChoreo.begin();
    
    // set Temboo account credentials
    HomeTimelineChoreo.setAccountName(TEMBOO_ACCOUNT);
    HomeTimelineChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    HomeTimelineChoreo.setAppKey(TEMBOO_APP_KEY);

    // tell the Temboo client which Choreo to run (Twitter > Timelines > HomeTimeline)
    HomeTimelineChoreo.setChoreo("/Library/Twitter/Timelines/HomeTimeline");
    
    
    // set the required choreo inputs
    // see https://www.temboo.com/library/Library/Twitter/Timelines/HomeTimeline/
    // for complete details about the inputs for this Choreo

    HomeTimelineChoreo.addInput("Count", "1"); // the max number of Tweets to return from each request
    HomeTimelineChoreo.addInput("AccessToken", TWITTER_ACCESS_TOKEN);
    HomeTimelineChoreo.addInput("AccessTokenSecret", TWITTER_ACCESS_TOKEN_SECRET);
    HomeTimelineChoreo.addInput("ConsumerKey", TWITTER_API_KEY);    
    HomeTimelineChoreo.addInput("ConsumerSecret", TWITTER_API_SECRET);

    // next, we'll define two output filters that let us specify the 
    // elements of the response from Twitter that we want to receive.
    // see the examples at http://www.temboo.com/arduino
    // for more on using output filters
   
    // we want the text of the tweet
    HomeTimelineChoreo.addOutputFilter("tweet", "/[1]/text", "Response");
    
    // and the name of the author
    HomeTimelineChoreo.addOutputFilter("author", "/[1]/user/screen_name", "Response");


    // tell the Process to run and wait for the results. The 
    // return code will tell us whether the Temboo client 
    // was able to send our request to the Temboo servers
    unsigned int returnCode2 = HomeTimelineChoreo.run();
    
   // a response code of 0 means success; print the API response
    if(returnCode2 == 0) {
      
      String author; // a String to hold the tweet author's name
      String tweet; // a String to hold the text of the tweet


      // choreo outputs are returned as key/value pairs, delimited with 
      // newlines and record/field terminator characters, for example:
      // Name1\n\x1F
      // Value1\n\x1E
      // Name2\n\x1F
      // Value2\n\x1E      
      
      // see the examples at http://www.temboo.com/arduino for more details
      // we can read this format into separate variables, as follows:
      
      while(HomeTimelineChoreo.available()) {
        // read the name of the output item
        String name = HomeTimelineChoreo.readStringUntil('\x1F');
        name.trim();

        // read the value of the output item
        String data = HomeTimelineChoreo.readStringUntil('\x1E');
        data.trim();

        // assign the value to the appropriate String
        if (name == "tweet") {
          tweet = data;
        } else if (name == "author") {
          author = data;
        }
      }
      String stuff = String("@" + author + " - " + tweet);
      int len = stuff.length();
      Serial.println("@" + author + " - " + tweet);
      lcd.println("@" + author + " - " + tweet);
      for (int positionCounter = 0; positionCounter < len - 14; positionCounter++) {
      // scroll one position left:
      lcd.scrollDisplayLeft();
      delay(150);
      }
    
    } else {
      // there was an error
      // print the raw output from the choreo
      while(HomeTimelineChoreo.available()) {
        char r = HomeTimelineChoreo.read();
        Serial.print(r);
        
      }
    }

    HomeTimelineChoreo.close();

  }

  Serial.println("Waiting...");    
    delay(65000);
}
