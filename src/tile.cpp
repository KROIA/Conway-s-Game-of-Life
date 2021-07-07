#include "tile.h"

Tile::Tile(Vector2u coord)
    :   GameObject()
{
    m_coord = coord;

    GameObject::setPosInital(Vector2f(m_coord));
    //m_collider->addHitbox(RectF(0,0,1,1));
    m_neighbourList.reserve(8);

    m_deadColor = Color(0,0,0);

    int r = rand()%3;
    if(r == 0)
        setDead();
    else
        setAlive();

    m_nextTickState = m_state;
    m_colorFadeFactor = 0.8;
    //GameObject::setVisibility_collider_hitbox(true);
}

void Tile::addNeighbour(Tile *neighbour)
{
    if(neighbour != nullptr)
        m_neighbourList.push_back(neighbour);
}
void Tile::setNeighbourList(const vector<Tile*> &list)
{
    m_neighbourList = list;
}
void Tile::setColorFadeFactor(float factor)
{
    if(factor > 0 && factor < 1)
        m_colorFadeFactor = factor;
}

const Vector2u &Tile::getCoord() const
{
    return m_coord;
}
const Tile::State &Tile::getState() const
{
    return m_state;
}
const Color    &Tile::getAliveColor() const
{
    return m_aliveColor;
}
void Tile::prepareForNextMove()
{
    int aliveNeighbourCount = 0;
    m_colorList.clear();
    m_colorList.reserve(8);
    for(size_t i=0; i<m_neighbourList.size(); i++)
    {
        if(m_neighbourList[i]->getState() == alive)
        {

            aliveNeighbourCount++;
            m_colorList.push_back(m_neighbourList[i]->getAliveColor());
        }
    }

    if(aliveNeighbourCount == 3 &&
       m_state == dead)
    {
        m_nextTickState = alive;
    }else if(aliveNeighbourCount < 2 &&
             m_state == alive)
    {
        m_nextTickState = dead;
    }if((aliveNeighbourCount == 2 ||
         aliveNeighbourCount == 3) &&
         m_state == alive)
    {
        m_nextTickState = alive;
    }else if(aliveNeighbourCount > 3)
    {
        m_nextTickState = dead;
    }

    if(m_state == alive && m_nextTickState == dead)
        m_deadColor = m_aliveColor;

}
void Tile::tick(const Vector2i &direction)
{
    if(direction.x)
    {
        prepareForNextMove();
    }
    else
    {
        mixColors();
        m_state = m_nextTickState;        
    }
}
void Tile::draw(PixelDisplay &display)
{
    if(m_state == dead)
    {
        fadeColor(m_deadColor,m_colorFadeFactor);
        display.setPixel(Vector2i(m_coord),m_deadColor);
    }
    else
    {

        display.setPixel(Vector2i(m_coord),m_aliveColor);
    }
    //GameObject::draw(display);
}
void Tile::setAlive()
{
    m_state = alive;
    m_nextTickState = alive;
    m_aliveColor.r = (rand()%155)+100;
    m_aliveColor.g = (rand()%155)+100;
    m_aliveColor.b = (rand()%155)+100;
}
void Tile::setAlive(const Color &color)
{
    m_state = alive;
    m_nextTickState = alive;
    m_aliveColor = color;
}
void Tile::setDead()
{
    m_nextTickState = dead;
    m_state = dead;
}
void Tile::mixColors()
{
    if(m_colorList.size() == 0)
        return;
    long r;
    long g;
    long b;
   // if(m_state == dead)
    {
        r= (rand()%10);
        g= (rand()%10);
        b= (rand()%10);
    }
    for(size_t i=0; i<m_colorList.size(); i++)
    {
        r+=m_colorList[i].r;
        g+=m_colorList[i].g;
        b+=m_colorList[i].b;
    }

    m_aliveColor.r = r / m_colorList.size();
    m_aliveColor.g = g / m_colorList.size();
    m_aliveColor.b = b / m_colorList.size();
}
void Tile::fadeColor(Color &c,float factor)
{
    c.r *= factor;
    c.g *= factor;
    c.b *= factor;
}
