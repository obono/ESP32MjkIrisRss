#include "MyDisplayManager.h"
#include "MyRssManager.h"

#define MILLIS_PER_FRAME    25

static const String loadingMsg = "RSS 取得中...";
static const String errorMsg = "ニュースの取得に失敗しました";
static const String emptyMsg = ""; 

static MyDisplayManager disp;
static MyRssManager     rss;
static long             lastTime;

/*---------------------------------------------------------------------------*/

static void nextTitle(void)
{
    String title, info;
    if (rss.getNextTitle(title, info)) {
        title = "◆" + title;
        info = "   (" + info + ")";
        disp.setMessage(title, info, true);
    } else {
        disp.setMessage(errorMsg, emptyMsg, false);
    }
}

/*---------------------------------------------------------------------------*/

void setup(void)
{
#ifdef DEBUG
    Serial.begin(115200);
#endif
    dprintln("Build: " __DATE__ " " __TIME__);

    disp.setup();
    disp.setMessage(loadingMsg, emptyMsg, false);
    rss.updateTitles();
    nextTitle();
    lastTime = millis();
}

void loop(void)
{
    disp.update();
    if (disp.isMessageScrolled()) {
        nextTitle();
    }

    long currentTime = millis();
    long wait = lastTime + MILLIS_PER_FRAME - currentTime;
    if (bitRead(wait, 31)) {
        lastTime = currentTime;
    } else {
        delay(wait);
        lastTime += MILLIS_PER_FRAME;
    }
}
