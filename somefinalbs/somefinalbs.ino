#include <LiquidCrystal.h>
LiquidCrystal LcdDriver(11,9,5,6,7,8);
unsigned long timer;
int curPos;
boolean board[4][20]; 
int snake[2];
int fruitPos[2];
int snakeState;
int snakeCount = 0;
int lives = 3;
int updateSpeed = 500;
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
  snakeState = 0;

  timer = millis();
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

void updateGameA()
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

void updateGameB()
{

  int pos = abs(curPos/4) % 4;
  
  if(fruitPos[0] == snake[0] && fruitPos[1] == snake[1])
  {
    fruitPos[0] = random(0,4);
    fruitPos[1] = random(0,20);
    snakeCount+=1;
    updateSpeed -= 50;
  }
  if(pos == 0)
  {
    snake[0] = snake[0] - 1;
    snake[1] = snake[1];
  }
  else if(pos == 1)
  {
    snake[0] = snake[0];
    snake[1] = snake[1] - 1;
  }
  else if(pos == 2)
  {
    snake[0] = snake[0] + 1;
    snake[1] = snake[1];
  }
  else if(pos == 3)
  {
    snake[0] = snake[0];
    snake[1] = snake[1] + 1;
  }
  if(snake[0] == -1 || snake[0] == 5)
  {
    snakeState = 2;
    return;
  }
  else if(snake[1] == -1 || snake[1] == 20)
  {
    snakeState = 2;
    return;
  }
  if(snakeCount == 10)
  {
    snakeState = 1;
    return;
  }
}

void drawToLCD()
{
  LcdDriver.clear();

  
  if(Game == GameA)
  {
    int tempPos = (abs(curPos/4) % 4);
    LcdDriver.setCursor(19,tempPos);
    LcdDriver.print("<");
    LcdDriver.setCursor(0,0);
    LcdDriver.print((char)(lives + 48));

    for(int r = 0; r < 4; r++)
    {
      for(int k = 0; k < 20; k++)
      {
          LcdDriver.setCursor(k,r);
          if(board[r][k])
            LcdDriver.print("*");
      }
    }
  }
  else
  {    
    int tempPos = (abs(curPos/4) % 4);
    LcdDriver.setCursor(0,0);
    LcdDriver.print(snakeCount);
    LcdDriver.setCursor(snake[1],snake[0]);
    if(tempPos == 0)
      LcdDriver.print("^");
    else if(tempPos == 1)
      LcdDriver.print("<");
    else if(tempPos == 2)
      LcdDriver.print("v");
    else
      LcdDriver.print(">");
    LcdDriver.setCursor(fruitPos[1],fruitPos[0]);
    LcdDriver.print("X");
  }
  
  
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

  curPos = 0;
  snake[0] = 2;
  snake[1] = 10;
        
  snakeState = 0;
  snakeCount = 0; 
  fruitPos[0] = random(0,4);
  fruitPos[1] = random(0,20);
  if(Game == GameA)
  {
    updateSpeed = 500;
  }
  else 
  {
    updateSpeed = 1000;
  }
}

void ChangeGame()
{
  ResetGame();
  if(Game == GameA)
  {
    Game = GameB;
    updateSpeed = 1000;
  }
  else 
  {
    Game = GameA;
    updateSpeed = 500;
  }
  
}

void loop() 
{
  int state = ButtonNextState(digitalRead(4));
  if(state == 2)
    ResetGame();
  else if(state == 3)
    ChangeGame();

  if(GameA == Game)
  {
    if(lives > 0)
    {
      if(millis() - timer >= updateSpeed)
      {
        updateGameA();
        drawToLCD();      
        timer += updateSpeed;
      }
  
    }
    else if(lives == 0)
    {
      //drawToLCD();
      LcdDriver.setCursor(19,(abs(curPos/4) % 4));
      LcdDriver.print("X");
      lives = -1;
    }
  }
  else 
  {
    if(snakeState == 0)
    {
      if(millis() - timer >= updateSpeed)
      {
        updateGameB();
        drawToLCD();      
        timer += updateSpeed;
      }
    }
    else if(snakeState == 1)
    {
      LcdDriver.setCursor(0,0);
      LcdDriver.print("You won!");
    }
    else if(snakeState == 2)
    {
      LcdDriver.setCursor(0,0);
      LcdDriver.print("You lost!");
    }
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
      else if(millis() - ButtonTime < 250)
        return 2;
      else
        return 3;
        }//end low case
    }//end switch
    return 0;//return 0 if not pressed
}//end ButtonNextState
