#include <LovyanGFX.hpp>

#include "MyDisplayManager.h"

#define DISPLAY_W   640
#define DISPLAY_H   48
#define TEXT_H      40
#define TEXT_Y      ((DISPLAY_H - TEXT_H) / 2)
#define TEXT_SPEED  4
#define ADDR_W      320
#define ADDR_H      96
#define PANEL_MEM_W 320
#define PANEL_MEM_H 240
#define COLOR_DEPTH 16
#define DISPLAY_ROT 2 // 0 or 2
#define WAIT_FRAMES 80

/*---------------------------------------------------------------------------*/

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9342 panel_instance;
    lgfx::Bus_Parallel8 bus_instance;

public:
    LGFX(void)
    {
        auto bus_config = bus_instance.config();
        bus_config.freq_write = 16000000;
        bus_config.pin_wr = 16;
        bus_config.pin_rd = 22;
        bus_config.pin_rs = 19;
        bus_config.pin_d0 = 23;
        bus_config.pin_d1 = 32;
        bus_config.pin_d2 = 25;
        bus_config.pin_d3 = 27;
        bus_config.pin_d4 = 13;
        bus_config.pin_d5 = 14;
        bus_config.pin_d6 = 26;
        bus_config.pin_d7 = 4;
        bus_instance.config(bus_config);
        panel_instance.setBus(&bus_instance);

        auto panel_config = panel_instance.config();
        panel_config.pin_cs = 17;
        panel_config.pin_rst = 33;
        panel_config.pin_busy = -1;
        panel_config.panel_width = ADDR_W;
        panel_config.panel_height = ADDR_H;
        panel_config.memory_width = PANEL_MEM_W;
        panel_config.memory_height = PANEL_MEM_H;
        panel_config.offset_x = 0;
        panel_config.offset_y = PANEL_MEM_H - ADDR_H;
        panel_config.offset_rotation = 0;
        panel_config.dummy_read_pixel = 8;
        panel_config.dummy_read_bits = 0;
        //panel_config.readable = true;
        //panel_config.invert = false;
        //panel_config.rgb_order = false;
        //panel_config.dlen_16bit = false;
        panel_config.bus_shared = false;
        panel_instance.config(panel_config);

        setPanel(&panel_instance);
    }
};

static LGFX lcd;
static lgfx::LGFX_Sprite buf(&lcd);

/*---------------------------------------------------------------------------*/

MyDisplayManager::MyDisplayManager(void)
{
}

MyDisplayManager::~MyDisplayManager()
{
}

void MyDisplayManager::setup(void)
{
    lcd.init();
    lcd.setColorDepth(COLOR_DEPTH);
    lcd.setRotation(DISPLAY_ROT);
    buf.setColorDepth(COLOR_DEPTH);
    buf.createSprite(DISPLAY_W, DISPLAY_H);
    buf.setSwapBytes(true);
    buf.setFont(&lgfxJapanGothicP_40);
    buf.setTextWrap(false);
}

void MyDisplayManager::setMessage(const String &main, const String &sub, bool isScroll)
{
    this->main = main;
    this->sub = sub;
    this->isScroll = isScroll;
    msgMainWidth = buf.textWidth(this->main.c_str());
    msgTotalWidth = msgMainWidth;
    if (sub.length() > 0) {
        msgTotalWidth += buf.textWidth(this->sub.c_str());
    }
    msgPosition = isScroll ? DISPLAY_W : 0;
    waitCounter = WAIT_FRAMES;
    refresh();
}

bool MyDisplayManager::isMessageScrolled(void)
{
    return isScroll && -msgPosition >= msgTotalWidth;
}

void MyDisplayManager::update(void)
{
    if (msgPosition > 0) {
        if (msgPosition > TEXT_SPEED) {
            msgPosition = msgPosition * 3 / 4;
        } else {
            msgPosition = 0;
        }
    } else if (isScroll) {
        if (waitCounter > 0) {
            waitCounter--;
            return;
        } else {
            msgPosition -= TEXT_SPEED;
        }
    }
    refresh();
}

/*---------------------------------------------------------------------------*/

void MyDisplayManager::refresh(void)
{
    buf.fillRect(0, 0, DISPLAY_W, DISPLAY_H, TFT_BLACK);
    if (isScroll && waitCounter == 0) {
        drawMessage(msgPosition + TEXT_SPEED, 2);
    }
    drawMessage(msgPosition);
    lcd.setAddrWindow(0, 0, ADDR_W, ADDR_H);
    lcd.pushPixels((lgfx::swap565_t *)buf.getBuffer(), ADDR_W * ADDR_H);
}

void MyDisplayManager::drawMessage(int position, uint8_t colorShift)
{
    buf.setTextColor(lcd.color888(255 >> colorShift, 255 >> colorShift, 255 >> colorShift)); // TFT_WHITE
    buf.setCursor(position, TEXT_Y);
    buf.print(main);
    if (sub.length() > 0) {
        buf.setTextColor(lcd.color888(135 >> colorShift, 206 >> colorShift, 235 >> colorShift)); // TFT_SKYBLUE
        buf.setCursor(position + msgMainWidth, TEXT_Y);
        buf.print(sub);
    }
}
