#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GraphObject.h"
#include <string>
#include <queue>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool canDig(int x, int y);
    void updateText();
    std::string formatText(int score, int level, int lives, int hp, int water, int gold, int sonar, int barrels);
    void addActor(Actor* actor);
    void addItem();
    void addProtestor();
    void removeProtestor();
    void removeBarrel();
    bool isInRadius(int x1, int y1, int x2, int y2, int radius);
    bool isActorInRadius(int x, int y, int radius);
    void addObject(int num, std::string type);
    bool isAboveEarth(int x, int y);
    bool isThereEarth(int x, int y);
    bool isThereBoulder(int x, int y, int radius);
    bool isPlayerInRadius(Actor* actor, int radius);
    void removeDead();
    void dropGold();
    void useSonar();
    bool canMoveInDir(int x, int y, GraphObject::Direction dir);
    
    
    Protester* isProtesterInRadius(Actor* actor, int radius);
    Tunnelman* getPlayer();

    
private:
    Tunnelman* m_player;
    Earth* m_earth[64][64];
    std::vector<Actor*> m_actors;
    
    bool m_isFirstTick;
    int m_barrels;
    int m_protestors;
    int m_ticks;
};

#endif // STUDENTWORLD_H_
