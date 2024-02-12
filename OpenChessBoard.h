#pragma once

#include <Arduino.h>
#include <WiFiNINA.h>

// Debug settings
#define DEBUG true // set to true for debug output, false for no debug output
#define HTTP_DEBUG true // set to true to print all HTTP connections content
#define DEBUG_SERIAL if (DEBUG) Serial

// WiFi variables
extern int status;
extern char server[];  // name address for lichess (using DNS)
extern WiFiSSLClient StreamClient; // WIFISSLClient for move stream, always connects via SSL (port 443 for https)

// Lichess variables
extern const char* currentGameID;
extern bool myturn;
extern String lastMove;
extern String myMove;
extern bool is_castling_allowed;

// LED and state variables
extern bool boot_flipstate;
extern bool is_booting;
extern bool isr_first_run;
extern bool connect_flipstate;
extern bool is_connecting;
extern bool is_game_running;

void loop();
void setup();
