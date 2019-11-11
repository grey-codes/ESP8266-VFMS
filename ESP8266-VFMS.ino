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

//FS* filesystem = &SPIFFS;
FS* filesystem = &LittleFS;

#define DBG_OUTPUT_PORT Serial

#ifndef STASSID
#define STASSID "Strange Man In Your Attic"
#define STAPSK	"JaWBnFAmG2015"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "esp8266vfms";

ESP8266WebServer server(80);

void setup(void) {
	DBG_OUTPUT_PORT.begin(115200);
	DBG_OUTPUT_PORT.print("\n");
	DBG_OUTPUT_PORT.setDebugOutput(true);
	filesystem->begin();
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

	server.on("/login",[]() {
		svLogIn();
	});
	server.on("/logout",[]() {
		svLogOut();
	});
	server.on("/register",[]() {
		svRegister();
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
	DBG_OUTPUT_PORT.println("HTTP server started");

}

void loop(void) {
	server.handleClient();
	MDNS.update();
}
