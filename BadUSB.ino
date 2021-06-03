#include <ArduinoKeyboardLib.h>
#include <elapsedMillis.h>
#include <IRremote.h>

#include <SPI.h>
#include <SD.h>
#include <string.h>

#define KEY_BREAK 0xD0
#define KEY_NUMLOCK 0xDB
#define KEY_SCROLLLOCK 0xCF
#define KEY_SPACE 0xB4

const int chipSelect = 4;
String cmd;
String arg;
String mode;
String lang = "en";
String payload;
String prevCmd;
String prevArg;
char argChar;
char prevArgChar;
char charBuff;
char breakChar;
int defaultDelay = 0;
int led2 = 8;
File root;
File myFile;
bool errLog;
byte inChar[64];
byte modifier[64];
byte outChar[64];
byte modifierKey;

int RECV_PIN = 2;


#define waitDuration 500
int prevCommand=0;

elapsedMillis timeElapsed;
void executeAction(int code);

void setup()
{
  Serial.begin(9600);
  IrReceiver.begin(RECV_PIN,false);
  pinMode(13,OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }


  root = SD.open("/");


  digitalWrite(13,HIGH);
  delay(300);
  digitalWrite(13,LOW);

}


byte convertLangChar(byte in) {
  for (int i = 0; i <= 64; i++) {
    if (inChar[i]) {
      if (inChar[i] == in) {
        modifierKey = modifier[i];
        return outChar[i];
      }
    }
    else {
      return in;
    }
  }
}

void printChar(byte in) {
  if (modifierKey) {
    Keyboard.press(modifierKey);
    delay(5);
    Keyboard.write(in);
    delay(5);
    Keyboard.release(modifierKey);
    modifierKey = 0;
  }
  else {
    Keyboard.write(in);
    delay(5);
  }
}

void pressChar(byte in) {
  if (modifierKey) {
    Keyboard.press(modifierKey);
    delay(5);
    Keyboard.press(in);
    delay(5);
    Keyboard.release(modifierKey);
    modifierKey = 0;
  }
  else {
    Keyboard.press(in);
    delay(5);
  }
}


void cmdGui (char argChar_l) {
  Keyboard.press(KEY_LEFT_GUI);
  delay(100);
  if (argChar != 0x00) {
    //Keyboard.press(argChar_l);
    pressChar(convertLangChar(argChar_l));
    delay(100);
  }
  Keyboard.releaseAll();
  prevCmd = cmd;
  cmd = "";
  prevArgChar = argChar_l;
  argChar = 0x00;
}

void cmdRem () {
  cmd = "";
  arg = "";
}

void cmdDefaultDelay (String arg_l) {
  defaultDelay = arg_l.toInt();
  cmd = "";
  arg = "";
}

void cmdDelay (String arg_l) {
  delay(arg_l.toInt());
  prevCmd = cmd;
  cmd = "";
  prevArg = arg_l;
  arg = "";
}

void cmdPressKey(int key) {
  //Keyboard.press(key);
  pressChar(convertLangChar(key));
  delay(100);
  Keyboard.release(key);
  prevCmd = cmd;
  cmd = "";
  prevArg = arg;
  arg = "";
}

void cmdString (File dataFile) {
  while (true) {
    charBuff = dataFile.read();
    if (charBuff == '\n') {
      //Keyboard.print(charBuff); //adds \n at the end of the line
      break;
    }
    else {
      //Keyboard.print(charBuff);
      printChar(convertLangChar(charBuff));
    }
  }
  cmd = "";
  arg = "";
}

void parseCmd(File dataFile) {
  while (true) {
    charBuff = dataFile.read();
    if (charBuff == ' ' || charBuff == '\n' || charBuff == '\r'|| cmd.length() > 15) {
      breakChar = charBuff;
      break;
    }
    else {
      cmd = cmd + charBuff;
    }
  }
}

void parseArg(File dataFile) {
  while (true) {
    charBuff = dataFile.read();
    if (charBuff == '\n') {
      break;
    }
    else {
      arg = arg + charBuff;
    }
  }
}

