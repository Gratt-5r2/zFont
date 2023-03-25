# zFont
Initially, the project was started at the request of the Chinese community.  
The plugin allows to use TTF font in the Gothic 1 and Gothic 2. It is recommended to use UTF-8 in the scripts.  
  
  
For the plugin to work correctly place `Default.ttf` to the `Gothic\System\Fonts` directory. This font will replace all game fonts.  
To replace a specific font, its name should be identical to the game one: `FONT_10_BOOK.TGA` -> `FONT_10_BOOK.TTF`.  

If the font you have chosen seems small, increase the `SystemPack.ini > [Interface] > Scale` option.  

Font configurations:
```ini
SystemPack.ini > [Font] > Scale
; ... Increases the font size without interface scaling.
; Be careful, so large font may be out of item bounds.

SystemPack.ini > [Font] > DrawShadow
; ... Drops a font shadow to make it more authentic.

SystemPack.ini > [Font] > DrawHighlight
; ... Highlights the font at the top.

SystemPack.ini > [Font] > DefaultSystemFont
; ... Name of the default system font name
; if the Default.ttf is inexists.
```

# Third-party:  
**libschrift:** https://github.com/tomolt/libschrift
