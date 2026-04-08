# 🔥 LibrePower: Extreme Writing Experience

LibrePower is a high-performance fork of LibreOffice Writer designed to maximize motivation and flow through gamified visual feedback.

![LibrePower Fire Mode](https://raw.githubusercontent.com/zedraxa/LibreWriter-PowerMode/main/branding/fire_preview.gif)

## 🚀 Features

- **Fire Mode Engine**: Real-time flame animations that respond to your typing speed.
- **Total Chaos System**: 
  - **Debris**: Page breaking particles fall as you type.
  - **Embers**: Rising sparks for added heat.
  - **Global Screen Shake**: Every stroke impacts the whole viewport.
- **Combo System**: Build streaks to unlock more intense effects.
- **Smart Placement**: Combo text carefully positioned to stay out of your way.
- **Toggle Control**: Enable or disable the chaos with a single button in the Standard toolbar.

## 📦 Installation

### Linux (.deb)
1. Download the latest `.deb` from the [Releases](https://github.com/zedraxa/LibreWriter-PowerMode/releases) page.
2. Install via terminal: `sudo dpkg -i librepower-*.deb`

### Windows (.exe / .msi)
1. Download the `.msi` installer from the [Releases](https://github.com/zedraxa/LibreWriter-PowerMode/releases) page.
2. Run the installer and follow the instructions.

## 🛠 Building from Source

If you want to build LibrePower manually:

```bash
./autogen.sh --disable-calc --disable-draw --disable-impress
make sw
./instdir/program/soffice.bin --writer
```

## 🤝 Contributing

This is a community-driven project. Hackers and writers welcome!

## 📜 License

LibrePower is part of the LibreOffice project and is subject to the **Mozilla Public License, v. 2.0**.
