# CZ Robotic Cat 🐱⚙️

![CZ Robotic Cat](assets/c3b26964-d6f5-4961-8b3f-12e7e591df06.png)

一个会根据 BNB Chain 链上数据做出物理反应的机器人猫项目，行为模型灵感来自 CZ 在公开场合展现出的决策风格。

> 个人研究 / 玩具项目，与 Binance 或 CZ 本人没有任何从属或合作关系。

---

## 功能特点

- 实时获取 BNB 行情（价格、24 小时成交量、涨跌幅等）
- 简单的「情绪」/ 状态机，用来判断市场处于：
  - **CALM**（平静）
  - **ALERT**（留意）
  - **CAUTIOUS**（谨慎）
  - **PANIC**（极端波动）
- 将状态映射到实体机器人猫的动作：
  - 眼睛（RGB / NeoPixel）颜色变化
  - 尾巴 / 头部舵机姿态变化
  - 可选蜂鸣器 / 震动等提示
- 使用 ESP32 通过 Wi‑Fi 连接到后端的 MQTT 服务，实时接收状态

---

## 项目结构

```text
CZ-Robotic-Cat/
├─ README.md                # 项目说明（本文件）
├─ LICENSE                  # MIT 开源协议
├─ firmware/                # 运行在 ESP32 上的固件代码
│  ├─ platformio.ini
│  └─ src/
│     └─ main.cpp
├─ backend/                 # Python 后端，负责抓取 BNB 数据并计算状态
│  ├─ app.py
│  ├─ requirements.txt
│  ├─ config.example.yaml
│  └─ services/
│     ├─ bnb_client.py
│     └─ cat_state.py
└─ docs/                    # 文档
   ├─ architecture.md
   ├─ hardware.md
   └─ behavior_model.md
```

---

## 硬件假设

参考搭建配置：

- ESP32 DevKitC 或类似开发板
- 2× SG90 / MG90S 舵机（一个控制头部，一个控制尾巴）
- 2× RGB LED 或一小段 NeoPixel 灯条作为「眼睛」
- 可选：
  - 小蜂鸣器（提示音）
  - IMU / 姿态传感器
  - LiPo 电池 + 充电 / 供电模块

具体接线说明见 `docs/hardware.md`。

---

## 后端（backend）使用方法

需要安装 Python 3.9+。

```bash
cd backend
python -m venv .venv
source .venv/bin/activate  # Windows: .venv\Scripts\activate
pip install -r requirements.txt

cp config.example.yaml config.yaml
# 按实际情况修改 config.yaml（BNB 接口地址、MQTT 服务器等）

python app.py
```

后端会循环执行：

1. 向交易所 / API 请求 BNB 的 24h 行情数据；
2. 通过简单的行为模型计算出一个 `cat_state`；
3. 以 JSON 的形式发布到 MQTT 主题 `cat/state`。

---

## 固件（firmware）使用方法

固件使用 PlatformIO 构建，目标平台为 ESP32。

```bash
cd firmware
pio run
pio run --target upload
```

在刷写前，需要编辑 `src/main.cpp` 中的：

- `WIFI_SSID` / `WIFI_PASSWORD`
- `MQTT_BROKER` / `MQTT_PORT`

---

## 状态机说明

机器人猫目前支持四种高层状态：

- `CALM`  —— 市场波动很小，链上情况健康
- `ALERT` —— 波动开始变大，需要留意
- `CAUTIOUS` —— 波动较大或成交量异常，需要谨慎
- `PANIC` —— 极端波动或异常情况，进入高度紧张状态

更细节的状态设计和映射规则见 `docs/behavior_model.md`。

---

## 声明 / 免责声明

- 这是一个个人实验性质的机器人项目；
- **不是** 交易系统，不构成任何投资建议；
- 项目中提到的「CZ 风格」仅基于公开访谈 / 推文等可见信息的抽象总结，并非真实人格建模；
- 与 Binance 交易所及 CZ 本人均无官方关联。

---
