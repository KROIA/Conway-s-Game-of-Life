#ifndef TILE_H
#define TILE_H
#include "gameobject.h"
#include "pixelPainter.h"

//#define COLORED_TILE
//#define SIMPLE_ALGORYTHM
//#define   DISPLAY_NEIGHBOR_COUNT

#ifndef SIMPLE_ALGORYTHM
    #define TILE_ALIVE_BIT 0x01      // Bit 0
    #define TILE_neighbor_BITS 0x3C // Bits 1 - 4

    #define TILE_SET_ALIVE(tile) tile |= TILE_ALIVE_BIT
    #define TILE_SET_DEAD(tile) tile  &= ~TILE_ALIVE_BIT
    #define TILE_GET_IS_ALIVE(tile) tile & TILE_ALIVE_BIT
#endif

class Tile  :   public GameObject
{
    public:
        enum State{
            dead,
            alive
        };
        Tile(Vector2u coord);
        ~Tile();
        void setup();

        void addneighbor(Tile *neighbor);
        void setneighborList(const vector<Tile*> &list);
        void setColorFadeFactor(float factor);

        const Vector2u &getCoord() const;
        bool     getState() const;
        const Color    &getAliveColor() const;
        const Color    &getColor() const;

        void prepareForNextMove();
        void tick(const Vector2i &direction);

        void preDraw();

        void setAlive();
        void setAlive(const Color &color);
        void setDead();

        void setPause(bool paused);
    protected:

    private:
#ifdef COLORED_TILE
        void mixColors();
        void fadeColor(Color &c,float factor);
#endif

      //  PixelPainter *m_pixPainter_alive;
      //  PixelPainter *m_pixPainter_dead;
        Color m_aliveColor;
        Color m_deadColor;
        Vector2u m_coord;
#ifdef SIMPLE_ALGORYTHM
        State m_state;
        vector<Tile*> m_neighborTileList;
#else
        inline void tellneighbor();
        unsigned char m_tileData;
        unsigned char** m_neighborList;
        size_t m_addedneighborIndex;
        size_t m_maxneighborCount;
#ifdef COLORED_TILE
        vector<Tile*> m_neighborTileList;
#endif

#endif
#ifdef DISPLAY_NEIGHBOR_COUNT
        TextPainter *m_textPainter;
#endif
        State m_nextTickState;
        size_t m_activeneighborCount;


#ifdef COLORED_TILE
        vector<Color> m_colorList;
#endif
        float m_colorFadeFactor;
        static float perlinXOffset;
        static float perlinYOffset;
        bool m_paused;

#ifdef PIXELENGINE_USE_THREADS
        static pthread_mutex_t mutex;
#endif
};

#ifdef COLORED_TILE
float interpolate(float a0, float a1, float w);
Vector2f randomGradient(int ix, int iy);
float dotGridGradient(int ix, int iy, float x, float y);
float perlin(float x, float y);
#endif
#endif // TILE_H
