#include <LiquidCrystal.h>
LiquidCrystal LcdDriver(11,9,5,6,7,8);
unsigned long timer; //keeps track of game time
int asteroidsPassed; //keeps track of asteroids passed
int curPos; //keep track of the current position in each game
boolean board[4][20]; //the game board
int snake[2]; //location of the snake
int fruitPos[2]; //location of the fruit for the snake
int snakeState; //current state of the snake (win, lose, continue)
int snakeCount; //keeps track of how many fruits the snake as eaten
int lives; //the current lives for the meteor game
int updateSpeed; //the current update speed;
enum ButtonState { Idle, Wait, Low }; //enum for the currrent button state
enum GameState { GameA, GameB }; //enum for the current game state

long ButtonTime; //keeps track of how long the button was pressed.

ButtonState BS; //keeps track of button state
GameState Game; //keeps track of game state
void setup() //sets up all needed variables
{
  curPos = 0; //sets curpos to 0
  pinMode(2, INPUT); //sets pin 2 as input
  pinMode(3, INPUT); //sets pin 3 as input
  pinMode(4, INPUT); //sets pin 4 as input
  
  attachInterrupt(0, MonitorA, CHANGE); //attaches an interrupt to pin 2
  attachInterrupt(1, MonitorB, CHANGE); //attaches an interrupt to pin 3
  
  LcdDriver.begin(20,4); //set board size
  LcdDriver.clear(); //clears the board
  LcdDriver.setCursor(0,0);//sets cursor to top left corner
  
  BS = Idle; //sets button state to idle
  Game = GameA; //sets game state to GameA
  snakeState = 0; //sets snake state to 0
  snakeCount = 0; //sets snake state to 0
  lives = 3; //sets lives to three
  asteroidsPassed = 0; //sets asteriods passed to 0
  updateSpeed = 500; //sets update speed to 500

  timer = millis();//sets timer to current time in millis
  ButtonTime = millis(); //sets button to current time in millis

} //end setup

void MonitorA() // handles interrupts
{
  if(digitalRead(2) == digitalRead(3)) //if states are equal
    curPos+=1; //add one to cur pos
  else//states not equal
    curPos-=1; //remove one from cur pos
}//end MonitorB

void MonitorB()//handles interrupts
{
  if(digitalRead(2) == digitalRead(3))//if states are equal
    curPos -= 1;//remove one from cur pos
  else//states not equal
    curPos += 1;//add one to curpos
}//end MonitorB

void updateGameA()// handles GameA updates
{  
  int pos = abs(curPos/4) % 4;//bounds the pos to a number between 0 and 3
  boolean temp[4][20];//makes a temporary array to shift
  for(int r = 0; r < 4; r++)//runs from 0 -> 3
  {
    for(int k = 20; k > 0; k--)//runs from 0 -> 19
    {
      temp[r][k] = board[r][(k - 1) % 20];//sets the current position with the one infront of it.
    }//end for
  }//end for
  for(int k = 0; k < 4; k++)//runs from 0 -> 3
  {
    temp[k][0] = false;//sets position to false
    if(random(0,100) < 20)//randomly adds a position about 20% of the time
      temp[k][0] = true;//set the position to true
  }//end for

  for(int r = 0; r < 4; r++)// runs from 0 -> 3
  {
    for(int k = 0; k < 20; k++) //runs from 0 -> 19
    {
        board[r][k] = temp[r][k];//copys the state of temp to board
    }//end for
  }//end for
  asteroidsPassed += 1; //adds 1 to asteroids passed
  if(board[pos][19])//if the curPOs and theres an asteroid are in the same position
    lives -= 1;//remove one from lives.
}//end updateGameA

void updateGameB()//handles GameB updates
{

  int pos = abs(curPos/4) % 4;//bounds the pos to a number between 0 and 3
  
  if(fruitPos[0] == snake[0] && fruitPos[1] == snake[1]) //if fruitpos is the same as snakepos
  {
    fruitPos[0] = random(0,4);// randomly places on y axis
    fruitPos[1] = random(0,20);//randomly places on x acis
    snakeCount+=1;//update snake count 1
    updateSpeed -= 50;//increments updatespeed
  }
  if(pos == 0)//if facing up
  {
    snake[0] = snake[0] - 1;//decrement current y pos
  }//end if
  else if(pos == 1)//if facing right
  {
    snake[1] = snake[1] - 1;//decrement x pos
  }//end if
  else if(pos == 2)//if facing down
  {
    snake[0] = snake[0] + 1;//increment y pos
  }//end if
  else if(pos == 3)//if facingleft
  {
    snake[1] = snake[1] + 1;//increment x pos
  }//end if
  if(snake[0] == -1 || snake[0] == 5) //if out of y bounds
  {
    snakeState = 2; //set state to lose
    return;//exit method
  }//end if
  else if(snake[1] == -1 || snake[1] == 20)//if out of x bounds
  {
    snakeState = 2;//set state to lose
    return;//exit method
  }//end if
  if(snakeCount == 10)//if collected 10 fruits
  {
    snakeState = 1;//set state to win
    return;//exit method
  }//end if
}// end updateGameB

