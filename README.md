# zFont
Initially, the project was started at the request of the Chinese community.  
The plugin allows to use TTF font in the Gothic 1 and Gothic 2. It is recommended to use UTF-8 in the scripts.  
  
  
For the plugin to work correctly `Default.ttf` will be placed to the `Gothic\System\Fonts` directory. This font will replace all game fonts.  
If this font is not exists, then the system font, which is specified in the SystemPack.ini, will be taken.  
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

<details>
  <summary>Pictures</summary>
  
![2023-03-26_03-57-24](https://user-images.githubusercontent.com/55413597/227749850-2a11009f-01bf-4975-85b8-07b5981347a2.png)  

![2023-03-26_03-57-01](https://user-images.githubusercontent.com/55413597/227749853-3f7c4573-adf5-43d2-9fff-04102e102e91.png)  

![2023-03-26_03-58-35](https://user-images.githubusercontent.com/55413597/227749862-53fc5e70-f9d1-4dd3-b9d6-12448d9674ce.png)  

![2023-03-26_03-58-55](https://user-images.githubusercontent.com/55413597/227749865-1685a7fb-0f81-439c-88fe-e33685d4ae37.png)  

![2023-03-26_04-00-02](https://user-images.githubusercontent.com/55413597/227749869-2a466053-a656-4c4f-8d39-5969e7d719ca.png)  

</details>

# Third-party:  
**libschrift:** https://github.com/tomolt/libschrift
