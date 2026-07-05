### Plan

### Decided
 - [x] Item
    - Item
        - Hame


- [x] Test
- [ ] Build

# Project

- [Introduction](#introduction)
- [Installation](#installation)
- [Usage](#usage)


#### [Goggle](https://www.google.com/)

USE `protocol()` function to start `communicating`

> this is a quote

***

|Name|Age|City|
|:----|:---:|----:|
|Sunil Kumar|38|<details><summary>Game</summary> ``` Hello ``` </details>|

1. Fruits
   - Apple
   - > [Introduction](#introduction)

2. Vegetables
   - Potato
   - Onion

> [!NOTE]
> This is a note.
> develper 


### Mathematics
$$
x = \frac{-b \pm \sqrt{b^2-4ac}}{2a \int \frac{an-2}{b^2}}
$$


## Introduction

## Installation

## Usage

```
      ┌------------┐                    
----->│ Display    │
      │            |
      └────────────┘ 
            ↑
      ┌------------┐
----->│ Controller │
      │            |
      └────────────┘
            ↑
      ┌------------┐                    
----->│ Interface  │
      │            |
      └────────────┘ 



        <------ Period T ------>
      ┌────────┐      ┌────────┐
CLK───┘        └──────┘        └────► Time
      ↑        ↑      ↑
   Rising   Falling Rising

   
```



> [!SUCCESS]
> Information

> [!TIP]
> Success

> [!WARNING]
> Warning

> [!IMPORTANT]
> Important





```mermaid
flowchart TD
    Start --> Init[Initialize MCU]
    Init --> OLED[Initialize OLED]
    OLED --> MainLoop[Main Loop]
    MainLoop --> ReadGPIO[Read GPIO]
    ReadGPIO --> Decision{Button Pressed?}
    Decision -->|Yes| SendIR[Send NEC IR]
    Decision -->|No| UpdateDisplay[Update OLED]
    SendIR --> MainLoop
    UpdateDisplay --> MainLoop


    
```

```mermaid
flowchart LR
    A[Power On] --> B{Battery OK?}
    B -->|Yes| C[Start System]
    B -->|No| D[Show Error]
```


```mermaid
flowchart TD
    A[Rectangle]
    B(Rounded Rectangle)
    C{Decision}
    D((Circle))
    E[/Input/]
    F[\Output\]
```

```mermaid
flowchart TD
    A[Start] --> B[Process]
    B --> C[End]

    style A fill:#90EE90
    style B fill:#87CEFA
    style C fill:#FFD700
```

```mermaid
flowchart TD
    Start([Start])

    Start --> Init[Initialize Hardware]
    Init --> OLED[OLED Init]
    OLED --> GPIO[GPIO Init]
    GPIO --> Loop[Main Loop]

    Loop --> Button{Button Pressed?}

    Button -->|Yes| IR[Transmit NEC Code]
    Button -->|No| Display[Refresh OLED]

    IR --> Delay[Delay]
    Display --> Delay

    Delay --> Loop
```

### Sequence
```mermaid
sequenceDiagram
    participant User
    participant MCU
    participant OLED

    User->>OLED: Press Button
    MCU->>OLED: Update Display
    OLED-->>MCU: Done
    MCU-->>User: LED ON
```


### state diagram

```mermaid
stateDiagram
    [*] --> Idle
    Idle --> Running : Start
    Running --> Paused : Pause
    Paused --> Running : Resume
    Running --> Finished : Stop
    Finished --> [*]
```

### Class Diagram

```mermaid
classDiagram

class Screen{
    char* name
    Screen* parent
    addChild()
}

class Display{
char test
    draw()
    clear()
}

class OLEDDisplay{
char test
    draw()
    clear()
}

Screen --> Display
Screen --> OLEDDisplay
```


### Entity Diagram

```mermaid
erDiagram

    USER ||--o{ ORDER : places
    ORDER ||--|{ PRODUCT : contains
    PEER ||--o{ PRODUCT : owns

    USER {
        int id PK
        string name
        string email
    }

    ORDER {
        int id PK
        int user_id FK
        date order_date
    }

    PRODUCT {
        int id PK
        int order_id FK
        int peer_id FK
        string name
        float price
    }

    PEER {
        int id PK
        string name
    }
```

### git graph
```mermaid
gitGraph
    commit
    commit
    commit
    branch feature
    commit
    commit
    branch test
    commit
    commit
    branch game
    checkout game
    commit
    checkout test
    merge feature
    checkout game
    commit
    merge feature
    checkout feature
    commit
    commit
    checkout main
    merge feature
    commit
```


### Grant Chart
```mermaid
gantt
    title Embedded Project

    dateFormat YYYY-MM

    section Hardware
    PCB Design      :done, h1, 2026-01-01,5d
    Assembly        :active, h2, after h1,4d
    Build           :active, h3, after h1,3d

    section Software
    Driver          :s1, 2026-01-10,5d
    Testing         :s2, after s1,4d
```


### Pie chart

```mermaid
pie
    title Project Progress
    "Completed" : 90
    "Pending" : 8
    "Testing" : 2
```


### Mind Map

```mermaid
mindmap
  root((Project))
    Software
      Hello
            Testing
            Hllo
                  ((PPTES))
                  Hllo
    Teting
      Gameplay

    Controller
      GPIO
      UART
      SPI
      I2C
    OLED
      SSD1306
      Fonts
            Large
            [Midium]
            ((Small))

      [PPI_D]
            HELLO
    IR
      NEC
      RC5

```



### Time Line

```mermaid
timeline
    title STM32 Learning

    2025-06 : GPIO
         : UART

    2026 : OLED
         : FreeRTOS
         : BLE

    2027 : RTOS Project
```

```vega-lite
{
  "mark": "bar",
  "data": {
    "values": [
      {"module":"GPIO","value":100},
      {"module":"UART","value":80},
      {"module":"Test","value":10}
    ]
  },
  "encoding": {
    "x": {"field":"module","type":"nominal"},
    "y": {"field":"value","type":"quantitative"},
    "color": {"field":"module"}
  }
}
```


### Journey Diagram

```mermaid
journey
    title Device Setup

    section Installation
      Connect Power: 5: User
      Connect OLED: 4: User

    section Programming
      Flash Firmware: 5: User
      Test Display: 4: Engineer
```


### Requirement Diagram

```mermaid
requirementDiagram

requirement R1 {
    id: 1
    text: OLED shall initialize in under 100 ms
    risk: Low
    verifymethod: Test
}
```


### Footnotes
STM32 supports multiple timers.[^1]

[^1]: See the reference manual.

### Math

$
F = (ma^{\sin (2 \cdot\theta)^2\int {5+\frac{\pi}{\exp}}})^{\pi - 4}
$

<kbd>Ctrl</kbd> + <kbd>C</kbd>



### Defination List
GPIO
: General Purpose Input Output

UART
: Universal Asynchronous Receiver Transmitter


### Video Demo
<video controls width="400">
    <source src="demo.mp4" type="video/mp4">
</video>




```wavedrom
  { name: "CLK",  wave: "P......."},
  { name: "DATA", wave: "x.1010x."}
]}
```

```wavedrom
{ signal: [
  { name: "SCLK", wave: "P.....P.|......." },
  { name: "MOSI", wave: "x.=.=.=.|=.=.=.x", data:["1","0","1","0","1","1"] },
  { name: "MISO", wave: "x.=.=.=.|=.=.=.x", data:["0","1","1","0","0","1"] },
  { name: "CS",   wave: "10......|......1" }
]}
```

### Custom wave
```wavedrom
{ signal: [
  { name: "SCLK", wave: "P.......|......." },
  { name: "MOSI", wave: "x.=.=.=.|=.=.=.x", data:["Sunil","0","1","0","1","1"] },
 
]}
```

### Reset
```wavedrom
{ signal: [
  { name: "RESET", wave: "01......." },
  { name: "CLK",   wave: "P........" },
  { name: "READY", wave: "0.....1.." }
]}
```

### Define
```wavedrom
{ signal: [
  { name: "RESET", wave: "01...x...." },
  { name: "CLK",   wave: "P......." },
  { name: "READY", wave: "0.....1." }
]}
```

### Test

```wavedrom
{ signal: [
  { name: "TX",
    wave: "1x1010101=====x"
  }
]}
```


| Status | Symbol |
|:--------|:--------:|
| Pending | ⏳ |
| In Progress | 🔄 |
| Completed | ✅ |
| Failed | ❌ |
| Skipped | ⏭️ |
| Blocked | 🚫 |