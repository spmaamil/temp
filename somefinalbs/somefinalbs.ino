#include <LiquidCrystal.h>
LiquidCrystal LcdDriver(11,9,5,6,7,8);
unsigned long timer;
unsigned long timer2;
int curPos;
boolean board[4][20]; 
int lives = 3;
void setup() 
{
  curPos = 0;
  pinMode(2, INPUT);
  pinMode(3, INPUT);

  attachInterrupt(0, MonitorA, CHANGE);
  attachInterrupt(1, MonitorB, CHANGE);
  
  
  // put your setup code here, to run once:
  LcdDriver.begin(20,4);
  LcdDriver.clear();
  LcdDriver.setCursor(0,0);
  
  timer = millis();
  timer2 = millis();

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
    temp[k][0] = false;
    
  //shift[0] = new bool[20];
  /* TODO: FIX THIS */
  if(abs(random())%2 == 0) 
    temp[abs(random())%4][0] = true;

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
  ///LcdDriver.setCursor(0,0);
  ///LcdDriver.print(lives);
  
}
void loop() {
  if(lives != 0)
  {
    if(millis() - timer >= 200)
    {
      updateGame();
      drawToLCD();      
      timer += 200;
    }

  }
  else
  {
    //drawToLCD();
    LcdDriver.setCursor(19,(abs(curPos/4) % 4));
    LcdDriver.print("X");
  }
  
  // put your main code here, to run repeatedly:
  
}