void drawToLCD()//Draws to the lcd
{
  LcdDriver.clear();//clear the LCD
  
  if(Game == GameA)//if the current game is GameA
  {
    int tempPos = (abs(curPos/4) % 4);//bound position from 0 -> 3
    LcdDriver.setCursor(19,tempPos);//set to position of characted
    LcdDriver.print("<");//print 
    LcdDriver.setCursor(0,0);//set cursor to top left
    LcdDriver.print((char)(lives + 48));//prints the lives

    for(int r = 0; r < 4; r++)//increment from 0->3
    {
      for(int k = 0; k < 20; k++)//increment from 0->19
      {
          LcdDriver.setCursor(k,r);//sets to current position in board.
          if(board[r][k])//if the board at this point is true
            LcdDriver.print("*");//draw an asteroid
      }//end for
    }//end for
  }//end if
  else//Game == GameB
  {    
    LcdDriver.setCursor(fruitPos[1],fruitPos[0]);//set position to where the fruit is
    LcdDriver.print("*");//print at fruit point
    
    int tempPos = (abs(curPos/4) % 4);//bounds pos between 0 and 3
    LcdDriver.setCursor(0,0);// sets cursor to top left
    LcdDriver.print(snakeCount);//print snake count
    LcdDriver.setCursor(snake[1],snake[0]);//sets to position of snake
    if(tempPos == 0)//if facing up
      LcdDriver.print("^");//print ^
    else if(tempPos == 1)//if facing right
      LcdDriver.print("<");//print <
    else if(tempPos == 2)//if facing down
      LcdDriver.print("v");//print v
    else//facing left
      LcdDriver.print(">");//print >
    //end if
  }//end if
}//end draw to lcd

void ResetGame()//resets everything.
{
  lives = 3;//resets 
  for(int r = 0; r < 4; r++)
  {
    for(int k = 0; k < 20; k++)
    {
        board[r][k] = false;//resets board pos to false
    }
  }
  asteroidsPassed = 0;//resets asteroids to 0
  curPos = 0;//resets curpos to 0
  snake[0] = 2;//resets to middle
  snake[1] = 10;//resets to middle
        
  snakeState = 0;//resets to 0
  snakeCount = 0; //resets to 0 
  fruitPos[0] = random(0,4);//resets to randomPos
  fruitPos[1] = random(0,20);//resets to randomPos
  if(Game == GameA)//is GameA
  {
    updateSpeed = 500;//resets to 500
  }
  else //is GameB
  {
    updateSpeed = 1000;//resets to 1000
  }//end if

  timer = millis();//resets to current time

}//End Reset Game

void ChangeGame()//set needed stuff to changegame
{
  ResetGame();//calls reset game
  if(Game == GameA)//If on GameA
  {
    Game = GameB;//Change to GameB
    updateSpeed = 1000;//sets update interval to 1000
  }
  else //is GameB
  {
    Game = GameA;//Change to GameA
    updateSpeed = 500;//sets update interval to 500
  }//end if  
}//end ChangeGame

void loop() //main method constantly loops
{
  int state = ButtonNextState(digitalRead(4));//gets the current button state
  if(state == 2)//if short press
    ResetGame();//call reset game
  else if(state == 3)//if long press
    ChangeGame();//call change game
  if(GameA == Game)//if GameA 
  {
    if(lives > 0)//if lives are greater then 0
    {
      if(millis() - timer >= updateSpeed)//if updateSpeed time has passed
      {
        updateGameA();//call updateGameA method
        drawToLCD();//call drawToLcd
        timer += updateSpeed;//update timer
        if(asteroidsPassed % 19 == 0 && timer - 50 > 250)//if 19 asteroids have updated
        {
          asteroidsPassed = 0;//reset them to 0
          updateSpeed -= 50;//increment update speed
        }//end if
      }//end if
    }//end if
    else if(lives == 0)//if lives == 0
    {
      //drawToLCD();
      LcdDriver.setCursor(19,(abs(curPos/4) % 4));//sets to current pos of person
      LcdDriver.print("X");//draws an x
      lives = -1;//sets lives to -1 as not to update constantly
    }//end if
  }//end if
  else //is GameB
  {
    if(snakeState == 0)// If haven't lost yet
    {
      if(millis() - timer >= updateSpeed)//if updateSpeed time has passed
      {
        updateGameB();//call updateGameB method
        drawToLCD();// call drawToLcd
        timer += updateSpeed;//update timer by updateSpeed
      }//end if
    }//end if
    else if(snakeState == 1)//if state == 1
    {
      LcdDriver.setCursor(0,0);//sets cursor to top left
      LcdDriver.print("You won!");//print you won
    }//end if
    else if(snakeState == 2)//if state == 2
    {
      LcdDriver.setCursor(0,0);//sets cursor to top left
      LcdDriver.print("You lost!");//print you lost
    }//end if
  }//end if
}//end loop


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