void cmdKeyCombo(int key, String arg_l) {
  String argKey;
  int argLength = arg_l.length();

  Keyboard.press(key);
  delay(100);

  for (int i = 0; i <= argLength; i++) {
    charBuff = arg_l.charAt(i);
    if (charBuff == ' ' || charBuff == '\n') {
      if (argKey.length() == 1) {
        //Keyboard.press(argKey.charAt(0));
        pressChar(convertLangChar(argKey.charAt(0)));
        delay(100);
      }
      else {
        if (argKey == "ENTER") {
          Keyboard.press(KEY_RETURN);
        }
        else if (argKey == "MENU" || argKey == "APP") {
          Keyboard.press(KEY_MENU);
        }
        else if (argKey == "DOWNARROW" || argKey == "DOWN") {
          Keyboard.press(KEY_DOWN_ARROW);
        }
        else if (argKey == "LEFTARROW" || argKey == "LEFT") {
          Keyboard.press(KEY_LEFT_ARROW);
        }
        else if (argKey == "RIGHTARROW" || argKey == "RIGHT") {
          Keyboard.press(KEY_RIGHT_ARROW);
        }
        else if (argKey == "UPARROW" || argKey == "UP") {
          Keyboard.press(KEY_UP_ARROW);
        }
        else if (argKey == "BREAK" || argKey == "PAUSE") {
          Keyboard.press(KEY_BREAK);
        }
        else if (argKey == "CAPSLOCK") {
          Keyboard.press(KEY_CAPS_LOCK);
        }
        else if (argKey == "DELETE") {
          Keyboard.press(KEY_DELETE);
        }
        else if (argKey == "END") {
          Keyboard.press(KEY_END);
        }
        else if (argKey == "ESC" || argKey == "ESCAPE") {
          Keyboard.press(KEY_ESC);
        }
        else if (argKey == "HOME") {
          Keyboard.press(KEY_HOME);
        }
        else if (argKey == "INSERT") {
          Keyboard.press(KEY_INSERT);
        }
        else if (argKey == "NUMLOCK") {
          Keyboard.press(KEY_NUMLOCK);
        }
        else if (argKey == "PAGEUP") {
          Keyboard.press(KEY_PAGE_UP);
        }
        else if (argKey == "PAGEDOWN") {
          Keyboard.press(KEY_PAGE_DOWN);
        }
        else if (argKey == "PRINTSCREEN") {
          Keyboard.press(KEY_PRINTSCREEN);
        }
        else if (argKey == "SCROLLLOCK") {
          Keyboard.press(KEY_SCROLLLOCK);
        }
        else if (argKey == "SPACE") {
          Keyboard.press(KEY_SPACE);
        }
        else if (argKey == "TAB") {
          Keyboard.press(KEY_TAB);
        }
        else if (argKey == "ALT") {
          Keyboard.press(KEY_LEFT_ALT);
        }
        else if (argKey == "SHIFT") {
          Keyboard.press(KEY_LEFT_SHIFT);
        }
        else if (argKey == "CTRL") {
          Keyboard.press(KEY_LEFT_CTRL);
        }
        else if (argKey == "F1") {
          Keyboard.press(KEY_F1);
        }
        else if (argKey == "F2") {
          Keyboard.press(KEY_F2);
        }
        else if (argKey == "F3") {
          Keyboard.press(KEY_F3);
        }
        else if (argKey == "F4") {
          Keyboard.press(KEY_F4);
        }
        else if (argKey == "F5") {
          Keyboard.press(KEY_F5);
        }
        else if (argKey == "F6") {
          Keyboard.press(KEY_F6);
        }
        else if (argKey == "F7") {
          Keyboard.press(KEY_F7);
        }
        else if (argKey == "F8") {
          Keyboard.press(KEY_F8);
        }
        else if (argKey == "F9") {
          Keyboard.press(KEY_F9);
        }
        else if (argKey == "F10") {
          Keyboard.press(KEY_F10);
        }
        else if (argKey == "F11") {
          Keyboard.press(KEY_F11);
        }
        else if (argKey == "F12") {
          Keyboard.press(KEY_F12);
        }
        else if (argKey == "GUI" || argKey == "WINDOWS") {
          Keyboard.press(KEY_LEFT_GUI);
        }
        delay(100);
      }
      argKey = "";
    }
    else {
      argKey = argKey + charBuff;
    }
  }
  Keyboard.releaseAll();
  prevCmd = cmd;
  cmd = "";
  prevArg = arg;
  arg = "";
}

