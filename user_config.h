/*
 * Secret data, change to your credentials!
 */
char ssid[] = "my_network";            // your network SSID (name), must be 2.4 GHz WiFi!!!
char pass[] = "my_password";           // your network password
char token[] = "my_lichess_API_token"; // your lichess API token, generated with all rights enabled (all sliders set to green)

/*
 * The standard orientation of the OPENCHESSBOARD is when the USB-Plug is on the right,
 * You can change the orientation by using the following define
*/
//#define PLUG_AT_TOP

/*
 * Debug settings
 */
#define DEBUG true // set to true for debug output, false for no debug output
#define HTTP_DEBUG true // set to true to print all HTTP connections content
#define DEBUG_SERIAL if (DEBUG) Serial
