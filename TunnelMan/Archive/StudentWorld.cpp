#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir)
    : GameWorld(assetDir) {}

StudentWorld::~StudentWorld() // same as clean up
{
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 60; y++) {
            delete m_earth[x][y];
        }
    }
    
    for (vector<Actor*>:: iterator i = m_actors.begin(); i != m_actors.end(); i++) {
        delete *i;
    }
    m_actors.clear();
    
    delete m_player;

}

int StudentWorld::init()
{
    // initialize data structures to keep track of virtual world
    // construct new oil field that meets requirements
    // allocate and insert valid TunnelMan object at proper location
    
    m_barrels = 0;
    m_protestors = 0;
    m_ticks = 0;
    m_isFirstTick = true;
    
    for (int x = 0; x < 64; x++) { // new oil field
        for (int y = 0; y < 60; y++) {
            if (x < 30 || x > 33 || y < 4) { // leave middle empty
                m_earth[x][y] = new Earth(this, x, y);
            }
        }
    }
    
    m_player = new Tunnelman(this); // new tunnelman
    
    int level = getLevel();
    int B = min((level / 2) + 2, 9);
    int G = max(5 - (level / 2), 2);
    int L = min(2 + level, 21);
    
    addObject(B, "Boulder");
    addObject(G, "Gold");
    addObject(L, "Oil");
    
    return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    updateText();
    
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++) {
        if ((*i)->isAlive()) {
            (*i)->doSomething();
        }
        if (!m_player->isAlive()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if (m_barrels == 0) {
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    m_player->doSomething();
    
    addItem();
    addProtestor();
    
    removeDead();
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() // delete earth, delete player
{
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 60; y++) {
            delete m_earth[x][y];
        }
    }
    
    for (vector<Actor*>:: iterator i = m_actors.begin(); i != m_actors.end(); i++) {
        delete *i;
    }
    m_actors.clear();
    
    delete m_player;

}

bool StudentWorld::canDig(int x, int y)
{
    bool isClear = false;
    
    for (int i = x; i < x + 4; i++) {
        for (int j = y; j < y + 4; j++) {
            if (m_earth[i][j] != nullptr) { // if earth in the way, delete and dig
                delete m_earth[i][j];
                m_earth[i][j] = nullptr;
                isClear = true;
            }
        }
    }
    
    return isClear;
}

void StudentWorld::updateText()
{
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int hp = m_player->getHp();
    int water = m_player->getWater();
    int gold = m_player->getGold();
    int sonar = m_player->getSonar();
    
    string text = formatText(score, level, lives, hp, water, gold, sonar, m_barrels);
    setGameStatText(text);
}
std::string StudentWorld::formatText(int score, int level, int lives, int hp, int water, int gold, int sonar, int barrels)
{
    stringstream text;
    int hpPercent = hp * 10;
    

    text.fill(' ');
    text << " Lvl: " << setw(2) << level;
    text << " Lives: " << setw(1) << lives;
    text << "  Hlth: " << setw(3) << hpPercent << '%';
    text << "  Wtr: " << setw(2) << water;
    text << "  Gld: " << setw(2) << gold;
    text << "  Oil Left: " << setw(2) << barrels;
    text << "  Sonar: " << setw(2) << sonar;
    text.fill('0');
    text << "Scr: " << setw(6) << score;

    return text.str();
    
}

void StudentWorld::addActor(Actor* actor)
{
    m_actors.push_back(actor);
}

void StudentWorld::addItem()
{

    int G = getLevel() * 25 + 300;
    if ((int)(rand() % G) + 1 == 1) {
        int x;
        int y;
        
        if ((int)(rand() % 5) + 1 == 1) {
            addActor(new SonarKit(this, 0, 60));
        }
        else {
            do {
                x = rand() % 60 + 1;
                y = rand() % 60 + 1;
            } while (isThereEarth(x, y) || isThereBoulder(x, y, 0) || isActorInRadius(x, y, 0));
            
            addActor(new WaterPool(this, x, y));
        }
    }
}

void StudentWorld::addProtestor()
{
    int T = max(25, 200 - (int)getLevel());
    int P = min(15, 2 + (int)(getLevel() * 1.5));
    
    int probabilityOfHardcore = min(90, (int)(getLevel() * 10) + 30);
    
    if (m_isFirstTick || (m_ticks > T && m_protestors < P)) {
        if (probabilityOfHardcore > rand() % 100) {
            addActor(new HardcoreProtester(this));
        }
        else {
            addActor(new RegularProtester(this));
        }
        
        m_isFirstTick = false;
        m_protestors++;
        m_ticks = 0;
    }
    m_ticks++;
}
        
void StudentWorld::removeProtestor()
{
        m_protestors--;
}

void StudentWorld::removeBarrel()
{
    m_barrels--;
}

bool StudentWorld::isInRadius(int x1, int y1, int x2, int y2, int radius)
{
    int distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
    if (distance <= radius) {
        return true;
    }
    else {
        return false;
    }
}

bool StudentWorld::isActorInRadius(int x, int y, int radius)
{
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++) {
        if (isInRadius(x, y, (*i)->getX(), (*i)->getY(), radius)) {
            return true;
        }
    }
    return false;
}