void cmdRepeat (String arg_l) {
  int counter = arg_l.toInt();
  argChar = prevArgChar;

  while (counter) {
    if (defaultDelay != 0) {
      delay(defaultDelay);
    }

    cmd = prevCmd;
    arg = prevArg;

    counter -= 1;

    if (prevCmd == "GUI" || prevCmd == "WINDOWS") {
      cmdGui(prevArgChar);
    }
    else if (prevCmd == "DELAY") {
      cmdDelay(prevArg);
    }
    else if (prevCmd == "ENTER") {
      cmdPressKey(KEY_RETURN);
    }
    else if (prevCmd == "MENU" || prevCmd == "APP") {
      cmdPressKey(KEY_MENU);
    }
    else if (prevCmd == "DOWNARROW" || prevCmd == "DOWN") {
      cmdPressKey(KEY_DOWN_ARROW);
    }
    else if (prevCmd == "LEFTARROW" || prevCmd == "LEFT") {
      cmdPressKey(KEY_LEFT_ARROW);
    }
    else if (prevCmd == "RIGHTARROW" || prevCmd == "RIGHT") {
      cmdPressKey(KEY_RIGHT_ARROW);
    }
    else if (prevCmd == "UPARROW" || prevCmd == "UP") {
      cmdPressKey(KEY_UP_ARROW);
    }
    else if (prevCmd == "BREAK" || prevCmd == "PAUSE") {
      cmdPressKey(KEY_BREAK);
    }
    else if (prevCmd == "CAPSLOCK") {
      cmdPressKey(KEY_CAPS_LOCK);
    }
    else if (prevCmd == "DELETE") {
      cmdPressKey(KEY_DELETE);
    }
    else if (prevCmd == "END") {
      cmdPressKey(KEY_END);
    }
    else if (prevCmd == "ESC" || prevCmd == "ESCAPE") {
      cmdPressKey(KEY_ESC);
    }
    else if (prevCmd == "HOME") {
      cmdPressKey(KEY_HOME);
    }
    else if (prevCmd == "INSERT") {
      cmdPressKey(KEY_INSERT);
    }
    else if (prevCmd == "NUMLOCK") {
      cmdPressKey(KEY_NUMLOCK);
    }
    else if (prevCmd == "PAGEUP") {
      cmdPressKey(KEY_PAGE_UP);
    }
    else if (prevCmd == "PAGEDOWN") {
      cmdPressKey(KEY_PAGE_DOWN);
    }
    else if (prevCmd == "PRINTSCREEN") {
      cmdPressKey(KEY_PRINTSCREEN);
    }
    else if (prevCmd == "SCROLLLOCK") {
      cmdPressKey(KEY_SCROLLLOCK);
    }
    else if (prevCmd == "SPACE") {
      cmdPressKey(KEY_SPACE);
    }
    else if (prevCmd == "TAB") {
      cmdPressKey(KEY_TAB);
    }
    else if (cmd == "CTRL" || cmd == "CONTROL" ) {
      cmdKeyCombo(KEY_LEFT_CTRL, prevArg);
    }
    else if (cmd == "ALT") {
      cmdKeyCombo(KEY_LEFT_ALT, prevArg);
    }
    else if (cmd == "SHIFT") {
      cmdKeyCombo(KEY_LEFT_SHIFT, prevArg);
    }
    else if (cmd == "F1") {
      cmdPressKey(KEY_F1);
    }
    else if (cmd == "F2") {
      cmdPressKey(KEY_F2);
    }
    else if (cmd == "F3") {
      cmdPressKey(KEY_F3);
    }
    else if (cmd == "F4") {
      cmdPressKey(KEY_F4);
    }
    else if (cmd == "F5") {
      cmdPressKey(KEY_F5);
    }
    else if (cmd == "F6") {
      cmdPressKey(KEY_F6);
    }
    else if (cmd == "F7") {
      cmdPressKey(KEY_F7);
    }
    else if (cmd == "F8") {
      cmdPressKey(KEY_F8);
    }
    else if (cmd == "F9") {
      cmdPressKey(KEY_F9);
    }
    else if (cmd == "F10") {
      cmdPressKey(KEY_F10);
    }
    else if (cmd == "F11") {
      cmdPressKey(KEY_F11);
    }
    else if (cmd == "F12") {
      cmdPressKey(KEY_F12);
    }
  }
  cmd = "";
  arg = "";
}


