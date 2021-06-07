#include "iGraphics.h"
#include<cstdlib>
#include<Windows.h>
#include<MMSystem.h>

typedef struct _dropping{
    double x;
    double y;
    double speed;
    int activeStatus; //1 means egg exists on-screen, 0 means otherwise
    int activeUntill; //To determine until when a broken egg should be shown on ground; 0 means it is not on ground
    char filename[100]; //The sprite used for the dropping
    char floorFileName[100]; //The bmp shown when the dropping is on the ground
}dropping;

struct record{
    char name[101];
    int score;
}highScores[10],newScore;

enum MENU{
    mainMenu,game,help,gameOver,highScore,selectTime
};

int n = 20,len = 0,basketWidth = 130;
dropping egg[20];
dropping perk[3];
double g = 0.3;
int elapsedTime = 0, remainingTime = 1000*180, interval = 500, point = 0, pointFactor = 1, gameDuration = 20;
int murgiPos,basketPos = 360, old_mx = 0, old_my = 0;
char pointLine[101], timeLine[101], temporary[101], input_str[101];
char basket_filename[101] = "images\\basket.bmp";
enum MENU menuStatus = mainMenu;

void loadHighScore()
{
    int i;
    for(i=0;i<10;i++)
    {
        highScores[i].name[0] = 0;
        highScores[i].score = 0;
    }
    FILE *fp = fopen("high_scores.txt","r");
    for(i=0;i<10;i++)
        fscanf(fp,"%d %[^\n]s",&highScores[i].score,&highScores[i].name);
    fclose(fp);
}

void updateHighScore()
{
    //After a new high score is achieved, places it in the correct place and writes it to the file
    FILE *fp = fopen("high_scores.txt","w");
    int i,pos = -1;
    for(i=9;i>=0;i--)
    {
        if(highScores[i].score<newScore.score)
            pos = i;
    }
    if(pos > -1)
    {
        for(i=9;i>pos;i--)
        {
            strcpy(highScores[i].name,highScores[i-1].name);
            highScores[i].score = highScores[i-1].score;
        }
        strcpy(highScores[pos].name,newScore.name);
        highScores[pos].score = newScore.score;
    }
    for(i=0;i<10;i++)
        fprintf(fp,"%d %s\n",highScores[i].score,highScores[i].name);
    fclose(fp);
}

void updateTime()
{
    int minute = (remainingTime/1000)/60, second = (remainingTime/1000)%60;
    strcpy(timeLine,"Time: ");
    if(minute/10 == 0)
        strcat(timeLine,"0");
    strcat(timeLine,itoa(minute,temporary,10));
    strcat(timeLine,":");
    if(second/10 == 0)
        strcat(timeLine,"0");
    strcat(timeLine,itoa(second,temporary,10));
}

void freshStart()
{
    //Starts a new game with the duration given in input
    int i;
    elapsedTime = 0;
    remainingTime = gameDuration*1000;
    point = 0;
    strcpy(basket_filename,"images\\basket.bmp");
    basketWidth = 130;
    for(i = 0;i<=7;i++)
    {
        egg[i].y = 488;
        egg[i].activeStatus = 0;
        egg[i].activeUntill = 0;
        egg[i].speed = 0;
        strcpy(egg[i].filename,"images\\white_egg.bmp");
        strcpy(egg[i].floorFileName,"images\\fallen_egg.bmp");
    }
    for(i = 8;i<=13;i++)
    {
        egg[i].y = 488;
        egg[i].activeStatus = 0;
        egg[i].activeUntill = 0;
        egg[i].speed = 0;
        strcpy(egg[i].filename,"images\\shit.bmp");
        strcpy(egg[i].floorFileName,"images\\fallen_shit.bmp");
    }
    for(i = 14;i<=17;i++)
    {
        egg[i].y = 488;
        egg[i].activeStatus = 0;
        egg[i].activeUntill = 0;
        egg[i].speed = 0;
        strcpy(egg[i].filename,"images\\blue_egg.bmp");
        strcpy(egg[i].floorFileName,"images\\fallen_egg.bmp");
    }
    for(i = 18;i<=19;i++)
    {
        egg[i].y = 488;
        egg[i].activeStatus = 0;
        egg[i].activeUntill = 0;
        egg[i].speed = 0;
        strcpy(egg[i].filename,"images\\gold_egg.bmp");
        strcpy(egg[i].floorFileName,"images\\fallen_egg.bmp");
    }

    for(i=0;i<3;i++)
    {
        perk[i].y = 488;
        perk[i].activeStatus = 0;
        perk[i].activeUntill = 0;
        perk[i].speed = 0;
    }
    strcpy(perk[0].filename,"images\\time.bmp");
    strcpy(perk[1].filename,"images\\2x.bmp");
    strcpy(perk[2].filename,"images\\basket_perk.bmp");

    strcpy(pointLine,"Points: ");
    strcat(pointLine,itoa(point,temporary,10));
    updateTime();
}