void StudentWorld::addObject(int num, std::string type)
{
    int x;
    int y;
    for (int i = 0; i < num; i ++) {
        do {
            x = rand() % 60 + 1;
            if (type == "Boulder") {
                y = rand() % 36 + 21;
            }
            else {
                y = rand() % 56 + 1;
            }
        } while (isActorInRadius(x, y, 6) || (x > 26 && x < 34 && y > 0));
        
        if (type == "Boulder") {
            addActor(new Boulder(this, x, y));
        }
        else if (type == "Gold") {
            addActor(new GoldNugget(this, x, y, false, false));
        }
        else if (type == "Oil") {

            addActor(new OilBarrel(this, x, y));
            m_barrels++;
        }
    }
}

bool StudentWorld::isAboveEarth(int x, int y)
{
    for (int i = x; i < x + 4; i++) {
        if (m_earth[i][y] != nullptr) {
            return true;
        }
    }
    return false;
}

bool StudentWorld::isThereEarth(int x, int y)
{
    for (int i = x; i < x + 4; i++) {

        for (int j = y; j < y + 4; j++) {

            if (m_earth[i][j] != nullptr)
                return true;
        }
    }
    return false;
}

bool StudentWorld::isThereBoulder(int x, int y, int radius)
{
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++) {
            if ((*i)->getID() == TID_BOULDER && isInRadius(x, y, (*i)->getX(), (*i)->getY(), radius)) {
                return true;
            }
    }
    
    return false;

}

bool StudentWorld::isPlayerInRadius(Actor* actor, int radius)
{
    return isInRadius(actor->getX(), actor->getY(), m_player->getX(), m_player->getY(), radius);
}


void StudentWorld::removeDead()
{
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++)
        {
            if (!(*i)->isAlive())
            {
                (*i)->setVisible(false);
            }
        }

}

void StudentWorld::dropGold()
{
    if (m_player->getGold() > 0)
    {
        addActor(new GoldNugget(this, m_player->getX(), m_player->getY(), true, false));
    }


}

void StudentWorld::useSonar()
{
    if (m_player->getSonar() > 0)
    {
        playSound(SOUND_SONAR);
        for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++)
        {
            if (isInRadius(m_player->getX(), m_player->getY(), (*i)->getX(), (*i)->getY(), 12) &&
                ((*i)->getID() == TID_GOLD || (*i)->getID() == TID_BARREL))
                (*i)->setVisible(true);
        }
    }
}

Tunnelman* StudentWorld::getPlayer()
{
    return m_player;
}

Protester* StudentWorld::isProtesterInRadius(Actor* actor, int radius)
{
    for (vector<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++) {
            if ((*i)->getID() == TID_PROTESTER || (*i)->getID() == TID_HARD_CORE_PROTESTER)
                if (isInRadius(actor->getX(), actor->getY(), (*i)->getX(), (*i)->getY(), radius))
                {
                    return dynamic_cast<Protester*> (*i);
                }
        }
        return nullptr;

}

bool StudentWorld::canMoveInDir(int x, int y, GraphObject::Direction dir)
{
    int newX = x;
    int newY = y;
    
    switch (dir) {
    case GraphObject::left:
        newX--;
        break;
    case GraphObject::right:
        newX++;
        break;
    case GraphObject::up:
        newY++;
        break;
    case GraphObject::down:
        newY--;
        break;
    case GraphObject::none:
            return false;
    }
    
    if (isThereEarth(newX, newY))
    {
        return false;
    }
    return (newX >= 0 && newX < 61 && newY >= 0 && newY < 61 &&
        !isThereEarth(newX, newY) && !isThereBoulder(newX, newY, 3));
}


