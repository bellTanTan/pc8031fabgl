/*
  Created by tan (trinity09181718@gmail.com)
  Copyright (c) 2022 tan
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
// 2022/12/18 v1.0.1 オーディオ出力回路間違い修正
//                   Wi-Fi接続後に接続断検出後の復帰シーケンスでメモリーリーク発生するのを改修
//                     microSD /PC8001/MEDIA/DISKの拡張子d88ファイルリスト内strdup()ぶんの解放もれ
// 2022/12/17 v1.0.0 GitHub公開 
// 2022/12/11 v1.0.0
//                   

#pragma once

#define PC8031FABGL_VERSION_MAJOR       1
#define PC8031FABGL_VERSION_MINOR       0
#define PC8031FABGL_VERSION_REVISION    1

#define PC8031FABGL_VERSION             ( ((int)PC8031FABGL_VERSION_MAJOR) << 16 \
                                        | ((int)PC8031FABGL_VERSION_MINOR) << 8 \
                                        | ((int)PC8031FABGL_VERSION_REVISION) )