void droppingCreation(int type,int start_x)
{
    //Spawns droppings at start_x
    //The variable type dictates what kind of egg/shit is being created
    int i;
    if(type<=40)
    {
        for(i = 0; i<=7; i++)
            if(egg[i].activeStatus==0)
            {
                egg[i].activeStatus=1;
                egg[i].x = 1.00*start_x;
                break;
            }
    }
    else if(type<=70)
    {
        for(i = 8; i<=13; i++)
            if(egg[i].activeStatus==0)
            {
                egg[i].activeStatus=1;
                egg[i].x = 1.00*start_x;
                break;
            }
    }
    else if(type<=90)
    {
        for(i = 14; i<=17; i++)
            if(egg[i].activeStatus==0)
            {
                egg[i].activeStatus=1;
                egg[i].x = 1.00*start_x;
                break;
            }
    }
    else if(type<=100)
    {
        for(i = 18; i<=19; i++)
            if(egg[i].activeStatus==0)
            {
                egg[i].activeStatus=1;
                egg[i].x = 1.00*start_x;
                break;
            }
    }

    else if(type<=103)
    {
        if(perk[0].activeStatus == 0)
        {
            perk[0].activeStatus = 1;
            perk[0].x = 1.00*start_x;
        }
    }
    else if(type<=106)
    {
        if(perk[1].activeStatus == 0)
        {
            perk[1].activeStatus = 1;
            perk[1].x = 1.00*start_x;
        }
    }
    else if(type<=109)
    {
        if(perk[2].activeStatus == 0)
        {
            perk[2].activeStatus = 1;
            perk[2].x = 1.00*start_x;
        }
    }
}

void saveGame()
{
    //Saves the position of eggs, remaining time and point to save.txt
    FILE *fp = fopen("save.txt","w");
    int i;
    fprintf(fp,"%d\n",1);
    fprintf(fp,"%d %d %d %d %d\n",elapsedTime,remainingTime,point,basketWidth,pointFactor);
    fprintf(fp,"%s\n",basket_filename);
    for(i=0;i<n;i++)
        fprintf(fp,"%lf %lf %lf %d %d\n",egg[i].x,egg[i].y,egg[i].speed,egg[i].activeStatus,egg[i].activeUntill);
    for(i=0;i<3;i++)
        fprintf(fp,"%lf %lf %lf %d %d\n",perk[i].x,perk[i].y,perk[i].speed,perk[i].activeStatus,perk[i].activeUntill);
    fclose(fp);
}

int loadGame()
{
    //If a save game exists, loads all necessary information into the game variables and returns 1
    //Else returns 0
    int i,status;
    FILE *fp = fopen("save.txt","r");
    fscanf(fp,"%d",&status);
    if(status == 0)
    {
        fclose(fp);
        return 0;
    }
    fscanf(fp,"%d %d %d %d %d",&elapsedTime,&remainingTime,&point,&basketWidth,&pointFactor);
    fscanf(fp,"%s",basket_filename);
    for(i=0;i<n;i++)
        fscanf(fp,"%lf %lf %lf %d %d",&egg[i].x,&egg[i].y,&egg[i].speed,&egg[i].activeStatus,&egg[i].activeUntill);
    for(i=0;i<3;i++)
        fscanf(fp,"%lf %lf %lf %d %d",&perk[i].x,&perk[i].y,&perk[i].speed,&perk[i].activeStatus,&perk[i].activeUntill);
    updateTime();
    strcpy(pointLine,"Points: ");
    strcat(pointLine,itoa(point,temporary,10));
    fclose(fp);
    return 1;
}

