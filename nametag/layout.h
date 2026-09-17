/*
 * ==============================================================================
 *  layout.h  —  where everything sits on the 320x240 screen
 * ==============================================================================
 *  Every coordinate the badge uses is defined once, here. The static render
 *  pass and the animation loop both read from these, so nudging a card moves
 *  its animation with it — no hunting for duplicated numbers.
 *
 *  Screen origin is top-left. X grows right (0-319), Y grows down (0-239).
 *
 *  Want a different arrangement? Change the numbers here; draw.cpp adapts.
 * ==============================================================================
 */

#pragma once

// ---- Screen -----------------------------------------------------------------
#define SCREEN_W            320
#define SCREEN_H            240

// ---- Header strip -----------------------------------------------------------
#define HEADER_H             24
#define HEADER_TEXT_Y         8
#define HEADER_SPARKLE_INSET 16

// ---- Cards ------------------------------------------------------------------
#define CARD_TOP             34    // both cards share this top edge
#define CARD_H              172
#define CARD_RADIUS          14

// Left card: avatar + org + status
#define CARD_L_X             12
#define CARD_L_W            106

// Right card: name + role + tags
#define CARD_R_X            126
#define CARD_R_W            (SCREEN_W - CARD_R_X - 12)   // 182

// ---- Left card contents -----------------------------------------------------
#define AVATAR_CX           (CARD_L_X + CARD_L_W / 2)    // 65
#define AVATAR_CY           (CARD_TOP + 54)              // 88
#define AVATAR_R             32

#define ORG_PILL_X          (CARD_L_X + 12)
#define ORG_PILL_Y          (CARD_TOP + 104)
#define ORG_PILL_W          (CARD_L_W - 24)
#define ORG_PILL_H           18

#define STATUS_PILL_X       (CARD_L_X + 8)
#define STATUS_PILL_Y       (CARD_TOP + 130)
#define STATUS_PILL_W       (CARD_L_W - 16)
#define STATUS_PILL_H        22

// ---- Right card contents ----------------------------------------------------
#define NAME_X              (CARD_R_X + 16)
#define NAME_Y_BITMAP       (CARD_TOP + 14)   // bitmap names sit slightly higher
#define NAME_Y_TEXT         (CARD_TOP + 18)   // than font-rendered ones

#define ROLE_PILL_X         (CARD_R_X + 16)
#define ROLE_PILL_Y         (CARD_TOP + 44)
#define ROLE_PILL_W         145
#define ROLE_PILL_H          20

#define DIVIDER_X           (CARD_R_X + 14)
#define DIVIDER_Y           (CARD_TOP + 74)
#define DIVIDER_W           (CARD_R_W - 28)

#define TAG1_X              (CARD_R_X + 14)
#define TAG1_Y              (CARD_TOP + 86)
#define TAG1_W               96
#define TAG2_X              (CARD_R_X + 114)
#define TAG2_Y              (CARD_TOP + 86)
#define TAG2_W               56
#define TAG3_X              (CARD_R_X + 14)
#define TAG3_Y              (CARD_TOP + 110)
#define TAG3_W              110
#define TAG_H                18

// ---- Sparkles (animated) ----------------------------------------------------
#define SPARKLE_1_X         (CARD_R_X + CARD_R_W - 24)
#define SPARKLE_1_Y         (CARD_TOP + 120)
#define SPARKLE_2_X         (CARD_R_X + CARD_R_W - 36)
#define SPARKLE_2_Y         (CARD_TOP + 140)
#define SPARKLE_CLEAR_R       7    // half-size of the box repainted each frame

// ---- Footer -----------------------------------------------------------------
#define FOOTER_RULE_Y       (SCREEN_H - 22)
#define FOOTER_TEXT_Y       (SCREEN_H - 14)
#define FOOTER_LEFT_X        14
#define FOOTER_RIGHT_X      (SCREEN_W - 130)

// ---- Background dot grid ----------------------------------------------------
#define DOT_GRID_START       20
#define DOT_GRID_STEP        40
