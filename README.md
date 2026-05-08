# StreamBeauty

OBS Studio 向けリアルタイム美肌フィルタープラグインです。  
そばかす・ニキビ跡・肌荒れを目立たなくし、配信映像の肌を自然に整えます。  
GPU（GLSL シェーダー）で処理するためCPU負荷が低く、ゲーム配信との同時使用にも対応しています。

---

## 機能

- **肌スムージング** — YCbCr 色空間による肌領域検出 + バイラテラルフィルター近似
- **明るさ調整** — 肌の透明感を底上げ
- **ブレンド率** — フィルターの強さを全体的にコントロール
- **彩度調整** — 血色感の微調整
- **ビフォーアフター比較** — 画面を左右に分割してフィルター効果を確認

---

## 動作環境

- **OS**: Windows 10 / 11（64bit）
- **OBS Studio**: 30.0 以上

---

## インストール方法

1. [Releases](https://github.com/kanimegane/StreamBeauty/releases) から最新の以下2ファイルをダウンロード
   - `stream-beauty.dll`
   - `stream-beauty-data.zip`
2. 以下の場所にコピー（管理者権限が必要な場合があります）

| ファイル | コピー先 |
|----------|----------|
| `stream-beauty.dll` | `C:\Program Files\obs-studio\obs-plugins\64bit\` |
| `stream-beauty-data.zip` を展開した `stream-beauty\`フォルダ | `C:\Program Files\obs-studio\data\obs-plugins\` |

3. OBS Studio を再起動

---

## 使い方

1. OBS Studio でカメラソース（映像キャプチャデバイス）を右クリック
2. **「フィルタ」** を選択
3. 左下の **「＋」** → **「エフェクトフィルタ」** → **「StreamBeauty Skin Filter」** を追加
4. スライダーで効果を調整

### パラメーター

| パラメーター | 説明 | デフォルト |
|-------------|------|-----------|
| Smoothness | 肌のなめらかさ（高いほど強くかかる） | 0.60 |
| Brightness | 明るさの補正 | 0.05 |
| Blend | フィルター全体の強さ | 0.80 |
| Saturation | 彩度（1.0が元の色、上げると血色感アップ） | 1.10 |
| Before/After Compare | チェックで左半分を元映像・右半分をフィルター適用後に分割表示 | OFF |

---

## 自分でビルドする場合

### 必要なもの

- Visual Studio 2022 以上（C++によるデスクトップ開発）
- CMake 3.28 以上
- Git

### 手順

```bash
git clone --recursive https://github.com/kanimegane/StreamBeauty.git
cd StreamBeauty
cmake --preset windows-x64
cmake --build --preset windows-x64
```

ビルド後、`build_x64/RelWithDebInfo/stream-beauty.dll` と `build_x64/rundir/RelWithDebInfo/stream-beauty/` を上記のインストール先にコピーしてください。

---

## ライセンス

[GPL-2.0](LICENSE)  
OBS Plugin Template をベースに開発しています。