void iDraw()
{
    iClear();
    int i;
    char str[101];
    if(menuStatus == mainMenu)
        iShowBMP(0,0,"images\\main_menu.bmp");

    else if(menuStatus == game)
    {
        iShowBMP(0,0,"images\\background.bmp");
        for(i=0; i<20; i++)
        {
            if(egg[i].activeStatus == 1 && egg[i].activeUntill == 0)
                iShowBMP2((int)egg[i].x,(int)egg[i].y,egg[i].filename,0xFFFFFF);
            else if(egg[i].activeStatus == 1)
                iShowBMP2((int)egg[i].x,50,egg[i].floorFileName,0xFFFFFF);
        }
        for(i = 0; i<3; i++)
        {
            if(perk[i].activeStatus == 1 && perk[i].activeUntill == 0)
                iShowBMP2((int)perk[i].x,(int)perk[i].y,perk[i].filename,0xFFFFFF);
            else if(perk[i].activeStatus == 1)
                iShowBMP2(450+40*i,670,perk[i].filename,0xFFFFFF);
        }
        iShowBMP2(murgiPos,515,"images\\murgi.bmp",0xFFFFFF);
        iShowBMP2(basketPos,50,basket_filename,0xFFFFFF);
        iSetColor(0,0,0);
        iText(10,700,pointLine,GLUT_BITMAP_TIMES_ROMAN_24);
        iText(1000,700,timeLine,GLUT_BITMAP_TIMES_ROMAN_24);
    }

    else if(menuStatus == highScore)
    {
        int height = 525;
        iShowBMP(0,0,"images\\high_score.bmp");
        iSetColor(0,0,0);
        for(i = 0;i < 10;i++)
        {
            itoa(i+1,str,10);
            iText(83,height,str,GLUT_BITMAP_TIMES_ROMAN_24);
            itoa(highScores[i].score,str,10);
            iText(315,height,str,GLUT_BITMAP_TIMES_ROMAN_24);
            iText(610,height,highScores[i].name,GLUT_BITMAP_TIMES_ROMAN_24);
            height -= 40;
        }
    }

    else if(menuStatus == gameOver)
    {
        iShowBMP(0,0,"images\\game_over.bmp");
        iSetColor(0,0,0);
        iText(500,600,pointLine,GLUT_BITMAP_TIMES_ROMAN_24);
        strcpy(str,"Enter your name: ");
        strcat(str,input_str);
        iText(500,550,str,GLUT_BITMAP_TIMES_ROMAN_24);
    }

    else if(menuStatus == selectTime)
    {
        iShowBMP(0,0,"images\\select_time.bmp");
        iSetColor(0,0,0);
        strcpy(str,"Enter game duration in second: ");
        strcat(str,input_str);
        iText(300,360,str,GLUT_BITMAP_TIMES_ROMAN_24);
    }

    else if(menuStatus == help)
        iShowBMP(0,0,"images\\help.bmp");
}

void iMouseMove(int mx, int my)
{
    //Moves basket
    if(old_mx>=basketPos && old_mx<=basketPos+basketWidth && old_my>=50 && old_my<=50+130)
    {
        if(basketPos+mx-old_mx >=0 && basketPos+mx-old_mx <= 1150)
            basketPos+=mx-old_mx;
    }
    old_mx = mx;
    old_my = my;
}

