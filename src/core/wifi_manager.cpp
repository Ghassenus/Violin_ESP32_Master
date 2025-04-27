#include "wifi_manager.h"
#include <WiFi.h>
#include <Preferences.h>
#include <logger.h>

static const int MAX_AP = 16;
static char ssids[MAX_AP][33];
static int  ap_count = 0;
static bool scanning = false;

void wifi_manager_scan_start() {
  scanning = true;
  WiFi.mode(WIFI_STA);
  WiFi.scanNetworks(true, true);
}

bool wifi_manager_scan_in_progress() {
  return scanning;
}

void wifi_manager_scan_handle() {
  if(!scanning) return;
  int n = WiFi.scanComplete();
  if(n < 0) return;
  ap_count = min(n, MAX_AP);
  for(int i=0;i<ap_count;i++) {
    String s = WiFi.SSID(i);
    s.toCharArray(ssids[i],33);
  }
  WiFi.scanDelete();
  scanning = false;
}

int wifi_manager_get_scan_count() {
  return ap_count;
}

const char* wifi_manager_get_ssid(int i) {
  if(i<0 || i>=ap_count) return "";
  return ssids[i];
}

void wifi_manager_connect() {
  Preferences p;
  p.begin("wifi", false); // <--- Correction ici (false pour lecture/écriture)

  if (!p.isKey("ssid")) {
    log_warn("Pas de credentials Wi-Fi sauvegardés !");
      p.end();
      return;
  }

  String ssid = p.getString("ssid", "");
  String pass = p.getString("pass", "");
  p.end();

  if (ssid.length()) {
      WiFi.begin(ssid.c_str(), pass.c_str());
      unsigned long t0 = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - t0 < 10000) {
          delay(200);
      }
  }
}


void wifi_manager_start_ap() {
  if(WiFi.status()!=WL_CONNECTED) {
    WiFi.softAP("VIOLIN-Setup","Violin1234");
  }
}