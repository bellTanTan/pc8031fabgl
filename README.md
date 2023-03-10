# pc8031FabGL

## 1.概要
拙作n80FabGLよりディスクユニット部のみ抜き出してNEC/PC-8001シリーズのミニフロッピーディスクユニットとして動作するハードとソフトです。microSDに用意した2Dタイプd88ファイルを利用することが出来ます。

## 2.はじめに

旧友から煽られて作ってみました(笑) 拙作n80FabGLより抜き出した形式なのでDMAインタフェース実装はありません。あくまでNEC/PC-8001シリーズの8255PIOインタフェースと言うことです。

## 3.最重要項目

大切な過去資産/中古資産が作業内容によっては復旧不可能な破損状態になるかも知れません。このリポジトリを参考にして損害が発生しても一切保証出来ません。ご了承願います。

## 4.回路図

回路図を示します

![回路図](/img/000.jpg)

PC本体とのつなぎとしてPC-8001mkIIミニフロッピーディスクインタフェース端子番号を明記しています。PC-8001対応の場合はPC-8033等の外部8255PIOインタフェースが必要です。PC-8033の情報は[MukaiLab様](https://blog.goo.ne.jp/tk-80/e/fd42e45f6f2e993327d567582c0df1d0)の情報が、とてもまとまっていますので一読されると良いと思います。回路としてlsbfirst/msbfirst混在してます。PCB化して販売する訳でもないし理屈がわかっていればOKだと思うので、これでいいかと思ってます(笑)

実験レベルと言う事でブレッドボード回路化でPC本体との接続ケーブルはアンフェノール36ピンオスコネクタで全ピンストレート結線なプリンタケーブル(手持ち死蔵品)を途中で切って各ピンをテスターで導通確認し信号線を見極めて1本ずつかしめてピンソケット化しテプラ貼って再利用しました。FG網線(シールド線)とGND線は全結線して2本ピン化しました。手持ちPC-8001mkIIユーザーズマニュアルは自炊していないので掲載すると見にくいです。

![手持ちユーザーズマニュアル](/img/013.jpg)

PC-8001mkIIミニフロッピーディスクインタフェースの詳細をインターネットアーカイブで公開されている[PC-8001mkIIユーザーズマニュアル](https://archive.org/details/PC8001mk-II-users-manual)より以下無断転載します。自作フロッピーディスクケーブルも掲載します。

![ミニフロッピーディスクインタフェース](/img/014.jpg)

![フロッピーディスクケーブル](/img/050.jpg)

電源は本体からとってPC-8001mkIIの場合はPC-8001mkII汎用I/Oポートインタフェースよりとるのが良いと思います。同じくインターネットアーカイブで公開されている[PC-8001mkIIユーザーズマニュアル](https://archive.org/details/PC8001mk-II-users-manual)より以下無断転載します。自作電源ケーブル(VCC/GND線のみD-SUB9シェル内結線)も掲載します。

![汎用I/Oポートインタフェース](/img/015.jpg)

![電源ケーブル](/img/051.jpg)

## 5.部品表
手持ち資材で購入元/品番が不明のものがあります。ご了承願います。

|部品番号|部品名|URL|数量|備考|
|----|----|----|----|----|
|ESP32-WROOM-32 Devkit|ESP32-DevkitC|[ESP32-DevkitC](https://akizukidenshi.com/catalog/g/gM-11819/)|1|新規に購入する場合は各種不具合改修されたD版以降が良いと思います。E版だとフラッシュメモリサイズが16Mあるので1Mファーム(現状ファームサイズは912173bytes)に使って残り15Mをファイルシステム化するとmicroSDも不要なシステムが出来そうです。ESP32-WROVER Devkitの場合FabGLでのmicroSD接続方法が異なりますので要注意です。|
|AE-MICRO-SD-DIP|マイクロSDカードスロットDIP化キット|[AE-MICRO-SD-DIP](https://akizukidenshi.com/catalog/g/gK-05488/)|1|プルアップ/レベルシフト/5V化無しです|
|PJ321S3|3.5mmオーディオコネクタ変換基板|[PJ321S3](https://www.aitendo.com/product/18208)|1|秋月さんの同等基板化品利用もありです|
|MM-TXS01|2電源8bit双方向レベルシフト基板|[MM-TXS01](https://www.amazon.co.jp/%E3%82%B5%E3%83%B3%E3%83%8F%E3%83%A4%E3%83%88-MM-TXS01-%E3%83%A2%E3%82%B8%E3%83%A5%E3%83%BC%E3%83%AB/dp/B00J7LEQUS/ref=sr_1_1?__mk_ja_JP=%E3%82%AB%E3%82%BF%E3%82%AB%E3%83%8A&keywords=MM-TXS01&linkCode=qs&qid=1671160238&sourceid=Mozilla-search&sr=8-1)|1|手持ち品から利用しました|
|IC1|SN74HC165N||1|手持ち品から利用しました|
|IC2|SN74HC165N||1|手持ち品から利用しました|
|IC3|SN74HC595N||1|手持ち品から利用しました|
|IC4|SN74HC595N||1|手持ち品から利用しました|
|LED1|砲弾型5mm青色LED||1|手持ち品から利用しました|
|LED2|砲弾型5mm赤色LED||1|手持ち品から利用しました|
|LED3|砲弾型5mm赤色LED||1|手持ち品から利用しました|
|LED4|砲弾型5mm赤色LED||1|手持ち品から利用しました|
|LED5|砲弾型5mm赤色LED||1|手持ち品から利用しました|
|C1|セラミックコンデンサ 50V/0.1μF||1|手持ち品から利用しました|
|C2|小形アルミニウム電解コンデンサ 105℃ 50V/10μF||1|手持ち品から利用しました|
|C3|セラミックコンデンサ 50V/0.1μF||1|手持ち品から利用しました|
|C4|セラミックコンデンサ 50V/0.1μF||1|手持ち品から利用しました|
|C5|セラミックコンデンサ 50V/0.1μF||1|手持ち品から利用しました|
|C6|セラミックコンデンサ 50V/0.1μF||1|手持ち品から利用しました|
|R1|1/4W 10kΩ||1|手持ち品から利用しました|
|R2|1/4W 270Ω||1|手持ち品から利用しました|
|R3|1/4W 150Ω||1|手持ち品から利用しました|
|R4|1/4W 220Ω||1|手持ち品から利用しました|
|R5|1/4W 330Ω||1|手持ち品から利用しました|
|R6|1/4W 330Ω||1|手持ち品から利用しました|
|R7|1/4W 330Ω||1|手持ち品から利用しました|
|R8|1/4W 330Ω||1|手持ち品から利用しました|
||SAD-101ブレッドボード|[SAD-101](https://www.amazon.co.jp/%E3%82%B5%E3%83%B3%E3%83%8F%E3%83%A4%E3%83%88-SAD-101-%E3%83%8B%E3%83%A5%E3%83%BC%E3%83%96%E3%83%AC%E3%83%83%E3%83%89%E3%83%9C%E3%83%BC%E3%83%89/dp/B00DSKCS68/ref=sr_1_1?__mk_ja_JP=%E3%82%AB%E3%82%BF%E3%82%AB%E3%83%8A&crid=2YXYDA8FBFZD9&keywords=SAD-101&qid=1671278670&sprefix=sad-101%2Caps%2C269&sr=8-1&th=1)|4|手持ち品から利用しました|
||アンフェノール36ピンオスコネクタ全ピンストレート結線プリンタケーブル||1|手持ち品から利用しました|
||8ピンコネクタ||2|手持ち品から利用しました|
||4ピンコネクタ||3|手持ち品から利用しました|
||かしめ用オスピン||28|手持ち品から利用しました|

## 6.ブレッドボード回路化

部品配置と配線を順追って記載します。LED/抵抗/コンデンサ/SN74HC165N/SN74HC595Nの配置とジャンパー線

![ブレッドボード1](/img/001.jpg)

オーディオ出力関連箇所

![ブレッドボード2](/img/002.jpg)

ESP32-DevkitC/マイクロSDカードスロットDIP化キット/3.5mmオーディオコネクタ変換基板/MM-TXS01の配置

![ブレッドボード3](/img/003.jpg)

3.3V電源/microSD/オーディオ出力のジャンプワイヤ。オーディオ出力回路に間違いがありました。オーディオ出力はb-7より行って下さい。

![ブレッドボード4](/img/004.jpg)

ESP32-DevkitC/MM-TXS01のジャンプワイヤ

![ブレッドボード5](/img/005.jpg)

MM-TXS01/SN74HC165N/SN74HC595Nのジャンプワイヤ

![ブレッドボード6](/img/006.jpg)

SN74HC165N/SN74HC595Nの外部IOのジャンプワイヤ

![ブレッドボード7](/img/007.jpg)

ミニフロッピーディスクケーブル接続

![ブレッドボード8](/img/008.jpg)

ミニフロッピーディスクケーブルのPC側8255PA(PA0〜PA7)とPC側8255PC(PC0〜PC3)の接続状態の拡大

![ブレッドボード9](/img/009.jpg)

ミニフロッピーディスクケーブルのPC側8255PB(PB0〜PB7)とPC側8255PC(PC4〜PC7)と/RESET /EXTON GNDの接続状態の拡大

![ブレッドボード10](/img/010.jpg)

/EXTON有効(外部接続機器有無信号の有化)状態。ミニフロッピーディスクとして利用する場合/EXTONは有効(GND落とし)です。

![ブレッドボード11](/img/011.jpg)

/EXTON無効(外部接続機器有無信号の無化)状態。ミニフロッピーディスクとして利用しない場合(配線確認/他利用(フォトカプラ入れて...))/EXTONは無効(5V上げ)です。

![ブレッドボード12](/img/012.jpg)

# 7. microSD 準備

以下の構成で準備します。

    microSD
    /
    +--PC8001
       +-- PC-8031-2W.ROM [2048bytes]
       +-- PC-80S31.ROM   [2048bytes=PC-8801mkIISR/FR/...]
       +-- DISK.ROM       [8192bytes=PC-8801MA/MC/MH/...]
       |
       +--MEDIA
          +-- DISK
              +--- *.d88  5.25 inch 2D Type

PC-8031-2W.ROMは2KiB(2048byte)の物です。PC-80S31.ROMは2KiB(2048byte)の物です。(PC-80S31Kは8KiB(8192byte)のマスクROMだったような気がします) DISK.ROMは 8KiB(8192byte)の物です。ファイル名を合わせて1つmicroSDに入れます。複数用意した場合ロード優先順位は上記順です。  

2KiB(2048byte)なROMファイルを利用できてる場合、フロッピーディスクは4基(fd0,fd1,fd2,fd3)です。8KiB(8192byte)なROMファイルでも4基可能だと思ったのですが動作させてみると2基(fd0,fd1)しか出来ませんでした。  

d88ファイルは規定箇所へ配置します。d88ファイルのファイル名は半角英数字記号(一部UNIX的ファイルシステムとして利用不可な記号あり)で準備します。  

手抜きなIPLコードをd88ファイルとしてトラック0セクタ1(256バイト)に書き込んだファイルtest2/[OS]_IPL_TEST_disk_(BASIC_formatted).d88を用意しています。IPLコードの雛形Z80アセンブラソースファイルとd88ファイルのトラック0セクタ1(256)バイトを更新するCソースファイルも入れています。linuxな環境があると $ make で[OS]_IPL_TEST_disk_(BASIC_formatted).d88を更新するようMakefileも入れています。動作確認(boot実験/拡張お遊び/等)の一つとして利用して下さい。

## 8.ESP32-WROOM-32へのファーム書き込み

資料整備時点(2022/12/17)で[Arduino IDE](https://www.arduino.cc/en/software)は2.0.3がリリースされています。ここでは1.8.19で記載します。Arduino IDE環境定義、esp32環境定義は他サイトを参考にして下さい。[DEKO様サイト](https://ht-deko.com/arduino/esp-wroom-32.html)は情報の網羅レベルが半端ないです。一読されるのをお勧めします。

必要になるライブラリは以下1点です。

    FabGL v1.0.9

ツール→ライブラリ管理よりダウンロードします。関連するライブラリもあると自動判断されたら全てダウンロードします。

ご自身のネットワーク環境に合わせて定義を行います。src/const.hの定義を書き換えます。

ここで実装した手法はファーム書き込み済みESP32-WROOM-32チップ破棄の時にゴミ拾いされて解析されWiFiネットワークを踏み台にされるセキュリティリスクを伴います。企業等で大量(1万個とか10万個とか)生産/破棄する場合はたしかにヤバいと思います。しかしどこの誰とも知れない一般家庭のWiFiネットワーク侵入(踏み台化)をゴミ拾いから行うヤツ居るのか(笑) エロサイト閲覧でbot感染してbot組み込まれる方がよっぽど自然な流れなセキュリティリスクと個人的には思います。どうしても気になる場合は廃棄の時、金槌等で文字通り破片になるまでESP32-WROOM-32を粉々に砕いて廃棄して下さい。セキュリティガチガチな実装じゃないとイヤだ星人な方はご自身が納得行く形式に組み直して下さい。

以下書き換え対象のsrc/const.hの内容です。元値は架空のネットワーク定義値です。  

    12 const char * ssid     = "test001";
    13 const char * password = "password001";
    17 const char * ntpServer       = "192.168.1.250";
    21 const IPAddress ip( 192, 168, 1, 101 );     // for fixed IP Address
    22 const IPAddress gateway( 192, 168, 1, 1 );  // gateway
    23 const IPAddress subnet( 255, 255, 255, 0 ); // subnet mask
    24 const IPAddress DNS( 192, 168, 1, 1 );      // DNS
    26 const char * mDNS_NAME = "pc8031fabgl";
    27 const uint16_t httpTcpPort = 80;            // http TCP Port No

12行目はWi-Fi 2.4GHz 11a/11b/11g/11nのSSIDです。  
13行目は接続先SSIDのパスワードです。  
17行目は利用するntpサーバーです。ローカルネットに自前のntpサーバーが無い場合は普段利用定義してるntpサーバーを記述します。以下から1つ選んでも良いと思います。選択の基準はネットワーク的距離(応答時間)と思います。昔、桜時計で...(草)  

    ntp.nict.jp
    ntp1.jst.mfeed.ad.jp
    ntp2.jst.mfeed.ad.jp
    ntp3.jst.mfeed.ad.jp

21行目は、このESP32-WROOM-32で利用する固定IPアドレス値です。  
22行目は、このESP32-WROOM-32で利用するゲートウェイIPアドレス値です。  
23行目は、このESP32-WROOM-32で利用するサブネットマスク値です。  
24行目は、このESP32-WROOM-32で利用するDNS IPアドレス値です。  
dhcpじゃないとイヤだ星人の方は21行目〜24行目定義は放置してsrc/machine.cppの112行目をコメントにして下さい。  

元

    111 WiFi.mode( WIFI_STA );
    112 WiFi.config( ip, gateway, subnet, DNS );
    113 delay( 10 );

dhcp化

    111 WiFi.mode( WIFI_STA );
    112 //WiFi.config( ip, gateway, subnet, DNS );
    113 delay( 10 );

26行目はmDNSの名称です。  
27行目はhttpサーバーとして利用するTCPポート番号値です。  

/RESETを直接ENに入れています。PC本体にミニフロッピーディスクケーブル挿してPC本体の電源断状態だとESP32-Devkitにファーム書き込みが出来ません。EN(RESET)が入りっぱなしと言うことです。ミニフロッピーディスクケーブルを外してESP32-Devkitにファーム書き込んだら良いと思います。以下組み合わせパターンです。

    ミニフロッピーディスクケーブル装着かつPC本体電源断
    ESP32-Devkitファーム書き込み不可

    ミニフロッピーディスクケーブル装着かつPC本体電源入
    ESP32-Devkitファーム書き込み可

    ミニフロッピーディスクケーブル未装着
    ESP32-Devkitファーム書き込み可

bootログを示します。赤枠の箇所には接続先SSID/定義したIPアドレス値が入ります。

![bootログ](/img/016.jpg)

## 9.SPIFFS利用について

src/emu.hの42行目を有効化(先頭の//を削除)することでSPIFFSを使用可能にしています。

元

    42 //#define _USED_SPIFFS

spiffsを利用する(microSDは利用しない)

    42 #define _USED_SPIFFS

SPIFFSを利用する場合アクセス速度の都合上read only(読み出し専用)が許容範囲です。writeは余りにも遅すぎて実用に耐えられません。このリポジトリのdataディレクトリと同じ配置でdataフォルダを準備してそこにROMイメージファイル、利用予定のd88ファイルを置いてesp32のSPIFFSへ書き込みます。

[ESP32 Sketch Data Upload](https://github.com/lorol/arduino-esp32fs-plugin)を使用しますので環境定義して下さい。Arduino IDE 2.0.x系では利用不可です。Arduino IDE 1.8.19にて利用します。

esp32のE版(16Mbyte FLASH)を利用できる場合はアプリ1MB残りSPIFFSとしたパーティション情報を定義したら良いと思います。

以下、linux版Arduino esp32 v2.0.6の場合のパーティション情報追加です。

    $ cd .arduino15/packages/esp32/hardware/esp32/2.0.6/tools/partitions
    $ vi noota_app1M_spiffs15M.csv
    ------------------------------------------------------------------------------- ↓入力
    # Name,   Type, SubType, Offset,  Size, Flags
    nvs,      data, nvs,     0x9000,  0x5000,
    otadata,  data, ota,     0xe000,  0x2000,
    app0,     app,  ota_0,   0x10000, 0x100000,
    spiffs,   data, spiffs,  0x110000,0xEE0000,
    coredump, data, coredump,0xFF0000,0x10000,
    ------------------------------------------------------------------------------- ↑入力
    $ cat noota_app1M_spiffs15M.csv
    # Name,   Type, SubType, Offset,  Size, Flags
    nvs,      data, nvs,     0x9000,  0x5000,
    otadata,  data, ota,     0xe000,  0x2000,
    app0,     app,  ota_0,   0x10000, 0x100000,
    spiffs,   data, spiffs,  0x110000,0xEE0000,
    coredump, data, coredump,0xFF0000,0x10000,

Arduino esp32 v2.0.6(SDK v4.4.3)よりcoredumpという名称のパーティションが最終箇所に必要になったようです。他のパーティション情報ファイルにも記載あり。詳細不明ｗ

以下、linux版Arduino esp32 v2.0.6の場合のパーティションスキーム定義の追加です。

    $ cd .arduino15/packages/esp32/hardware/esp32/2.0.6/
    $ vi boards.txt
    esp32.menu.PartitionScheme.rainmaker.upload.maximum_size=3145728 <--------------------------- この位置の下へ以下3行追加
    esp32.menu.PartitionScheme.noota_app1M_spiffs15M=16M Flash (No OTA/1MB APP/15MB SPIFFS)
    esp32.menu.PartitionScheme.noota_app1M_spiffs15M.build.partitions=noota_app1M_spiffs15M
    esp32.menu.PartitionScheme.noota_app1M_spiffs15M.upload.maximum_size=1048576

これでArduino IDEのメニューよりパーティション構成として「16M Flash (No OTA/1MB APP/15MB SPIFFS)」が選択可能になります。

参考例としてこのリポジトリ内dataディレクトリに[OS]_IPL_TEST_disk_BASIC.d88を用意しています。ファイル名は30byteが最大です。SPIFFSのイメージ生成にてESP32側のSPIFFSパーティションに書き込むのですが残念ながらイメージ生成時各ファイル日時はtime_t = -1です。それではとても寂しいのでdataディレクトリに元々のファイルの日時情報を別ファイルに持ち込んでESP32側でそのファイルがある場合SPIFFSの各ファイル日時を更新するようにしています。(別件過去プログラムからのコピペですｗ)

使用されているArduino IDE環境がUNIX or *BSD or linux or Macintosh(Mac OS X 以降)の場合SPIFFS向けのdataディレクトリをカレントディレクトリとしたコマンドプロンプト内で以下を実行します。

    $ ls -lan --time-style="+%Y-%m-%d %H:%M:%S" > fileDateTimeList.txt

使用されているArduino IDE環境がWindowsの場合SPIFFS向けのdataディレクトリをカレントディレクトリとしたコマンドプロンプト内で以下を実行します。

    C:>forfiles /c "cmd /c echo @fsize @fdate @ftime @file" > fileDateTimeList.txt

esp32側でファーム書き込み受信待機させます。DevkitC等の場合は特になにもする必要なしです。Arduino IDEのシリアルモニタが開いているとSPIFFS書き込み失敗しますのでArduino IDEのシリアルモニタは閉じて下さい。Arduino IDEのメニューよりESP32 Sketch Data Uploadをクリックして書き込みます。

書き込み系でトラブル回避のためにFLASH ALL ERASE→SPIFFS書き込み→アプリ書き込みの流れで進めると良いと思います。

## 10.動作確認とfdメディア操作

青色LED点灯にてWebサーバ準備完了です。fdメディア操作はWeb画面にて行います。

![Webサーバ準備完](/img/017.jpg)

mDNS名の定義が元のままの場合は以下のように利用されているWebブラウザのURLに入力します。

    http://pc8031fabgl.local/

特定のOS(Android)で特定のWebブラウザ(Google Chrome)でmDNS利用不可が続いています(笑) 絶対mDNS実装するのがイヤなんだと思います(草) AppleのBonjourがイヤなんだろうかｗ RFC定義されてるしAvahiあるし持ってくるだけじゃダメなのかなｗ そんな特定環境の場合はIPアドレス値で入力して下さい。ローカルDNS or hostsファイル等で名前解決できる場合はその名称を利用されているWebブラウザのURLに入力します。

    http://192.168.1.150/

httpのTCPポート番号値を80以外で8080値を指定してる場合は以下の形式になります。

    http://192.168.1.150:8080/

接続成功すると以下の状態が表示されます。以下の画だと既に各ドライブ選択されていますが無い物として見て下さいｗ

![Webサーバ状態1](/img/018.jpg)

各fdのドロップリストをクリックするとmicroSDの/PC8001/MEDIA/DISKに収納した拡張子d88ファイルがアルファベット順で表示されます。

![Webサーバ状態2](/img/019.jpg)

このリポジトリtest2/[OS]_IPL_TEST_disk_(BASIC_formatted).d88をmicroSDの/PC8001/MEDIA/DISKに収納してる場合選択して適用(apply)をクリックします。  

![Webサーバ状態3](/img/020.jpg)

この時、青色LEDが物凄く早く点滅(50msec周期で1秒間)します。それを視認して頂いて本体PCリセットします。モニタに「Hello World!」が出てきたでしょうか？ こんなメッセージを出して終わりなIPLコードを埋め込んでいます(笑)  

![動作確認1](/img/021.jpg)

本体電源を切って最低5秒は待ってから本体電源入れます。今度はモニタに「Hello World!」出てこないかも(爆) 色々な初期化手抜きしてるIPLコードとまるっとわかってしまいます。本体リセットするとモニタに「Hello World!」出てくると思います(草) 

各fdのドロップリストからejectを選択して適用(apply)にてd88なfdメディア排出です。  

d88ファイルでマスターファイルの消失には十分に注意して下さい。損害保証出来ません。  

複数fd装着boot。fd#0以外にメディア装着boot。動作中eject。fd#0,#1装着boot後にfd#2,fd#3に装着。N BASIC DISK SYSTEMでmount/files/remove。N BASIC DISK SYSTEMでremove忘れで複数メディア破損事件。色々と思い出す必要がある昔のルールがあると思います(笑)

よくはまるのがPC-8001mkIIディップスイッチ8ON(Nモード)でN-80 DISK SYSTEMセットしてbootしたら画面に「NON DISK BASIC」と出て「ありゃDISK bootしてNON DISKって...　あっ CTRL+RESETか。ﾃﾍﾍﾟﾛ」あるあるですっｗ

## 11.誤配線等の確認

/EXTON無効状態にして、このリポジトリのtest1/ShiftInOut/ShiftInOut.inoをESP32-DevkitCへファーム書き込んで下さい。PC側にはこのリポジトリのtest1/dio-test {cload DIO}.cmtをCMTロードして下さい。PC側のBASICプログラムをRUNします。0x0000〜0x0FFFの値を8255PB(PB0〜PB7)と8255PC(PC4〜PC7)へ出力します。ESP32側は自己タイミングでポーリングShiftInし前回入力値と変化があれば全ビット反転(NOT)します。そのなかで上位4ビットを赤色LED1〜4に0x01→0x02→0x04→0x08→0x08→0x04→0x02→0x01と定義したのちにShiftOutします。PC側はハンドシェイク無しの自己タイミングで8255PA(PA0〜PA7)と8255PC(PC0〜PC3)を入力し自己が出力した値がビット反転されて来てるか判断します。PC側のロジックをうまく活用(改変)して誤配線の特定に利用して下さい。

どうしてもCMTロード出来ない(環境構築出来ない/環境構築してもCMTロード出来ない/等)場合のためにPC側BASICプログラムのリストを示します。手打ちして下さい。

![動作確認2](/img/023.jpg)

## 12.Webサーバについて

いわゆるセッション時間管理は実装していません。一度Web画面を開くと何時間でもセッション有効です(笑) 製品化して販売する訳でもないのでこれでいいかと思ってます。誤作動するパターンは「Web画面開きっぱでmicroSDを抜いて別機器でmicroSD内容更新してmicroSDを挿してPC電源入れたのちに開きっぱなWeb画面からfdメディア選択すると異なるfdメディアが選択される場合がある」です(笑)

## 13.LED状態表示について

青色LED

    消灯→Wi-Fi接続未
    点灯→Wi-Fi接続完/Webサーバ準備完了
    点滅(500msec周期)→Wi-Fi接続中
    早い点滅(100msec周期)→Wi-Fi接続失敗
    物凄く早い点滅(50msec周期1秒間)→Webサーバfdメディア設定完了

赤色LED1(SN74HC595Nカスケード#0 QD)

    fd#0 busy

赤色LED2(SN74HC595Nカスケード#0 QC)

    fd#1 busy

赤色LED3(SN74HC595Nカスケード#0 QB)

    fd#2 busy

赤色LED4(SN74HC595Nカスケード#0 QA)

    fd#3 busy

青色LEDと赤色LEDの組み合わせ

    青色LED赤色LED1が点滅(500msec周期)→microSDマウント失敗
    青色LED赤色LED2が点滅(500msec周期)→ROMイメージファイルロード失敗(ファイルが無い/特定位置のバイナリ値が既出バイナリ値外/等含む)
    青色LED赤色LED1,2が点滅(500msec周期)→seek音生成のためのメモリ確保失敗

## 14.問題点

拙作n80FabGLと同様でseek音が微妙(笑) format(uPD765AコマンドwriteID)だとまるわかり。旧友曰く「うーん、これ違うやん。もっとこもった感じでドッドッドッドッじゃね」はいはい、そのとおりですっｗ だって実装むずいんだもん。

## 15.最後に

旧友に煽られて実機接続可なDISKユニットとしてエミュ実装してみました。microSDのメンテナンスをWeb画面経由(リスト表示/アップロード対応/ダウンロード対応/名称変更/削除)で出来るようにしてくれと言われそう(笑) そこまで実装したら某製品やん。ヤダよｗ それでもどうしてもと押し切られそう(笑)