/*
	function iMouse() is called when the user presses/releases the mouse.
	(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my)
{
    if(menuStatus == mainMenu && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if(my>=422 && my<=481 && mx>=434 && mx<=799)
        {
            //Clicking the new game button
            //Going from main menu from time selection
            input_str[0] = 0;
            len = 0;
            menuStatus = selectTime;
        }
        if(my>=172 && my<=231 && mx>=434 && mx<=799)
            exit(0);
        if(my>=509 && my<=564 && mx>=434 && mx<=799)
        {
            //Clicking the resume button
            //Loads the saved game if a saved game exists
            if(loadGame())
            {
                iResumeTimer(0);
                iResumeTimer(1);
                menuStatus = game;
            }
        }
        if(my>=340 && my<=400 && mx>=434 && mx<=799)
        {
            //Clicking the high score button
            loadHighScore();
            menuStatus = highScore;
        }
        if(my>=257 && my<=317 && mx>=434 && mx<=799)
        {
            //Clicking the help button
            menuStatus = help;
        }
    }

    else if(menuStatus == highScore && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if(mx>=400 && mx<= 860 && my>=25 && my<=102) //Clicking the back to menu button
            menuStatus = mainMenu;
    }

    else if(menuStatus == help && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if(mx>=413 && mx<=873 && my>=21 && my<=82)
            menuStatus = mainMenu;
    }
}

/*
	function iKeyboard() is called whenever the user hits a key in keyboard.
	key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key)
{
    if(menuStatus == game)
    {
        if(key == 'p' || key == 'P')
        {
            iPauseTimer(0);
            iPauseTimer(1);
        }
        if(key == 'r' || key == 'R')
        {
            iResumeTimer(0);
            iResumeTimer(1);
        }
    }

    if(menuStatus == gameOver)
    {
        //Taking name of the player as input after a game is completed
        if(key == '\r')
        {
            newScore.score = point;
            strcpy(newScore.name,input_str);
            input_str[0] = 0;
            len = 0;
            loadHighScore();
            updateHighScore(); //Saves the new high score to the file
            FILE *fp = fopen("save.txt","w");
            fprintf(fp,"0");  //Since the last game is complete, no save game now exists and game cannot be resumed
            fclose(fp);
            menuStatus = highScore; //Shows the updated high score page
        }
        else
        {
            input_str[len++] = key;
            input_str[len] = 0;
        }
    }

    if(menuStatus == selectTime)
    {
        //Taking the duration of game as input when the new game button in main menu is clicked
        if(key == '\r')
        {
            gameDuration = atoi(input_str);
            input_str[0] = 0;
            len = 0;
            freshStart();
            iResumeTimer(0);
            iResumeTimer(1);
            menuStatus = game;
        }
        else
        {
            input_str[len++] = key;
            input_str[len] = 0;
        }
    }
}

/*
	function iSpecialKeyboard() is called whenever user hits special keys like-
	function keys, home, end, pg up, pg down, arrows etc. you have to use
	appropriate constants to detect them. A list is:
	GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
	GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
	GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
	GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
*/
void iSpecialKeyboard(unsigned char key)
{
    if(menuStatus == game)
    {
        //Moves the basket using left and right arrows
        if(key == GLUT_KEY_RIGHT && basketPos+30 <= 1150)
            basketPos += 30;
        if(key == GLUT_KEY_LEFT && basketPos-30 >= 0)
            basketPos -= 30;
        if(key == GLUT_KEY_F1)
        {
            iPauseTimer(0);
            iPauseTimer(1);
            saveGame();
            menuStatus = mainMenu;
        }
    }
}

