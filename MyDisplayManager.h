#pragma once

#include "common.h"

class MyDisplayManager
{
public:
    MyDisplayManager(void);
    ~MyDisplayManager();
    void setup(void);
    void setMessage(const String &main, const String &sub, bool isScroll);
    bool isMessageScrolled(void);
    void update(void);

private:
    void refresh(void);
    void drawMessage(int position, uint8_t colorShift = 0);

    String main, sub;
    int msgTotalWidth, msgMainWidth, msgPosition, waitCounter;
    bool isScroll;
};
