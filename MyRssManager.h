#pragma once

#include <vector>
#include "common.h"

typedef struct {
    int     sourceIndex;
    String  title;
} RSS_ITEM_T;

class MyRssManager
{
public:
    MyRssManager(void);
    ~MyRssManager();
    void updateTitles(void);
    bool getNextTitle(String &title, String &info);

private:
    void enableNetwork(void);
    void disableNetwork(void);
    void fetchTitles(void);
    void getAndParseRss(const char *host, const char *rootCA, const char *path);

    vector<String>      sources;
    vector<RSS_ITEM_T>  items;
    int     itemIndex;
    bool    isActive;
};
