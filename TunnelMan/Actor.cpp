#include "Actor.h"
#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>
using namespace std;

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor Implementation
Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
: GraphObject(imageID, startX, startY, dir, size, depth), m_world(world), m_isAlive(true)
{
    setVisible(true);
}

Actor::~Actor()
{
    setVisible(false);
}

StudentWorld* Actor::getWorld()
{
    return m_world;
}

bool Actor::isAlive()
{
    return m_isAlive;
}

void Actor::die()
{
    m_isAlive = false;
}

void Actor::moveTo(int x, int y) // can only move within bounds
{
    if (x < 0)
        x = 0;
    if (x > 60)
        x = 60;
    if (y < 0)
        y = 0;
    if (y > 60)
        y = 60;
    
    GraphObject::moveTo(x, y);
}

void Actor::doSomething() {}

// Earth Implementation
Earth::Earth(StudentWorld* world, int startX, int startY)
: Actor(world, TID_EARTH, startX, startY, right, 0.25, 3) {}


void Earth::doSomething() {}

// Human Implementation
Human::Human(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp)
: Actor(world, imageID, startX, startY, dir, 1.0, 0), m_hp(hp) {}

int Human::getHp()
{
    return m_hp;
}

void Human::subtractHp(int pts)
{
    m_hp -= pts;
}

void Human::doSomething() {}

// Tunnelman Implementation
Tunnelman::Tunnelman(StudentWorld* world)
: Human(world, TID_PLAYER, 30, 60, right, 10), m_water(5), m_gold(0), m_sonarCharge(1) {}




void Tunnelman::doSomething()
{
    if (!isAlive()) // if dead, can't move
        return;
    
    if (getWorld()->canDig(getX(), getY())) {  // if digging, play dig sound
        getWorld()->playSound(SOUND_DIG);
    }
    
    int key;
    if (getWorld()->getKey(key)) { // directional keys
        switch (key) {
            case KEY_PRESS_ESCAPE:
                die();
                break;
            case KEY_PRESS_UP:
                moveInDir(up);
                break;
            case KEY_PRESS_DOWN:
                moveInDir(down);
                break;
            case KEY_PRESS_LEFT:
                moveInDir(left);
                break;
            case KEY_PRESS_RIGHT:
                moveInDir(right);
                break;
            case KEY_PRESS_TAB:
                getWorld()->dropGold();
                m_gold--;
                break;
            case 'Z':
            case 'z':
                getWorld()->useSonar();
                m_sonarCharge--;
                break;
            case KEY_PRESS_SPACE:
                if (m_water > 0) {
                    m_water--;
                    shootWater();
                }
                break;
            default:
                break;
        }
    }
}

void Tunnelman::isAnnoyed(int hp)
{
    subtractHp(hp);
    if (getHp() <= 0) {
        die();
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}

void Tunnelman::moveInDir(Direction dir)
{
    const int radius = 3;
    switch (dir) {
        case up: // change direction if different key, move in direction if same key
            if (getDirection() == up) {
                if (!getWorld()->isThereBoulder(getX(), getY() + 1, radius)) {
                    moveTo(getX(), getY() + 1);
                }
                else {
                    moveTo(getX(), getY());
                }
            }
            else
                setDirection(up);
            break;
        case down:
            if (getDirection() == down) {
                if (!getWorld()->isThereBoulder(getX(), getY() - 1, radius)) {
                    moveTo(getX(), getY() - 1);
                }
                else {
                    moveTo(getX(), getY());
                }
            }
            else
                setDirection(down);
            break;
        case left:
            if (getDirection() == left) {
                if (!getWorld()->isThereBoulder(getX() - 1, getY(), radius)) {
                    moveTo(getX() - 1, getY());
                }
                else {
                    moveTo(getX(), getY());
                }
            }
            else
                setDirection(left);
            break;
        case right:
            if (getDirection() == right) {
                if (!getWorld()->isThereBoulder(getX() + 1, getY(), radius)) {
                    moveTo(getX() + 1, getY());
                }
                else {
                    moveTo(getX(), getY());
                }
            }
            else
                setDirection(right);
            break;
        case none:
        default:
            return;
            break;
    }
}

void Tunnelman::addItem(int itemId)
{
    if (itemId == TID_GOLD) {
        m_gold++;
    }
    if (itemId == TID_SONAR) {
        m_sonarCharge += 2 ;
    }
    if (itemId == TID_WATER_POOL) {
        m_water += 5;
    }
}

int Tunnelman::getWater()
{
    return m_water;
}

int Tunnelman::getSonar()
{
    return m_sonarCharge;
}

int Tunnelman::getGold()
{
    return m_gold;
}

void Tunnelman::shootWater()
{
    int startY = getY();
    int startX = getX();
    switch (getDirection())
    {
    case up:
        startY += 3;
        break;
    case down:
        startY -= 3;
        break;
    case left:
        startX -= 3;
        break;
    case right:
        startX = startX + 3;
        break;
    case none:
    default:
        return;
        break;
    }
    getWorld()->addActor(new Squirt(getWorld(), startX, startY, getDirection()));
    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
}

// Boulder Implementation
Boulder::Boulder(StudentWorld* world, int startX, int startY)
: Actor(world, TID_BOULDER, startX, startY, down, 1.0, 1), m_isStable(true), m_ticks(0), m_isFalling(false)
{
    world->canDig(startX, startY);
}


void Boulder::doSomething()
{
    if (!isAlive()) {
        return;
    }
    
    if (m_isStable) {
        if (getWorld()->isAboveEarth(getX(), getY() - 1)) {
            return;
        }
        else {
            m_isStable = false;
        }
    }
    if (m_ticks == 30) {
        m_isFalling = true;
        getWorld()->playSound(SOUND_FALLING_ROCK);
    }
    m_ticks++;
    if (m_isFalling) {
        if (getWorld()->isAboveEarth(getX(), getY() - 1) || getWorld()->isThereBoulder(getX(), getY() - 4, 0) || getY() == 0) {
            die();
        }
        else {
            moveTo(getX(), getY() - 1);
            annoyHuman();
        }
    }
}

void Boulder::annoyHuman()
{
    if (getWorld()->isPlayerInRadius(this, 3)) {
        getWorld()->getPlayer()->isAnnoyed(100);
    }
    Protester* protester = getWorld()->isProtesterInRadius(this, 3);
    if (protester != nullptr) {
        protester->isAnnoyed(100);
    }
}

// Squirt Implementation
Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction dir)
: Actor(world, TID_WATER_SPURT, startX, startY, dir, 1.0, 1), m_distance(0) {}


