#pragma once

#include <Arduino.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include "OpenChessBoard.h"
#include "board_driver.h"
#include "configuration.h"


#define CANNOT_CONNECT_TO_LICHESS F("Cannot connect to server. It may be down or you may have not properly uploaded SSL Root certificate for `lichess.org:443` onto the Arduino.")
#define ONGOING_GAME_NOT_FOUND F("Ongoing game not found. Please go to Lichess.org and start a new game against a computer or a human.")

struct GameStatus {
  const char * gameId;
  bool isMyTurn;
};

String getUsername();
GameStatus getGameStatus();
void postMove();
void getStream(WiFiSSLClient &client);
