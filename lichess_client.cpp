#include "lichess_client.h"

// Unexported functions
void httpGetRequest(String path, DynamicJsonDocument &jsonResponse);
void httpPostRequest(String path, DynamicJsonDocument &jsonResponse);
void httpRequest(String httpVerb, String path, DynamicJsonDocument &jsonResponse);
void readHeaders(WiFiSSLClient client);
void processHTTP(WiFiSSLClient client);

/* ---------------------------------------
*  Fetch current username from Lichess API.
*  @params[in] WiFiSSLClient
*  @return String
*/
String getUsername() {
  DynamicJsonDocument jsonResponse(1536);
  httpGetRequest("/api/account", jsonResponse);

  return jsonResponse["username"];
}

/* ---------------------------------------
 *  Fetch first currently playing game status (i.e. game ID and who's turn it is) from Lichess API.
 *  @return GameStatus
*/
GameStatus getGameStatus() {
  DynamicJsonDocument doc(1536);
  httpGetRequest("/api/account/playing?nb=1", doc);

  return {
    .gameId = doc["nowPlaying"][0]["gameId"],
    .isMyTurn = doc["nowPlaying"][0]["gameId"]
  };
}

/* ---------------------------------------
 *  Function to send post move request to Lichess API.
 *  Restarts client and stops client after request
 *  @params[in] WiFiSSLClient
 *  @return void
*/
void postMove() {
  while (myturn && is_game_running)
  {
    clearDisplay();
    String move_input = getMoveInput();
    clearDisplay();
    DEBUG_SERIAL.print("my move: ");
    DEBUG_SERIAL.println(move_input);

    myMove = move_input;

    TC4->COUNT32.CTRLA.bit.ENABLE = 0;

    DynamicJsonDocument doc(1024);
    httpPostRequest((String)"/api/board/game/" + currentGameID + "/move/" + move_input, doc);

    //check for sucessful move
    boolean moveSuccess = false;
    moveSuccess = doc["ok"];
    if (moveSuccess == true) {
      DEBUG_SERIAL.println("move success!");
      myturn = false;
      TC4->COUNT32.CTRLA.bit.ENABLE = 1;
    }
    else
    {
        TC4->COUNT32.CTRLA.bit.ENABLE = 1;
        DEBUG_SERIAL.println("wrong move!");
        displayMove(myMove);
        String reverse_move =  (String)myMove.charAt(2)
        +  (String)myMove.charAt(3)
        +  (String)myMove.charAt(0)
        +  (String)myMove.charAt(1);

        DEBUG_SERIAL.println(reverse_move);

        while(reverse_move != move_input && is_game_running){
          move_input = getMoveInput();
          DEBUG_SERIAL.println(move_input);
        }
    }
  }
}

/*
 * HTTP requests wrappers
 */

void httpGetRequest(String path, DynamicJsonDocument &jsonResponse) {
  httpRequest(F("GET"), path, jsonResponse);
}

void httpPostRequest(String path, DynamicJsonDocument &jsonResponse) {
  httpRequest(F("POST"), path, jsonResponse);
}

void httpRequest(String httpVerb, String path, DynamicJsonDocument &jsonResponse) {
  WiFiSSLClient client;

  if (!client.connect(server, 443)) {
    DEBUG_SERIAL.println(CANNOT_CONNECT_TO_LICHESS);
    return;
  };

  client.println((String)httpVerb + F(" ") + path + F(" HTTP/1.1"));
  client.println(F("Host: lichess.org"));
  client.print(F("Authorization: Bearer "));
  client.println(LICHESS_API_TOKEN);
  client.println(F("Connection: close"));
  client.println();

  if (HTTP_DEBUG) DEBUG_SERIAL.println((String)"→ " + httpVerb + path);

  readHeaders(client);

  DeserializationError jsonError = deserializeJson(jsonResponse, client);
  if (jsonError) {
    DEBUG_SERIAL.print(F("deserializeJson() failed: "));
    DEBUG_SERIAL.println(jsonError.f_str());
  } else if (HTTP_DEBUG) {
    serializeJsonPretty(jsonResponse, Serial); // we cannot use DEBUG_SERIAL here :-/
    DEBUG_SERIAL.println();
  }

  client.stop();
}

void readHeaders(WiFiSSLClient client) {
  char status[64] = {0};
  client.readBytesUntil('\r', status, sizeof(status));

  if (strcmp(status + 9, "200 OK") != 0) {
    DEBUG_SERIAL.print(F("Unexpected response: "));
    DEBUG_SERIAL.println(status);
  }
  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    DEBUG_SERIAL.println(F("Invalid response"));
  }
}

/* ---------------------------------------
 *  Function to send get stream request to Lichess API.
 *  Starts the move stream on client.
 *  @params[in] WiFiSSLClient
 *  @return void
*/
void getStream(WiFiSSLClient &client){
    client.print("GET /api/board/game/stream/");
    client.print((String)currentGameID);
    client.println(" HTTP/1.1");
    client.println("Host: lichess.org");
    client.print("Authorization: Bearer ");
    client.println(LICHESS_API_TOKEN);
    client.println("Connection: close");
    client.println();
    delay(500);
    processHTTP(client);
  }

/* ---------------------------------------
 *  Function to evaluate http requests on wifi client.
 *  Generic function that checks for http status and skips headers
 *  @params[in] WiFiSSLClient
 *  @return void
*/
void processHTTP(WiFiSSLClient client) {
  if (client.println() == 0) {
    return;
  }

  char status[64] = {0};
  client.readBytesUntil('\r', status, sizeof(status));

  // It should be "HTTP/1.0 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    DEBUG_SERIAL.print(F("Unexpected response: "));
    DEBUG_SERIAL.println(status);
    if (strcmp(status + 9, "400 Bad Request") == 0) {
      //catch bad request
    }
    else {
      return;
    }
  }
  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    DEBUG_SERIAL.println(F("Invalid response"));
    return;
  }
}