void Squirt::doSomething()
{
    const int radius = 3;
    
    if (!isAlive()) {
        return;
    }
    if (annoyProtesters() || m_distance == 4) {
        die();
        return;
    }
    
    switch (getDirection()) {
        case up: // change direction if different key, move in direction if same key
            if (getWorld()->isThereEarth(getX(), getY() + 1) || getWorld()->isThereBoulder(getX(), getY() + 1, radius)) {
                    die();
                    return;
            }
            else {
                moveTo(getX(), getY() + 1);
            }
            break;
        case down:
            if (getWorld()->isThereEarth(getX(), getY() - 1) || getWorld()->isThereBoulder(getX(), getY() - 1, radius)) {
                    die();
                    return;
            }
            else {
                moveTo(getX(), getY() - 1);
            }
            break;
        case left:
            if (getWorld()->isThereEarth(getX() - 1, getY()) || getWorld()->isThereBoulder(getX() - 1, getY(), radius)) {
                    die();
                    return;
            }
            else {
                moveTo(getX() - 1, getY());
            }
            break;
        case right:
            if (getWorld()->isThereEarth(getX() + 1, getY()) || getWorld()->isThereBoulder(getX() + 1, getY(), radius)) {
                    die();
                    return;
            }
            else {
                moveTo(getX() + 1, getY());
            }
            break;
        case none:
        default:
            return;
            break;
    }
    m_distance++;
}

bool Squirt::annoyProtesters()
{
    Protester* protester = getWorld()->isProtesterInRadius(this, 3);
        if (protester != nullptr) {
            protester->isAnnoyed(2);
            return true;
        }
        return false;
}

// Item Implementation
Item::Item(StudentWorld* world, int imageID, int startX, int startY)
: Actor(world, imageID, startX, startY, right, 1.0, 2) {}


void Item::lifetime(int time)
{
    if (m_ticks == time) {
        die();
    }
    else {
        m_ticks++;
    }
}

// OilBarrel Implementation
OilBarrel::OilBarrel(StudentWorld* world, int startX, int startY)
: Item(world, TID_BARREL, startX, startY)
{
    setVisible(false);
    
}

    
void OilBarrel::doSomething()
{
    if (!isAlive()) {
        return;
    }
    if (!isVisible() && getWorld()->isPlayerInRadius(this, 4)) {
        setVisible(true);
        return;
    }
    if (getWorld()->isPlayerInRadius(this, 3)) {
        die();
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        getWorld()->removeBarrel();
        return;
    }
}

