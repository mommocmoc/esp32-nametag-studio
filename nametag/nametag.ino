/*
 * ==============================================================================
 *  NAMETAG STUDIO  —  a programmable badge for the ESP32-S3
 * ==============================================================================
 *  Board    : Waveshare ESP32-S3-Touch-LCD-2  (2.0" ST7789 IPS, 320x240)
 *  Renders  : a two-card name badge with live animations, optional microSD
 *             artwork (PNG with alpha, JPG), and five built-in themes.
 *
 *  YOU PROBABLY DON'T NEED TO READ THIS FILE.
 *  ------------------------------------------
 *  Everything personal lives in  config.h.
 *  Colors live in  theme.h.  Positions live in  layout.h.
 *  This file is the engine that draws them.
 *
 *  Build & flash:   ./tools/flash.sh
 *  Full guide:      README.md
 * ==============================================================================
 */

#include <Arduino_GFX_Library.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <PNGdec.h>
#include <JPEGDEC.h>

#include "config.h"
#include "theme.h"
#include "layout.h"

#if USE_NAME_BITMAP
  #include "name_bitmap.h"
#endif

// ==============================================================================
//  Display + storage
// ==============================================================================
Arduino_DataBus *bus = new Arduino_ESP32SPI(
    LCD_PIN_DC, LCD_PIN_CS, LCD_PIN_SCLK, LCD_PIN_MOSI, LCD_PIN_MISO);

Arduino_GFX *gfx = new Arduino_ST7789(
    bus, LCD_PIN_RST, NAMETAG_ROTATION, true /* IPS */, LCD_PANEL_W, LCD_PANEL_H);

static bool sd_ready = false;

PNG    png;
JPEGDEC jpeg;
static File  img_file;
static int   img_x = 0;
static int   img_y = 0;

// ==============================================================================
//  microSD image slots (from config.h)
// ==============================================================================
struct ImageSlot {
  bool        enabled;
  const char *path;
  int x, y, w, h;
};

static const ImageSlot IMAGE_SLOTS[] = {
  IMAGE_SLOT_BACKGROUND,
  IMAGE_SLOT_AVATAR,
  IMAGE_SLOT_STICKER,
  IMAGE_SLOT_ICON,
};
#define IMAGE_SLOT_COUNT (sizeof(IMAGE_SLOTS) / sizeof(IMAGE_SLOTS[0]))

// ==============================================================================
//  Avatar eye states
// ==============================================================================
enum EyeState { EYE_OPEN = 0, EYE_BLINK, EYE_WINK };

// ==============================================================================
//  PNG decoding (alpha-aware)
// ==============================================================================
void *pngOpen(const char *filename, int32_t *size) {
  img_file = SD.open(filename, FILE_READ);
  if (!img_file || img_file.isDirectory()) return NULL;
  *size = img_file.size();
  return &img_file;
}
void pngClose(void *handle) { if (img_file) img_file.close(); }
int32_t pngRead(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  return img_file ? img_file.read(buffer, length) : 0;
}
int32_t pngSeek(PNGFILE *handle, int32_t position) {
  return img_file ? img_file.seek(position) : 0;
}
int pngDrawCallback(PNGDRAW *pDraw) {
  uint16_t linePixels[SCREEN_W];
  uint8_t  lineMask[SCREEN_W];
  int w = (pDraw->iWidth > SCREEN_W) ? SCREEN_W : pDraw->iWidth;

  png.getLineAsRGB565(pDraw, linePixels, PNG_RGB565_LITTLE_ENDIAN, 0x00000000);
  png.getAlphaMask(pDraw, lineMask, 1);
  gfx->draw16bitRGBBitmapWithMask(img_x, img_y + pDraw->y, linePixels, lineMask, w, 1);
  return 1;
}

static bool drawPngFile(const char *path, int x, int y) {
  img_x = x; img_y = y;
  if (png.open(path, pngOpen, pngClose, pngRead, pngSeek, pngDrawCallback) != PNG_SUCCESS)
    return false;
  int rc = png.decode(NULL, 0);
  png.close();
  return rc == PNG_SUCCESS;
}

