/*
	FSWebServer - Example WebServer with SPIFFS backend for esp8266
	Copyright (c) 2015 Hristo Gochkov. All rights reserved.
	This file is part of the ESP8266WebServer library for Arduino environment.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
	Lesser General Public License for more details.
	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA	02110-1301	USA

	upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
	or you can upload the contents of a folder if you CD in that folder and run the following command:
	for file in `\ls -A1`; do curl -F "file=@$PWD/$file" esp8266vfms.local/edit; done

	access the sample web page at http://esp8266vfms.local
	edit the page by going to http://esp8266vfms.local/edit
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <LittleFS.h>
#include <time.h>
#include <string.h>

//FS* filesystem = &SPIFFS;
FS* filesystem = &LittleFS;

#define DBG_OUTPUT_PORT Serial

#ifndef LOGIN_FILENAME
#define LOGIN_FILENAME "/logins.dat"
#endif

#ifndef WIFI_FILENAME
#define WIFI_FILENAME "/wifi.dat"
#endif

#ifndef LOGIN_STRUCTS
#define LOGIN_STRUCTS 1

#define USERNAME_MIN_LEN 3
#define USERNAME_MAX_LEN 32

#define PASSWORD_MIN_LEN 8
#define PASSWORD_MAX_LEN 128
#define PASSWORD_HASH_LEN 32

struct logininfo {
  char username[USERNAME_MAX_LEN+1]; //+1 for null terminators aaaa
  char password[PASSWORD_HASH_LEN+1]; //+1 for null terminators aaaa
  unsigned int userID;
  char group;
};

typedef struct logininfo LoginInfo;

struct sessmap {
	char hash[PASSWORD_HASH_LEN+1]; // do not forget terminator !!!
	struct logininfo userInfo;
	time_t lastUsed;
};

typedef struct sessmap SessionMap;
#endif

const char* host = "esp8266vfms";

ESP8266WebServer server(80);

void setup(void) {
	DBG_OUTPUT_PORT.begin(115200);
	DBG_OUTPUT_PORT.print("\n");
	DBG_OUTPUT_PORT.setDebugOutput(true);
	filesystem->begin();

	if (!filesystem->open(LOGIN_FILENAME,"r")) {
		File f = filesystem->open(LOGIN_FILENAME,"w");
		f.close();
	}

	{
		Dir dir = filesystem->openDir("/");
		while (dir.next()) {
			String fileName = dir.fileName();
			size_t fileSize = dir.fileSize();
			DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
		}
		DBG_OUTPUT_PORT.printf("\n");
	}

	//WIFI INIT

	DBG_OUTPUT_PORT.printf("Reading wifi file %s\n", WIFI_FILENAME);
    File wifiDat = filesystem->open(WIFI_FILENAME,"r");
	String ssid_S, password_S;

    wifiDat.setTimeout(10);
	ssid_S = wifiDat.readStringUntil('\n');
    password_S = wifiDat.readStringUntil('\n');
	ssid_S.trim();
	password_S.trim();
	const char* ssid = ssid_S.c_str();
	const char* password = password_S.c_str();
	wifiDat.close();

	DBG_OUTPUT_PORT.printf("Connecting to %s\n", ssid);

	if (String(WiFi.SSID()) != String(ssid)) {
		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid, password);
	}

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		DBG_OUTPUT_PORT.print(".");
	}
	DBG_OUTPUT_PORT.println("");
	DBG_OUTPUT_PORT.print("Connected! IP address: ");
	DBG_OUTPUT_PORT.println(WiFi.localIP());

	MDNS.begin(host);
	DBG_OUTPUT_PORT.print("Open http://");
	DBG_OUTPUT_PORT.print(host);
	DBG_OUTPUT_PORT.println(".local/edit to see the file browser");

	server.on(LOGIN_FILENAME,[]() {
		server.send(403, "text/plain", "You aren't allowed to see the credentials!");
	});
	server.on(WIFI_FILENAME,[]() {
		server.send(403, "text/plain", "You aren't allowed to see the credentials!");
	});
	server.on("/login",[]() {
		svLogIn();
	});
	server.on("/logout",[]() {
		svLogOut();
	});
	server.on("/register",[]() {
		svRegister();
	});
	server.on("/user_info",[]() {
		svLoginJSON();
	});
	server.on("/led_info",[]() {
		svLEDJSON();
	});
	server.on("/led_set",[]() {
		svLEDSet();
	});
	server.on("/",[]() {
		handleIndex();
	});
	server.on("/index.htm",[]() {
		server.sendHeader("Location", "/",true); //redirect to home
		server.send(301, "text/plain",""); 
	});
	server.on("/prompt.htm",[]() {
		server.sendHeader("Location", "/",true); //redirect to home
		server.send(301, "text/plain",""); 
	});
	server.on("/succ.htm",[]() {
		server.sendHeader("Location", "/",true); //redirect to home
		server.send(301, "text/plain",""); 
	});
	
	server.onNotFound([]() {
		if (!handleFileRead(server.uri())) {
			server.send(404, "text/plain", "FileNotFound");
		}
	});

	//here the list of headers to be recorded
	const char * headers[] = {"User-Agent","Cookie"} ;
	size_t headersize = sizeof(headers)/sizeof(char*);
	server.collectHeaders(headers, headersize );
	server.begin();
	LEDsetup();
	DBG_OUTPUT_PORT.println("HTTP server started");
	
}

void loop(void) {
	server.handleClient();
	MDNS.update();
}
