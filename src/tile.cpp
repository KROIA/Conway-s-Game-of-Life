#include "tile.h"

float Tile::perlinXOffset = 0;
float Tile::perlinYOffset = 0;

#ifdef PIXELENGINE_USE_THREADS
pthread_mutex_t Tile::mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
Tile::Tile(Vector2u coord)
    :   GameObject()
{
    m_deadColor = Color(0,0,0);
#ifndef COLORED_TILE
    m_aliveColor = Color(255,255,255);
#endif
#ifdef SIMPLE_ALGORYTHM
    m_state         = dead;
    m_nextTickState = m_state;
#else
    m_coord = coord;
    m_maxneighborCount     = 8;
    m_addedneighborIndex   = 0;
    m_neighborList         = new unsigned char*[m_maxneighborCount];
    m_tileData             = 0;
    m_nextTickState        = dead;

#endif
#ifdef DISPLAY_NEIGHBOR_COUNT
    m_textPainter           = new TextPainter();

    m_textPainter->setVisibility(true);
    m_textPainter->setColor(Color(100,100,100));
    m_textPainter->setCharacterSize(40);
    m_textPainter->setScale(Vector2f(0.02,0.02));
    m_textPainter->setPos(Vector2f(coord)+Vector2f(0.25,0));
#endif
    m_activeneighborCount = 0;
    m_colorFadeFactor = 0.8;
    m_paused = true;

    //m_pixPainter_alive = new PixelPainter();
    //m_pixPainter_dead = new PixelPainter();

}
Tile::~Tile()
{
#ifndef SIMPLE_ALGORYTHM
    delete[] m_neighborList;
#endif
#ifdef DISPLAY_NEIGHBOR_COUNT
    delete m_textPainter;
#endif
}
void Tile::setup()
{
     GameObject::setPosInital(Vector2f(m_coord));


  /*   m_pixPainter_alive->resize(Vector2u(1,1));
     m_pixPainter_alive->setVisibility(false);
     m_pixPainter_alive->setPos(Vector2f(m_coord));
     m_pixPainter_alive->setPixelColor(Vector2u(0,0),m_aliveColor);
     m_pixPainter_alive->update();

     m_pixPainter_dead->resize(Vector2u(1,1));
     m_pixPainter_dead->setVisibility(true);
     m_pixPainter_dead->setPos(Vector2f(m_coord));
     m_pixPainter_dead->setPixelColor(Vector2u(0,0),m_deadColor);
     m_pixPainter_dead->update();
     GameObject::addPainter(m_pixPainter_alive);
     GameObject::addPainter(m_pixPainter_dead);*/
#ifdef DISPLAY_NEIGHBOR_COUNT
    display.subscribePainter(m_textPainter);
#endif
}