// GoldNugget Implementation
GoldNugget::GoldNugget(StudentWorld* world, int startX, int startY, bool isVisible, bool isPickupable)
: Item(world, TID_GOLD, startX, startY),  m_isPickupable(isPickupable)
{
    setVisible(isVisible);
}

    
void GoldNugget::doSomething()
{
    if (!isAlive()) {
        return;
    }
    if (!isVisible() && getWorld()->isPlayerInRadius(this, 4)) {
        setVisible(true);
        return;
    }
    if (!m_isPickupable && getWorld()->isPlayerInRadius(this, 3)) {
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getPlayer()->addItem(getID());
        getWorld()->increaseScore(10);
        return;
    }
    if (m_isPickupable) {
        Protester* protester = (getWorld()->isProtesterInRadius(this, 3));
        if (protester != nullptr) {
            die();
            protester->bribeProtester();
        };
        lifetime(100);
    }
}

//SonarKit Implementation
SonarKit::SonarKit(StudentWorld* world, int startX, int startY)
: Item(world, TID_SONAR, startX, startY)
{
    m_ticks = max(100, 300 - 10 * (int)getWorld()->getLevel());
}

    
void SonarKit::doSomething()
{
    if (!isAlive()) {
        return;
    }
    if (getWorld()->isPlayerInRadius(this, 3))
    {
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getPlayer()->addItem(TID_SONAR);
        getWorld()->increaseScore(75);
    }
        lifetime(m_ticks);

}

// WaterPool Implementation
WaterPool::WaterPool(StudentWorld* world, int startX, int startY)
: Item(world, TID_WATER_POOL, startX, startY)
{
    m_ticks = max(100, 300 - 10 * (int)getWorld()->getLevel());
}


    
void WaterPool::doSomething()
{
    if (!isAlive()) {
        return;
    }
    if (getWorld()->isPlayerInRadius(this, 3))
    {
        die();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getPlayer()->addItem(TID_WATER_POOL);
        getWorld()->increaseScore(100);
    }
        lifetime(m_ticks);
}

// Protester Implementation
Protester::Protester(StudentWorld* world, int imageID, int hp)
: Human(world, imageID, 60, 60, left, hp), m_leaveField(false), m_ticksSinceLast(200), m_ticksSinceYell(15)
{
    m_ticksResting = max(0, 3 - (int)getWorld()->getLevel() / 4);
    m_numToMove = rand() % 53 + 8;
}
    
void Protester::doSomething()
{
    if (!isAlive()) {
        return;
    }
    
    if (m_ticksResting > 0) {
        m_ticksResting--;
        return;
    }
    else {
        m_ticksResting = max(0, 3 - (int)getWorld()->getLevel() / 4);
        m_ticksSinceLast++;
        m_ticksSinceYell++;
    }
    
    if (m_leaveField) {
        if (getX() == 60 && getY() == 60) {
            die();
            getWorld()->removeProtestor();
            return;
        }
        return;
    }
    
    if (getWorld()->isPlayerInRadius(this, 4) && isFacingPlayer()) {
        if (m_ticksSinceYell > 15) {
            getWorld()->getPlayer()->Tunnelman::isAnnoyed(2);
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            m_ticksSinceYell = 0;
            return;
        }
    }
    
    
    Direction dir = dirToPlayer();
    if (dir != none && isTherePathToPlayer(dir) && (!getWorld()->isPlayerInRadius(this, 4))) {
        setDirection(dir);
        moveInDir(dir);
        m_numToMove = 0;
        return;
    }
    
    m_numToMove--;
    
    if (m_numToMove <= 0) {
        Direction d = none;
        d = randomDir();
        do {
            d = randomDir();
        } while (!getWorld()->canMoveInDir(getX(), getY(), d));
        
        setDirection(d);
        m_numToMove = rand() % 53 + 8;
    }
    else if (canTurn() && m_ticksSinceLast > 200) {
        pickDir();
        m_ticksSinceLast = 0;
        m_numToMove = rand() % 53 + 8;
    }
    
    moveInDir(getDirection());
    
    if (!getWorld()->canMoveInDir(getX(), getY(), getDirection())) {
        m_numToMove = 0;
    }
}

void Protester::moveInDir(Direction dir)
{
    switch (dir) {
        case up: // change direction if different key, move in direction if same key
            if (getDirection() == up) {
                if (getY() == 60) {
                    setDirection(down);
                }
                moveTo(getX(), getY() + 1);
            }
            else setDirection(up);
            break;
        case down:
            if (getDirection() == down) {
                if (getY() == 0) {
                    setDirection(up);
                }
                moveTo(getX(), getY() - 1);
            }
            else setDirection(down);
            break;
        case left:
            if (getDirection() == left) {
                if (getX() == 0) {
                    setDirection(right);
                }
                moveTo(getX() - 1, getY());
            }
            else setDirection(left);
            break;
        case right:
            if (getDirection() == right) {
                if (getX() == 0) {
                    setDirection(left);
                }
                moveTo(getX() + 1, getY());
            }
            else setDirection(right);
            break;
        case none:
        default:
            return;
            break;
    }
}

