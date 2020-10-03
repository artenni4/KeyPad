# KeyPad
A small program written on C++ that allows to interpretate some gamepad actions as mouse or keyboard

It uses Xinput library to recieve commands from connected Xbox 360 controller.

It does not include own virtual keyboard, so I recommnend you to download one from site https://freevirtualkeyboard.com/.
You should name it "FreeVK.exe" and put it into some PATH folder(for example C:\Windows, etc or just in same folder with program)

Controls:
Left Analog Stick - Moving mouse
Right Analog stick - Moving mouse Wheel
A button - Left Mouse Click
B button - Right Mouse Click
X button - Call virtual keyboard(look above how to connect it with program)
Start Button - Show controls(there is a bug here, so program is not working during MessageBox)
Back Button - Close program(as it has no GUI, you should close program like that)
Digital Pad - As arrows on keyboard

P.S For saving battary purpose, program will go in sleep mode in 5 seconds inactivity. That means the state of controller will be checked in longer period of time. You can quit sleep mode by just pressing or moving some buttons.
