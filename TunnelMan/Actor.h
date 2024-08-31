#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

// base class for game objects
class Actor : public GraphObject {
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);
    virtual ~Actor();
    
    StudentWorld* getWorld();
    bool isAlive();
    void die();
    void moveTo(int x, int y);
    virtual void doSomething();

private:
    StudentWorld* m_world;
    bool m_isAlive;
};

class Earth : public Actor {
public:
    Earth(StudentWorld* world, int startX, int startY);
    
    virtual void doSomething();
};

class Human : public Actor {
public:
    Human(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp);
    
    
    int getHp();
    void subtractHp(int pts);
    virtual void doSomething();
    
    virtual void moveInDir(Direction dir) = 0;
    
private:
    int m_hp;
};

class Tunnelman : public Human {
public:
    Tunnelman(StudentWorld* world);
    
    virtual void doSomething();
    virtual void isAnnoyed(int hp);
    virtual void moveInDir(Direction dir);
    
    void addItem(int itemId);
    int getWater();
    int getSonar();
    int getGold();
    
    void shootWater();
    
private:
    int m_water;
    int m_sonarCharge;
    int m_gold;
};

class Boulder : public Actor {
public:
    Boulder(StudentWorld* world, int startX, int startY);
    
    virtual void doSomething();
    void annoyHuman();

private:
    bool m_isStable;
    bool m_isFalling;
    int m_ticks;
};

class Squirt : public Actor {
public:
    Squirt(StudentWorld* world, int startX, int startY, Direction dir);
    
    virtual void doSomething();
    bool annoyProtesters();

private:
    int m_distance;
    
};

class Item : public Actor {
public:
    Item(StudentWorld* world, int imageID, int startX, int startY);
    
    virtual void doSomething() = 0;
    virtual void lifetime(int time);
    
private:
    int m_ticks;
};

class OilBarrel : public Item {
public:
    OilBarrel(StudentWorld* world, int startX, int startY);
    
    virtual void doSomething();
};

class GoldNugget : public Item {
public:
    GoldNugget(StudentWorld* world, int startX, int startY, bool isVisible, bool isPickupable);
    
    virtual void doSomething();
    
private:
    bool m_isPickupable;
};

class SonarKit : public Item {
public:
    SonarKit(StudentWorld* world, int startX, int startY);
    
    virtual void doSomething();
    
private:
    int m_ticks;
};

class WaterPool : public Item {
public:
    WaterPool(StudentWorld* world, int startX, int startY);
    
    virtual void doSomething();
    
private:
    int m_ticks;
};

class Protester : public Human {
public:
    Protester(StudentWorld* world, int imageID,int hp);
    virtual void doSomething();
    virtual void bribeProtester();
    void moveInDir(Direction dir);
    virtual void isAnnoyed(int hp);
    void stunProtester();
    bool isFacingPlayer();
    Direction dirToPlayer();
    bool isTherePathToPlayer(Direction dir);
    bool canTurn();
    void pickDir();
    Direction randomDir();
    


private:
    bool m_leaveField;
    int m_numToMove;
    int m_ticksSinceLast;
    int m_ticksSinceYell;
    int m_ticksResting;
};

class RegularProtester : public Protester {
public:
    RegularProtester(StudentWorld* world);
};

class HardcoreProtester : public Protester {
public:
    HardcoreProtester(StudentWorld* world);
};

#endif // ACTOR_H_