void timer()
{
    elapsedTime+=100;
    remainingTime-=100;
    if(elapsedTime%interval == 0)
    {
        //Spawns eggs after a set interval
        //start_x is a random x-position where the next egg is to be spawned
        //type is a random number that dictates the type of dropping to be spawned
        int start_x = rand()%1120 + 80, type = rand()%109 + 1;
        murgiPos = start_x - 80;
        droppingCreation(type,start_x);
    }

    int j;
    for(j = 0;j < 20;j++)
    {
        //Clears the eggs that had been dropped previously so that they can be spawned again
        if(egg[j].activeStatus == 1 && egg[j].activeUntill == elapsedTime)
        {
            egg[j].activeStatus = 0;
            egg[j].activeUntill = 0;
        }
    }

    for(j=0;j<3;j++)
    {
        if(perk[j].activeStatus == 1 && perk[j].activeUntill == elapsedTime)
        {
            perk[j].activeStatus = 0;
            perk[j].activeUntill = 0;
            if(!strcmp(perk[j].filename,"images\\2x.bmp"))
                pointFactor = 1;
            if(!strcmp(perk[j].filename,"images\\basket_perk.bmp"))
            {
                strcpy(basket_filename,"images\\basket.bmp");
                basketWidth = 130;
            }
        }
    }

    if(elapsedTime%1000 == 0) //Updates the time display every second
        updateTime();

    if(remainingTime == 0)
    {
        //Ends the game when the time is over
        iPauseTimer(0);
        iPauseTimer(1);
        input_str[0] = 0;
        len = 0;
        menuStatus = gameOver;
    }
}

void droppingCalc()
{
    int i;
    for(i = 0; i < 20; i++)
    {
        //Calculates and changes the position of all active droppings
        if(egg[i].activeStatus)
        {
            if(egg[i].activeUntill == 0)
            {
                egg[i].y -= egg[i].speed;
                egg[i].speed += g;
            }

            if(egg[i].y <= 125 && egg[i].x >= basketPos && egg[i].x <= basketPos + basketWidth)
            {
                //The dropping is now in the basket
                //Point is changed and the egg is reset so that it can be spawned again
                egg[i].activeStatus = 0;
                egg[i].y = 488;
                egg[i].speed = 0;
                if(!strcmp(egg[i].filename,"images\\white_egg.bmp"))
                    point += pointFactor*1;
                if(!strcmp(egg[i].filename,"images\\blue_egg.bmp"))
                    point += pointFactor*5;
                if(!strcmp(egg[i].filename,"images\\gold_egg.bmp"))
                    point += pointFactor*10;
                if(!strcmp(egg[i].filename,"images\\shit.bmp"))
                {
                    point-=pointFactor*10;
                    if(point<0)
                        point = 0;
                }
                PlaySound("sound\\egg_catch.wav",NULL,SND_ASYNC);
                strcpy(pointLine,"Points: ");
                strcat(pointLine,itoa(point,temporary,10));
            }

            else if(egg[i].y<50)
            {
                //Egg has dropped on the ground
                egg[i].y = 488;
                egg[i].speed = 0;
                egg[i].activeUntill = elapsedTime + 1000; //The egg will remain on the ground as a broken egg for the next 1 second
                PlaySound("sound\\egg_drop.wav",NULL,SND_ASYNC);
            }
        }
    }

    for(i = 0; i<3; i++)
    {
        if(perk[i].activeStatus)
        {
            if(perk[i].activeUntill == 0)
            {
                perk[i].y -= perk[i].speed;
                perk[i].speed += g;
            }

            if(perk[i].y <= 125 && perk[i].x >= basketPos && perk[i].x <= basketPos + basketWidth)
            {
                perk[i].y = 488;
                perk[i].speed = 0;
                if(!strcmp(perk[i].filename,"images\\time.bmp"))
                {
                    perk[i].activeStatus = 0;
                    remainingTime += 10*1000;
                }
                else if(!strcmp(perk[i].filename,"images\\basket_perk.bmp"))
                {
                    perk[i].activeUntill = elapsedTime + 7*1000;
                    basketWidth = 200;
                    strcpy(basket_filename,"images\\basket_big.bmp");
                }
                else
                {
                    perk[i].activeUntill = elapsedTime + 10*1000;
                    pointFactor = 2;
                }
            }

            else if(perk[i].y < 50)
            {
                perk[i].activeStatus = 0;
                perk[i].y = 488;
                perk[i].speed = 0;
            }
        }
    }
}

int main()
{
    freshStart();
    iSetTimer(100,timer);
    iSetTimer(20,droppingCalc);
    iPauseTimer(0);
    iPauseTimer(1);
    iInitialize(1280,720,"Catch the Egg!");
    return 0;
}
