CY8CKIT-049_touch_to_serial
===

PSoC 4200 Prototyping Kit (CY8CKIT-049-42XX)で、横一列に配線されたタッチセンサに触っている手の状態を取得し、手が左右どちらに動いているかを検出してシリアル(今の所I2C)で送信します。自作Project DIVA Arcadeコントローラのスライダー部分に使っています。

[【スライダー付】DIVAアーケードコントローラの創りかた【C94】](http://www.nicovideo.jp/watch/sm32448697)

## 使い方
このリポジトリをクローンした上で、SCB_Bootloaderディレクトリ([CY8CKIT-049-42xx Kit Only (Kit Design Files, Documentation, Examples)](http://japan.cypress.com/documentation/development-kitsboards/psoc-4-cy8ckit-049-4xxx-prototyping-kits)をインストール後PSoC CreatorのStart Page内Examples and Kitsから辿ることで同名ディレクトリのプロジェクトを生成できます)をクローンしたディレクトリと同じ場所に置きます。
```bash
$ git clone (ここ).git
$ ls
CY8CKIT-049_touch_to_serial SCB_Bootloader
```
PSoC Creator上でビルドし、ブートローダーを経由してバイナリを書き込みます。
