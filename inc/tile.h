#ifndef TILE_H
#define TILE_H
#include "gameobject.h"
#include "pixelPainter.h"

class Tile  :   public GameObject
{
    public:
        enum State{
            dead,
            alive
        };
        Tile(Vector2u coord);

        void addNeighbour(Tile *neighbour);
        void setNeighbourList(const vector<Tile*> &list);
        void setColorFadeFactor(float factor);

        const Vector2u &getCoord() const;
        const State    &getState() const;
        const Color    &getAliveColor() const;
        const Color    &getColor() const;

        void prepareForNextMove();
        void tick(const Vector2i &direction);
        void draw(PixelDisplay &display);

        void setAlive();
        void setAlive(const Color &color);
        void setDead();
    protected:

    private:
        void mixColors();
        void fadeColor(Color &c,float factor);

        Color m_aliveColor;
        Color m_deadColor;
        Vector2u m_coord;

        State m_state;
        State m_nextTickState;

        vector<Tile*> m_neighbourList;
        vector<Color> m_colorList;
        float m_colorFadeFactor;
        static float perlinXOffset;
        static float perlinYOffset;
};

float interpolate(float a0, float a1, float w);
Vector2f randomGradient(int ix, int iy);
float dotGridGradient(int ix, int iy, float x, float y);
float perlin(float x, float y);

#endif // TILE_H