int delivery (String fileName) {
  File dataFile = SD.open(fileName);

  if (dataFile) {
    while (dataFile.available()) {
      if (defaultDelay != 0) {
        delay(defaultDelay);
      }

      parseCmd(dataFile);

//      Serial.println(cmd);
      if (cmd == "GUI" || cmd == "WINDOWS") {
        if (breakChar == ' ') {
          argChar = dataFile.read();
          cmdGui(argChar);
          //just to remove trailing \n
          dataFile.read();
        }
        else {
          cmdGui(0x00);
        }

      }
      else if (cmd == "DELAY") {
        parseArg(dataFile);
        cmdDelay(arg);
      }
      else if (cmd == "STRING") {
        cmdString(dataFile);
      }
      else if (cmd == "ENTER") {

        cmdPressKey(KEY_RETURN);
      }
      else if (cmd == "REM") {
        if (breakChar == ' ') {
          parseArg(dataFile);
        }
        cmdRem();
      }
      else if (cmd == "DEFAULT_DELAY" || cmd == "DEFAULTDELAY") {
        parseArg(dataFile);
        cmdDefaultDelay(arg);
      }
      else if (cmd == "MENU" || cmd == "APP") {
        cmdPressKey(KEY_MENU);
      }
      else if (cmd == "DOWNARROW" || cmd == "DOWN") {
        cmdPressKey(KEY_DOWN_ARROW);
      }
      else if (cmd == "LEFTARROW" || cmd == "LEFT") {
        Serial.println("HERERRRR");
        cmdPressKey(KEY_LEFT_ARROW);
      }
      else if (cmd == "RIGHTARROW" || cmd == "RIGHT") {
        cmdPressKey(KEY_RIGHT_ARROW);
      }
      else if (cmd == "UPARROW" || cmd == "UP") {
        cmdPressKey(KEY_UP_ARROW);
      }
      else if (cmd == "BREAK" || cmd == "PAUSE") {
        cmdPressKey(KEY_BREAK);
      }
      else if (cmd == "CAPSLOCK") {
        cmdPressKey(KEY_CAPS_LOCK);
      }
      else if (cmd == "DELETE") {
        cmdPressKey(KEY_DELETE);
      }
      else if (cmd == "END") {
        cmdPressKey(KEY_END);
      }
      else if (cmd == "ESC" || cmd == "ESCAPE") {
        cmdPressKey(KEY_ESC);
      }
      else if (cmd == "HOME") {
        cmdPressKey(KEY_HOME);
      }
      else if (cmd == "INSERT") {
        cmdPressKey(KEY_INSERT);
      }
      else if (cmd == "NUMLOCK") {
        cmdPressKey(KEY_NUMLOCK);
      }
      else if (cmd == "PAGEUP") {
        cmdPressKey(KEY_PAGE_UP);
      }
      else if (cmd == "PAGEDOWN") {
        cmdPressKey(KEY_PAGE_DOWN);
      }
      else if (cmd == "PRINTSCREEN") {
        cmdPressKey(KEY_PRINTSCREEN);
      }
      else if (cmd == "SCROLLLOCK") {
        cmdPressKey(KEY_SCROLLLOCK);
      }
      else if (cmd == "SPACE") {
        cmdPressKey(KEY_SPACE);
      }
      else if (cmd == "TAB") {
        cmdPressKey(KEY_TAB);
      }
      else if (cmd == "REPEAT") {
        parseArg(dataFile);
        cmdRepeat(arg);
      }
      else if (cmd == "CTRL" || cmd == "CONTROL" ) {
        parseArg(dataFile);
        arg = arg + '\n';
        cmdKeyCombo(KEY_LEFT_CTRL, arg);
      }
      else if (cmd == "ALT") {
        parseArg(dataFile);
        arg = arg + '\n';
        cmdKeyCombo(KEY_LEFT_ALT, arg);
      }
      else if (cmd == "SHIFT") {
        parseArg(dataFile);
        arg = arg + '\n';
        cmdKeyCombo(KEY_LEFT_SHIFT, arg);
      }
      else if (cmd == "F1") {
        cmdPressKey(KEY_F1);
      }
      else if (cmd == "F2") {
        cmdPressKey(KEY_F2);
      }
      else if (cmd == "F3") {
        cmdPressKey(KEY_F3);
      }
      else if (cmd == "F4") {
        cmdPressKey(KEY_F4);
      }
      else if (cmd == "F5") {
        cmdPressKey(KEY_F5);
      }
      else if (cmd == "F6") {
        cmdPressKey(KEY_F6);
      }
      else if (cmd == "F7") {
        cmdPressKey(KEY_F7);
      }
      else if (cmd == "F8") {
        cmdPressKey(KEY_F8);
      }
      else if (cmd == "F9") {
        cmdPressKey(KEY_F9);
      }
      else if (cmd == "F10") {
        cmdPressKey(KEY_F10);
      }
      else if (cmd == "F11") {
        cmdPressKey(KEY_F11);
      }
      else if (cmd == "F12") {
        cmdPressKey(KEY_F12);
      }
      else {
        errLog = true;
        cmd = "";
        arg = "";
        continue;
      }
    }
    dataFile.close();
    return 0;
  }
  else {
    Serial.println("Error opening script file");
    return 1;
  }
}