// ==============================================================================
//  JPEG decoding
// ==============================================================================
void *jpegOpen(const char *szFilename, int32_t *pFileSize) {
  img_file = SD.open(szFilename, FILE_READ);
  if (!img_file || img_file.isDirectory()) return NULL;
  *pFileSize = img_file.size();
  return &img_file;
}
void jpegClose(void *pHandle) { if (img_file) img_file.close(); }
int32_t jpegRead(JPEGFILE *pFile, uint8_t *pBuf, int32_t iLen) {
  return img_file ? img_file.read(pBuf, iLen) : 0;
}
int32_t jpegSeek(JPEGFILE *pFile, int32_t iPosition) {
  return img_file ? img_file.seek(iPosition) : 0;
}
int jpegDrawCallback(JPEGDRAW *pDraw) {
  gfx->draw16bitRGBBitmap(img_x + pDraw->x, img_y + pDraw->y,
                          pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

static bool drawJpegFile(const char *path, int x, int y) {
  img_x = x; img_y = y;
  if (!jpeg.open(path, jpegOpen, jpegClose, jpegRead, jpegSeek, jpegDrawCallback))
    return false;
  jpeg.setPixelType(RGB565_LITTLE_ENDIAN);
  jpeg.decode(0, 0, 0);
  jpeg.close();
  return true;
}

// Dispatch on file extension. Unknown extensions are skipped silently.
static bool drawImageFile(const char *path, int x, int y) {
  if (!sd_ready || path == NULL) return false;
  String p = String(path);
  p.toLowerCase();
  if (p.endsWith(".png"))                          return drawPngFile(path, x, y);
  if (p.endsWith(".jpg") || p.endsWith(".jpeg"))   return drawJpegFile(path, x, y);
  return false;
}

// ==============================================================================
//  Drawing primitives — the badge's visual vocabulary
// ==============================================================================

// A chunky rounded card with a bold double outline and an offset drop shadow.
static void drawPopCard(int x, int y, int w, int h, int r,
                        uint16_t fill, uint16_t border, uint16_t shadow) {
  gfx->fillRoundRect(x + 3, y + 3, w, h, r, shadow);
  gfx->fillRoundRect(x, y, w, h, r, fill);
  gfx->drawRoundRect(x, y, w, h, r, border);
  gfx->drawRoundRect(x + 1, y + 1, w - 2, h - 2, r - 1, border);  // 2px effect
}

// A sticker-style pill with centered text.
static void drawPill(int x, int y, int w, int h, const char *label,
                     uint16_t fill, uint16_t text_color) {
  gfx->fillRoundRect(x + 2, y + 2, w, h, h / 2, C_INK);
  gfx->fillRoundRect(x, y, w, h, h / 2, fill);
  gfx->drawRoundRect(x, y, w, h, h / 2, C_INK);

  gfx->setTextSize(1);
  gfx->setTextColor(text_color);
  int16_t x1, y1; uint16_t tw, th;
  gfx->getTextBounds(label, 0, 0, &x1, &y1, &tw, &th);
  gfx->setCursor(x + (w - (int)tw) / 2, y + (h - (int)th) / 2 - 1);
  gfx->print(label);
}

// A four-point sparkle. `s` is the radius along each axis.
static void drawSparkle(int x, int y, uint16_t color, int s = 5) {
  int t = s * 3 / 5;
  gfx->fillTriangle(x, y - s, x - t, y, x + t, y, color);
  gfx->fillTriangle(x, y + s, x - t, y, x + t, y, color);
  gfx->fillTriangle(x - s, y, x, y - t, x, y + t, color);
  gfx->fillTriangle(x + s, y, x, y - t, x, y + t, color);
}

// Just the eyes — repainted every blink without redrawing the whole face.
static void drawEyes(int cx, int cy, EyeState state) {
  if (state == EYE_OPEN) {
    gfx->fillCircle(cx - 10, cy - 4, 3, C_INK);
    gfx->fillCircle(cx + 10, cy - 4, 3, C_INK);
  } else if (state == EYE_BLINK) {
    gfx->drawFastHLine(cx - 13, cy - 4, 7, C_INK);
    gfx->drawFastHLine(cx - 13, cy - 3, 7, C_INK);
    gfx->drawFastHLine(cx +  7, cy - 4, 7, C_INK);
    gfx->drawFastHLine(cx +  7, cy - 3, 7, C_INK);
  } else { // EYE_WINK — left closed, right open
    gfx->drawFastHLine(cx - 13, cy - 4, 7, C_INK);
    gfx->drawFastHLine(cx - 13, cy - 3, 7, C_INK);
    gfx->fillCircle(cx + 10, cy - 4, 3, C_INK);
  }
}

static void updateEyes(int cx, int cy, EyeState state) {
  gfx->fillRect(cx - 15, cy - 8, 30, 9, C_ACCENT_B);   // clear to face color
  drawEyes(cx, cy, state);
}

// The smiley keyring character: eyelet, face, eyes, blush, smile.
static void drawAvatar(int cx, int cy, int radius, EyeState state = EYE_OPEN) {
  gfx->fillCircle(cx, cy - radius - 6, 6, C_INK);      // keyring hole
  gfx->fillCircle(cx, cy - radius - 6, 3, C_BG);

  gfx->fillCircle(cx + 2, cy + 2, radius, C_INK);      // offset outline
  gfx->fillCircle(cx, cy, radius, C_ACCENT_B);
  gfx->drawCircle(cx, cy, radius, C_INK);
  gfx->drawCircle(cx, cy, radius - 1, C_INK);

  drawEyes(cx, cy, state);

  gfx->fillCircle(cx - 14, cy + 5, 4, C_ACCENT_C);     // blush
  gfx->fillCircle(cx + 14, cy + 5, 4, C_ACCENT_C);

  gfx->fillRoundRect(cx - 6, cy + 5, 12, 6, 3, C_INK); // smile
  gfx->fillRoundRect(cx - 4, cy + 3,  8, 4, 2, C_ACCENT_B);
}

// ==============================================================================
//  The badge
// ==============================================================================
static void drawNameTag() {
  gfx->fillScreen(C_BG);

#if NAMETAG_SHOW_DOT_GRID
  for (int x = DOT_GRID_START; x < SCREEN_W; x += DOT_GRID_STEP)
    for (int y = DOT_GRID_START; y < SCREEN_H; y += DOT_GRID_STEP) {
      gfx->drawPixel(x, y, C_DOT);
      gfx->drawPixel(x + 1, y, C_DOT);
    }
#endif

  // --- Header strip ---
  gfx->fillRect(0, 0, SCREEN_W, HEADER_H, C_ACCENT_A);
  gfx->drawFastHLine(0, HEADER_H, SCREEN_W, C_INK);
  drawSparkle(HEADER_SPARKLE_INSET, HEADER_H / 2, C_INK);
  drawSparkle(SCREEN_W - HEADER_SPARKLE_INSET, HEADER_H / 2, C_INK);

  gfx->setTextSize(1);
  gfx->setTextColor(C_INK);
  {
    int16_t tx, ty; uint16_t tw, th;
    gfx->getTextBounds(NAMETAG_TITLE, 0, 0, &tx, &ty, &tw, &th);
    gfx->setCursor((SCREEN_W - (int)tw) / 2, HEADER_TEXT_Y);
    gfx->print(NAMETAG_TITLE);
  }

  // --- Left card: avatar, org, status ---
  drawPopCard(CARD_L_X, CARD_TOP, CARD_L_W, CARD_H, CARD_RADIUS,
              C_CARD, C_INK, C_SHADOW);
#if NAMETAG_SHOW_AVATAR
  drawAvatar(AVATAR_CX, AVATAR_CY, AVATAR_R);
#endif
  drawPill(ORG_PILL_X, ORG_PILL_Y, ORG_PILL_W, ORG_PILL_H,
           NAMETAG_ORG, C_ACCENT_A, C_INK);
  drawPill(STATUS_PILL_X, STATUS_PILL_Y, STATUS_PILL_W, STATUS_PILL_H,
           NAMETAG_STATUS, C_ACCENT_D, C_INK);

  // --- Right card: name, role, tags ---
  drawPopCard(CARD_R_X, CARD_TOP, CARD_R_W, CARD_H, CARD_RADIUS,
              C_CARD, C_INK, C_SHADOW);

#if USE_NAME_BITMAP
  gfx->drawBitmap(NAME_X, NAME_Y_BITMAP, NAME_BITMAP,
                  NAME_BITMAP_W, NAME_BITMAP_H, C_INK);
#else
  gfx->setTextSize(2);
  gfx->setTextColor(C_INK);
  gfx->setCursor(NAME_X, NAME_Y_TEXT);
  gfx->print(NAMETAG_NAME);
#endif

  drawPill(ROLE_PILL_X, ROLE_PILL_Y, ROLE_PILL_W, ROLE_PILL_H,
           NAMETAG_ROLE, C_ACCENT_B, C_INK);

  gfx->drawFastHLine(DIVIDER_X, DIVIDER_Y, DIVIDER_W, C_HAIRLINE);

  drawPill(TAG1_X, TAG1_Y, TAG1_W, TAG_H, NAMETAG_TAG_1, C_ACCENT_A, C_INK);
  drawPill(TAG2_X, TAG2_Y, TAG2_W, TAG_H, NAMETAG_TAG_2, C_ACCENT_C, C_ON_ACCENT_C);
  drawPill(TAG3_X, TAG3_Y, TAG3_W, TAG_H, NAMETAG_TAG_3, C_ACCENT_D, C_INK);

  drawSparkle(SPARKLE_1_X, SPARKLE_1_Y, C_ACCENT_A);
  drawSparkle(SPARKLE_2_X, SPARKLE_2_Y, C_ACCENT_C);

  // --- microSD artwork, drawn on top of the generated UI ---
  if (sd_ready) {
    for (size_t i = 0; i < IMAGE_SLOT_COUNT; i++) {
      const ImageSlot &s = IMAGE_SLOTS[i];
      if (!s.enabled) continue;
      Serial.printf("[IMG] %s -> (%d, %d)\n", s.path, s.x, s.y);
      if (!drawImageFile(s.path, s.x, s.y))
        Serial.printf("[IMG] FAILED: %s (missing file, or not PNG/JPG?)\n", s.path);
    }
  }

  // --- Footer ---
  gfx->drawFastHLine(12, FOOTER_RULE_Y, SCREEN_W - 24, C_HAIRLINE);
  gfx->setTextSize(1);
  gfx->setTextColor(C_MUTED);
  gfx->setCursor(FOOTER_LEFT_X, FOOTER_TEXT_Y);
  gfx->printf("ID: %08X | SD: %s", (uint32_t)ESP.getEfuseMac(), sd_ready ? "OK" : "NO-CARD");
  gfx->setCursor(FOOTER_RIGHT_X, FOOTER_TEXT_Y);
  gfx->print(NAMETAG_FOOTER);
}

// ==============================================================================
//  setup / loop
// ==============================================================================
static unsigned long next_blink_at   = 0;
static unsigned long blink_started   = 0;
static bool          eyes_closed     = false;
static int           sparkle_frame   = 0;
static int           float_offset    = 0;
static int           float_dir       = 1;

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n=== Nametag Studio ===");

  // Backlight
  ledcAttach(LCD_PIN_BL, 5000 /* Hz */, 10 /* bits */);
  ledcWrite(LCD_PIN_BL, (1 << 10) * NAMETAG_BRIGHTNESS / 100);

  // Display
  if (!gfx->begin()) {
    Serial.println("[LCD] init FAILED — check wiring / board selection");
  } else {
    Serial.printf("[LCD] ready (%dx%d)\n", gfx->width(), gfx->height());
  }

  // microSD (optional — the badge renders fine without it)
  SPI.begin(LCD_PIN_SCLK, LCD_PIN_MISO, LCD_PIN_MOSI, SD_PIN_CS);
  if (!SD.begin(SD_PIN_CS, SPI, 20000000)) {
    Serial.println("[SD] no card — using drawn UI only");
    sd_ready = false;
  } else {
    Serial.printf("[SD] mounted (%lluMB)\n", SD.cardSize() / (1024 * 1024));
    sd_ready = true;
  }

  drawNameTag();
  Serial.println("[SYS] badge rendered");

  next_blink_at = millis() + random(2000, 4000);
}

void loop() {
  unsigned long now = millis();

#if ANIM_AVATAR_BLINK && NAMETAG_SHOW_AVATAR
  // Blink every 2.5-5s. 30% of the time it's a wink instead.
  if (!eyes_closed && now >= next_blink_at) {
    eyes_closed  = true;
    blink_started = now;
    updateEyes(AVATAR_CX, AVATAR_CY, random(0, 100) < 30 ? EYE_WINK : EYE_BLINK);
  }
  if (eyes_closed && now - blink_started > 180) {
    updateEyes(AVATAR_CX, AVATAR_CY, EYE_OPEN);
    eyes_closed   = false;
    next_blink_at = now + random(2500, 5000);
  }
#endif

#if ANIM_SPARKLE
  // Two sparkles pulsing out of phase.
  static unsigned long last_sparkle = 0;
  if (now - last_sparkle > 350) {
    last_sparkle  = now;
    sparkle_frame = (sparkle_frame + 1) % 3;

    const int r = SPARKLE_CLEAR_R;
    gfx->fillRect(SPARKLE_1_X - r, SPARKLE_1_Y - r, r * 2 + 1, r * 2 + 1, C_CARD);
    gfx->fillRect(SPARKLE_2_X - r, SPARKLE_2_Y - r, r * 2 + 1, r * 2 + 1, C_CARD);

    static const int size_a[3] = { 5, 3, 6 };
    static const int size_b[3] = { 3, 5, 4 };
    drawSparkle(SPARKLE_1_X, SPARKLE_1_Y, C_ACCENT_A, size_a[sparkle_frame]);
    drawSparkle(SPARKLE_2_X, SPARKLE_2_Y, C_ACCENT_C, size_b[sparkle_frame]);
  }
#endif

#if ANIM_FLOATING
  // Status pill drifts +/-1px.
  static unsigned long last_float = 0;
  if (now - last_float > 250) {
    last_float = now;
    float_offset += float_dir;
    if (float_offset >= 2)       float_dir = -1;
    else if (float_offset <= -1) float_dir =  1;

    gfx->fillRect(CARD_L_X + 6, STATUS_PILL_Y - 2,
                  STATUS_PILL_W + 6, STATUS_PILL_H + 8, C_CARD);
    drawPill(STATUS_PILL_X, STATUS_PILL_Y + float_offset,
             STATUS_PILL_W, STATUS_PILL_H, NAMETAG_STATUS, C_ACCENT_D, C_INK);
  }
#endif

  delay(20);   // ~50fps
}
