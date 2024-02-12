#include <Arduino.h>
#include <WiFiNINA.h>
#include "ArduinoJson-v6.19.4.h"
#include "OpenChessBoard.h"
#include "lichess_client.h"

                         // NOT A DEVELOPER?
#include "user_config.h" // 👈 HEAD TO THIS FILE (user_config.h)
                         // TO CONFIGURE YOUR WIFI & LICHESS SETTINGS


// WiFi variables
int status = WL_IDLE_STATUS;
char server[] = "lichess.org";  // name address for lichess (using DNS)
WiFiSSLClient StreamClient; // WIFISSLClient for move stream, always connects via SSL (port 443 for https)

//lichess variables
const char* currentGameID;
bool myturn = true;
String lastMove;
String myMove;
bool is_castling_allowed = true;

// LED and state variables
bool boot_flipstate = true;
bool is_booting = true;
bool isr_first_run = false;
bool connect_flipstate = false;
bool is_connecting = false;
bool is_game_running = false;


void setup() {
  //Initialize HW
  initHW();
  isr_setup();


#if DEBUG || HTTP_DEBUG
  // Initialize DEBUG_SERIAL and wait for port to open
  Serial.begin(9600);
  while (!Serial) delay(100);
#endif

  wifi_setup();
}

void loop() {
  is_booting = false;
  is_connecting = true;
  isr_first_run = false;
  lastMove = "xx";
  myMove = "ff";

  DEBUG_SERIAL.print(F("Connecting to Lichess…"));
  if (!StreamClient.connect(server, 443)) {
    DEBUG_SERIAL.println(CANNOT_CONNECT_TO_LICHESS);
    delay(5000);
    return;
  }

  fetch_and_print_username();
  fetch_and_print_ongoing_game();

  if (currentGameID)
  {
    DEBUG_SERIAL.println("Start move stream from game");
    getStream(StreamClient);

    delay(500);// make sure first move is catched by isr

    is_game_running = true;
    is_connecting = false;

    while (is_game_running)
    {
      while(!myturn && is_game_running){

        //isr parses move stream once a second, exits when game ends or myturn is set to true
        delay(300);
      }

      if (myturn && is_game_running && isr_first_run)
      {
        String accept_move = "none";

        //print last move if move was detected
        if (lastMove.length() > 3){
          DEBUG_SERIAL.print("opponents move: ");
          DEBUG_SERIAL.println(lastMove);

          // wait for oppents move to be played
          DEBUG_SERIAL.println("wait for move accept...");

          while(accept_move != lastMove && is_game_running){
            displayMove(lastMove);
            accept_move = getMoveInput();

            // if king move is a castling move, wait for rook move
            checkCastling(accept_move);

            }



          DEBUG_SERIAL.println("move accepted!");
        }

        // run isr at least once to catch first move of the game
        if(isr_first_run){
          //wait for my move and send it to API
          DEBUG_SERIAL.print("wait for move input...");
          postMove();
        }
      }
    }
  }

  StreamClient.stop();
}

void fetch_and_print_username() {
  String username = getUsername();
  DEBUG_SERIAL.print(F("Connected to Lichess as: "));
  DEBUG_SERIAL.println(username);
}

void fetch_and_print_ongoing_game() {
  DEBUG_SERIAL.print(F("Looking for ongoing game…"));
  GameStatus gameStatus = getGameStatus();

  currentGameID = gameStatus.gameId;
  myturn = gameStatus.isMyTurn;

  if (!currentGameID) {
    DEBUG_SERIAL.println(ONGOING_GAME_NOT_FOUND);
    DEBUG_SERIAL.println("Retrying in 5 seconds…");
    delay(5000);
  }

  DEBUG_SERIAL.print(F("Ongoing game found: https://lichess.org/"));
  DEBUG_SERIAL.println(currentGameID);
  DEBUG_SERIAL.print(F("Is it my turn? → "));
  DEBUG_SERIAL.println(myturn ? F("yes") : F("no"));
}