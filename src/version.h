/*
  Created by tan (trinity09181718@gmail.com)
  Copyright (c) 2022-2023 tan
  All rights reserved.

* Please contact trinity09181718@gmail.com if you need a commercial license.
* This software is available under GPL v3.

* This library and related software is available under GPL v3.

  FabGL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FabGL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with FabGL.  If not, see <http://www.gnu.org/licenses/>.
 */

// 更新履歴
// 2023/04/15 V1.0.3 format(write_id)で.d88のsectorSizeフィールドが0x00になっていた不具合改修。
// 2022/12/30 v1.0.2 uPD765A writeIDコマンド処理(format)速度改善。microSD(Transcend 8GB)で計算上20秒弱。
//                   実測(n-basicコマンドformat 2)だと19秒。
//                   fdモータ停止後のfdモータスピンアップ待ち時間が入った場合の実測(n-basicコマンドformat 2)だと22秒。
//                   spiffsマウント対応。Read Onlyとしての運用なら利用可。詳細は uPD765A.CPP uPD765A::write_id() メソッド内コメントを参照。
//                   青色LED点滅で、こもり50msec周期1秒間をオープンループ形式50msec周期1秒間に変更しpreferences保管処理完了後に開始するようにした。
//                   html 表示でfd0~fd3に併記でdrive 1~4とdrive A: ~ drive D:を入れた。
// 2022/12/25 v1.0.1 Arduino 2.0.6 based on ESP-IDF 4.4.3での動作確認。特に異常検出無し。
//                   .arduino15/packages/esp32/hardware/esp32/2.0.6/tools/partitions のパーティション定義csvに必須(?)追加セクション有り(coredump)
//                   JTAGと関連? 独自パーティション定義する場合は要注意。
// 2022/12/18 v1.0.1 オーディオ出力回路間違い修正
//                   Wi-Fi接続後に接続断検出後の復帰シーケンスでメモリーリーク発生するのを改修
//                     microSD /PC8001/MEDIA/DISKの拡張子d88ファイルリスト内strdup()ぶんの解放もれ
// 2022/12/17 v1.0.0 GitHub公開 
// 2022/12/11 v1.0.0
//                   

#pragma once

#define PC8031FABGL_VERSION_MAJOR       1
#define PC8031FABGL_VERSION_MINOR       0
#define PC8031FABGL_VERSION_REVISION    3

#define PC8031FABGL_VERSION             ( ((int)PC8031FABGL_VERSION_MAJOR) << 16 \
                                        | ((int)PC8031FABGL_VERSION_MINOR) << 8 \
                                        | ((int)PC8031FABGL_VERSION_REVISION) )

