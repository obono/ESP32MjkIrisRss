#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "MyRssManager.h"
#include "credentials.h"
#include "certificates.h"

/*---------------------------------------------------------------------------*/

MyRssManager::MyRssManager(void) : isActive(false)
{
}

MyRssManager::~MyRssManager()
{
}

void MyRssManager::updateTitles(void)
{
    enableNetwork();
    fetchTitles();
    disableNetwork();
    isActive = true;
}

bool MyRssManager::getNextTitle(String &title, String &info)
{
    if (isActive && items.size() > 0) {
        title = items[itemIndex].title;
        info = sources[items[itemIndex].sourceIndex];
        if (++itemIndex >= items.size()) {
            itemIndex = 0;
        }
        return true;
    } else {
        return false;
    }
}

/*---------------------------------------------------------------------------*/

void MyRssManager::enableNetwork(void)
{
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(MY_SSID, MY_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    dprint(F("Wi-Fi connected to "));
    dprintln(WiFi.localIP());
}

void MyRssManager::disableNetwork(void)
{
    WiFi.disconnect(true);
    dprintln(F("Wi-Fi disconnected."));
}

void MyRssManager::getAndParseRss(const char *host, const char *rootCA, const char *path)
{
    WiFiClientSecure httpsClient;
    httpsClient.setCACert(rootCA);

    if (httpsClient.connect(host, 443)) {
        String req = String("GET https://") + String(host) + String(path) + " HTTP/1.1\r\n";
        req += "Host: " + String(host) + "\r\n";
        req += "User-Agent: BuildFailureDetectorESP32\r\n";
        req += "Connection: close\r\n\r\n";
        req += "\0";
        httpsClient.print(req);
        httpsClient.flush();
        dprint(F("HTTPS connecting to "));
        dprintln(host);
    } else {
        dprintln(F("HTTPS connection failed."));
    }

    if (httpsClient) {
        bool isFirst = true;
        int sourceIndex = -1;
        while (httpsClient.connected()) {
            while (httpsClient.available()) {
                const String beginTag = "<title>", endTag = "</title>";
                String line = httpsClient.readStringUntil('\n');
                if (line.indexOf(beginTag) >= 0) {
                    uint16_t from = line.indexOf(beginTag) + beginTag.length();
                    uint16_t to = line.indexOf(endTag);
                    String title = line.substring(from, to);
                    title.replace("&amp;", "&");
                    title.replace("&#039;", "\'");
                    title.replace("&#39;", "\'");
                    title.replace("&apos;", "\'");
                    title.replace("&quot;", "\"");
                    if (isFirst) {
                        sources.push_back(title);
                        sourceIndex = sources.size() - 1;
                        isFirst = false;
                    } else if (sourceIndex >= 0) {
                        RSS_ITEM_T item;
                        item.sourceIndex = sourceIndex;
                        item.title = title;
                        items.push_back(item);
                    }
                    dprintln(title);
                }
            }
        }
        delay(10);
        httpsClient.stop();
        delay(10);
        dprintln(F("HTTPS connection closed."));
    }
}

void MyRssManager::fetchTitles(void)
{
    sources.clear();
    items.clear();
    getAndParseRss("www3.nhk.or.jp", rootCA_NHK, "/rss/news/cat0.xml");
    getAndParseRss("www.asahi.com", rootCA_AsahiCom, "/rss/asahi/newsheadlines.rdf");
    itemIndex = 0;
}