void loop() {
    
if (IrReceiver.decode()) {
  int currentCommand = IrReceiver.decodedIRData.command;


   if (currentCommand==prevCommand){
    if (timeElapsed> waitDuration){
      timeElapsed=0;
      executeAction(currentCommand);
    }
   }



   if (currentCommand!=prevCommand){
     timeElapsed=0;
     prevCommand=currentCommand;
     executeAction(currentCommand); 
    }

  
  IrReceiver.resume(); // Receive the next value
}
}


void ledFailRoutine(){
  int i=0;
  for(i=0;i<3;i++){
  digitalWrite(13,HIGH);
  delay(200);
  digitalWrite(13,LOW);
  delay(100);
  }

}


void ledOKRoutine(){
  digitalWrite(13,HIGH);
  delay(200);
  digitalWrite(13,LOW);
  delay(100);

}



void executeAction(int code){
  switch (code){
    case 70: //2
      if (delivery("2.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    case 71: //3
      if (delivery("3.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    case 68: //4
      if (delivery("4.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    case 64: //5
      if (delivery("5.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    case 67: //6
      if (delivery("6.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    case 7: //7
      if (delivery("7.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    case 21: //8
      if (delivery("8.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    case 24: //up
      if (delivery("up.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    case 82: //down
      if (delivery("down.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    case 90: //right
      if (delivery("right.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    case 8: //left
      if (delivery("left.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    case 28: //OK
      if (delivery("ok.txt")==0)
        ledOKRoutine();
      else ledFailRoutine();
      break;
    default:
      ledFailRoutine();

  }
  
}