void Tile::addneighbor(Tile *neighbor)
{
    if(neighbor == nullptr)
        return;
#ifdef SIMPLE_ALGORYTHM
    m_neighborTileList.push_back(neighbor);
#else
    if(m_addedneighborIndex >= m_maxneighborCount)
        return;
    m_neighborList[m_addedneighborIndex] = &neighbor->m_tileData;
    m_addedneighborIndex++;
#ifdef COLORED_TILE
    m_neighborTileList.push_back(neighbor);
#endif
#endif
}
void Tile::setneighborList(const vector<Tile*> &list)
{
#ifdef SIMPLE_ALGORYTHM
    m_neighborTileList = list;
#else
    for(size_t i=0; i<list.size(); i++)
        addneighbor(list[i]);
#endif

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
bool Tile::getState() const
{
#ifdef SIMPLE_ALGORYTHM
    return m_state;
#else
    return (bool)TILE_GET_IS_ALIVE(m_tileData);
#endif
}
const Color    &Tile::getAliveColor() const
{
    return m_aliveColor;
}
const Color    &Tile::getColor() const
{
#ifdef SIMPLE_ALGORYTHM
    if(m_state == alive)
        return m_aliveColor;
    return m_deadColor;
#else
    if((TILE_GET_IS_ALIVE(m_tileData)))
        return m_aliveColor;
    return m_deadColor;
#endif
}
void Tile::prepareForNextMove()
{


#ifdef SIMPLE_ALGORYTHM

#ifdef COLORED_TILE
    m_colorList.clear();
    m_colorList.reserve(8);
#endif
    m_activeneighborCount = 0;
    for(size_t i=0; i<m_neighborTileList.size(); i++)
    {
        if(m_neighborTileList[i]->getState() == alive)
        {

            m_activeneighborCount++;
#ifdef COLORED_TILE
            m_colorList.push_back(m_neighborTileList[i]->getAliveColor());
#endif
        }
    }

    if(m_activeneighborCount == 3 &&
       m_state == dead)
    {
        m_nextTickState = alive;
    }else if(m_activeneighborCount < 2 &&
             m_state == alive)
    {
        m_nextTickState = dead;
    }if((m_activeneighborCount == 2 ||
         m_activeneighborCount == 3) &&
         m_state == alive)
    {
        m_nextTickState = alive;
    }else if(m_activeneighborCount > 3)
    {
        m_nextTickState = dead;
    }
#ifdef COLORED_TILE
    if(m_state == alive && m_nextTickState == dead)
        m_deadColor = m_aliveColor;
#endif
#else
#ifdef COLORED_TILE
    m_colorList.clear();
    m_colorList.reserve(8);
    for(size_t i=0; i<m_neighborTileList.size(); i++)
    {
        if((TILE_GET_IS_ALIVE(m_neighborTileList[i]->m_tileData)))
        {
            m_colorList.push_back(m_neighborTileList[i]->getAliveColor());
        }
    }
#endif

    if(!m_tileData)
    {
        return;
    }

    m_activeneighborCount = m_tileData >> 1;

    if(TILE_GET_IS_ALIVE(m_tileData))
    {
        if(m_activeneighborCount != 2 && m_activeneighborCount != 3)
        {
            m_nextTickState = dead;
#ifdef COLORED_TILE
            m_deadColor = m_aliveColor;
#endif
        }
    }
    else
    {
        if(m_activeneighborCount == 3)
        {
            m_nextTickState = alive;
        }
    }

#endif
}

void Tile::tick(const Vector2i &direction)
{
    if(m_paused)
        return;
    if(direction.x)
    {
        prepareForNextMove();
    }
    else
    {

#ifdef COLORED_TILE
        mixColors();
#endif
#ifdef SIMPLE_ALGORYTHM
        m_state = m_nextTickState;
#else
        bool lastAlive = TILE_GET_IS_ALIVE(m_tileData);
        if(m_nextTickState != lastAlive)
        {
            if(m_nextTickState)
                setAlive(m_aliveColor);
            else
                setDead();
        }
#endif
    }
}
void Tile::preDraw()
{
#ifdef SIMPLE_ALGORYTHM
    if(m_state == dead)
    {
#ifdef COLORED_TILE
        fadeColor(m_deadColor,m_colorFadeFactor);
#endif
        GameObject::display_setPixel(m_coord,m_deadColor);
    }
    else
    {
        GameObject::display_setPixel(m_coord,m_aliveColor);
    }
#else
    m_activeneighborCount = m_tileData >> 1;
    if(TILE_GET_IS_ALIVE(m_tileData))
    {
        GameObject::display_setPixel(m_coord,m_aliveColor);
        //m_pixPainter_dead->setPixelColor(Vector2u(0,0),m_aliveColor);
       // m_pixPainter_dead->setVisibility(false);
        //m_pixPainter_alive->setVisibility(true);
    }
    else
    {
#ifdef COLORED_TILE
        fadeColor(m_deadColor,m_colorFadeFactor);
#endif
        GameObject::display_setPixel(m_coord,m_deadColor);
        //m_pixPainter_dead->setPixelColor(Vector2u(0,0),m_deadColor);

       // m_pixPainter_alive->setVisibility(false);
        //m_pixPainter_dead->setVisibility(true);
    }
#endif

#ifdef DISPLAY_NEIGHBOR_COUNT
    m_textPainter->setText(std::to_string(m_activeneighborCount));
#endif
}
void Tile::setAlive()
{
   // qDebug() << "set alive: "<<m_coord.x<<" "<<m_coord.y;
#ifdef SIMPLE_ALGORYTHM
    m_state = alive;
    m_nextTickState = alive;
#else
    if(TILE_GET_IS_ALIVE(m_tileData))
        return;
#ifdef PIXELENGINE_USE_THREADS
     pthread_mutex_lock(&mutex);
#endif
     TILE_SET_ALIVE(m_tileData);
#ifdef PIXELENGINE_USE_THREADS
     pthread_mutex_unlock(&mutex);
#endif
    tellneighbor();
    m_nextTickState = alive;

    //TILE_TELL_neighbor(*(m_neighborList+i),m_tileData);i++;
#endif
#ifdef COLORED_TILE
    float scale = 50;
    float offset = 30;
    float colorStrength = 128;
    m_aliveColor.r =(1+perlin((float)m_coord.x/scale+perlinXOffset,(float)m_coord.y/scale+perlinYOffset))*colorStrength;
    m_aliveColor.g =(1+perlin((float)m_coord.x/scale+offset+perlinXOffset,(float)m_coord.y/scale+offset+perlinYOffset))*colorStrength;;
    m_aliveColor.b =(1+perlin((float)m_coord.x/scale+2*offset+perlinXOffset,(float)m_coord.y/scale+2*offset+perlinYOffset))*colorStrength;
#endif

}
void Tile::setAlive(const Color &color)
{
#ifdef SIMPLE_ALGORYTHM
    m_state = alive;
    m_nextTickState = alive;
#else

    if(TILE_GET_IS_ALIVE(m_tileData))
        return;
#ifdef PIXELENGINE_USE_THREADS
     pthread_mutex_lock(&mutex);
#endif
     TILE_SET_ALIVE(m_tileData);
#ifdef PIXELENGINE_USE_THREADS
     pthread_mutex_unlock(&mutex);
#endif

    tellneighbor();
    m_nextTickState = alive;
#endif
#ifdef COLORED_TILE
    m_aliveColor = color;
#endif
}
void Tile::setDead()
{
#ifdef SIMPLE_ALGORYTHM
    m_nextTickState = dead;
    m_state = dead;
#else
    if(!(TILE_GET_IS_ALIVE(m_tileData)))
        return;
#ifdef PIXELENGINE_USE_THREADS
     pthread_mutex_lock(&mutex);
#endif
     TILE_SET_DEAD(m_tileData);
#ifdef PIXELENGINE_USE_THREADS
     pthread_mutex_unlock(&mutex);
#endif

    m_nextTickState = dead;
    tellneighbor();
#endif
}
void Tile::setPause(bool paused)
{
    m_paused = paused;
}

#ifndef SIMPLE_ALGORYTHM
inline void Tile::tellneighbor()
{
    if((TILE_GET_IS_ALIVE(m_tileData)))
    {
        for(size_t i=0; i<m_addedneighborIndex; i++)
        {
#ifdef PIXELENGINE_USE_THREADS
            pthread_mutex_lock(&mutex);
#endif
            *(m_neighborList[i]) += 0x02;
#ifdef PIXELENGINE_USE_THREADS
            pthread_mutex_unlock(&mutex);
#endif
        }
    }
    else
    {
        for(size_t i=0; i<m_addedneighborIndex; i++)
        {
#ifdef PIXELENGINE_USE_THREADS
            pthread_mutex_lock(&mutex);
#endif
            *(m_neighborList[i]) -= 0x02;
#ifdef PIXELENGINE_USE_THREADS
            pthread_mutex_unlock(&mutex);
#endif
        }
    }
}
#endif

#ifdef COLORED_TILE
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
#ifdef SIMPLE_ALGORYTHM
    if(m_state == dead)
#else
    if(!(TILE_GET_IS_ALIVE(m_tileData)))
#endif
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
#endif
