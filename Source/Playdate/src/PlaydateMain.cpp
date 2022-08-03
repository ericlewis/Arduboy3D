#include "Defines.h"
#include "Game.h"
#include "Map.h"
#include "Platform.h"
#include "FixedMath.h"
#include "Common.h"

#define TONES_END 0x8000

static LCDBitmap *screenBitmap = nullptr;
uint8_t sBuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];

void Platform::FillScreen(uint8_t colour)
{
     pd->graphics->clear(colour);
}

void Platform::DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
    uint8_t w = bitmap[0];
    uint8_t h = bitmap[1];

    bitmap += 2;

    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            int blockY = j / 8;
            int blockIndex = (w * blockY + i) * 2;
            uint8_t pixels = bitmap[blockIndex];
            uint8_t maskPixels = bitmap[blockIndex + 1];
            uint8_t bitmask = 1 << (j % 8);

            if (maskPixels & bitmask)
            {
                if (x + i >= 0 && y + j >= 0)
                {
                    if (pixels & bitmask)
                    {
                        PutPixel(x + i, y + j, 1);
                    }
                    else
                    {
                        PutPixel(x + i, y + j, 0);
                    }
                }
            }
        }
    }
}

void Platform::DrawVLine(uint8_t x, int8_t y1, int8_t y2, uint8_t pattern)
{
    for (int y = y1; y <= y2; y++)
    {
        if (y >= 0)
        {
            uint8_t patternIndex = y % 8;
            uint8_t mask = 1 << patternIndex;
            PutPixel(x, y, (mask & pattern) != 0 ? 1 : 0);
        }
    }
}

void Platform::PutPixel(uint8_t x, uint8_t y, uint8_t colour)
{
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
    {
        return;
    }

    uint16_t row_offset;
    uint8_t bit;

    bit = 1 << (y & 7);
    row_offset = (y & 0xF8) * DISPLAY_WIDTH / 8 + x;
    uint8_t data = sBuffer[row_offset] | bit;
    if (!colour) data ^= bit;
    sBuffer[row_offset] = data;
}

static uint8_t* m_buffer;
static int m_rowBytes;
static int dummy;
static uint8_t* pdummy;

void PutPixel(uint8_t x, uint8_t y, uint8_t color)
{
    uint8_t* block = m_buffer + (y * m_rowBytes) + (x / 8);
    uint8_t data = 0x80 >> (x % 8);
    *block = color ? *block | data : *block & ~data;
}

void ResolveScreen()
{
    pd->graphics->clearBitmap(screenBitmap, kColorBlack);
    pd->graphics->getBitmapData(screenBitmap, &dummy, &dummy, &m_rowBytes, &pdummy, &m_buffer);
    for (int j = 0; j < DISPLAY_HEIGHT; j++)
    {
        for (int i = 0; i < DISPLAY_WIDTH; i++)
        {
            int blockX = i / 8;
            int blockY = j / 8;
            int blocksPerWidth = DISPLAY_WIDTH / 8;
            int blockIndex = blockY * blocksPerWidth + blockX;
            uint8_t pixels = sBuffer[blockIndex * 8 + i % 8];
            uint8_t mask = 1 << (j % 8);
            if (pixels & mask)
            {
                PutPixel(0 + i, 0 + j, 1);
            }
        }
    }
    pd->graphics->drawScaledBitmap(screenBitmap, 9, 25, 3, 3);
}

uint8_t* Platform::GetScreenBuffer()
{
    return sBuffer;
}

void Platform::PlaySound(const uint16_t* audioPattern)
{
    // TODO: implement me.
}

bool Platform::IsAudioEnabled()
{
    // TODO: implement me
    return true;
}

void Platform::SetAudioEnabled(bool isEnabled)
{
    // TODO: implement me
}

uint8_t Platform::GetInput()
{
    PDButtons current;
    pd->system->getButtonState(&current, NULL, NULL);

    if (current & kButtonA) {
        return INPUT_B;
    }
    if (current & kButtonB) {
        return INPUT_A;
    }
    if (current & kButtonLeft) {
        return INPUT_LEFT;
    }
    if (current & kButtonRight) {
        return INPUT_RIGHT;
    }
    if (current & kButtonUp) {
        return INPUT_UP;
    }
    if (current & kButtonDown) {
        return INPUT_DOWN;
    }

    return 0;
}

void Platform::ExpectLoadDelay()
{
    // noop
}

int updateCallback(__attribute__ ((unused)) void* ud)
{
    Game::Tick();
    Game::Draw();
    ResolveScreen();
    return 1;
}

extern "C" int eventHandler(PlaydateAPI *playdate, PDSystemEvent event, uint32_t arg) {
    if (event == kEventInit) {
        pd = playdate;

        screenBitmap = pd->graphics->newBitmap(DISPLAY_WIDTH, DISPLAY_HEIGHT, kColorBlack);
        pd->graphics->setBackgroundColor(kColorBlack);
        pd->display->setRefreshRate(TARGET_FRAMERATE);

        SeedRandom(0);
        Game::Init();

        pd->system->setUpdateCallback(updateCallback, NULL);
    }
    return 0;
}