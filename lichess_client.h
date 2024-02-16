#pragma once

#include <Arduino.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include "OpenChessBoard.h"
#include "board_driver.h"

void postMove(WiFiSSLClient &client);
void getUsername(WiFiSSLClient &client);
void getStream(WiFiSSLClient &client);
void getGameID(WiFiSSLClient &client);
