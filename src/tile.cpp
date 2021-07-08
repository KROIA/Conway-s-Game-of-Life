#include "tile.h"

float Tile::perlinXOffset = 0;
float Tile::perlinYOffset = 0;
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
    if(factor >= 0 && factor <= 1)
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
const Color    &Tile::getColor() const
{
    if(m_state == alive)
        return m_aliveColor;
    return m_deadColor;
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
       /* if(m_coord.x == 0 && m_coord.y == 0)
        {
            perlinXOffset+= 0.01;
            perlinYOffset+= 0.01;
           // qDebug() << perlinXOffset;
        }*/

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
    float scale = 50;
    float offset = 30;
    float colorStrength = 128;
    m_aliveColor.r =(1+perlin((float)m_coord.x/scale+perlinXOffset,(float)m_coord.y/scale+perlinYOffset))*colorStrength;
    m_aliveColor.g =(1+perlin((float)m_coord.x/scale+offset+perlinXOffset,(float)m_coord.y/scale+offset+perlinYOffset))*colorStrength;;
    m_aliveColor.b =(1+perlin((float)m_coord.x/scale+2*offset+perlinXOffset,(float)m_coord.y/scale+2*offset+perlinYOffset))*colorStrength;


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
    long r = 0;
    long g = 0;
    long b = 0;
    float scale = 50;
    float offset = 10;

    float colorStrength = 5;
    if(m_state == dead)
    {
        r= (1+perlin((float)m_coord.x/scale+perlinXOffset,(float)m_coord.y/scale+perlinYOffset))*colorStrength;
        g= (1+perlin((float)m_coord.x/scale+offset+perlinXOffset,(float)m_coord.y/scale+offset+perlinYOffset))*colorStrength;;
        b= (1+perlin((float)m_coord.x/scale+2*offset+perlinXOffset,(float)m_coord.y/scale+2*offset+perlinYOffset))*colorStrength;
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


/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 */
float interpolate(float a0, float a1, float w) {
    /* // You may want clamping by inserting:
     * if (0.0 > w) return a0;
     * if (1.0 < w) return a1;
     */
    return (a1 - a0) * w + a0;
    /* // Use this cubic interpolation [[Smoothstep]] instead, for a smooth appearance:
     * return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
     *
     * // Use [[Smootherstep]] for an even smoother result with a second derivative equal to zero on boundaries:
     * return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
     */
}



/* Create random direction vector
 */
Vector2f randomGradient(int ix, int iy) {
    // Random float. No precomputed gradients mean this works for any number of grid coordinates
    float random = 2920.f * sin(ix * 21942.f + iy * 171324.f + 8912.f) * cos(ix * 23157.f * iy * 217832.f + 9758.f);
    return Vector2f(cos(random),sin(random));
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y) {
    // Get gradient from integer coordinates
    Vector2f gradient = randomGradient(ix, iy);

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx*gradient.x + dy*gradient.y);
}

// Compute Perlin noise at coordinates x, y
float perlin(float x, float y) {
    // Determine grid cell coordinates
    int x0 = (int)x;
    int x1 = x0 + 1;
    int y0 = (int)y;
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return value;
}