void Protester::bribeProtester()
{
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    if (getID() == TID_PROTESTER) {
        getWorld()->increaseScore(25);
        m_leaveField = true;
    }
    else {
        getWorld()->increaseScore(50);
        m_ticksResting = max(50, 100 - (int)(getWorld()->getLevel() * 10));
    }
}

void Protester::isAnnoyed(int hp)
{
    if (m_leaveField) {
        return;
    }
    subtractHp(hp);
    getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    stunProtester();
    if (getHp() <= 0) {
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        m_leaveField = true;
        m_ticksResting = 0;
        if (hp == 100) {
            getWorld()->increaseScore(500);
        }
        else if (getID() == TID_PROTESTER) {
            getWorld()->increaseScore(100);
        }
        else {
            getWorld()->increaseScore(250);
        }
    }
}

void Protester::stunProtester()
{
    m_ticksResting = max(50, 100 - (int)getWorld()->getLevel() * 10);
}

bool Protester::isFacingPlayer()
{
    switch (getDirection()) {
        case up:
            return getWorld()->getPlayer()->getY() >= getY();
        case down:
            return getWorld()->getPlayer()->getY() <= getY();
        case left:
            return getWorld()->getPlayer()->getX() <= getX();
        case right:
            return getWorld()->getPlayer()->getX() >= getX();
        case none:
            return false;
    }
    return false;
}

GraphObject::Direction Protester::dirToPlayer()
{
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getY();
    Direction dir;

    if (getY() == playerY && getX() == playerX) {
        dir = getDirection();
    }
    else if (getX() == playerX) {
        if (getY() < playerY) {
        dir = up;
        }
        else if (getY() > playerY) {
            dir = down;
        }
    }
    else if (getY() == playerY) {
        if (getX() > playerX) {
            dir = left;
        }
        else if (getX() < playerX) {
            dir = right;
        }
    }
    else {
        dir = none;
    }
    return dir;

}

bool Protester::isTherePathToPlayer(Direction dir)
{
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getX();
    const int radius = 3;
    
    switch (dir) {
        case left:
            for (int i = getX(); i >= playerX; i--) {
                if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereBoulder(i, getY(), radius))
                    return false;
            }
            return true;
            break;
        case right:
            for (int i = getX(); i <= playerX; i++) {
                if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereBoulder(i, getY(), radius))
                    return false;
            }
            return true;
            break;
        case up:
            for (int i = getY(); i <= playerY; i++) {
                if (getWorld()->isThereEarth(getX(), i) || getWorld()->isThereBoulder(getX(), i, radius))
                    return false;
            }
            return true;
            break;
        case down:
            for (int i = getY(); i >= playerY; i--) {
                if (getWorld()->isThereEarth(getX(), i) || getWorld()->isThereBoulder(getX(), i, radius))
                    return false;
            }
            return true;
            break;
        case none:
            return false;
        }
}

bool Protester::canTurn()
{
    if (getDirection() == up || getDirection() == down) {
        return (getWorld()->canMoveInDir(getX(),getY(), left) || getWorld()->canMoveInDir(getX(),getY(), right));
        }
    else {
        return (getWorld()->canMoveInDir(getX(),getY(), up) || getWorld()->canMoveInDir(getX(),getY(), down));
    }
}

GraphObject::Direction Protester::randomDir()
{
    int num;
    num = rand() % 4;
    switch (num) {
        case 0: 
            return left;
            break;
        case 1:
            return right;
            break;
        case 2:
            return up;
            break;
        case 3:
            return down;
            break;
        default:
            break;
    }
    return none;
}

void Protester::pickDir()
{
    if (getDirection() == up || getDirection() == down) {
        if (!getWorld()->canMoveInDir(getX(),getY(), left)) { setDirection(right);
        }
        else if (!getWorld()->canMoveInDir(getX(),getY(), right)) { setDirection(left);
        }
        else {
            switch (rand() % 2 + 1) {
            case 1: setDirection(left);
            case 2: setDirection(right);
            }
        }
    }
    else {
        if (!getWorld()->canMoveInDir(getX(),getY(), up)) {
            setDirection(down);
        }
        else if (!getWorld()->canMoveInDir(getX(),getY(), down)) { setDirection(up);
        }
        else {
            switch (rand() % 2 + 1) {
            case 1: setDirection(up);
            case 2: setDirection(down);
            }
        }
    }
}


// RegularProtester Implementation
RegularProtester::RegularProtester(StudentWorld* world)
: Protester(world, TID_PROTESTER, 5) {}



// HardcoreProtester Implementaion
HardcoreProtester::HardcoreProtester(StudentWorld* world)
: Protester(world, TID_HARD_CORE_PROTESTER, 20) {}

