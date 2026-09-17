# Prompts that work

Copy-paste these into Claude Code, Codex, Antigravity, or Cursor with this
folder open. They're written the way that actually gets good results: concrete,
one job at a time, and specific about what "done" looks like.

The agent reads `AGENTS.md` on its own, so you don't need to explain the project.

---

## Getting set up

> Set up this project on my machine and confirm it compiles. Don't flash
> anything yet.

> Run the doctor script and tell me what's missing.

> My board isn't being detected. Walk me through it.

---

## Making it yours

> Make the badge say Jun Park, Product Designer at Northwind. Status should be
> "ask me about typography".

> I'm going to a climate tech conference. Rewrite all the badge text — name Mia
> Torres, I work on carbon accounting, and make the three tags something a
> stranger would want to ask about.

> Switch to the midnight theme and drop the brightness to 60 — the venue is dark
> and I want the battery to last all day.

> Turn off the sparkle animation. Keep the blinking.

Ask for several changes at once — they all land in the same file:

> Midnight theme, brightness 70, no dot grid, floating animation on, and change
> my status to "looking for a cofounder".

---

## Non-Latin names

> My name is 김민준 — make the badge show it in Korean.

> Show my name as こんにちは in Japanese. Preview it before you change the config
> so I can check it rendered.

> The Korean name is too wide and getting clipped. Regenerate it smaller.

The agent should run `make_name_bitmap.py --preview` and show you the ASCII art
before flashing. If it doesn't, ask — a missing font produces an empty bitmap
that looks fine until you're staring at a blank screen.

---

## Colors

> Add a theme called THEME_SUNSET using these colors: #2d1b4e background,
> #ff6b35 accent, #f7c59f secondary, #1a0f2e for text.

> Make a theme that matches my company's brand — primary is #0066cc and we use
> #ff9900 for highlights. Fill in the rest so it looks cohesive.

> The mono ink theme's red is too orange. Make it a true red.

Say the word *theme* and the agent edits `theme.h`. Say *change the colors* and
you might get `config.h` instead — be specific.

---

## Layout

> The role pill is cutting off "Senior Staff Engineer". Make it wider.

> Move the sparkles to the top-right corner of the main card.

> Get rid of the left card entirely and make the name huge across the whole
> screen.

That last one is a real code change, not a config change. Expect the agent to
edit `layout.h` and `nametag.ino`, and expect it to take a few tries.

---

## Photos

> I want my photo on the badge. Here's the file: ~/Desktop/me.jpg. Walk me
> through the whole thing including the SD card.

> Prepare this logo for the sticker slot and tell me exactly what to copy where.

---

## Going further

> Add a fourth animation: make the header strip slowly cycle through the theme's
> accent colors.

> Read the battery voltage and show a battery icon in the footer.

> Add a second screen that shows a QR code of my LinkedIn, and switch between
> them when I touch the display.

> Port this to a different ESP32 board — I have a T-Display-S3. Here's its
> pinout: [paste]

---

## Habits worth keeping

**One thing at a time for anything structural.** Text and color changes batch
fine. "Redesign the layout and add three animations and port to a new board" in
one message gets you a mess.

**Make it prove the change.** `./tools/build.sh` takes under a minute and is the
only real evidence. If an agent says "done" without compiling, ask it to
compile.

**Say "don't flash."** Some agents will happily write to your hardware. If you
just want to see the diff, say so.

**Paste the actual error.** Not "it didn't work" — the real output from
`./tools/build.sh` or `./tools/monitor.sh`.

**After engine changes, ask for `./tools/verify.sh`.** It compiles all five
themes. Code that builds under one theme can fail under another.
