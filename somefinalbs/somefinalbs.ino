#include <LiquidCrystal.h>
LiquidCrystal LcdDriver(11,9,5,6,7,8);
unsigned long timer;
unsigned long timer2;
int curPos;
boolean board[4][20]; 
int lives = 3;
enum ButtonState { Idle, Wait, Low };
enum GameState { GameA, GameB };

long ButtonTime;

ButtonState BS;
GameState Game;
void setup() 
{
  curPos = 0;
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  
  attachInterrupt(0, MonitorA, CHANGE);
  attachInterrupt(1, MonitorB, CHANGE);
  
  
  // put your setup code here, to run once:
  LcdDriver.begin(20,4);
  LcdDriver.clear();
  LcdDriver.setCursor(0,0);
  
  

  BS = Idle;
  Game = GameA;

  timer = millis();
  timer2 = millis();
  ButtonTime = millis();

}

void MonitorA()
{
  if(digitalRead(2) == digitalRead(3))
    curPos+=1;
  else
    curPos-=1;
  
}

void MonitorB()
{
  if(digitalRead(2) == digitalRead(3))
    curPos -= 1;
  else
    curPos += 1;
}

void updateGame()
{  
  int pos = abs(curPos/4) % 4;
  boolean temp[4][20];
  for(int r = 0; r < 4; r++)
  {
    for(int k = 20; k > 0; k--)
    {
      temp[r][k] = board[r][(k - 1) % 20];
    }
  }
  for(int k = 0; k < 4; k++)
  {
    temp[k][0] = false;
    if(random(0,100) < 20)
      temp[k][0] = true;
  }

  for(int r = 0; r < 4; r++)
  {
    for(int k = 0; k < 20; k++)
    {
        board[r][k] = temp[r][k];
    }
  }

  if(board[pos][19])
    lives -= 1;
}


void drawToLCD()
{
  LcdDriver.clear();

  for(int r = 0; r < 4; r++)
  {
    for(int k = 0; k < 20; k++)
    {
        LcdDriver.setCursor(k,r);
        if(board[r][k])
          LcdDriver.print("*");
    }
  }
    
  int tempPos = (abs(curPos/4) % 4);
  LcdDriver.setCursor(19,tempPos);
  LcdDriver.print("<");
  LcdDriver.setCursor(0,0);
    LcdDriver.print((char)(lives + 48));
  
}

void ResetGame()
{
  lives = 3;
  for(int r = 0; r < 4; r++)
  {
    for(int k = 0; k < 20; k++)
    {
        board[r][k] = false;
    }
  }
  
  timer = millis();

}

void ChangeGame()
{
  if(Game == GameA)
    Game = GameB;
  else 
    Game = GameA;
  ResetGame();
}

void loop() 
{
  int state = ButtonNextState(digitalRead(4));
  if(state == 2)
    ResetGame();
  else if(state == 3)
    ChangeGame();
  
  if(lives > 0)
  {
    if(millis() - timer >= 500)
    {
      updateGame();
      drawToLCD();      
      timer += 500;
    }

  }
  else if(lives == 0)
  {
    //drawToLCD();
    LcdDriver.setCursor(19,(abs(curPos/4) % 4));
    LcdDriver.print("X");
    lives = -1;
  }
  
  // put your main code here, to run repeatedly:
  
}


int ButtonNextState(int state)//Checks the state of the button
{
    switch(BS)//Checks the button state
    {
        case Idle:
        {//if it is idle
            if(state == LOW)
            {//if the state was low reset stuff
                ButtonTime = millis();//reset the button timer
                BS = Wait;//
                PORTB |= 0x2000;//set pin to high
            }//end if
            break;
        }//end idle case
        case Wait:
        {//if the button state is idle
            if(state == HIGH)//if the state is high set b to idle
                BS = Idle;
            else if(millis() - ButtonTime >= 5)
            {//the button was pressed and the state was low
                BS = Low;//set the state to low
                PORTB &= 0xDFFF;//set the led to high
                return 1;//return 1
            }break;//end if
        }//end wait case
        case Low:
        {//if it is low
            if(state == HIGH)//if the button state is high
                BS = Idle;//if the state is idle
      else if(millis() - ButtonTime < 500)
        return 2;
      else
        return 3;
        }//end low case
    }//end switch
    return 0;//return 0 if not pressed
}//end ButtonNextState
