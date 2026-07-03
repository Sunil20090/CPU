### Module

Creating a multipurpose module



### Objective

- Work #In-Progress

    - [ ] Creating a `purf board` design using `blue-pill`
    - [ ] attaching components
        - [x] 4 - `Input Buttons`
        - [ ] 1 - `OLED 128x64`
    

    - [ ] Features 
        - [ ] Interrupt
        - [x] LCD driver
        - [x] NEC protocol Driver
        - [ ] Radio Transmitter
        - [x] Radio Reciever
        - [ ] Connector module
        - [x] Programm runner
        - [ ] Walky Talky
            - Controller
            - Speaker
            - Microphone
            - Radio Send
            - Radio Recieve


#### Program Loader Flow

```mermaid

flowchart TD
    I[\KEYBOARD\] --> 
    |UP| C{{Controller}} -->
    O[[OLED]]
    C --> |Sending| NEC
    NEC --> IR-LED
    T{Intrupt} --> |YES| NEC
    C --> |Storing| statue
    I --> 5


style I stroke: #ff0000, fill: #00fcb5f2
style C stroke: #501bb1, fill: #b17327f2

```

### Tasks to do

    - Software
        - Programm runner #WIP⏳
        - Program name to be display ⏳
        - Storage Manager ⏳
        - About page ⏳




<div class="box"></div>
<style>
.box{
    width:41px;
    height:41px;
    background:red;
    animation:spin 2s linear infinite;
}

@keyframes spin{
    from{transform:rotate(0deg);}
    to{transform:rotate(-360deg);}
}
</style>