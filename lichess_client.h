#pragma once

#include <Arduino.h>
#include <WiFiNINA.h>
#include "ArduinoJson-v6.19.4.h"
#include "OpenChessBoard.h"
#include "board_driver.h"
#include "configuration.h"

void postMove(WiFiSSLClient &client);
void getUsername(WiFiSSLClient &client);
void getStream(WiFiSSLClient &client);
void getGameID(WiFiSSLClient &client);
